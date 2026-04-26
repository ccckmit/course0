/*
 * spice0.c — 簡易 SPICE 電路模擬器
 *
 * 支援元件：
 *   R  — 電阻  (Resistor)
 *   V  — 獨立電壓源 (Voltage source)
 *   I  — 獨立電流源 (Current source)
 *   C  — 電容  (Capacitor，暫態分析)
 *   L  — 電感  (Inductor，暫態分析)
 *
 * 分析類型：
 *   .op          — 直流工作點 (DC Operating Point)
 *   .tran        — 暫態分析   (Transient)
 *   .dc          — 直流掃描   (DC Sweep)
 *
 * 解法：改良節點分析法 (Modified Nodal Analysis, MNA)
 *        Ax = b  → LU 分解求解
 *
 * 輸出：
 *   .print  — 指定輸出節點電壓或支路電流
 *
 * 編譯：
 *   gcc -O2 -lm -o spice0 spice0.c
 *
 * 使用：
 *   ./spice0 <netlist.sp>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

/* ─────────────────────── 常數 ─────────────────────── */

#define MAX_NODES    64
#define MAX_ELEMS    256
#define MAX_PRINTS   32
#define MAX_LINE     512
#define MAX_NAME     64
#define GMIN         1e-12   /* 最小電導，防止奇異矩陣 */

/* ─────────────────────── 型別 ─────────────────────── */

typedef enum {
    ELEM_R, ELEM_V, ELEM_I, ELEM_C, ELEM_L
} ElemType;

typedef struct {
    ElemType type;
    char     name[MAX_NAME];
    int      n1, n2;        /* 正負節點 */
    double   value;         /* R/C/L 值，或 V/I 直流值 */
    /* 暫態用 */
    double   state;         /* C: 電壓；L: 電流（上一步） */
    int      vsrc_idx;      /* V/L 在 MNA 電壓源向量的索引 */
} Elem;

typedef struct {
    char name[MAX_NAME];
    int  idx;               /* 節點編號 (1-based，0 = GND) */
} Node;

typedef enum { AN_OP, AN_TRAN, AN_DC } AnalysisType;

typedef struct {
    AnalysisType type;
    /* .tran */
    double tstep, tstop;
    /* .dc */
    char   src[MAX_NAME];
    double dc_start, dc_stop, dc_step;
} Analysis;

typedef struct {
    char var[MAX_NAME];     /* e.g. "V(3)" or "I(R1)" */
} PrintVar;

/* ─────────────────────── 全域狀態 ─────────────────────── */

static Node     nodes[MAX_NODES];
static int      node_cnt = 0;

static Elem     elems[MAX_ELEMS];
static int      elem_cnt = 0;

static Analysis analysis;
static int      has_analysis = 0;

static PrintVar prints[MAX_PRINTS];
static int      print_cnt = 0;

static char     title[MAX_LINE] = "(untitled)";

/* ─────────────────────── 節點查詢 / 新增 ─────────────────────── */

/* 將節點名稱轉為編號；"0"/"gnd"/"GND" → 0 */
static int node_idx(const char *name)
{
    if (strcmp(name,"0")==0 || strcasecmp(name,"gnd")==0) return 0;
    for (int i=0;i<node_cnt;i++)
        if (strcasecmp(nodes[i].name, name)==0) return nodes[i].idx;
    /* 新節點 */
    if (node_cnt >= MAX_NODES-1) {
        fprintf(stderr,"Too many nodes\n"); exit(1);
    }
    nodes[node_cnt].idx = node_cnt+1;
    strncpy(nodes[node_cnt].name, name, MAX_NAME-1);
    return nodes[node_cnt++].idx;
}

/* 根據編號找名稱 */
static const char *node_name(int idx)
{
    if (idx==0) return "0";
    for (int i=0;i<node_cnt;i++)
        if (nodes[i].idx==idx) return nodes[i].name;
    return "?";
}

/* ─────────────────────── 工程記號解析 ─────────────────────── */

static double parse_value(const char *s)
{
    char *end;
    double v = strtod(s, &end);
    if (end==s) return 0.0;
    switch (tolower((unsigned char)*end)) {
        case 'f': v *= 1e-15; break;
        case 'p': v *= 1e-12; break;
        case 'n': v *= 1e-9;  break;
        case 'u': v *= 1e-6;  break;
        case 'm':
            /* 避免與 meg 混淆 */
            if (tolower((unsigned char)end[1])=='e' &&
                tolower((unsigned char)end[2])=='g')
                v *= 1e6;
            else
                v *= 1e-3;
            break;
        case 'k': v *= 1e3;  break;
        case 'g': v *= 1e9;  break;
        case 't': v *= 1e12; break;
    }
    return v;
}

/* ─────────────────────── Netlist 解析 ─────────────────────── */

static char *trim(char *s)
{
    while (isspace((unsigned char)*s)) s++;
    char *e = s+strlen(s)-1;
    while (e>=s && isspace((unsigned char)*e)) *e--='\0';
    return s;
}

static void parse_netlist(FILE *fp)
{
    char line[MAX_LINE];
    int first = 1;

    while (fgets(line, sizeof(line), fp)) {
        /* 去除換行 */
        line[strcspn(line,"\r\n")] = '\0';
        char *p = trim(line);

        /* 空行 / 註解 */
        if (*p=='\0' || *p=='*') continue;

        /* 第一行為標題 */
        if (first) { strncpy(title,p,MAX_LINE-1); first=0; continue; }

        /* 轉小寫備用 */
        char low[MAX_LINE];
        strncpy(low,p,MAX_LINE-1);
        for (char *c=low;*c;c++) *c=tolower((unsigned char)*c);

        /* .end */
        if (strncmp(low,".end",4)==0) break;

        /* .tran tstep tstop */
        if (strncmp(low,".tran",5)==0) {
            char s1[32],s2[32];
            if (sscanf(p+5,"%s %s",s1,s2)==2) {
                analysis.type   = AN_TRAN;
                analysis.tstep  = parse_value(s1);
                analysis.tstop  = parse_value(s2);
                has_analysis    = 1;
            }
            continue;
        }

        /* .op */
        if (strncmp(low,".op",3)==0) {
            analysis.type = AN_OP;
            has_analysis  = 1;
            continue;
        }

        /* .dc src start stop step */
        if (strncmp(low,".dc",3)==0) {
            char src[MAX_NAME],s1[32],s2[32],s3[32];
            if (sscanf(p+3,"%s %s %s %s",src,s1,s2,s3)==4) {
                analysis.type     = AN_DC;
                strncpy(analysis.src,src,MAX_NAME-1);
                analysis.dc_start = parse_value(s1);
                analysis.dc_stop  = parse_value(s2);
                analysis.dc_step  = parse_value(s3);
                has_analysis      = 1;
            }
            continue;
        }

        /* .print  V(node) I(elem) ... */
        if (strncmp(low,".print",6)==0) {
            char *tok = strtok(p+6," \t");
            while (tok && print_cnt<MAX_PRINTS) {
                strncpy(prints[print_cnt++].var, tok, MAX_NAME-1);
                tok = strtok(NULL," \t");
            }
            continue;
        }

        /* 元件行：第一個字元決定型別 */
        char ename[MAX_NAME], en1[MAX_NAME], en2[MAX_NAME], eval[32];
        if (sscanf(p,"%s %s %s %s",ename,en1,en2,eval)!=4) continue;

        Elem *el = &elems[elem_cnt];
        strncpy(el->name,ename,MAX_NAME-1);
        el->n1    = node_idx(en1);
        el->n2    = node_idx(en2);
        el->value = parse_value(eval);
        el->state = 0.0;
        el->vsrc_idx = -1;

        switch (tolower((unsigned char)ename[0])) {
            case 'r': el->type = ELEM_R; break;
            case 'v': el->type = ELEM_V; break;
            case 'i': el->type = ELEM_I; break;
            case 'c': el->type = ELEM_C; break;
            case 'l': el->type = ELEM_L; break;
            default:
                fprintf(stderr,"Unknown element: %s\n",ename);
                continue;
        }
        elem_cnt++;
    }
}

/* ─────────────────────── MNA 矩陣求解 ─────────────────────── */
/*
 * 系統大小 = node_cnt + vsrc_cnt
 * 前 node_cnt 列/行 → 節點電壓
 * 後 vsrc_cnt  列/行 → 電壓源電流
 */

static int    N;          /* 矩陣維度 */
static double A[MAX_NODES*2][MAX_NODES*2];
static double b[MAX_NODES*2];
static double x[MAX_NODES*2];   /* 解向量 */

/* LU 分解 + 前後代換（列主元素） */
static int lu_solve(int n)
{
    int perm[MAX_NODES*2];
    double mat[MAX_NODES*2][MAX_NODES*2];
    double rhs[MAX_NODES*2];

    memcpy(mat,A,sizeof(double)*n*n*1);
    /* 展開複製 */
    for (int i=0;i<n;i++) {
        for (int j=0;j<n;j++) mat[i][j]=A[i][j];
        rhs[i]=b[i];
        perm[i]=i;
    }

    for (int k=0;k<n;k++) {
        /* 找最大主元 */
        int    maxr=k;
        double maxv=fabs(mat[k][k]);
        for (int i=k+1;i<n;i++)
            if (fabs(mat[i][k])>maxv) { maxv=fabs(mat[i][k]); maxr=i; }
        if (maxv < 1e-18) { fprintf(stderr,"Singular matrix\n"); return -1; }
        /* 換列 */
        if (maxr!=k) {
            int tmp=perm[k]; perm[k]=perm[maxr]; perm[maxr]=tmp;
            double t;
            for (int j=0;j<n;j++) { t=mat[k][j]; mat[k][j]=mat[maxr][j]; mat[maxr][j]=t; }
            t=rhs[k]; rhs[k]=rhs[maxr]; rhs[maxr]=t;
        }
        /* 消去 */
        for (int i=k+1;i<n;i++) {
            double f=mat[i][k]/mat[k][k];
            for (int j=k;j<n;j++) mat[i][j]-=f*mat[k][j];
            rhs[i]-=f*rhs[k];
        }
    }
    /* 後代換 */
    for (int i=n-1;i>=0;i--) {
        double s=rhs[i];
        for (int j=i+1;j<n;j++) s-=mat[i][j]*x[j];
        x[i]=s/mat[i][i];
    }
    return 0;
}

/* 清零矩陣 */
static void mna_clear(void)
{
    memset(A,0,sizeof(A));
    memset(b,0,sizeof(b));
}

/*
 * 填入元件貢獻
 * mode: 0=OP/DC(電感短路,電容斷路)  1=暫態(梯形法)
 * dt  : 時間步 (mode==1 時有效)
 */
static void mna_stamp(int mode, double dt)
{
    /* GMIN 注入每個節點 */
    for (int i=1;i<=node_cnt;i++)
        A[i-1][i-1] += GMIN;

    int vsrc_row = node_cnt;   /* 電壓源額外列起始 */

    for (int k=0;k<elem_cnt;k++) {
        Elem *el = &elems[k];
        int a = el->n1-1;  /* matrix row/col (0-based, -1 for GND) */
        int c = el->n2-1;

        switch (el->type) {

        case ELEM_R: {
            double g = 1.0/el->value;
            if (a>=0) A[a][a]+=g;
            if (c>=0) A[c][c]+=g;
            if (a>=0&&c>=0) { A[a][c]-=g; A[c][a]-=g; }
            break;
        }

        case ELEM_V: {
            int vr = vsrc_row++;
            el->vsrc_idx = vr;
            /* KVL: Vn1 - Vn2 = V */
            if (a>=0) { A[vr][a]+=1; A[a][vr]+=1; }
            if (c>=0) { A[vr][c]-=1; A[c][vr]-=1; }
            b[vr] += el->value;
            break;
        }

        case ELEM_I: {
            /* SPICE 標準：I(n1,n2,val) 代表電流從 n1→n2（外部方向）
             * 亦即電流「流出」n1（KCL: n1 - val），「流入」n2（KCL: n2 + val） */
            if (a>=0) b[a] -= el->value;
            if (c>=0) b[c] += el->value;
            break;
        }

        case ELEM_C: {
            if (mode==0) {
                /* OP: 電容開路 → 不加任何電導 */
            } else {
                /* 梯形法等效：G_eq = 2C/dt，I_eq = G_eq*Vc(t-) + Ic(t-) */
                /* 簡化：向後歐拉  G_eq=C/dt，I_eq=C/dt*Vc(t-) */
                double geq = el->value / dt;
                double ieq = geq * el->state;   /* state = 上步電壓 */
                if (a>=0) { A[a][a]+=geq; b[a]+=ieq; }
                if (c>=0) { A[c][c]+=geq; b[c]-=ieq; }
                if (a>=0&&c>=0) { A[a][c]-=geq; A[c][a]-=geq; }
            }
            break;
        }

        case ELEM_L: {
            if (mode==0) {
                /* OP: 電感短路 → 用電壓源(0V) 模擬 */
                int vr = vsrc_row++;
                el->vsrc_idx = vr;
                if (a>=0) { A[vr][a]+=1; A[a][vr]+=1; }
                if (c>=0) { A[vr][c]-=1; A[c][vr]-=1; }
                /* b[vr]=0 → 短路 */
            } else {
                /* 向後歐拉伴隨模型：
                 *   Geq = dt/L  (並聯電導)
                 *   Ieq = IL(t-1)  (並聯電流源，方向 n1→n2，即流出 n1)
                 * KCL: 電流流出 n1 → b[n1] -= Ieq；流入 n2 → b[n2] += Ieq */
                double geq = dt / el->value;
                double ieq = el->state;   /* state = 上步電感電流 */
                if (a>=0) { A[a][a]+=geq; b[a]-=ieq; }
                if (c>=0) { A[c][c]+=geq; b[c]+=ieq; }
                if (a>=0&&c>=0) { A[a][c]-=geq; A[c][a]-=geq; }
            }
            break;
        }

        }
    }
}

/* 解完後更新電容電壓/電感電流狀態 */
static void update_state(double dt)
{
    for (int k=0;k<elem_cnt;k++) {
        Elem *el = &elems[k];
        int a = el->n1-1;
        int c = el->n2-1;
        double va = (a>=0)?x[a]:0.0;
        double vc = (c>=0)?x[c]:0.0;

        if (el->type==ELEM_C) {
            /* 向後歐拉：Vc_new = 本步節點電壓差 */
            el->state = va - vc;
        }
        if (el->type==ELEM_L) {
            /* 向後歐拉：IL_new = IL_old + VL*(dt/L)
             * 注意：stamp 用的 geq=dt/L，ieq=IL_old
             * 本步 VL = va-vc，所以 IL_new = state + (va-vc)*(dt/L) */
            el->state = el->state + (va - vc) * (dt / el->value);
        }
    }
}

/* ─────────────────────── 輸出解析 ─────────────────────── */

/* 計算 V(node) */
static double get_voltage(const char *nname)
{
    if (strcmp(nname,"0")==0 || strcasecmp(nname,"gnd")==0) return 0.0;
    for (int i=0;i<node_cnt;i++)
        if (strcasecmp(nodes[i].name,nname)==0)
            return x[nodes[i].idx-1];
    return 0.0/0.0; /* NaN = not found */
}

/* 計算 I(Vxxx) 或 I(Lxxx) — 從 MNA 電壓源電流 */
static double get_current(const char *ename)
{
    for (int k=0;k<elem_cnt;k++) {
        if (strcasecmp(elems[k].name,ename)==0) {
            if (elems[k].vsrc_idx>=0)
                return x[elems[k].vsrc_idx];
            /* 電阻電流 */
            if (elems[k].type==ELEM_R) {
                int a=elems[k].n1-1, c=elems[k].n2-1;
                double va=(a>=0)?x[a]:0.0;
                double vc=(c>=0)?x[c]:0.0;
                return (va-vc)/elems[k].value;
            }
            /* 電容電流（暫態） */
            return 0.0;
        }
    }
    return 0.0/0.0;
}

/* 解析並印出一個 print 變數 */
static double eval_print(const char *var)
{
    char tmp[MAX_NAME];
    strncpy(tmp,var,MAX_NAME-1);

    /* V(node) */
    if ((tmp[0]=='V'||tmp[0]=='v') && tmp[1]=='(') {
        char *p=tmp+2;
        p[strlen(p)-1]='\0'; /* 去 ')' */
        return get_voltage(p);
    }
    /* I(elem) */
    if ((tmp[0]=='I'||tmp[0]=='i') && tmp[1]=='(') {
        char *p=tmp+2;
        p[strlen(p)-1]='\0';
        return get_current(p);
    }
    return 0.0/0.0;
}

/* ─────────────────────── 計算矩陣維度 ─────────────────────── */

static void calc_N(int mode)
{
    int vsrc_cnt=0;
    for (int k=0;k<elem_cnt;k++) {
        if (elems[k].type==ELEM_V) vsrc_cnt++;
        if (elems[k].type==ELEM_L && mode==0) vsrc_cnt++;
        elems[k].vsrc_idx=-1;
    }
    N = node_cnt + vsrc_cnt;
    if (N >= MAX_NODES*2) { fprintf(stderr,"Matrix too large\n"); exit(1); }
}

/* ─────────────────────── 印出標頭 ─────────────────────── */

static void print_header(FILE *out)
{
    fprintf(out,"%-14s","time/sweep");
    for (int i=0;i<print_cnt;i++)
        fprintf(out," %14s",prints[i].var);
    fprintf(out,"\n");
    fprintf(out,"%-14s","----------");
    for (int i=0;i<print_cnt;i++)
        fprintf(out," %14s","--------------");
    fprintf(out,"\n");
}

static void print_row(FILE *out, double t)
{
    fprintf(out,"%-14.6g",t);
    for (int i=0;i<print_cnt;i++) {
        double v = eval_print(prints[i].var);
        if (isnan(v)) fprintf(out," %14s","N/A");
        else          fprintf(out," %14.6g",v);
    }
    fprintf(out,"\n");
}

/* ─────────────────────── 分析主程式 ─────────────────────── */

static void run_op(FILE *out)
{
    fprintf(out,"\n=== DC Operating Point ===\n");
    calc_N(0);
    mna_clear();
    mna_stamp(0,0);
    if (lu_solve(N)<0) return;

    /* 若沒有指定 .print，印出所有節點電壓 */
    if (print_cnt==0) {
        for (int i=0;i<node_cnt;i++)
            fprintf(out,"  V(%s) = %14.6g V\n",nodes[i].name,x[i]);
        /* 電壓源電流 */
        for (int k=0;k<elem_cnt;k++)
            if (elems[k].vsrc_idx>=0)
                fprintf(out,"  I(%s) = %14.6g A\n",
                        elems[k].name, x[elems[k].vsrc_idx]);
    } else {
        print_header(out);
        print_row(out,0.0);
    }
}

static void run_tran(FILE *out)
{
    fprintf(out,"\n=== Transient Analysis  step=%g  stop=%g ===\n",
            analysis.tstep, analysis.tstop);

    /* 暫態初始條件：電容電壓=0，電感電流=0（零初始條件）
     * 進階用法可在 netlist 加 IC=值 指定初始條件 */
    for (int k=0;k<elem_cnt;k++) {
        elems[k].state = 0.0;
        elems[k].vsrc_idx = -1;
    }

    print_header(out);

    double dt = analysis.tstep;
    for (double t=0.0; t<=analysis.tstop+dt*0.5; t+=dt) {
        calc_N(1);
        mna_clear();
        mna_stamp(1,dt);
        if (lu_solve(N)<0) break;
        print_row(out,t);
        update_state(dt);
    }
}

static void run_dc(FILE *out)
{
    fprintf(out,"\n=== DC Sweep  src=%s  %.4g to %.4g  step=%.4g ===\n",
            analysis.src,analysis.dc_start,analysis.dc_stop,analysis.dc_step);

    /* 找目標電壓源 */
    Elem *src=NULL;
    for (int k=0;k<elem_cnt;k++)
        if (strcasecmp(elems[k].name,analysis.src)==0) { src=&elems[k]; break; }
    if (!src) { fprintf(stderr,".dc source not found: %s\n",analysis.src); return; }

    double orig = src->value;
    print_header(out);

    for (double v=analysis.dc_start;
         v<=analysis.dc_stop+fabs(analysis.dc_step)*0.5;
         v+=analysis.dc_step) {
        src->value = v;
        calc_N(0);
        mna_clear();
        mna_stamp(0,0);
        if (lu_solve(N)<0) break;
        print_row(out,v);
    }
    src->value = orig;
}

/* ─────────────────────── main ─────────────────────── */

int main(int argc, char *argv[])
{
    if (argc<2) {
        fprintf(stderr,"Usage: %s <netlist.sp>\n",argv[0]);
        return 1;
    }
    FILE *fp = fopen(argv[1],"r");
    if (!fp) { perror(argv[1]); return 1; }
    parse_netlist(fp);
    fclose(fp);

    fprintf(stdout,"SPICE0 Simulator — %s\n",title);
    fprintf(stdout,"Nodes: %d   Elements: %d\n",node_cnt,elem_cnt);

    if (!has_analysis) {
        /* 預設 .op */
        analysis.type = AN_OP;
        has_analysis  = 1;
    }

    switch (analysis.type) {
        case AN_OP:   run_op  (stdout); break;
        case AN_TRAN: run_tran(stdout); break;
        case AN_DC:   run_dc  (stdout); break;
    }

    return 0;
}
