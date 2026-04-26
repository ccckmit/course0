#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LEN 256
char token[MAX_TOKEN_LEN];
FILE *in_fp, *out_fp;
int t_idx = 0;
int l_idx = 0;

// Track function parameters
int in_function = 0;
char param_name[MAX_TOKEN_LEN] = {0};

void next_token();
void parse_statement();
void parse_expression(char *res_t);
void parse_or(char *res_t);
void parse_and(char *res_t);
void parse_not(char *res_t);
void parse_cmp(char *res_t);

// --- 工具與 IR 輸出函式 ---
void get_t(char* buf, size_t size) { snprintf(buf, size, "t%d", t_idx++); }
void get_l(char* buf, size_t size) { snprintf(buf, size, "L_%d", l_idx++); }

// 依照 DynIR 規格輸出純文字對齊的四元組格式： OP ARG1 ARG2 RESULT
void emit(const char* op, const char* a, const char* b, const char* r) {
    fprintf(out_fp, "%-15s %-10s %-4s %s\n", op, a, b, r);
}

// 檢查並消耗指定的 token
int accept(const char* s) {
    if (strcmp(token, s) == 0) { next_token(); return 1; }
    return 0;
}

// 強制消耗指定的 token，否則報錯
void expect(const char* s) {
    if (strcmp(token, s) != 0) {
        fprintf(stderr, "語法錯誤：預期 '%s'，但得到 '%s'\n", s, token);
        exit(1);
    }
    next_token();
}

// --- 增強版詞法分析器 (Lexer) ---
void next_token() {
    int c = fgetc(in_fp);
    while (c != EOF && isspace(c)) c = fgetc(in_fp);
    if (c == EOF) { strcpy(token, "EOF"); return; }

    if (isalpha(c) || c == '_') {
        int i = 0;
        while (isalnum(c) || c == '_') { token[i++] = c; c = fgetc(in_fp); }
        ungetc(c, in_fp); token[i] = '\0';
    } else if (isdigit(c)) {
        int i = 0;
        while (isdigit(c)) { token[i++] = c; c = fgetc(in_fp); }
        ungetc(c, in_fp); token[i] = '\0';
    } else if (c == '"') {
        int i = 0; token[i++] = c;
        while ((c = fgetc(in_fp)) != '"' && c != EOF) token[i++] = c;
        token[i++] = '"'; token[i] = '\0';
    } else if (c == '=') {
        int next_c = fgetc(in_fp);
        if (next_c == '=') { strcpy(token, "=="); } 
        else { ungetc(next_c, in_fp); strcpy(token, "="); }
    } else if (c == '!') {
        int next_c = fgetc(in_fp);
        if (next_c == '=') { strcpy(token, "!="); }
        else { ungetc(next_c, in_fp); strcpy(token, "!"); }
    } else if (c == '<') {
        int next_c = fgetc(in_fp);
        if (next_c == '=') { strcpy(token, "<="); }
        else { ungetc(next_c, in_fp); strcpy(token, "<"); }
    } else if (c == '>') {
        int next_c = fgetc(in_fp);
        if (next_c == '=') { strcpy(token, ">="); }
        else { ungetc(next_c, in_fp); strcpy(token, ">"); }
    } else {
        token[0] = c; token[1] = '\0';
    }
}

// --- 遞迴下降表達式解析器 (Expression Parser) ---

// 處理最基本的單元 (常數、變數、函數呼叫、括號)
void parse_primary(char *res_t) {
    if (isdigit(token[0]) || token[0] == '"') {
        get_t(res_t, 16);
        emit("LOAD_CONST", token, "_", res_t);
        next_token();
    } else if (isalpha(token[0]) || token[0] == '_') {
        char name[MAX_TOKEN_LEN]; strcpy(name, token); next_token();
        
        // Debug output
        // fprintf(stderr, "DEBUG: name='%s', in_function=%d, param_name='%s'\n", 
        //         name, in_function, param_name);
        
        // 如果是函數參數，目前qd0不支持LOAD_FAST參數，故仍使用LOAD_NAME
        // (這樣會有作用域問題，但是目前qd0的實現就是這樣)
        if (in_function && strcmp(name, param_name) == 0) {
            // fprintf(stderr, "DEBUG: Loading param '%s' as LOAD_NAME (qd0 fallback)\n", name);
            get_t(res_t, 16);
            emit("LOAD_NAME", name, "_", res_t);
        } else if (accept("(")) { // 函數呼叫： factorial(n - 1)
            int argc = 0;
            while (strcmp(token, ")") != 0) {
                char arg_t[16]; parse_expression(arg_t);
                char idx_str[16]; snprintf(idx_str, sizeof(idx_str), "%d", argc++);
                emit("ARG_PUSH", arg_t, idx_str, "_");
                if (accept(",")) continue;
            }
            expect(")");
            char func_t[16]; get_t(func_t, 16);
            emit("LOAD_NAME", name, "_", func_t);
            get_t(res_t, 16);
            char argc_str[16]; snprintf(argc_str, sizeof(argc_str), "%d", argc);
            emit("CALL", func_t, argc_str, res_t);
        } else { // 單純變數： n
            get_t(res_t, 16);
            emit("LOAD_NAME", name, "_", res_t);
        }
    } else if (accept("(")) { // 處理括號 (n - 1)
        parse_expression(res_t);
        expect(")");
    }
}

// 乘除法 (*)
void parse_mult(char *res_t) {
    parse_primary(res_t);
    while (strcmp(token, "*") == 0) {
        next_token();
        char rhs_t[16]; parse_primary(rhs_t);
        char new_res[16]; get_t(new_res, 16);
        emit("MUL", res_t, rhs_t, new_res);
        strcpy(res_t, new_res);
    }
}

// 加減法 (+, -)
void parse_add(char *res_t) {
    parse_mult(res_t);
    while (strcmp(token, "+") == 0 || strcmp(token, "-") == 0) {
        char op[16]; strcpy(op, token); next_token();
        char rhs_t[16]; parse_mult(rhs_t);
        char new_res[16]; get_t(new_res, 16);
        if (strcmp(op, "+") == 0) emit("ADD", res_t, rhs_t, new_res);
        else emit("SUB", res_t, rhs_t, new_res);
        strcpy(res_t, new_res);
    }
}

// 比較運算 (==, !=, <, >, <=, >=)
void parse_cmp(char *res_t) {
    parse_add(res_t);
    while (strcmp(token, "==") == 0 || strcmp(token, "!=") == 0 ||
           strcmp(token, "<") == 0 || strcmp(token, ">") == 0 ||
           strcmp(token, "<=") == 0 || strcmp(token, ">=") == 0) {
        char op[16]; strcpy(op, token); next_token();
        char rhs_t[16]; parse_add(rhs_t);
        char new_res[16]; get_t(new_res, 16);
        if (strcmp(op, "==") == 0) emit("CMP_EQ", res_t, rhs_t, new_res);
        else if (strcmp(op, "!=") == 0) emit("CMP_NE", res_t, rhs_t, new_res);
        else if (strcmp(op, "<") == 0) emit("CMP_LT", res_t, rhs_t, new_res);
        else if (strcmp(op, ">") == 0) emit("CMP_GT", res_t, rhs_t, new_res);
        else if (strcmp(op, "<=") == 0) emit("CMP_LE", res_t, rhs_t, new_res);
        else if (strcmp(op, ">=") == 0) emit("CMP_GE", res_t, rhs_t, new_res);
        strcpy(res_t, new_res);
    }
}

// 邏輯 OR 運算 (or) -> 對應 DynIR 的 BITOR
void parse_or(char *res_t) {
    parse_and(res_t);
    while (accept("or")) {
        char rhs_t[16]; parse_and(rhs_t);
        char new_res[16]; get_t(new_res, 16);
        emit("BITOR", res_t, rhs_t, new_res);
        strcpy(res_t, new_res);
    }
}

// 邏輯 AND 運算 (and)
void parse_and(char *res_t) {
    parse_not(res_t);
    while (accept("and")) {
        char rhs_t[16]; parse_not(rhs_t);
        char new_res[16]; get_t(new_res, 16);
        emit("BITAND", res_t, rhs_t, new_res);
        strcpy(res_t, new_res);
    }
}

// 邏輯 NOT 運算 (not)
void parse_not(char *res_t) {
    if (accept("not")) {
        char rhs_t[16]; parse_not(rhs_t);
        char new_res[16]; get_t(new_res, 16);
        emit("NOT", rhs_t, "_", new_res);
        strcpy(res_t, new_res);
    } else {
        parse_cmp(res_t);
    }
}

// 總表達式入口
void parse_expression(char *res_t) {
    parse_or(res_t);
}

    // --- 語句解析器 (Statement Parser) ---
void parse_statement() {
    // Debug at start of each statement
    // static int depth = 0;
    // depth++;
    // static int last_in_function = 0;
    // if (depth > 1 && last_in_function == 1 && in_function == 0) {
    //     fprintf(stderr, "DEBUG: in_function changed 1->0 at depth %d, token='%s'\n", depth, token);
    // }
    // last_in_function = in_function;
    // 
    // fprintf(stderr, "DEBUG parse_statement: depth=%d, in_function=%d, param_name='%s', token='%s'\n", 
    //         depth, in_function, param_name, token);
    
    if (accept("def")) {
        char func_name[64]; strcpy(func_name, token); next_token();
        expect("("); 
        char param[64]; strcpy(param, token); next_token(); // 簡化：只讀取一個參數 n
        expect(")"); expect(":");
        
        // 設置函數參數追蹤
        in_function = 1;
        strcpy(param_name, param);
        
        emit("LABEL", func_name, "_", "_");
        // 進入函數本體解析 - 迴圈解析多個語句
        while (strcmp(token, "EOF") != 0 && strcmp(token, "def") != 0) {
            parse_statement(); 
        }
        
        // 重置函數參數追蹤
        in_function = 0;
        param_name[0] = '\0';
    } 
    else if (accept("if")) {
        char cond_t[16]; parse_expression(cond_t);
        expect(":");
        
        char l_else[16], l_end[16]; 
        get_l(l_else, 16); get_l(l_end, 16);
        emit("BRANCH_IF_FALSE", cond_t, "_", l_else);
        
        // Save param state before recursive call
        int saved_in_function = in_function;
        char saved_param_name[MAX_TOKEN_LEN];
        strcpy(saved_param_name, param_name);
        
        parse_statement(); // if 的內容 (return 1)
        
        // Restore param state
        in_function = saved_in_function;
        strcpy(param_name, saved_param_name);
        // fprintf(stderr, "DEBUG after if-body: in_function=%d, param_name='%s'\n", in_function, param_name);
        
        emit("JUMP", l_end, "_", "_");
        emit("LABEL", l_else, "_", "_");
        
        if (accept("else")) {
            expect(":");
            // Save param state before else-body too
            int saved_in_function2 = in_function;
            char saved_param_name2[MAX_TOKEN_LEN];
            strcpy(saved_param_name2, param_name);
            
            parse_statement(); // else 的內容 (return n * factorial(n - 1))
            
            // Restore param state after else-body
            in_function = saved_in_function2;
            strcpy(param_name, saved_param_name2);
        }
        
        // Restore param state after else
        in_function = saved_in_function;
        strcpy(param_name, saved_param_name);
        // fprintf(stderr, "DEBUG after if-else block: in_function=%d, param_name='%s'\n", in_function, param_name);
        
        emit("LABEL", l_end, "_", "_");
    } 
    else if (accept("while")) {
        char l_start[16], l_end[16];
        get_l(l_start, 16); get_l(l_end, 16);
        emit("LABEL", l_start, "_", "_");
        
        char cond_t[16]; parse_expression(cond_t);
        expect(":");
        
        emit("BRANCH_IF_FALSE", cond_t, "_", l_end);
        
        int saved_in_function = in_function;
        char saved_param_name[MAX_TOKEN_LEN];
        strcpy(saved_param_name, param_name);
        
        parse_statement();
        
        in_function = saved_in_function;
        strcpy(param_name, saved_param_name);
        
        emit("JUMP", l_start, "_", "_");
        emit("LABEL", l_end, "_", "_");
    }
    else if (accept("for")) {
        char var_name[64]; strcpy(var_name, token); next_token();
        expect("in");
        expect("range");
        expect("(");
        char limit_t[16]; parse_expression(limit_t);
        expect(")");
        expect(":");
        
        char l_start[16], l_end[16];
        get_l(l_start, 16); get_l(l_end, 16);
        
        char zero_t[16]; get_t(zero_t, 16);
        emit("LOAD_CONST", "0", "_", zero_t);
        emit("STORE", zero_t, "_", var_name);
        emit("STORE", limit_t, "_", "__range_end");
        emit("LABEL", l_start, "_", "_");
        
        char i_t[16]; get_t(i_t, 16);
        emit("LOAD_NAME", var_name, "_", i_t);
        char cond_t[16]; get_t(cond_t, 16);
        char limit2_t[16]; get_t(limit2_t, 16);
        emit("LOAD_NAME", "__range_end", "_", limit2_t);
        emit("CMP_LT", i_t, limit2_t, cond_t);
        emit("BRANCH_IF_FALSE", cond_t, "_", l_end);
        
        int saved_in_function = in_function;
        char saved_param_name[MAX_TOKEN_LEN];
        strcpy(saved_param_name, param_name);
        
        parse_statement();
        
        in_function = saved_in_function;
        strcpy(param_name, saved_param_name);
        
        char next_i[16]; get_t(next_i, 16);
        char i_t2[16]; get_t(i_t2, 16);
        char one_t[16]; get_t(one_t, 16);
        emit("LOAD_CONST", "1", "_", one_t);
        emit("LOAD_NAME", var_name, "_", i_t2);
        emit("ADD", i_t2, one_t, next_i);
        emit("STORE", next_i, "_", var_name);
        emit("JUMP", l_start, "_", "_");
        emit("LABEL", l_end, "_", "_");
    }
    else if (accept("return")) {
        // Save/restore param state for return expression
        int saved_in_function = in_function;
        char saved_param_name[MAX_TOKEN_LEN];
        strcpy(saved_param_name, param_name);
        
        // fprintf(stderr, "DEBUG return: saved in_function=%d, param_name='%s'\n", saved_in_function, param_name);
        
        char res_t[16]; parse_expression(res_t);
        emit("RETURN", res_t, "_", "_");
        
        in_function = saved_in_function;
        strcpy(param_name, saved_param_name);
        
        // fprintf(stderr, "DEBUG return done: restored in_function=%d, param_name='%s'\n", in_function, param_name);
    }
    else {
        // 賦值 (result = ...) 或是 獨立函數呼叫 (print(...))
        char id[MAX_TOKEN_LEN]; strcpy(id, token); next_token();
        
        if (accept("=")) {
            char expr_t[16]; parse_expression(expr_t);
            emit("STORE", expr_t, "_", id);
        } 
        else if (accept("(")) { // 如 print("...", result)
            int argc = 0;
            while (strcmp(token, ")") != 0) {
                char arg_t[16]; parse_expression(arg_t);
                char idx_str[16]; snprintf(idx_str, sizeof(idx_str), "%d", argc++);
                emit("ARG_PUSH", arg_t, idx_str, "_");
                if (accept(",")) continue;
            }
            expect(")");
            char func_t[16]; get_t(func_t, 16);
            emit("LOAD_NAME", id, "_", func_t);
            char res_t[16]; get_t(res_t, 16);
            char argc_str[16]; snprintf(argc_str, sizeof(argc_str), "%d", argc);
            emit("CALL", func_t, argc_str, res_t);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("用法: ./py0c input.py -o output.qd\n");
        return 1;
    }
    
    in_fp = fopen(argv[1], "r");
    out_fp = fopen(argv[3], "w");
    if (!in_fp || !out_fp) { perror("檔案開啟失敗"); return 1; }

    next_token();
    while (strcmp(token, "EOF") != 0) {
        parse_statement();
    }
    
    fclose(in_fp); fclose(out_fp);
    return 0;
}