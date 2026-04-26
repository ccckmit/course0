#include "qd0lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ------------------------------------------------------------------ */
/*  內部輔助                                                            */
/* ------------------------------------------------------------------ */

static void qd_panic(const char *msg) {
    fprintf(stderr, "[qd0 runtime error] %s\n", msg);
    exit(1);
}

QdObj *qd_alloc(void) {
    QdObj *o = (QdObj *)calloc(1, sizeof(QdObj));
    if (!o) qd_panic("out of memory");
    return o;
}

/* ------------------------------------------------------------------ */
/*  建構函式                                                            */
/* ------------------------------------------------------------------ */

QdObj *qd_none(void) {
    static QdObj singleton = { .type = QD_NONE };
    return &singleton;
}

QdObj *qd_bool(int b) {
    QdObj *o = qd_alloc(); o->type = QD_BOOL; o->ival = b ? 1 : 0; return o;
}

QdObj *qd_int(int64_t v) {
    QdObj *o = qd_alloc(); o->type = QD_INT; o->ival = v; return o;
}

QdObj *qd_float(double v) {
    QdObj *o = qd_alloc(); o->type = QD_FLOAT; o->fval = v; return o;
}

QdObj *qd_string(const char *s) {
    QdObj *o = qd_alloc(); o->type = QD_STRING; o->sval = strdup(s); return o;
}

QdObj *qd_list_new(void) {
    QdObj *o = qd_alloc(); o->type = QD_LIST;
    o->list.cap = 8; o->list.len = 0;
    o->list.items = (QdObj **)malloc(8 * sizeof(QdObj *)); return o;
}

QdObj *qd_map_new(void) {
    QdObj *o = qd_alloc(); o->type = QD_MAP;
    o->map.cap = 8; o->map.len = 0;
    o->map.keys = (char **)malloc(8 * sizeof(char *));
    o->map.vals = (QdObj **)malloc(8 * sizeof(QdObj *)); return o;
}

QdObj *qd_func(QdObj *(*fn)(QdObj **, int), const char *name) {
    QdObj *o = qd_alloc(); o->type = QD_FUNC;
    o->func.fn = fn; o->func.name = strdup(name); return o;
}

/* ------------------------------------------------------------------ */
/*  真值測試                                                            */
/* ------------------------------------------------------------------ */

int qd_truthy(QdObj *obj) {
    if (!obj || obj->type == QD_NONE) return 0;
    switch (obj->type) {
        case QD_BOOL:   return obj->ival != 0;
        case QD_INT:    return obj->ival != 0;
        case QD_FLOAT:  return obj->fval != 0.0;
        case QD_STRING: return obj->sval && obj->sval[0] != '\0';
        case QD_LIST:   return obj->list.len > 0;
        case QD_MAP:    return obj->map.len > 0;
        case QD_FUNC:   return 1;
        default:        return 0;
    }
}

/* ------------------------------------------------------------------ */
/*  算術                                                                */
/* ------------------------------------------------------------------ */

QdObj *qd_add(QdObj *a, QdObj *b) {
    if (!a || !b) qd_panic("add: null operand");
    /* 字串串接 */
    if (a->type == QD_STRING && b->type == QD_STRING) {
        size_t la = strlen(a->sval), lb = strlen(b->sval);
        char *buf = (char *)malloc(la + lb + 1);
        memcpy(buf, a->sval, la); memcpy(buf+la, b->sval, lb); buf[la+lb] = '\0';
        QdObj *r = qd_string(buf); free(buf); return r;
    }
    if (a->type == QD_INT && b->type == QD_INT) return qd_int(a->ival + b->ival);
    double fa = (a->type==QD_FLOAT)?a->fval:(double)a->ival;
    double fb = (b->type==QD_FLOAT)?b->fval:(double)b->ival;
    return qd_float(fa + fb);
}

QdObj *qd_sub(QdObj *a, QdObj *b) {
    if (a->type==QD_INT && b->type==QD_INT) return qd_int(a->ival - b->ival);
    double fa=(a->type==QD_FLOAT)?a->fval:(double)a->ival;
    double fb=(b->type==QD_FLOAT)?b->fval:(double)b->ival;
    return qd_float(fa - fb);
}

QdObj *qd_mul(QdObj *a, QdObj *b) {
    if (a->type==QD_INT && b->type==QD_INT) return qd_int(a->ival * b->ival);
    double fa=(a->type==QD_FLOAT)?a->fval:(double)a->ival;
    double fb=(b->type==QD_FLOAT)?b->fval:(double)b->ival;
    return qd_float(fa * fb);
}

QdObj *qd_div(QdObj *a, QdObj *b) {
    double fa=(a->type==QD_FLOAT)?a->fval:(double)a->ival;
    double fb=(b->type==QD_FLOAT)?b->fval:(double)b->ival;
    if (fb==0.0) qd_panic("division by zero");
    return qd_float(fa / fb);
}

QdObj *qd_floordiv(QdObj *a, QdObj *b) {
    if (a->type==QD_INT && b->type==QD_INT) {
        if (b->ival==0) qd_panic("floor division by zero");
        int64_t q = a->ival/b->ival;
        if ((a->ival^b->ival)<0 && q*b->ival!=a->ival) q--;
        return qd_int(q);
    }
    double fa=(a->type==QD_FLOAT)?a->fval:(double)a->ival;
    double fb=(b->type==QD_FLOAT)?b->fval:(double)b->ival;
    return qd_float(floor(fa/fb));
}

QdObj *qd_mod(QdObj *a, QdObj *b) {
    if (a->type==QD_INT && b->type==QD_INT) {
        if (b->ival==0) qd_panic("modulo by zero");
        int64_t r = a->ival%b->ival;
        if (r!=0 && (r^b->ival)<0) r+=b->ival;
        return qd_int(r);
    }
    double fa=(a->type==QD_FLOAT)?a->fval:(double)a->ival;
    double fb=(b->type==QD_FLOAT)?b->fval:(double)b->ival;
    return qd_float(fmod(fa,fb));
}

QdObj *qd_pow(QdObj *a, QdObj *b) {
    double fa=(a->type==QD_FLOAT)?a->fval:(double)a->ival;
    double fb=(b->type==QD_FLOAT)?b->fval:(double)b->ival;
    return qd_float(pow(fa,fb));
}

QdObj *qd_neg(QdObj *a) {
    if (a->type==QD_INT)   return qd_int(-a->ival);
    if (a->type==QD_FLOAT) return qd_float(-a->fval);
    qd_panic("neg: unsupported type"); return qd_none();
}

QdObj *qd_not(QdObj *a) { return qd_bool(!qd_truthy(a)); }

QdObj *qd_bitand(QdObj *a, QdObj *b) {
    if (a->type==QD_INT && b->type==QD_INT) return qd_int(a->ival & b->ival);
    if (!qd_truthy(a)) return a;
    return b;
}

QdObj *qd_bitor(QdObj *a, QdObj *b) {
    if (a->type==QD_INT && b->type==QD_INT) return qd_int(a->ival | b->ival);
    if (qd_truthy(a)) return a;
    return b;
}

/* ------------------------------------------------------------------ */
/*  比較                                                                */
/* ------------------------------------------------------------------ */

static int obj_cmp(QdObj *a, QdObj *b) {
    if (a->type==QD_STRING && b->type==QD_STRING) return strcmp(a->sval,b->sval);
    double fa=(a->type==QD_FLOAT)?a->fval:(double)a->ival;
    double fb=(b->type==QD_FLOAT)?b->fval:(double)b->ival;
    return (fa>fb)-(fa<fb);
}

QdObj *qd_cmp_eq(QdObj *a, QdObj *b) {
    if (a->type==QD_STRING && b->type==QD_STRING) return qd_bool(strcmp(a->sval,b->sval)==0);
    return qd_bool(obj_cmp(a,b)==0);
}
QdObj *qd_cmp_ne(QdObj *a, QdObj *b) { return qd_bool(obj_cmp(a,b)!=0); }
QdObj *qd_cmp_lt(QdObj *a, QdObj *b) { return qd_bool(obj_cmp(a,b)< 0); }
QdObj *qd_cmp_le(QdObj *a, QdObj *b) { return qd_bool(obj_cmp(a,b)<=0); }
QdObj *qd_cmp_gt(QdObj *a, QdObj *b) { return qd_bool(obj_cmp(a,b)> 0); }
QdObj *qd_cmp_ge(QdObj *a, QdObj *b) { return qd_bool(obj_cmp(a,b)>=0); }
QdObj *qd_cmp_is(QdObj *a, QdObj *b) { return qd_bool(a==b); }

/* ------------------------------------------------------------------ */
/*  屬性 / 下標                                                        */
/* ------------------------------------------------------------------ */

QdObj *qd_getattr(QdObj *obj, const char *attr) {
    if (obj->type==QD_MAP) {
        for (size_t i=0;i<obj->map.len;i++)
            if (strcmp(obj->map.keys[i],attr)==0) return obj->map.vals[i];
    }
    if (obj->type==QD_LIST && strcmp(attr,"len")==0) return qd_int((int64_t)obj->list.len);
    if (obj->type==QD_STRING && strcmp(attr,"len")==0) return qd_int((int64_t)strlen(obj->sval));
    return qd_none();
}

void qd_setattr(QdObj *obj, const char *attr, QdObj *val) {
    if (obj->type!=QD_MAP) qd_panic("setattr on non-map");
    for (size_t i=0;i<obj->map.len;i++)
        if (strcmp(obj->map.keys[i],attr)==0) { obj->map.vals[i]=val; return; }
    if (obj->map.len==obj->map.cap) {
        obj->map.cap*=2;
        obj->map.keys=(char **)realloc(obj->map.keys,obj->map.cap*sizeof(char*));
        obj->map.vals=(QdObj **)realloc(obj->map.vals,obj->map.cap*sizeof(QdObj*));
    }
    obj->map.keys[obj->map.len]=strdup(attr);
    obj->map.vals[obj->map.len]=val;
    obj->map.len++;
}

QdObj *qd_getitem(QdObj *obj, QdObj *key) {
    if (obj->type==QD_LIST) {
        int64_t idx=key->ival;
        if (idx<0) idx+=(int64_t)obj->list.len;
        if (idx<0||idx>=(int64_t)obj->list.len) qd_panic("list index out of range");
        return obj->list.items[idx];
    }
    if (obj->type==QD_MAP && key->type==QD_STRING) return qd_getattr(obj,key->sval);
    qd_panic("getitem: unsupported"); return qd_none();
}

void qd_setitem(QdObj *obj, QdObj *key, QdObj *val) {
    if (obj->type==QD_LIST) {
        int64_t idx=key->ival;
        if (idx<0) idx+=(int64_t)obj->list.len;
        if (idx<0||idx>=(int64_t)obj->list.len) qd_panic("list index out of range");
        obj->list.items[idx]=val; return;
    }
    if (obj->type==QD_MAP && key->type==QD_STRING) { qd_setattr(obj,key->sval,val); return; }
    qd_panic("setitem: unsupported");
}

/* ------------------------------------------------------------------ */
/*  序列                                                                */
/* ------------------------------------------------------------------ */

void qd_list_append(QdObj *lst, QdObj *item) {
    if (lst->type!=QD_LIST) qd_panic("append on non-list");
    if (lst->list.len==lst->list.cap) {
        lst->list.cap*=2;
        lst->list.items=(QdObj **)realloc(lst->list.items,lst->list.cap*sizeof(QdObj*));
    }
    lst->list.items[lst->list.len++]=item;
}

QdObj *qd_list_get(QdObj *lst, int64_t idx) {
    if (idx<0) idx+=(int64_t)lst->list.len;
    if (idx<0||idx>=(int64_t)lst->list.len) qd_panic("list index out of range");
    return lst->list.items[idx];
}

int64_t qd_list_len(QdObj *lst) { return (int64_t)lst->list.len; }

/* ------------------------------------------------------------------ */
/*  迭代器                                                              */
/* ------------------------------------------------------------------ */

QdIter *qd_iter_new(QdObj *obj) {
    QdIter *it=(QdIter *)calloc(1,sizeof(QdIter));
    it->container=obj; it->pos=0; it->exhausted=0; return it;
}

QdObj *qd_iter_next(QdIter *it) {
    if (it->exhausted) return NULL;
    QdObj *c=it->container;
    if (c->type==QD_LIST) {
        if (it->pos>=(int64_t)c->list.len) { it->exhausted=1; return NULL; }
        return c->list.items[it->pos++];
    }
    if (c->type==QD_STRING) {
        size_t slen=strlen(c->sval);
        if (it->pos>=(int64_t)slen) { it->exhausted=1; return NULL; }
        char buf[2]={c->sval[it->pos++],'\0'};
        return qd_string(buf);
    }
    it->exhausted=1; return NULL;
}

int qd_iter_done(QdIter *it) { return it->exhausted; }

/* ------------------------------------------------------------------ */
/*  呼叫                                                                */
/* ------------------------------------------------------------------ */

QdObj *qd_call(QdObj *callable, QdObj **args, int argc) {
    if (!callable) qd_panic("call: null callable");
    if (callable->type!=QD_FUNC) qd_panic("call: not a function");
    return callable->func.fn(args, argc);
}

/* ------------------------------------------------------------------ */
/*  輸出                                                                */
/* ------------------------------------------------------------------ */

char *qd_to_str(QdObj *obj) {
    char buf[64];
    if (!obj||obj->type==QD_NONE) return strdup("None");
    switch (obj->type) {
        case QD_BOOL:   return strdup(obj->ival?"True":"False");
        case QD_INT:    snprintf(buf,sizeof(buf),"%lld",(long long)obj->ival); return strdup(buf);
        case QD_FLOAT:  snprintf(buf,sizeof(buf),"%g",obj->fval); return strdup(buf);
        case QD_STRING: return strdup(obj->sval);
        case QD_LIST: {
            char *r=strdup("[");
            for (size_t i=0;i<obj->list.len;i++) {
                char *s=qd_to_str(obj->list.items[i]);
                r=(char *)realloc(r,strlen(r)+strlen(s)+4);
                if (i>0) strcat(r,", ");
                strcat(r,s); free(s);
            }
            r=(char *)realloc(r,strlen(r)+2);
            strcat(r,"]"); return r;
        }
        case QD_MAP:  return strdup("{...}");
        case QD_FUNC: snprintf(buf,sizeof(buf),"<func %s>",obj->func.name?obj->func.name:"?"); return strdup(buf);
        default: return strdup("?");
    }
}

void qd_print(QdObj *obj) { char *s=qd_to_str(obj); printf("%s\n",s); free(s); }

/* ------------------------------------------------------------------ */
/*  內建函式                                                            */
/* ------------------------------------------------------------------ */

QdObj *qd_builtin_print(QdObj **args, int argc) {
    for (int i=0;i<argc;i++) { if (i>0) printf(" "); char *s=qd_to_str(args[i]); printf("%s",s); free(s); }
    printf("\n"); return qd_none();
}

QdObj *qd_builtin_len(QdObj **args, int argc) {
    if (argc!=1) qd_panic("len() takes 1 argument");
    QdObj *o=args[0];
    if (o->type==QD_LIST)   return qd_int((int64_t)o->list.len);
    if (o->type==QD_STRING) return qd_int((int64_t)strlen(o->sval));
    if (o->type==QD_MAP)    return qd_int((int64_t)o->map.len);
    qd_panic("len() unsupported type"); return qd_none();
}

QdObj *qd_builtin_range(QdObj **args, int argc) {
    int64_t start=0, stop, step=1;
    if (argc==1)      { stop=args[0]->ival; }
    else if (argc==2) { start=args[0]->ival; stop=args[1]->ival; }
    else if (argc==3) { start=args[0]->ival; stop=args[1]->ival; step=args[2]->ival; }
    else qd_panic("range() bad argc");
    QdObj *lst=qd_list_new();
    for (int64_t i=start; step>0?i<stop:i>stop; i+=step)
        qd_list_append(lst,qd_int(i));
    return lst;
}

QdObj *qd_builtin_str(QdObj **args, int argc) {
    if (argc!=1) qd_panic("str() takes 1 argument");
    char *s=qd_to_str(args[0]); QdObj *r=qd_string(s); free(s); return r;
}

QdObj *qd_builtin_int(QdObj **args, int argc) {
    if (argc!=1) qd_panic("int() takes 1 argument");
    QdObj *o=args[0];
    if (o->type==QD_INT)    return o;
    if (o->type==QD_FLOAT)  return qd_int((int64_t)o->fval);
    if (o->type==QD_STRING) return qd_int(atoll(o->sval));
    if (o->type==QD_BOOL)   return qd_int(o->ival);
    qd_panic("int() unsupported type"); return qd_none();
}

QdObj *qd_builtin_float(QdObj **args, int argc) {
    if (argc!=1) qd_panic("float() takes 1 argument");
    QdObj *o=args[0];
    if (o->type==QD_FLOAT)  return o;
    if (o->type==QD_INT)    return qd_float((double)o->ival);
    if (o->type==QD_STRING) return qd_float(atof(o->sval));
    qd_panic("float() unsupported type"); return qd_none();
}

QdObj *qd_builtin_append(QdObj **args, int argc) {
    if (argc!=2) qd_panic("append() takes 2 arguments");
    qd_list_append(args[0],args[1]); return qd_none();
}

/* ------------------------------------------------------------------ */
/*  全域環境                                                            */
/* ------------------------------------------------------------------ */

#define ENV_CAP 1024
static char  *env_keys[ENV_CAP];
static QdObj *env_vals[ENV_CAP];
static int    env_len=0;

void qd_env_init(void) { env_len=0; qd_builtins_register(); }

void qd_env_set(const char *name, QdObj *val) {
    for (int i=0;i<env_len;i++)
        if (strcmp(env_keys[i],name)==0) { env_vals[i]=val; return; }
    if (env_len>=ENV_CAP) qd_panic("env overflow");
    env_keys[env_len]=strdup(name); env_vals[env_len]=val; env_len++;
}

QdObj *qd_env_get(const char *name) {
    for (int i=0;i<env_len;i++)
        if (strcmp(env_keys[i],name)==0) return env_vals[i];
    fprintf(stderr,"[qd0 runtime error] undefined name: %s\n",name);
    exit(1);
}

void qd_builtins_register(void) {
    qd_env_set("print",  qd_func(qd_builtin_print,  "print"));
    qd_env_set("len",    qd_func(qd_builtin_len,    "len"));
    qd_env_set("range",  qd_func(qd_builtin_range,  "range"));
    qd_env_set("str",    qd_func(qd_builtin_str,    "str"));
    qd_env_set("int",    qd_func(qd_builtin_int,    "int"));
    qd_env_set("float",  qd_func(qd_builtin_float,  "float"));
    qd_env_set("append", qd_func(qd_builtin_append, "append"));
    qd_env_set("None",  qd_none());
    qd_env_set("True",  qd_bool(1));
    qd_env_set("False", qd_bool(0));
}

void qd_gc_collect(void) { /* stub */ }