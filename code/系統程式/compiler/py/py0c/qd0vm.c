#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_INS 10000
#define MAX_VARS 256
#define MAX_LABELS 1000
#define MAX_CALL_STACK 1024

/* ========================================================================= *
 * 型別與資料結構 (Types & Data Structures)
 * ========================================================================= */

typedef enum {
    VAL_NONE, VAL_INT, VAL_FLOAT, VAL_BOOL, VAL_STR, VAL_LIST, VAL_CLOSURE, VAL_NATIVE
} ValType;

struct Env;
struct Value;

typedef struct Value (*NativeFunc)(int argc, struct Value* args);

typedef struct Value {
    ValType type;
    union {
        long i;
        double f;
        bool b;
        char *s;
        struct {
            struct Value *items;
            int count;
            int capacity;
        } list;
        struct {
            char label[64];
            struct Env *env;
        } closure;
        NativeFunc native;
    } as;
} Value;

// 變數綁定
typedef struct {
    char name[64];
    Value val;
} Binding;

// 環境變數 (Scope)
typedef struct Env {
    struct Env *parent;
    Binding bindings[MAX_VARS];
    int count;
} Env;

// 指令四元組
typedef struct {
    char op[32];
    char arg1[128];
    char arg2[128];
    char res[128];
    int line_no;
} Instruction;

// 呼叫疊
typedef struct {
    int return_pc;
    char return_var[128];
    Env *saved_env;
} CallFrame;

/* ========================================================================= *
 * 全域狀態 (Global State)
 * ========================================================================= */

Instruction *prog;
int prog_size = 0;

char label_names[MAX_LABELS][64];
int label_pcs[MAX_LABELS];
int label_count = 0;

Env *global_env = NULL;

Value arg_buffer[256]; // 用於 ARG_PUSH / ARG_POP

CallFrame *call_stack;
int stack_ptr = 0;

/* ========================================================================= *
 * 基礎工具函數 (Utilities)
 * ========================================================================= */

void die(const char *msg) {
    fprintf(stderr, "Fatal Error: %s\n", msg);
    exit(1);
}

Value make_none() { Value v; v.type = VAL_NONE; return v; }
Value make_int(long i) { Value v; v.type = VAL_INT; v.as.i = i; return v; }
Value make_float(double f) { Value v; v.type = VAL_FLOAT; v.as.f = f; return v; }
Value make_bool(bool b) { Value v; v.type = VAL_BOOL; v.as.b = b; return v; }
Value make_str(const char *s) { Value v; v.type = VAL_STR; v.as.s = strdup(s); return v; }

Env *env_new(Env *parent) {
    Env *e = (Env*)malloc(sizeof(Env));
    e->parent = parent;
    e->count = 0;
    return e;
}

void env_set(Env *e, const char *name, Value val) {
    if (strcmp(name, "_") == 0) return;
    for (int i = 0; i < e->count; i++) {
        if (strcmp(e->bindings[i].name, name) == 0) {
            e->bindings[i].val = val;
            return;
        }
    }
    if (e->count >= MAX_VARS) die("Out of variable slots");
    strcpy(e->bindings[e->count].name, name);
    e->bindings[e->count].val = val;
    e->count++;
}

Value env_get(Env *e, const char *name) {
    for (Env *curr = e; curr != NULL; curr = curr->parent) {
        for (int i = 0; i < curr->count; i++) {
            if (strcmp(curr->bindings[i].name, name) == 0) {
                return curr->bindings[i].val;
            }
        }
    }
    fprintf(stderr, "Runtime Error: Undefined variable '%s'\n", name);
    exit(1);
}

bool is_truthy(Value v) {
    switch (v.type) {
        case VAL_NONE: return false;
        case VAL_BOOL: return v.as.b;
        case VAL_INT: return v.as.i != 0;
        case VAL_FLOAT: return v.as.f != 0.0;
        case VAL_STR: return strlen(v.as.s) > 0;
        case VAL_LIST: return v.as.list.count > 0;
        default: return true;
    }
}

int resolve_label(const char *name) {
    for (int i = 0; i < label_count; i++) {
        if (strcmp(label_names[i], name) == 0) return label_pcs[i];
    }
    fprintf(stderr, "Error: Unknown label '%s'\n", name);
    exit(1);
}

/* ========================================================================= *
 * 內建函數 (Built-ins)
 * ========================================================================= */

void print_value(Value v) {
    if (v.type == VAL_NONE) printf("None");
    else if (v.type == VAL_INT) printf("%ld", v.as.i);
    else if (v.type == VAL_FLOAT) printf("%g", v.as.f);
    else if (v.type == VAL_BOOL) printf("%s", v.as.b ? "True" : "False");
    else if (v.type == VAL_STR) printf("%s", v.as.s);
    else if (v.type == VAL_LIST) printf("<list size=%d>", v.as.list.count);
    else if (v.type == VAL_CLOSURE) printf("<closure %s>", v.as.closure.label);
    else if (v.type == VAL_NATIVE) printf("<native func>");
}

Value native_print(int argc, Value* args) {
    for (int i = 0; i < argc; i++) {
        if (i > 0) printf(" ");
        print_value(args[i]);
    }
    printf("\n");
    return make_none();
}

Value native_len(int argc, Value* args) {
    if (argc != 1) die("len() takes exactly 1 argument");
    if (args[0].type == VAL_STR) return make_int(strlen(args[0].as.s));
    if (args[0].type == VAL_LIST) return make_int(args[0].as.list.count);
    die("Object of type has no len()");
    return make_none();
}

/* ========================================================================= *
 * 解析器 (Parser)
 * ========================================================================= */

void parse_line(char *line, Instruction *ins) {
    strcpy(ins->op, "_"); strcpy(ins->arg1, "_"); strcpy(ins->arg2, "_"); strcpy(ins->res, "_");
    char *ptrs[4] = { ins->op, ins->arg1, ins->arg2, ins->res };
    int p_idx = 0;
    char *c = line;
    
    while (*c && p_idx < 4) {
        while (isspace(*c)) c++;
        if (!*c) break;
        
        int i = 0;
        if (*c == '"') {
            ptrs[p_idx][i++] = *c++;
            while (*c && *c != '"') {
                if (*c == '\\' && *(c+1)) { ptrs[p_idx][i++] = *c++; }
                ptrs[p_idx][i++] = *c++;
            }
            if (*c == '"') ptrs[p_idx][i++] = *c++;
        } else {
            while (*c && !isspace(*c)) {
                ptrs[p_idx][i++] = *c++;
            }
        }
        ptrs[p_idx][i] = '\0';
        p_idx++;
    }
}

void load_program(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) die("Cannot open input file");

    char line[512];
    int line_no = 1;
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '\n' || line[0] == '#' || line[0] == '\0') {
            line_no++;
            continue;
        }

        Instruction ins;
        ins.line_no = line_no;
        parse_line(line, &ins);

        if (strcmp(ins.op, "LABEL") == 0) {
            strcpy(label_names[label_count], ins.arg1);
            label_pcs[label_count] = prog_size;
            label_count++;
        }
        
        prog[prog_size++] = ins;
        line_no++;
    }
    fclose(f);
}

/* ========================================================================= *
 * 執行引擎 (VM Execution)
 * ========================================================================= */

Value eval_const(const char *str) {
    if (strcmp(str, "None") == 0) return make_none();
    if (strcmp(str, "True") == 0) return make_bool(true);
    if (strcmp(str, "False") == 0) return make_bool(false);
    if (str[0] == '"') {
        char buf[256]; int j = 0;
        for (int i = 1; str[i] != '"' && str[i] != '\0'; i++) {
            if (str[i] == '\\' && str[i+1] == 'n') { buf[j++] = '\n'; i++; }
            else { buf[j++] = str[i]; }
        }
        buf[j] = '\0';
        return make_str(buf);
    }
    if (strchr(str, '.')) return make_float(atof(str));
    return make_int(atol(str));
}

void run() {
    global_env = env_new(NULL);
    
    // 註冊內建函數
    Value p_func; p_func.type = VAL_NATIVE; p_func.as.native = native_print;
    env_set(global_env, "print", p_func);
    
    Value l_func; l_func.type = VAL_NATIVE; l_func.as.native = native_len;
    env_set(global_env, "len", l_func);

    int pc = 0;
    Env *env = global_env;

    while (pc < prog_size) {
        Instruction *ins = &prog[pc];
        
        if (strcmp(ins->op, "LOAD_CONST") == 0) {
            env_set(env, ins->res, eval_const(ins->arg1));
        }
        else if (strcmp(ins->op, "LOAD_NAME") == 0) {
            env_set(env, ins->res, env_get(env, ins->arg1));
        }
        else if (strcmp(ins->op, "STORE") == 0) {
            env_set(env, ins->res, env_get(env, ins->arg1));
        }
        else if (strcmp(ins->op, "ADD") == 0) {
            Value a = env_get(env, ins->arg1);
            Value b = env_get(env, ins->arg2);
            if (a.type == VAL_INT && b.type == VAL_INT) env_set(env, ins->res, make_int(a.as.i + b.as.i));
            else if (a.type == VAL_FLOAT || b.type == VAL_FLOAT) {
                double fa = (a.type == VAL_INT) ? a.as.i : a.as.f;
                double fb = (b.type == VAL_INT) ? b.as.i : b.as.f;
                env_set(env, ins->res, make_float(fa + fb));
            }
            else if (a.type == VAL_STR && b.type == VAL_STR) {
                char buf[512];
                snprintf(buf, sizeof(buf), "%s%s", a.as.s, b.as.s);
                env_set(env, ins->res, make_str(buf));
            } else die("Unsupported operand types for ADD");
        }
        else if (strcmp(ins->op, "SUB") == 0) {
            Value a = env_get(env, ins->arg1); Value b = env_get(env, ins->arg2);
            if (a.type == VAL_INT && b.type == VAL_INT) env_set(env, ins->res, make_int(a.as.i - b.as.i));
            else die("Unsupported operand types for SUB");
        }
        else if (strcmp(ins->op, "MUL") == 0) {
            Value a = env_get(env, ins->arg1); Value b = env_get(env, ins->arg2);
            if (a.type == VAL_INT && b.type == VAL_INT) env_set(env, ins->res, make_int(a.as.i * b.as.i));
            else die("Unsupported operand types for MUL");
        }
        // --- 比較運算擴充 ---
        else if (strcmp(ins->op, "CMP_GT") == 0) {
            Value a = env_get(env, ins->arg1); Value b = env_get(env, ins->arg2);
            if (a.type == VAL_INT && b.type == VAL_INT) env_set(env, ins->res, make_bool(a.as.i > b.as.i));
            else die("Type error in CMP_GT");
        }
        else if (strcmp(ins->op, "CMP_LT") == 0) {
            Value a = env_get(env, ins->arg1); Value b = env_get(env, ins->arg2);
            if (a.type == VAL_INT && b.type == VAL_INT) env_set(env, ins->res, make_bool(a.as.i < b.as.i));
            else die("Type error in CMP_LT");
        }
        else if (strcmp(ins->op, "CMP_GE") == 0) {
            Value a = env_get(env, ins->arg1); Value b = env_get(env, ins->arg2);
            if (a.type == VAL_INT && b.type == VAL_INT) env_set(env, ins->res, make_bool(a.as.i >= b.as.i));
            else die("Type error in CMP_GE");
        }
        else if (strcmp(ins->op, "CMP_LE") == 0) {
            Value a = env_get(env, ins->arg1); Value b = env_get(env, ins->arg2);
            if (a.type == VAL_INT && b.type == VAL_INT) env_set(env, ins->res, make_bool(a.as.i <= b.as.i));
            else die("Type error in CMP_LE");
        }
        else if (strcmp(ins->op, "CMP_EQ") == 0) {
            Value a = env_get(env, ins->arg1); Value b = env_get(env, ins->arg2);
            if (a.type == VAL_INT && b.type == VAL_INT) env_set(env, ins->res, make_bool(a.as.i == b.as.i));
            else if (a.type == VAL_STR && b.type == VAL_STR) env_set(env, ins->res, make_bool(strcmp(a.as.s, b.as.s) == 0));
            else env_set(env, ins->res, make_bool(false));
        }
        else if (strcmp(ins->op, "CMP_NE") == 0) {
            Value a = env_get(env, ins->arg1); Value b = env_get(env, ins->arg2);
            if (a.type == VAL_INT && b.type == VAL_INT) env_set(env, ins->res, make_bool(a.as.i != b.as.i));
            else if (a.type == VAL_STR && b.type == VAL_STR) env_set(env, ins->res, make_bool(strcmp(a.as.s, b.as.s) != 0));
            else env_set(env, ins->res, make_bool(true));
        }
        else if (strcmp(ins->op, "NEG") == 0) {
            Value a = env_get(env, ins->arg1);
            if (a.type == VAL_INT) env_set(env, ins->res, make_int(-a.as.i));
            else die("Type error in NEG");
        }
        else if (strcmp(ins->op, "JUMP") == 0) {
            pc = resolve_label(ins->arg1);
            continue;
        }
        else if (strcmp(ins->op, "BRANCH_IF_FALSE") == 0) {
            Value cond = env_get(env, ins->arg1);
            if (!is_truthy(cond)) {
                pc = resolve_label(ins->res);
                continue;
            }
        }
        else if (strcmp(ins->op, "BRANCH_IF_TRUE") == 0) {
            Value cond = env_get(env, ins->arg1);
            if (is_truthy(cond)) {
                pc = resolve_label(ins->res);
                continue;
            }
        }
        else if (strcmp(ins->op, "ARG_PUSH") == 0) {
            int idx = atoi(ins->arg2);
            arg_buffer[idx] = env_get(env, ins->arg1);
        }
        else if (strcmp(ins->op, "ARG_POP") == 0) {
            int idx = atoi(ins->arg2);
            env_set(env, ins->res, arg_buffer[idx]);
        }
        else if (strcmp(ins->op, "MAKE_CLOSURE") == 0) {
            Value v; v.type = VAL_CLOSURE;
            strcpy(v.as.closure.label, ins->arg1);
            v.as.closure.env = env;
            env_set(env, ins->res, v);
        }
        else if (strcmp(ins->op, "CALL") == 0) {
            Value func = env_get(env, ins->arg1);
            int argc = atoi(ins->arg2);

            if (func.type == VAL_NATIVE) {
                Value ret = func.as.native(argc, arg_buffer);
                env_set(env, ins->res, ret);
            } 
            else if (func.type == VAL_CLOSURE) {
                if (stack_ptr >= MAX_CALL_STACK) die("Call stack overflow");
                call_stack[stack_ptr].return_pc = pc + 1;
                strcpy(call_stack[stack_ptr].return_var, ins->res);
                call_stack[stack_ptr].saved_env = env;
                stack_ptr++;

                env = env_new(func.as.closure.env);
                pc = resolve_label(func.as.closure.label);
                continue;
            } else {
                die("Attempt to call non-callable object");
            }
        }
        else if (strcmp(ins->op, "RETURN") == 0) {
            Value ret = env_get(env, ins->arg1);
            if (stack_ptr == 0) break; // 主程式結束
            
            stack_ptr--;
            pc = call_stack[stack_ptr].return_pc;
            env = call_stack[stack_ptr].saved_env;
            env_set(env, call_stack[stack_ptr].return_var, ret);
            continue;
        }
        else if (strcmp(ins->op, "BUILD_LIST") == 0) {
            int count = atoi(ins->arg1);
            Value list_val; list_val.type = VAL_LIST;
            list_val.as.list.count = count;
            list_val.as.list.capacity = count > 0 ? count : 4;
            list_val.as.list.items = (Value*)malloc(sizeof(Value) * list_val.as.list.capacity);
            for (int i = 0; i < count; i++) list_val.as.list.items[i] = arg_buffer[i];
            env_set(env, ins->res, list_val);
        }
        else if (strcmp(ins->op, "LOAD_SUBSCRIPT") == 0) {
            Value obj = env_get(env, ins->arg1);
            Value idx = env_get(env, ins->arg2);
            if (obj.type != VAL_LIST || idx.type != VAL_INT) die("Type error in SUBSCRIPT");
            if (idx.as.i < 0 || idx.as.i >= obj.as.list.count) die("Index out of bounds");
            env_set(env, ins->res, obj.as.list.items[idx.as.i]);
        }
        else if (strcmp(ins->op, "STORE_SUBSCRIPT") == 0) {
            Value obj = env_get(env, ins->arg1);
            Value idx = env_get(env, ins->arg2);
            Value val = env_get(env, ins->res);
            if (obj.type != VAL_LIST || idx.type != VAL_INT) die("Type error in STORE_SUBSCRIPT");
            if (idx.as.i < 0 || idx.as.i >= obj.as.list.count) die("Index out of bounds");
            obj.as.list.items[idx.as.i] = val;
        }
        else if (strcmp(ins->op, "LABEL") != 0) {
            fprintf(stderr, "Unknown opcode: %s at line %d\n", ins->op, ins->line_no);
            exit(1);
        }
        
        pc++;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <program.qd>\n", argv[0]);
        return 1;
    }

    // 動態分配大型陣列以避免 macOS ld 警告
    prog = (Instruction *)malloc(sizeof(Instruction) * MAX_INS);
    call_stack = (CallFrame *)malloc(sizeof(CallFrame) * MAX_CALL_STACK);

    if (!prog || !call_stack) {
        die("Memory allocation failed");
    }

    load_program(argv[1]);
    run();

    free(prog);
    free(call_stack);

    return 0;
}