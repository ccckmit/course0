#ifndef QD0LIB_H
#define QD0LIB_H

#include <stdint.h>
#include <stddef.h>

/* ------------------------------------------------------------------ */
/*  QdObj — 動態物件系統                                               */
/* ------------------------------------------------------------------ */

typedef enum {
    QD_NONE   = 0,
    QD_BOOL   = 1,
    QD_INT    = 2,
    QD_FLOAT  = 3,
    QD_STRING = 4,
    QD_LIST   = 5,
    QD_MAP    = 6,
    QD_FUNC   = 7,
} QdType;

typedef struct QdObj QdObj;

struct QdObj {
    QdType type;
    union {
        int64_t  ival;   /* BOOL, INT */
        double   fval;   /* FLOAT */
        char    *sval;   /* STRING (heap-allocated, NUL-terminated) */
        struct {
            QdObj **items;
            size_t  len;
            size_t  cap;
        } list;          /* LIST */
        struct {
            char   **keys;
            QdObj  **vals;
            size_t   len;
            size_t   cap;
        } map;           /* MAP */
        struct {
            QdObj *(*fn)(QdObj **args, int argc);
            char  *name;
        } func;          /* FUNC */
    };
};

/* ------------------------------------------------------------------ */
/*  建構函式                                                            */
/* ------------------------------------------------------------------ */
QdObj *qd_none(void);
QdObj *qd_bool(int b);
QdObj *qd_int(int64_t v);
QdObj *qd_float(double v);
QdObj *qd_string(const char *s);
QdObj *qd_list_new(void);
QdObj *qd_map_new(void);
QdObj *qd_func(QdObj *(*fn)(QdObj **, int), const char *name);

/* ------------------------------------------------------------------ */
/*  算術與比較                                                          */
/* ------------------------------------------------------------------ */
QdObj *qd_add(QdObj *a, QdObj *b);
QdObj *qd_sub(QdObj *a, QdObj *b);
QdObj *qd_mul(QdObj *a, QdObj *b);
QdObj *qd_div(QdObj *a, QdObj *b);
QdObj *qd_floordiv(QdObj *a, QdObj *b);
QdObj *qd_mod(QdObj *a, QdObj *b);
QdObj *qd_pow(QdObj *a, QdObj *b);
QdObj *qd_neg(QdObj *a);
QdObj *qd_not(QdObj *a);
QdObj *qd_bitand(QdObj *a, QdObj *b);
QdObj *qd_bitor(QdObj *a, QdObj *b);

QdObj *qd_cmp_eq(QdObj *a, QdObj *b);
QdObj *qd_cmp_ne(QdObj *a, QdObj *b);
QdObj *qd_cmp_lt(QdObj *a, QdObj *b);
QdObj *qd_cmp_le(QdObj *a, QdObj *b);
QdObj *qd_cmp_gt(QdObj *a, QdObj *b);
QdObj *qd_cmp_ge(QdObj *a, QdObj *b);
QdObj *qd_cmp_is(QdObj *a, QdObj *b);

/* ------------------------------------------------------------------ */
/*  屬性 / 下標                                                        */
/* ------------------------------------------------------------------ */
QdObj *qd_getattr(QdObj *obj, const char *attr);
void   qd_setattr(QdObj *obj, const char *attr, QdObj *val);
QdObj *qd_getitem(QdObj *obj, QdObj *key);
void   qd_setitem(QdObj *obj, QdObj *key, QdObj *val);

/* ------------------------------------------------------------------ */
/*  序列操作                                                            */
/* ------------------------------------------------------------------ */
void   qd_list_append(QdObj *lst, QdObj *item);
QdObj *qd_list_get(QdObj *lst, int64_t idx);
int64_t qd_list_len(QdObj *lst);

/* ------------------------------------------------------------------ */
/*  迭代器                                                              */
/* ------------------------------------------------------------------ */
typedef struct {
    QdObj  *container;
    int64_t pos;
    int     exhausted;
} QdIter;

QdIter *qd_iter_new(QdObj *obj);
QdObj  *qd_iter_next(QdIter *it);   /* returns NULL when exhausted */
int     qd_iter_done(QdIter *it);

/* ------------------------------------------------------------------ */
/*  呼叫                                                                */
/* ------------------------------------------------------------------ */
QdObj *qd_call(QdObj *callable, QdObj **args, int argc);

/* ------------------------------------------------------------------ */
/*  真值測試                                                            */
/* ------------------------------------------------------------------ */
int qd_truthy(QdObj *obj);

/* ------------------------------------------------------------------ */
/*  輸出                                                                */
/* ------------------------------------------------------------------ */
void qd_print(QdObj *obj);
char *qd_to_str(QdObj *obj);   /* caller frees */

/* ------------------------------------------------------------------ */
/*  內建函式                                                            */
/* ------------------------------------------------------------------ */
void qd_builtins_register(void);
QdObj *qd_builtin_print(QdObj **args, int argc);
QdObj *qd_builtin_len(QdObj **args, int argc);
QdObj *qd_builtin_range(QdObj **args, int argc);
QdObj *qd_builtin_str(QdObj **args, int argc);
QdObj *qd_builtin_int(QdObj **args, int argc);
QdObj *qd_builtin_float(QdObj **args, int argc);
QdObj *qd_builtin_append(QdObj **args, int argc);

/* ------------------------------------------------------------------ */
/*  全域環境（給 qd0c 產生的程式碼使用）                               */
/* ------------------------------------------------------------------ */
void   qd_env_init(void);
void   qd_env_set(const char *name, QdObj *val);
QdObj *qd_env_get(const char *name);

/* ------------------------------------------------------------------ */
/*  記憶體（簡單 arena，不做 GC）                                      */
/* ------------------------------------------------------------------ */
QdObj *qd_alloc(void);
void   qd_gc_collect(void);   /* stub for now */

#endif /* QD0LIB_H */