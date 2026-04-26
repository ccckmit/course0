#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_INSTS 2048
#define MAX_VARS 512
#define MAX_FRAMES 128
#define MAX_BUFFER 1024

// --- 虛擬機資料結構 ---

typedef enum {
    VAL_NULL,
    VAL_INT,
    VAL_FLOAT,
    VAL_STR,
    VAL_BUILTIN,
    VAL_FUNCTION,
    VAL_LIST,
    VAL_DICT,
    VAL_ITER,
    VAL_BOUND_METHOD
} ValueType;

struct Value;
struct ListObj;
struct DictObj;
struct IterObj;

typedef struct Value (*BuiltinFunc)(int argc, struct Value* args);

typedef struct Value {
    ValueType type;
    union {
        int i;
        double f;
        char* s;
        BuiltinFunc builtin;
        int pc;
        struct ListObj* list;
        struct DictObj* dict;
        struct IterObj* iter;
        struct {
            void* obj;
            char method[32];
        } bound;
    } as;
} Value;

typedef struct ListObj {
    Value* items;
    int count;
    int capacity;
} ListObj;

typedef struct DictEntry {
    Value key;
    Value val;
} DictEntry;

typedef struct DictObj {
    DictEntry* entries;
    int count;
    int capacity;
} DictObj;

typedef struct IterObj {
    Value obj;
    int index;
} IterObj;

typedef enum {
    OP_LOAD_NAME, OP_LOAD_CONST, OP_LOAD_ATTR, OP_STORE,
    OP_DICT_INSERT, OP_BUILD_DICT, OP_LIST_APPEND, OP_BUILD_LIST, OP_SUBSCRIPT,
    OP_ARG_PUSH, OP_CALL, OP_RETURN, OP_FUNCTION, OP_FUNCTION_END,
    OP_ENTER_SCOPE, OP_EXIT_SCOPE, OP_PARAM,
    OP_CMP_EQ, OP_CMP_NE, OP_CMP_LT, OP_CMP_LE, OP_CMP_GT, OP_CMP_GE,
    OP_AND, OP_OR, OP_NOT, OP_BRANCH_IF_FALSE, OP_JUMP,
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_NEG,
    OP_GET_ITER, OP_ITER_NEXT, OP_BRANCH_IF_EXHAUST, OP_LABEL, OP_UNKNOWN
} Opcode;

typedef struct {
    Opcode op;
    char op_str[32];
    char arg1[64];
    char arg2[64];
    char res[64];
    int lineno;
} Instruction;

typedef struct {
    char name[64];
    Value val;
} Var;

typedef struct {
    Var vars[MAX_VARS];
    int count;
} Env;

typedef struct {
    int ret_pc;
    int env_base;
    int param_counter;
    int argc;
    Value args[32];  // 穩定保存每個函式呼叫的專屬參數
    char ret_var[64];
} CallFrame;

// Stack buffers
typedef struct { int index; Value val; } IndexedItem;
IndexedItem arg_buffer[MAX_BUFFER];  int arg_sp = 0;
IndexedItem list_buffer[MAX_BUFFER]; int list_sp = 0;
DictEntry dict_buffer[MAX_BUFFER];   int dict_sp = 0;

// 全域狀態
Instruction program[MAX_INSTS];
int num_insts = 0;
Env env_stack[MAX_FRAMES];
int env_sp = 1;
CallFrame call_stack[MAX_FRAMES];
int call_sp = 0;

// --- 輔助函式 ---

bool value_equals(Value a, Value b) {
    if (a.type != b.type) return false;
    if (a.type == VAL_INT) return a.as.i == b.as.i;
    if (a.type == VAL_FLOAT) return a.as.f == b.as.f;
    if (a.type == VAL_STR) return strcmp(a.as.s, b.as.s) == 0;
    return false;
}

Value get_var(const char* name) {
    if (strcmp(name, "_") == 0) return (Value){VAL_NULL, {0}};
    for (int i = env_sp - 1; i >= 0; i--) {
        for (int j = 0; j < env_stack[i].count; j++) {
            if (strcmp(env_stack[i].vars[j].name, name) == 0) {
                return env_stack[i].vars[j].val;
            }
        }
    }
    printf("Runtime Error: Undefined variable '%s'\n", name);
    exit(1);
}

void set_var(const char* name, Value val) {
    if (strcmp(name, "_") == 0) return;
    Env* current_env = &env_stack[env_sp - 1];
    for (int i = 0; i < current_env->count; i++) {
        if (strcmp(current_env->vars[i].name, name) == 0) {
            current_env->vars[i].val = val;
            return;
        }
    }
    strcpy(current_env->vars[current_env->count].name, name);
    current_env->vars[current_env->count].val = val;
    current_env->count++;
}

// 聰明的字串解析：處理跳脫字元 \n
Value parse_const(const char* str) {
    Value v;
    if (str[0] == '\'' || str[0] == '"') {
        v.type = VAL_STR;
        int len = strlen(str);
        char* buf = malloc(len + 1);
        int j = 0;
        for (int i = 1; i < len - 1; i++) {
            if (str[i] == '\\' && i + 1 < len - 1) {
                if (str[i+1] == 'n') { buf[j++] = '\n'; i++; }
                else if (str[i+1] == 't') { buf[j++] = '\t'; i++; }
                else { buf[j++] = str[i+1]; i++; }
            } else {
                buf[j++] = str[i];
            }
        }
        buf[j] = '\0';
        v.as.s = buf;
    } else if (strchr(str, '.')) {
        v.type = VAL_FLOAT;
        v.as.f = atof(str);
    } else {
        v.type = VAL_INT;
        v.as.i = atoi(str);
    }
    return v;
}

// 內建函式
Value builtin_print(int argc, Value* args) {
    for (int i = 0; i < argc; i++) {
        if (args[i].type == VAL_STR) printf("%s", args[i].as.s);
        else if (args[i].type == VAL_INT) printf("%d", args[i].as.i);
        else if (args[i].type == VAL_FLOAT) printf("%g", args[i].as.f);
    }
    printf("\n");
    return (Value){VAL_NULL, {0}};
}

Value builtin_len(int argc, Value* args) {
    Value r = {VAL_INT, {0}};
    if (args[0].type == VAL_LIST) r.as.i = args[0].as.list->count;
    else if (args[0].type == VAL_DICT) r.as.i = args[0].as.dict->count;
    else if (args[0].type == VAL_STR) r.as.i = strlen(args[0].as.s);
    return r;
}

Value builtin_str(int argc, Value* args) {
    Value r = {VAL_STR, {0}};
    char buf[128];
    if (args[0].type == VAL_INT) sprintf(buf, "%d", args[0].as.i);
    else if (args[0].type == VAL_FLOAT) sprintf(buf, "%g", args[0].as.f);
    else if (args[0].type == VAL_STR) return args[0];
    r.as.s = strdup(buf);
    return r;
}

Value builtin_format(int argc, Value* args) {
    Value val = args[0];
    Value fmt = args[1];
    Value r = {VAL_STR, {0}};
    char buf[128] = {0};
    if (strcmp(fmt.as.s, ".2f") == 0) {
        double f = (val.type == VAL_FLOAT) ? val.as.f : (double)val.as.i;
        sprintf(buf, "%.2f", f);
    } else {
        sprintf(buf, "<unsupported fmt>");
    }
    r.as.s = strdup(buf);
    return r;
}

Value builtin_sum(int argc, Value* args) {
    Value lst = args[0];
    double sum = 0;
    for (int i = 0; i < lst.as.list->count; i++) {
        Value el = lst.as.list->items[i];
        if (el.type == VAL_INT) sum += el.as.i;
        else if (el.type == VAL_FLOAT) sum += el.as.f;
    }
    Value r = {VAL_FLOAT, {0}};
    r.as.f = sum;
    return r;
}

void register_builtins() {
    set_var("print", (Value){VAL_BUILTIN, .as.builtin = builtin_print});
    set_var("len", (Value){VAL_BUILTIN, .as.builtin = builtin_len});
    set_var("str", (Value){VAL_BUILTIN, .as.builtin = builtin_str});
    set_var("format", (Value){VAL_BUILTIN, .as.builtin = builtin_format});
    set_var("sum", (Value){VAL_BUILTIN, .as.builtin = builtin_sum});
}

// 跳躍標籤尋找
int find_label(const char* label_name) {
    for (int i = 0; i < num_insts; i++) {
        if (program[i].op == OP_LABEL && strcmp(program[i].arg1, label_name) == 0) return i;
    }
    printf("Runtime Error: Label '%s' not found\n", label_name);
    exit(1);
}

// 指令字串對應
Opcode str_to_opcode(const char* str) {
    if (strcmp(str, "LOAD_NAME") == 0) return OP_LOAD_NAME;
    if (strcmp(str, "LOAD_CONST") == 0) return OP_LOAD_CONST;
    if (strcmp(str, "LOAD_ATTR") == 0) return OP_LOAD_ATTR;
    if (strcmp(str, "STORE") == 0) return OP_STORE;
    if (strcmp(str, "DICT_INSERT") == 0) return OP_DICT_INSERT;
    if (strcmp(str, "BUILD_DICT") == 0) return OP_BUILD_DICT;
    if (strcmp(str, "LIST_APPEND") == 0) return OP_LIST_APPEND;
    if (strcmp(str, "BUILD_LIST") == 0) return OP_BUILD_LIST;
    if (strcmp(str, "SUBSCRIPT") == 0) return OP_SUBSCRIPT;
    if (strcmp(str, "ARG_PUSH") == 0) return OP_ARG_PUSH;
    if (strcmp(str, "CALL") == 0) return OP_CALL;
    if (strcmp(str, "RETURN") == 0) return OP_RETURN;
    if (strcmp(str, "FUNCTION") == 0) return OP_FUNCTION;
    if (strcmp(str, "FUNCTION_END") == 0) return OP_FUNCTION_END;
    if (strcmp(str, "ENTER_SCOPE") == 0) return OP_ENTER_SCOPE;
    if (strcmp(str, "EXIT_SCOPE") == 0) return OP_EXIT_SCOPE;
    if (strcmp(str, "PARAM") == 0) return OP_PARAM;
    if (strcmp(str, "CMP_EQ") == 0) return OP_CMP_EQ;
    if (strcmp(str, "OR") == 0) return OP_OR;
    if (strcmp(str, "AND") == 0) return OP_AND;
    if (strcmp(str, "NOT") == 0) return OP_NOT;
    if (strcmp(str, "BRANCH_IF_FALSE") == 0) return OP_BRANCH_IF_FALSE;
    if (strcmp(str, "JUMP") == 0) return OP_JUMP;
    if (strcmp(str, "ADD") == 0) return OP_ADD;
    if (strcmp(str, "SUB") == 0) return OP_SUB;
    if (strcmp(str, "MUL") == 0) return OP_MUL;
    if (strcmp(str, "DIV") == 0) return OP_DIV;
    if (strcmp(str, "GET_ITER") == 0) return OP_GET_ITER;
    if (strcmp(str, "ITER_NEXT") == 0) return OP_ITER_NEXT;
    if (strcmp(str, "BRANCH_IF_EXHAUST") == 0) return OP_BRANCH_IF_EXHAUST;
    if (strcmp(str, "LABEL") == 0) return OP_LABEL;
    return OP_UNKNOWN;
}

// 智慧讀取 Tokens (支援空白與引號)
void load_program(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) exit(1);
    char line[512];
    int lineno = 1;
    while (fgets(line, sizeof(line), fp)) {
        char* comment = strchr(line, ';');
        if (comment) *comment = '\0';
        char tokens[4][128] = {"_", "_", "_", "_"};
        int count = 0;
        char* p = line;
        while (*p && count < 4) {
            while (*p && isspace(*p)) p++;
            if (!*p) break;
            int i = 0;
            if (*p == '\'' || *p == '"') {
                char quote = *p;
                tokens[count][i++] = *p++;
                while (*p && *p != quote) tokens[count][i++] = *p++;
                if (*p == quote) tokens[count][i++] = *p++;
                tokens[count][i] = '\0'; count++;
            } else {
                while (*p && !isspace(*p)) tokens[count][i++] = *p++;
                tokens[count][i] = '\0'; count++;
            }
        }
        if (count == 0) { lineno++; continue; }
        Instruction inst = { .op = OP_UNKNOWN, .arg1 = "_", .arg2 = "_", .res = "_", .lineno = lineno };
        if (strstr(tokens[0], "::")) {
            tokens[0][strlen(tokens[0]) - 2] = '\0';
            inst.op = OP_LABEL; strcpy(inst.arg1, tokens[0]);
        } else {
            inst.op = str_to_opcode(tokens[0]);
            strcpy(inst.op_str, tokens[0]);
            if (count > 1) strcpy(inst.arg1, tokens[1]);
            if (count > 2) strcpy(inst.arg2, tokens[2]);
            if (count > 3) strcpy(inst.res, tokens[3]);
        }
        program[num_insts++] = inst;
        lineno++;
    }
    fclose(fp);
}

// --- 執行核心 ---

// 排序輔助
int cmp_indexed(const void* a, const void* b) {
    return ((IndexedItem*)a)->index - ((IndexedItem*)b)->index;
}

void run() {
    register_builtins();
    int pc = 0;
    while (pc < num_insts) {
        Instruction* inst = &program[pc];
        #define V(arg) get_var(arg)
        #define LIT(arg) parse_const(arg)

        switch (inst->op) {
            case OP_LOAD_NAME: set_var(inst->res, V(inst->arg1)); break;
            case OP_LOAD_CONST: set_var(inst->res, LIT(inst->arg1)); break;
            case OP_STORE: set_var(inst->res, V(inst->arg1)); break;

            case OP_LOAD_ATTR: {
                Value obj = V(inst->arg1);
                if (obj.type == VAL_LIST && strcmp(inst->arg2, "append") == 0) {
                    Value r = {VAL_BOUND_METHOD, {0}};
                    r.as.bound.obj = obj.as.list;
                    strcpy(r.as.bound.method, "append");
                    set_var(inst->res, r);
                }
                break;
            }

            case OP_DICT_INSERT: {
                dict_buffer[dict_sp].key = V(inst->arg1);
                dict_buffer[dict_sp].val = V(inst->arg2);
                dict_sp++;
                break;
            }

            case OP_BUILD_DICT: {
                int count = LIT(inst->arg1).as.i;
                DictObj* d = malloc(sizeof(DictObj));
                d->count = count; d->capacity = count;
                d->entries = malloc(sizeof(DictEntry) * count);
                for (int i = 0; i < count; i++) d->entries[i] = dict_buffer[dict_sp - count + i];
                dict_sp -= count;
                Value r = {VAL_DICT, .as.dict = d};
                set_var(inst->res, r);
                break;
            }

            case OP_LIST_APPEND: {
                list_buffer[list_sp].val = V(inst->arg1);
                list_buffer[list_sp].index = LIT(inst->arg2).as.i;
                list_sp++;
                break;
            }

            case OP_BUILD_LIST: {
                int count = LIT(inst->arg1).as.i;
                IndexedItem* start = &list_buffer[list_sp - count];
                qsort(start, count, sizeof(IndexedItem), cmp_indexed);
                ListObj* l = malloc(sizeof(ListObj));
                l->count = count; l->capacity = count + 10;
                l->items = malloc(sizeof(Value) * l->capacity);
                for (int i = 0; i < count; i++) l->items[i] = start[i].val;
                list_sp -= count;
                Value r = {VAL_LIST, .as.list = l};
                set_var(inst->res, r);
                break;
            }

            case OP_SUBSCRIPT: {
                Value obj = V(inst->arg1);
                Value key = V(inst->arg2);
                Value r = {VAL_NULL, {0}};
                if (obj.type == VAL_LIST) r = obj.as.list->items[key.as.i];
                else if (obj.type == VAL_DICT) {
                    for (int i = 0; i < obj.as.dict->count; i++) {
                        if (value_equals(obj.as.dict->entries[i].key, key)) {
                            r = obj.as.dict->entries[i].val; break;
                        }
                    }
                }
                set_var(inst->res, r);
                break;
            }

            case OP_ADD: {
                Value v1 = V(inst->arg1), v2 = V(inst->arg2);
                Value r;
                if (v1.type == VAL_STR && v2.type == VAL_STR) {
                    char* buf = malloc(strlen(v1.as.s) + strlen(v2.as.s) + 1);
                    strcpy(buf, v1.as.s); strcat(buf, v2.as.s);
                    r.type = VAL_STR; r.as.s = buf;
                } else {
                    double f1 = (v1.type==VAL_FLOAT)?v1.as.f:v1.as.i;
                    double f2 = (v2.type==VAL_FLOAT)?v2.as.f:v2.as.i;
                    r.type = VAL_FLOAT; r.as.f = f1 + f2;
                }
                set_var(inst->res, r);
                break;
            }

            case OP_SUB: {
                Value v1 = V(inst->arg1), v2 = V(inst->arg2);
                if (v1.type == VAL_FLOAT || v2.type == VAL_FLOAT) {
                    double f1 = (v1.type==VAL_FLOAT)?v1.as.f:v1.as.i;
                    double f2 = (v2.type==VAL_FLOAT)?v2.as.f:v2.as.i;
                    set_var(inst->res, (Value){VAL_FLOAT, .as.f = f1 - f2});
                } else {
                    set_var(inst->res, (Value){VAL_INT, .as.i = v1.as.i - v2.as.i});
                }
                break;
            }

            case OP_MUL: {
                Value v1 = V(inst->arg1), v2 = V(inst->arg2);
                if (v1.type == VAL_FLOAT || v2.type == VAL_FLOAT) {
                    double f1 = (v1.type==VAL_FLOAT)?v1.as.f:v1.as.i;
                    double f2 = (v2.type==VAL_FLOAT)?v2.as.f:v2.as.i;
                    set_var(inst->res, (Value){VAL_FLOAT, .as.f = f1 * f2});
                } else {
                    set_var(inst->res, (Value){VAL_INT, .as.i = v1.as.i * v2.as.i});
                }
                break;
            }

            case OP_DIV: {
                Value v1 = V(inst->arg1), v2 = V(inst->arg2);
                double f1 = (v1.type==VAL_FLOAT)?v1.as.f:v1.as.i;
                double f2 = (v2.type==VAL_FLOAT)?v2.as.f:v2.as.i;
                Value r = {VAL_FLOAT, .as.f = f1 / f2};
                set_var(inst->res, r);
                break;
            }

            case OP_CMP_EQ: {
                Value r = {VAL_INT, .as.i = value_equals(V(inst->arg1), V(inst->arg2))};
                set_var(inst->res, r);
                break;
            }

            case OP_OR: {
                Value v1 = V(inst->arg1), v2 = V(inst->arg2);
                int i1 = (v1.type==VAL_INT)?v1.as.i:0;
                int i2 = (v2.type==VAL_INT)?v2.as.i:0;
                Value r = {VAL_INT, .as.i = i1 || i2};
                set_var(inst->res, r);
                break;
            }

            case OP_AND: {
                Value v1 = V(inst->arg1), v2 = V(inst->arg2);
                int i1 = (v1.type==VAL_INT)?v1.as.i:0;
                int i2 = (v2.type==VAL_INT)?v2.as.i:0;
                Value r = {VAL_INT, .as.i = i1 && i2};
                set_var(inst->res, r);
                break;
            }

            case OP_BRANCH_IF_FALSE: {
                Value cond = V(inst->arg1);
                if (!cond.as.i) pc = find_label(inst->res) - 1;
                break;
            }
            case OP_JUMP: pc = find_label(inst->arg1) - 1; break;

            case OP_GET_ITER: {
                IterObj* it = malloc(sizeof(IterObj));
                it->obj = V(inst->arg1);
                it->index = 0;
                Value r = {VAL_ITER, .as.iter = it};
                set_var(inst->res, r);
                break;
            }
            case OP_ITER_NEXT: {
                IterObj* it = V(inst->arg1).as.iter;
                bool exhausted = true;
                Value r = {VAL_NULL, {0}};
                if (it->obj.type == VAL_LIST && it->index < it->obj.as.list->count) {
                    r = it->obj.as.list->items[it->index++];
                    exhausted = false;
                }
                char ext_name[128];
                sprintf(ext_name, "__exhaust_%s", inst->arg1);
                Value ext_val = {VAL_INT, .as.i = exhausted};
                set_var(ext_name, ext_val);
                if (!exhausted) set_var(inst->res, r);
                break;
            }
            case OP_BRANCH_IF_EXHAUST: {
                char ext_name[128];
                sprintf(ext_name, "__exhaust_%s", inst->arg1);
                if (get_var(ext_name).as.i) pc = find_label(inst->res) - 1;
                break;
            }

            case OP_ARG_PUSH:
                arg_buffer[arg_sp].val = V(inst->arg1);
                arg_buffer[arg_sp].index = LIT(inst->arg2).as.i;
                arg_sp++;
                break;

            case OP_CALL: {
                Value func = V(inst->arg1);
                int argc = LIT(inst->arg2).as.i;
                IndexedItem* start = &arg_buffer[arg_sp - argc];
                qsort(start, argc, sizeof(IndexedItem), cmp_indexed);
                Value args[32];
                for (int i = 0; i < argc; i++) args[i] = start[i].val;
                arg_sp -= argc;

                if (func.type == VAL_BUILTIN) {
                    set_var(inst->res, func.as.builtin(argc, args));
                } else if (func.type == VAL_BOUND_METHOD) {
                    if (strcmp(func.as.bound.method, "append") == 0) {
                        ListObj* lst = (ListObj*)func.as.bound.obj;
                        if (lst->count >= lst->capacity) {
                            lst->capacity += 10;
                            lst->items = realloc(lst->items, sizeof(Value) * lst->capacity);
                        }
                        lst->items[lst->count++] = args[0];
                    }
                } else if (func.type == VAL_FUNCTION) {
                    CallFrame* frame = &call_stack[call_sp++];
                    frame->ret_pc = pc + 1; frame->env_base = env_sp;
                    frame->param_counter = 0; strcpy(frame->ret_var, inst->res);
                    // 保存專屬參數，不依賴外層的 stack buffer
                    frame->argc = argc;
                    for(int i=0; i<argc; i++) frame->args[i] = args[i];
                    pc = func.as.pc - 1;
                }
                break;
            }

            case OP_FUNCTION: {
                Value f = {VAL_FUNCTION, .as.pc = pc + 1};
                set_var(inst->res, f);
                while (program[pc].op != OP_FUNCTION_END) pc++;
                break;
            }
            case OP_PARAM: {
                // 從專屬的 CallFrame 參數中提取
                CallFrame* frame = &call_stack[call_sp - 1];
                set_var(inst->res, frame->args[frame->param_counter++]);
                break;
            }
            case OP_ENTER_SCOPE: env_stack[env_sp++].count = 0; break;
            case OP_EXIT_SCOPE: env_sp--; break;
            case OP_RETURN: {
                Value ret_val = V(inst->arg1);
                CallFrame* frame = &call_stack[--call_sp];
                env_sp = frame->env_base;
                set_var(frame->ret_var, ret_val);
                pc = frame->ret_pc - 1;
                break;
            }
            case OP_LABEL: case OP_FUNCTION_END: break;
            default: break;
        }
        pc++;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    env_stack[0].count = 0;
    load_program(argv[1]);
    run();
    return 0;
}