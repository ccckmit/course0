#include "fulltext0.h"
#include <sys/stat.h>
#include <sys/types.h>

/* ── In-memory index ─────────────────────────────────────────────────────── */
static TermEntry  g_terms[MAX_TERMS];
static PostingList g_posts[MAX_TERMS];
static int        g_num_terms = 0;

/* Simple open-addressing hash map: term string → index into g_terms[] */
#define HASH_SIZE (1<<17)   /* 131072 buckets */
static int32_t    g_hash[HASH_SIZE];  /* -1 = empty */

static uint32_t hash_term(const char *s)
{
    uint32_t h = 2166136261u;
    while (*s) { h ^= (unsigned char)*s++; h *= 16777619u; }
    return h;
}

static int find_or_insert(const char *term)
{
    uint32_t h = hash_term(term) & (HASH_SIZE-1);
    while (1) {
        if (g_hash[h] == -1) {
            /* insert */
            if (g_num_terms >= MAX_TERMS) {
                fprintf(stderr, "Too many terms\n"); exit(1);
            }
            int idx = g_num_terms++;
            g_hash[h] = idx;
            { size_t tl=strlen(term); if(tl>=MAX_TOKEN_LEN)tl=MAX_TOKEN_LEN-1;
              memcpy(g_terms[idx].term,term,tl); g_terms[idx].term[tl]='\0'; }
            g_terms[idx].post_offset = 0;
            g_terms[idx].post_count  = 0;
            g_posts[idx].doc_ids = NULL;
            g_posts[idx].count   = 0;
            g_posts[idx].cap     = 0;
            return idx;
        }
        if (strcmp(g_terms[g_hash[h]].term, term) == 0)
            return g_hash[h];
        h = (h+1) & (HASH_SIZE-1);
    }
}

static void add_posting(int term_idx, uint32_t doc_id)
{
    PostingList *pl = &g_posts[term_idx];
    /* dedup: last doc_id equals current → skip */
    if (pl->count > 0 && pl->doc_ids[pl->count-1] == doc_id) return;

    if (pl->count >= pl->cap) {
        pl->cap = pl->cap ? pl->cap*2 : 4;
        pl->doc_ids = realloc(pl->doc_ids, pl->cap * sizeof(uint32_t));
        if (!pl->doc_ids) { fprintf(stderr, "OOM\n"); exit(1); }
    }
    pl->doc_ids[pl->count++] = doc_id;
}

/* ── Write index to disk in 4K blocks ────────────────────────────────────── */
static void write_index(uint32_t num_docs)
{
    /* ensure output dir */
    mkdir("_index", 0755);

    FILE *fp = fopen(INDEX_PATH, "wb");
    if (!fp) { perror("fopen index"); exit(1); }

    /* ── Build term table bytes ── */
    /* estimate size: for each term: 2(len) + term_len + 4(offset) + 4(count) */
    size_t term_table_cap = 0;
    for (int i=0;i<g_num_terms;i++)
        term_table_cap += 2 + strlen(g_terms[i].term) + 8;

    unsigned char *term_buf = malloc(term_table_cap + 64);
    if (!term_buf) { fprintf(stderr,"OOM\n"); exit(1); }
    size_t tb_pos = 0;

    /* Build posting bytes to get offsets first */
    size_t post_total = 0;
    for (int i=0;i<g_num_terms;i++)
        post_total += g_posts[i].count * 4;

    unsigned char *post_buf = malloc(post_total + 4);
    if (!post_buf) { fprintf(stderr,"OOM\n"); exit(1); }
    size_t pb_pos = 0;

    for (int i=0;i<g_num_terms;i++) {
        /* record offset before writing this posting list */
        g_terms[i].post_offset = (uint32_t)pb_pos;
        g_terms[i].post_count  = g_posts[i].count;

        for (uint32_t j=0;j<g_posts[i].count;j++) {
            uint32_t v = g_posts[i].doc_ids[j];
            post_buf[pb_pos+0] = (v)      & 0xFF;
            post_buf[pb_pos+1] = (v>> 8)  & 0xFF;
            post_buf[pb_pos+2] = (v>>16)  & 0xFF;
            post_buf[pb_pos+3] = (v>>24)  & 0xFF;
            pb_pos += 4;
        }

        /* Now write term record */
        uint16_t tlen = (uint16_t)strlen(g_terms[i].term);
        term_buf[tb_pos+0] = tlen & 0xFF;
        term_buf[tb_pos+1] = (tlen>>8) & 0xFF;
        tb_pos += 2;
        memcpy(term_buf+tb_pos, g_terms[i].term, tlen);
        tb_pos += tlen;
        uint32_t po = g_terms[i].post_offset;
        term_buf[tb_pos+0]= po     &0xFF; term_buf[tb_pos+1]=(po>> 8)&0xFF;
        term_buf[tb_pos+2]=(po>>16)&0xFF; term_buf[tb_pos+3]=(po>>24)&0xFF;
        tb_pos += 4;
        uint32_t pc = g_terms[i].post_count;
        term_buf[tb_pos+0]= pc     &0xFF; term_buf[tb_pos+1]=(pc>> 8)&0xFF;
        term_buf[tb_pos+2]=(pc>>16)&0xFF; term_buf[tb_pos+3]=(pc>>24)&0xFF;
        tb_pos += 4;
    }

    /* Block layout */
    uint32_t term_blocks = (uint32_t)((tb_pos + BLOCK_SIZE-1) / BLOCK_SIZE);
    uint32_t term_block_start = 1;                         /* block 0 = header */
    uint32_t post_block_start = term_block_start + term_blocks;

    /* ── Block 0: header ── */
    unsigned char hdr[BLOCK_SIZE];
    memset(hdr, 0, BLOCK_SIZE);
    uint32_t magic   = IDX_MAGIC;
    uint32_t version = IDX_VERSION;
    memcpy(hdr+ 0, &magic,            4);
    memcpy(hdr+ 4, &version,          4);
    memcpy(hdr+ 8, &g_num_terms,      4);  /* uint32 cast */
    memcpy(hdr+12, &num_docs,         4);
    memcpy(hdr+16, &term_block_start, 4);
    memcpy(hdr+20, &post_block_start, 4);
    fwrite(hdr, 1, BLOCK_SIZE, fp);

    /* ── Term table blocks (pad to 4K boundaries) ── */
    size_t written = 0;
    while (written < tb_pos) {
        unsigned char blk[BLOCK_SIZE];
        size_t chunk = tb_pos - written;
        if (chunk > BLOCK_SIZE) chunk = BLOCK_SIZE;
        memcpy(blk, term_buf+written, chunk);
        if (chunk < BLOCK_SIZE) memset(blk+chunk, 0, BLOCK_SIZE-chunk);
        fwrite(blk, 1, BLOCK_SIZE, fp);
        written += chunk;
    }

    /* ── Posting blocks (pad to 4K boundaries) ── */
    written = 0;
    while (written < pb_pos) {
        unsigned char blk[BLOCK_SIZE];
        size_t chunk = pb_pos - written;
        if (chunk > BLOCK_SIZE) chunk = BLOCK_SIZE;
        memcpy(blk, post_buf+written, chunk);
        if (chunk < BLOCK_SIZE) memset(blk+chunk, 0, BLOCK_SIZE-chunk);
        fwrite(blk, 1, BLOCK_SIZE, fp);
        written += chunk;
    }

    fclose(fp);
    free(term_buf);
    free(post_buf);

    /* Report */
    long fsize = 0;
    {FILE*f2=fopen(INDEX_PATH,"rb"); if(f2){fseek(f2,0,SEEK_END);fsize=ftell(f2);fclose(f2);}}
    printf("Index written: %s\n", INDEX_PATH);
    printf("  docs     : %u\n", num_docs);
    printf("  terms    : %d\n", g_num_terms);
    printf("  file size: %ld bytes (%ld blocks)\n", fsize, fsize/BLOCK_SIZE);
}

/* ── main ────────────────────────────────────────────────────────────────── */
int main(int argc, char *argv[])
{
    const char *corpus = (argc>=2) ? argv[1] : CORPUS_PATH;

    /* init hash */
    memset(g_hash, -1, sizeof(g_hash));

    FILE *fp = fopen(corpus, "r");
    if (!fp) { perror("fopen corpus"); return 1; }

    char line[4096];
    uint32_t doc_id = 0;
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];

    while (fgets(line, sizeof(line), fp)) {
        /* strip trailing newline */
        int ll = strlen(line);
        while (ll>0 && (line[ll-1]=='\n'||line[ll-1]=='\r')) line[--ll]='\0';
        if (ll==0) continue;

        int ntok = tokenize(line, tokens, MAX_TOKENS);
        for (int t=0;t<ntok;t++) {
            int idx = find_or_insert(tokens[t]);
            add_posting(idx, doc_id);
        }
        doc_id++;

        if (doc_id % 100 == 0)
            fprintf(stderr, "\r  Indexed %u docs, %d terms ...", doc_id, g_num_terms);
    }
    fprintf(stderr, "\n");
    fclose(fp);

    write_index(doc_id);

    /* free posting memory */
    for (int i=0;i<g_num_terms;i++)
        free(g_posts[i].doc_ids);

    return 0;
}
