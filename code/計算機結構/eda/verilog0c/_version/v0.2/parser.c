#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

Node* parse_expression();
Node* parse_statement();

Node* parse_primary() {
    if (accept("(")) {
        Node* expr = parse_expression();
        expect(")");
        return expr;
    }
    if (accept("{")) {
        Node* concat = new_node("Concat", NULL);
        add_child(concat, parse_expression());
        while (accept(",")) {
            add_child(concat, parse_expression());
        }
        expect("}");
        return concat;
    }
    
    Node* term = new_node("Term", token);
    next_token();
    
    // Arrays / Bit slicing: id[ ... ] or id[ ... : ... ]
    if (accept("[")) {
        Node* slice = new_node("Slice", term->val);
        add_child(slice, parse_expression());
        if (accept(":")) {
            add_child(slice, parse_expression());
        }
        expect("]");
        return slice;
    }
    return term;
}

Node* parse_unary() {
    if (strcmp(token, "~") == 0 || strcmp(token, "!") == 0 || strcmp(token, "-") == 0) {
        Node* un = new_node("Unary", token);
        next_token();
        add_child(un, parse_unary());
        return un;
    }
    return parse_primary();
}

int is_binop(char *t) {
    char *ops[] = {"+", "-", "*", "/", "==", "!=", "<", ">", "<=", ">=", "&", "|", "^", "&&", "||", "<<", ">>", NULL};
    for (int i=0; ops[i]; i++) if (strcmp(t, ops[i]) == 0) return 1;
    return 0;
}

Node* parse_binop_rhs(int expr_prec, Node* lhs) {
    while (is_binop(token)) {
        char op[16];
        strcpy(op, token);
        next_token();
        Node* rhs = parse_unary();
        
        Node* bin = new_node("BinOp", op);
        add_child(bin, lhs);
        add_child(bin, rhs);
        lhs = bin;
    }
    return lhs;
}

Node* parse_expression() {
    Node* lhs = parse_unary();
    return parse_binop_rhs(0, lhs);
}

Node* parse_statement() {
    if (accept("begin")) {
        Node* b = new_node("Block", NULL);
        while (!accept("end") && !is_eof()) {
            add_child(b, parse_statement());
        }
        return b;
    }
    if (accept("if")) {
        Node* n = new_node("If", NULL);
        expect("(");
        add_child(n, parse_expression());
        expect(")");
        add_child(n, parse_statement());
        return n;
    }
    if (accept("case")) {
        Node* n = new_node("Case", NULL);
        expect("(");
        add_child(n, parse_expression());
        expect(")");
        while (!accept("endcase") && !is_eof()) {
            Node* ci = new_node("CaseItem", NULL);
            add_child(ci, parse_expression()); 
            expect(":");
            add_child(ci, parse_statement());
            add_child(n, ci);
        }
        return n;
    }
    if (accept("for")) {
        Node *n = new_node("For", NULL);
        expect("(");
        
        // init
        Node *init = new_node("Assign", NULL);
        add_child(init, parse_expression());
        expect("=");
        add_child(init, parse_expression());
        expect(";");
        add_child(n, init);
        
        // cond
        add_child(n, parse_expression());
        expect(";");
        
        // inc
        Node *inc = new_node("Assign", NULL);
        add_child(inc, parse_expression());
        expect("=");
        add_child(inc, parse_expression());
        expect(")");
        
        add_child(n, inc);
        add_child(n, parse_statement());
        return n;
    }
    if (accept("#")) {
        Node* n = new_node("Delay", token);
        next_token();
        if (strcmp(token, ";") != 0) { 
             add_child(n, parse_statement());
        } else {
             expect(";");
        }
        return n;
    }
    if (token[0] == '$') {
        Node *n = new_node("SystemTask", token);
        next_token();
        if (accept("(")) {
            while (!accept(")")) {
                add_child(n, parse_expression());
                accept(",");
            }
        }
        expect(";");
        return n;
    }

    if (accept(";")) {
        return new_node("Empty", NULL);
    }

    // Assign / evaluation statement
    Node* lhs = parse_expression();
    if (accept("=")) {
        Node* n = new_node("Assign", NULL);
        add_child(n, lhs);
        add_child(n, parse_expression());
        expect(";");
        return n;
    } else if (accept(";")) {
        return lhs; // valid function call theoretically, though Verilog uses tasks
    }

    printf("Parser Error: unhandled statement starting with '%s'\n", token);
    exit(1);
}

Node* parse_module_item() {
    if (strcmp(token, "input")==0 || strcmp(token, "output")==0 || 
        strcmp(token, "wire")==0 || strcmp(token, "reg")==0 || strcmp(token, "integer")==0) {
        
        Node* n = new_node("VarDecl", token);
        next_token();
        while (strcmp(token, ";") != 0) {
             add_child(n, new_node("Token", token));
             next_token();
        }
        expect(";");
        return n;
    }
    if (accept("parameter")) {
        Node* n = new_node("ParamDecl", NULL);
        while (strcmp(token, ";") != 0) {
             add_child(n, new_node("Token", token));
             next_token();
        }
        expect(";");
        return n;
    }
    if (accept("assign")) {
        Node* n = new_node("CombAssign", NULL);
        add_child(n, parse_expression());
        expect("=");
        add_child(n, parse_expression());
        expect(";");
        return n;
    }
    if (accept("always")) {
        Node* n = new_node("Always", NULL);
        if (accept("@")) {
            expect("(");
            while(!accept(")")) { add_child(n, new_node("Event", token)); next_token(); }
        }
        add_child(n, parse_statement());
        return n;
    }
    if (accept("initial")) {
        Node* n = new_node("Initial", NULL);
        add_child(n, parse_statement());
        return n;
    }
    
    // Module instantiation
    Node* n = new_node("Inst", token);
    next_token();
    Node* inst_name = new_node("Name", token);
    next_token();
    add_child(n, inst_name);
    if (accept("(")) {
        while(!accept(")")) {
            add_child(n, parse_expression());
            accept(",");
        }
    }
    expect(";");
    return n;
}

Node* parse_verilog() {
    Node* root = new_node("Program", NULL);
    
    while (!is_eof()) {
        if (accept("`define")) {
            Node* d = new_node("Define", token); // macro name
            next_token();
            add_child(d, parse_expression());
            add_child(root, d);
        }
        else if (accept("module")) {
            Node* mod = new_node("Module", token);
            next_token();
            
            // module ports
            if (accept("(")) {
                while(!accept(")")) {
                    add_child(mod, new_node("Port", token));
                    next_token();
                    accept(",");
                }
            }
            expect(";");
            
            while (!accept("endmodule") && !is_eof()) {
                add_child(mod, parse_module_item());
            }
            add_child(root, mod);
        } else {
            printf("Global Parser Error: unhandled token '%s'\n", token);
            exit(1);
        }
    }
    return root;
}
