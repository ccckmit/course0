




typedef enum {

    TK_IDENT = 1,
    TK_INT_LIT,
    TK_FLOAT_LIT,
    TK_CHAR_LIT,
    TK_STR_LIT,


    TK_AUTO, TK_BREAK, TK_CASE, TK_CHAR, TK_CONST,
    TK_CONTINUE, TK_DEFAULT, TK_DO, TK_DOUBLE, TK_ELSE,
    TK_ENUM, TK_EXTERN, TK_FLOAT, TK_FOR, TK_GOTO,
    TK_IF, TK_INT, TK_LONG, TK_REGISTER, TK_RETURN,
    TK_SHORT, TK_SIGNED, TK_SIZEOF, TK_STATIC, TK_STRUCT,
    TK_SWITCH, TK_TYPEDEF, TK_UNION, TK_UNSIGNED, TK_VOID,
    TK_VOLATILE, TK_WHILE,


    TK_LPAREN,
    TK_RPAREN,
    TK_LBRACE,
    TK_RBRACE,
    TK_LBRACKET,
    TK_RBRACKET,
    TK_SEMICOLON,
    TK_COMMA,
    TK_COLON,
    TK_QUESTION,
    TK_TILDE,


    TK_DOT,
    TK_ELLIPSIS,

    TK_PLUS,
    TK_PLUS_PLUS,
    TK_PLUS_EQ,

    TK_MINUS,
    TK_MINUS_MINUS,
    TK_MINUS_EQ,
    TK_ARROW,

    TK_STAR,
    TK_STAR_EQ,

    TK_SLASH,
    TK_SLASH_EQ,

    TK_PERCENT,
    TK_PERCENT_EQ,

    TK_AMP,
    TK_AMP_AMP,
    TK_AMP_EQ,

    TK_PIPE,
    TK_PIPE_PIPE,
    TK_PIPE_EQ,

    TK_CARET,
    TK_CARET_EQ,

    TK_BANG,
    TK_BANG_EQ,

    TK_EQ,
    TK_EQ_EQ,

    TK_LT,
    TK_LT_LT,
    TK_LT_LT_EQ,
    TK_LT_EQ,

    TK_GT,
    TK_GT_GT,
    TK_GT_GT_EQ,
    TK_GT_EQ,

    TK_HASH,


    TK_EOF,
    TK_UNKNOWN,

    TK_COUNT
} TokenKind;


typedef struct {
    TokenKind kind;
    char *text;
    int line;
    int col;
} Token;


typedef struct Lexer Lexer;



Lexer *lexer_new_buf(const char *filename, const char *src, int len);


Lexer *lexer_new_file(const char *filename);


Token lexer_next(Lexer *lx);


Token lexer_peek(Lexer *lx);


void lexer_free(Lexer *lx);


const char *token_kind_name(TokenKind k);


int lexer_test(const char *filename);







typedef struct ArenaBlock {
    char *data;
    int used;
    int cap;
    struct ArenaBlock *next;
} ArenaBlock;

typedef struct {
    ArenaBlock *head;
} Arena;

static ArenaBlock *arena_block_new(int cap) {
    ArenaBlock *b = malloc(sizeof(ArenaBlock));
    if (!b) { perror("malloc"); exit(1); }
    b->data = malloc((size_t)cap);
    if (!b->data) { perror("malloc"); exit(1); }
    b->used = 0;
    b->cap = cap;
    b->next = NULL;
    return b;
}

static char *arena_alloc(Arena *a, int n) {
    if (!a->head || a->head->used + n > a->head->cap) {
        int cap = n > 65536 ? n : 65536;
        ArenaBlock *b = arena_block_new(cap);
        b->next = a->head;
        a->head = b;
    }
    char *p = a->head->data + a->head->used;
    a->head->used += n;
    return p;
}

static char *arena_strdup(Arena *a, const char *s, int len) {
    char *p = arena_alloc(a, len + 1);
    memcpy(p, s, (size_t)len);
    p[len] = '\0';
    return p;
}

static void arena_free(Arena *a) {
    ArenaBlock *b = a->head;
    while (b) {
        ArenaBlock *next = b->next;
        free(b->data);
        free(b);
        b = next;
    }
    a->head = NULL;
}


typedef struct { const char *word; TokenKind kind; } KwEntry;

static const KwEntry KEYWORDS[] = {
    {"auto", TK_AUTO}, {"break", TK_BREAK},
    {"case", TK_CASE}, {"char", TK_CHAR},
    {"const", TK_CONST}, {"continue", TK_CONTINUE},
    {"default", TK_DEFAULT}, {"do", TK_DO},
    {"double", TK_DOUBLE}, {"else", TK_ELSE},
    {"enum", TK_ENUM}, {"extern", TK_EXTERN},
    {"float", TK_FLOAT}, {"for", TK_FOR},
    {"goto", TK_GOTO}, {"if", TK_IF},
    {"int", TK_INT}, {"long", TK_LONG},
    {"register", TK_REGISTER}, {"return", TK_RETURN},
    {"short", TK_SHORT}, {"signed", TK_SIGNED},
    {"sizeof", TK_SIZEOF}, {"static", TK_STATIC},
    {"struct", TK_STRUCT}, {"switch", TK_SWITCH},
    {"typedef", TK_TYPEDEF}, {"union", TK_UNION},
    {"unsigned", TK_UNSIGNED}, {"void", TK_VOID},
    {"volatile", TK_VOLATILE}, {"while", TK_WHILE},
    {NULL, 0}
};

static TokenKind keyword_lookup(const char *s) {
    for (int i = 0; KEYWORDS[i].word; i++)
        if (strcmp(s, KEYWORDS[i].word) == 0)
            return KEYWORDS[i].kind;
    return TK_IDENT;
}


struct Lexer {
    const char *filename;
    const char *src;
    int len;
    int pos;
    int line;
    int col;
    Arena arena;
    Token peek_tok;
    int peek_valid;
    char *owned_src;
};


static void lex_error(Lexer *lx, const char *fmt, ...) {
    va_list ap;
    fprintf(stderr, "%s:%d:%d: error: ", lx->filename, lx->line, lx->col);
    __builtin_va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    __builtin_va_end(ap);
    fprintf(stderr, "\n");
}


static int cur(Lexer *lx) {
    return lx->pos < lx->len ? (unsigned char)lx->src[lx->pos] : -1;
}
static int peek1(Lexer *lx) {
    return lx->pos + 1 < lx->len ? (unsigned char)lx->src[lx->pos + 1] : -1;
}
static void advance(Lexer *lx) {
    if (lx->pos >= lx->len) return;
    if (lx->src[lx->pos] == '\n') { lx->line++; lx->col = 1; }
    else { lx->col++; }
    lx->pos++;
}


static void skip_ws_comments(Lexer *lx) {
    for (;;) {

        while (lx->pos < lx->len && isspace((unsigned char)lx->src[lx->pos]))
            advance(lx);

        if (cur(lx) == '/' && peek1(lx) == '/') {
            while (lx->pos < lx->len && lx->src[lx->pos] != '\n')
                advance(lx);
            continue;
        }

        if (cur(lx) == '/' && peek1(lx) == '*') {
            int start_line = lx->line;
            advance(lx); advance(lx);
            for (;;) {
                if (lx->pos >= lx->len) {
                    lex_error(lx, "unterminated block comment (started at line %d)", start_line);
                    return;
                }
                if (cur(lx) == '*' && peek1(lx) == '/') {
                    advance(lx); advance(lx);
                    break;
                }
                advance(lx);
            }
            continue;
        }

        if (cur(lx) == '#') {
            while (lx->pos < lx->len && lx->src[lx->pos] != '\n')
                advance(lx);
            continue;
        }
        break;
    }
}




static Token scan(Lexer *lx) {
    skip_ws_comments(lx);

    Token t;
    t.line = lx->line;
    t.col = lx->col;
    t.text = NULL;
    t.kind = TK_UNKNOWN;

    if (lx->pos >= lx->len) {
        t.kind = TK_EOF;
        t.text = arena_strdup(&lx->arena, "", 0);
        return t;
    }

    char buf[4096];
    int bp = 0;
    int c = cur(lx);


    if (isalpha(c) || c == '_') {
        while (isalnum(cur(lx)) || cur(lx) == '_') {
            if (bp < 4096 - 1) buf[bp++] = (char)cur(lx);
            advance(lx);
        }
        buf[bp] = '\0';
        t.kind = keyword_lookup(buf);
        t.text = arena_strdup(&lx->arena, buf, bp);
        return t;
    }


    if (isdigit(c) || (c == '.' && isdigit(peek1(lx)))) {
        int is_float = 0;
        if (c == '0' && (peek1(lx) == 'x' || peek1(lx) == 'X')) {

            buf[bp++] = (char)cur(lx); advance(lx);
            buf[bp++] = (char)cur(lx); advance(lx);
            while (isxdigit(cur(lx)) || cur(lx) == '.') {
                if (cur(lx) == '.') is_float = 1;
                if (bp < 4096 - 1) buf[bp++] = (char)cur(lx);
                advance(lx);
            }
            if (cur(lx) == 'p' || cur(lx) == 'P') {
                is_float = 1;
                buf[bp++] = (char)cur(lx); advance(lx);
                if (cur(lx) == '+' || cur(lx) == '-') { buf[bp++] = (char)cur(lx); advance(lx); }
                while (isdigit(cur(lx))) { buf[bp++] = (char)cur(lx); advance(lx); }
            }
        } else {

            while (isdigit(cur(lx)) || cur(lx) == '.') {
                if (cur(lx) == '.') { is_float = 1; }
                if (bp < 4096 - 1) buf[bp++] = (char)cur(lx);
                advance(lx);
            }
            if (cur(lx) == 'e' || cur(lx) == 'E') {
                is_float = 1;
                buf[bp++] = (char)cur(lx); advance(lx);
                if (cur(lx) == '+' || cur(lx) == '-') { buf[bp++] = (char)cur(lx); advance(lx); }
                while (isdigit(cur(lx))) { buf[bp++] = (char)cur(lx); advance(lx); }
            }
        }

        while (cur(lx) == 'u' || cur(lx) == 'U' || cur(lx) == 'l' || cur(lx) == 'L' ||
               cur(lx) == 'f' || cur(lx) == 'F') {
            if (cur(lx) == 'f' || cur(lx) == 'F') is_float = 1;
            if (bp < 4096 - 1) buf[bp++] = (char)cur(lx);
            advance(lx);
        }
        buf[bp] = '\0';
        t.kind = is_float ? TK_FLOAT_LIT : TK_INT_LIT;
        t.text = arena_strdup(&lx->arena, buf, bp);
        return t;
    }


    if (c == '\'') {
        buf[bp++] = '\'';
        advance(lx);
        while (cur(lx) != '\'' && cur(lx) != -1) {
            if (cur(lx) == '\\') {
                buf[bp++] = '\\';
                advance(lx);
                if (cur(lx) != -1) { buf[bp++] = (char)cur(lx); advance(lx); }
            } else {
                buf[bp++] = (char)cur(lx); advance(lx);
            }
        }
        if (cur(lx) == '\'') { buf[bp++] = '\''; advance(lx); }
        else lex_error(lx, "unterminated character literal");
        buf[bp] = '\0';
        t.kind = TK_CHAR_LIT;
        t.text = arena_strdup(&lx->arena, buf, bp);
        return t;
    }


    if (c == '"') {
        buf[bp++] = '"';
        advance(lx);
        while (cur(lx) != '"' && cur(lx) != -1) {
            if (cur(lx) == '\\') {
                buf[bp++] = '\\';
                advance(lx);
                if (cur(lx) != -1) { buf[bp++] = (char)cur(lx); advance(lx); }
            } else {
                if (bp < 4096 - 1) buf[bp++] = (char)cur(lx);
                advance(lx);
            }
        }
        if (cur(lx) == '"') { buf[bp++] = '"'; advance(lx); }
        else lex_error(lx, "unterminated string literal");
        buf[bp] = '\0';
        t.kind = TK_STR_LIT;
        t.text = arena_strdup(&lx->arena, buf, bp);
        return t;
    }


    advance(lx);
    buf[0] = (char)c; buf[1] = '\0'; bp = 1;




    switch (c) {
        case '+':
            if (c==('+') && cur(lx)==('+')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_PLUS_PLUS); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            if (c==('+') && cur(lx)==('=')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_PLUS_EQ); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            t.kind = TK_PLUS; break;
        case '-':
            if (c==('-') && cur(lx)==('-')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_MINUS_MINUS); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            if (c==('-') && cur(lx)==('=')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_MINUS_EQ); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            if (c==('-') && cur(lx)==('>')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_ARROW); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            t.kind = TK_MINUS; break;
        case '*':
            if (c==('*') && cur(lx)==('=')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_STAR_EQ); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            t.kind = TK_STAR; break;
        case '/':
            if (c==('/') && cur(lx)==('=')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_SLASH_EQ); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            t.kind = TK_SLASH; break;
        case '%':
            if (c==('%') && cur(lx)==('=')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_PERCENT_EQ); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            t.kind = TK_PERCENT; break;
        case '&':
            if (c==('&') && cur(lx)==('&')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_AMP_AMP); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            if (c==('&') && cur(lx)==('=')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_AMP_EQ); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            t.kind = TK_AMP; break;
        case '|':
            if (c==('|') && cur(lx)==('|')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_PIPE_PIPE); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            if (c==('|') && cur(lx)==('=')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_PIPE_EQ); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            t.kind = TK_PIPE; break;
        case '^':
            if (c==('^') && cur(lx)==('=')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_CARET_EQ); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            t.kind = TK_CARET; break;
        case '!':
            if (c==('!') && cur(lx)==('=')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_BANG_EQ); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            t.kind = TK_BANG; break;
        case '=':
            if (c==('=') && cur(lx)==('=')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_EQ_EQ); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            t.kind = TK_EQ; break;
        case '<':
            if (c==('<') && cur(lx)==('<') && peek1(lx)==('=')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_LT_LT_EQ); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            if (c==('<') && cur(lx)==('<')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_LT_LT); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            if (c==('<') && cur(lx)==('=')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_LT_EQ); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            t.kind = TK_LT; break;
        case '>':
            if (c==('>') && cur(lx)==('>') && peek1(lx)==('=')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_GT_GT_EQ); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            if (c==('>') && cur(lx)==('>')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_GT_GT); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            if (c==('>') && cur(lx)==('=')) { buf[bp++]=(char)cur(lx); advance(lx); buf[bp]='\0'; t.kind=(TK_GT_EQ); t.text=arena_strdup(&lx->arena,buf,bp); return t; }
            t.kind = TK_GT; break;
        case '.':

            if (cur(lx) == '.' && peek1(lx) == '.') {
                buf[bp++] = (char)cur(lx); advance(lx);
                buf[bp++] = (char)cur(lx); advance(lx);
                buf[bp] = '\0';
                t.kind = TK_ELLIPSIS;
            } else {
                t.kind = TK_DOT;
            }
            break;
        case '(': t.kind = TK_LPAREN; break;
        case ')': t.kind = TK_RPAREN; break;
        case '{': t.kind = TK_LBRACE; break;
        case '}': t.kind = TK_RBRACE; break;
        case '[': t.kind = TK_LBRACKET; break;
        case ']': t.kind = TK_RBRACKET; break;
        case ';': t.kind = TK_SEMICOLON; break;
        case ',': t.kind = TK_COMMA; break;
        case ':': t.kind = TK_COLON; break;
        case '?': t.kind = TK_QUESTION; break;
        case '~': t.kind = TK_TILDE; break;
        case '#': t.kind = TK_HASH; break;
        default:
            lex_error(lx, "unexpected character '%c' (0x%02x)", c, (unsigned char)c);
            t.kind = TK_UNKNOWN;
    }

    buf[bp] = '\0';
    t.text = arena_strdup(&lx->arena, buf, bp);
    return t;
}



static Lexer *lexer_init(const char *filename, const char *src, int len, char *owned) {
    Lexer *lx = calloc(1, sizeof(Lexer));
    if (!lx) { perror("calloc"); exit(1); }
    lx->filename = filename;
    lx->src = src;
    lx->len = len;
    lx->pos = 0;
    lx->line = 1;
    lx->col = 1;
    lx->arena.head = NULL;
    lx->peek_valid = 0;
    lx->owned_src = owned;
    return lx;
}

Lexer *lexer_new_buf(const char *filename, const char *src, int len) {
    return lexer_init(filename, src, len, NULL);
}

Lexer *lexer_new_file(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) { perror(filename); return NULL; }
    fseek(fp, 0, 2);
    long sz = ftell(fp);
    rewind(fp);
    char *buf = malloc((size_t)sz + 1);
    if (!buf) { perror("malloc"); fclose(fp); exit(1); }
    size_t rd = fread(buf, 1, (size_t)sz, fp);
    fclose(fp);
    buf[rd] = '\0';
    return lexer_init(filename, buf, (int)rd, buf);
}

Token lexer_next(Lexer *lx) {
    if (lx->peek_valid) {
        lx->peek_valid = 0;
        return lx->peek_tok;
    }
    return scan(lx);
}

Token lexer_peek(Lexer *lx) {
    if (!lx->peek_valid) {
        lx->peek_tok = scan(lx);
        lx->peek_valid = 1;
    }
    return lx->peek_tok;
}

void lexer_free(Lexer *lx) {
    if (!lx) return;
    arena_free(&lx->arena);
    if (lx->owned_src) free(lx->owned_src);
    free(lx);
}


static const char *KIND_NAMES[TK_COUNT + 1] = {
    [0] = "INVALID",
    [TK_IDENT] = "IDENT",
    [TK_INT_LIT] = "INT_LIT",
    [TK_FLOAT_LIT] = "FLOAT_LIT",
    [TK_CHAR_LIT] = "CHAR_LIT",
    [TK_STR_LIT] = "STR_LIT",
    [TK_AUTO] = "auto",
    [TK_BREAK] = "break",
    [TK_CASE] = "case",
    [TK_CHAR] = "char",
    [TK_CONST] = "const",
    [TK_CONTINUE] = "continue",
    [TK_DEFAULT] = "default",
    [TK_DO] = "do",
    [TK_DOUBLE] = "double",
    [TK_ELSE] = "else",
    [TK_ENUM] = "enum",
    [TK_EXTERN] = "extern",
    [TK_FLOAT] = "float",
    [TK_FOR] = "for",
    [TK_GOTO] = "goto",
    [TK_IF] = "if",
    [TK_INT] = "int",
    [TK_LONG] = "long",
    [TK_REGISTER] = "register",
    [TK_RETURN] = "return",
    [TK_SHORT] = "short",
    [TK_SIGNED] = "signed",
    [TK_SIZEOF] = "sizeof",
    [TK_STATIC] = "static",
    [TK_STRUCT] = "struct",
    [TK_SWITCH] = "switch",
    [TK_TYPEDEF] = "typedef",
    [TK_UNION] = "union",
    [TK_UNSIGNED] = "unsigned",
    [TK_VOID] = "void",
    [TK_VOLATILE] = "volatile",
    [TK_WHILE] = "while",
    [TK_LPAREN] = "(",
    [TK_RPAREN] = ")",
    [TK_LBRACE] = "{",
    [TK_RBRACE] = "}",
    [TK_LBRACKET] = "[",
    [TK_RBRACKET] = "]",
    [TK_SEMICOLON] = ";",
    [TK_COMMA] = ",",
    [TK_COLON] = ":",
    [TK_QUESTION] = "?",
    [TK_TILDE] = "~",
    [TK_DOT] = ".",
    [TK_ELLIPSIS] = "...",
    [TK_PLUS] = "+",
    [TK_PLUS_PLUS] = "++",
    [TK_PLUS_EQ] = "+=",
    [TK_MINUS] = "-",
    [TK_MINUS_MINUS] = "--",
    [TK_MINUS_EQ] = "-=",
    [TK_ARROW] = "->",
    [TK_STAR] = "*",
    [TK_STAR_EQ] = "*=",
    [TK_SLASH] = "/",
    [TK_SLASH_EQ] = "/=",
    [TK_PERCENT] = "%",
    [TK_PERCENT_EQ] = "%=",
    [TK_AMP] = "&",
    [TK_AMP_AMP] = "&&",
    [TK_AMP_EQ] = "&=",
    [TK_PIPE] = "|",
    [TK_PIPE_PIPE] = "||",
    [TK_PIPE_EQ] = "|=",
    [TK_CARET] = "^",
    [TK_CARET_EQ] = "^=",
    [TK_BANG] = "!",
    [TK_BANG_EQ] = "!=",
    [TK_EQ] = "=",
    [TK_EQ_EQ] = "==",
    [TK_LT] = "<",
    [TK_LT_LT] = "<<",
    [TK_LT_LT_EQ] = "<<=",
    [TK_LT_EQ] = "<=",
    [TK_GT] = ">",
    [TK_GT_GT] = ">>",
    [TK_GT_GT_EQ] = ">>=",
    [TK_GT_EQ] = ">=",
    [TK_HASH] = "#",
    [TK_EOF] = "EOF",
    [TK_UNKNOWN] = "UNKNOWN",
};

const char *token_kind_name(TokenKind k) {
    if (k < 0 || k >= TK_COUNT) return "INVALID";
    const char *n = KIND_NAMES[k];
    return n ? n : "INVALID";
}


int lexer_test(const char *filename) {
    Lexer *lx = lexer_new_file(filename);
    if (!lx) return 1;

    int count = 0;
    for (;;) {
        Token t = lexer_next(lx);
        printf("%s:%d:%d\t%-16s\t|%s|\n",
               filename, t.line, t.col,
               token_kind_name(t.kind), t.text);
        count++;
        if (t.kind == TK_EOF) break;
        if (t.kind == TK_UNKNOWN) { lexer_free(lx); return 1; }
    }
    fprintf(stderr, "lexer_test: %s → %d tokens OK\n", filename, count);
    lexer_free(lx);
    return 0;
}
