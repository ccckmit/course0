#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "codegen.h"

int in_testbench_loop = 0;

static char generated_macros[64][64] = {0};
static int gen_count = 0;
static int already_gen(char *name) {
    for (int i = 0; i < gen_count; i++) {
        if (strcmp(generated_macros[i], name) == 0) return 1;
    }
    return 0;
}
static void mark_gen(char *name) {
    if (gen_count < 63) strcpy(generated_macros[gen_count++], name);
}

void codegen_expr(Node* n, FILE* fout) {
    if (!n) return;
    if (strcmp(n->type, "Term") == 0) {
        if (strcmp(n->val, "$stime") == 0) {
            fprintf(fout, "curr_time");
        } else if (n->val[0] == '`') {
            fprintf(fout, "%s", n->val + 1);
        } else {
            fprintf(fout, "%s", n->val);
        }
    } else if (strcmp(n->type, "Macro") == 0) {
        // `N -> N
        fprintf(fout, "%s", n->val + 1);
    } else if (strcmp(n->type, "Slice") == 0) {
        if (n->child && n->child->next) {
            int msb = atoi(n->child->val);
            int lsb = atoi(n->child->next->val);
            int width = msb - lsb + 1;
            fprintf(fout, "((%s >> %d) & 0x%X)", n->val, lsb, (1 << width) - 1);
        } else {
            fprintf(fout, "%s[", n->val);
            codegen_expr(n->child, fout);
            fprintf(fout, "]");
        }
    } else if (strcmp(n->type, "BinOp") == 0) {
        if (strcmp(n->val, ":") == 0) {
            int msb = atoi(n->child->val);
            int lsb = atoi(n->child->next->val);
            int width = msb - lsb + 1;
            fprintf(fout, "((%s >> %d) & 0x%X)", n->child->val, lsb, (1 << width) - 1);
        } else {
            fprintf(fout, "(");
            codegen_expr(n->child, fout);
            fprintf(fout, " %s ", n->val);
            codegen_expr(n->child->next, fout);
            fprintf(fout, ")");
        }
    } else if (strcmp(n->type, "Unary") == 0) {
        fprintf(fout, "(%s", n->val);
        codegen_expr(n->child, fout);
        fprintf(fout, ")");
    } else if (strcmp(n->type, "Concat") == 0) {
        if (n->child && n->child->next) {
            fprintf(fout, "((");
            codegen_expr(n->child, fout);
            fprintf(fout, "<<8) | ");
            codegen_expr(n->child->next, fout);
            fprintf(fout, ")");
        } else {
            codegen_expr(n->child, fout);
        }
    }
}

void codegen_stmt(Node* n, FILE* fout, int indent) {
    if (!n) return;
    for (int i=0; i<indent; i++) fprintf(fout, "    ");
    
    if (strcmp(n->type, "Block") == 0) {
        fprintf(fout, "{\n");
        Node *c = n->child;
        while(c) {
            codegen_stmt(c, fout, indent + 1);
            c = c->next;
        }
        for (int i=0; i<indent; i++) fprintf(fout, "    ");
        fprintf(fout, "}\n");
} else if (strcmp(n->type, "Assign") == 0) {
    Node* lhs = n->child;
    Node* rhs = n->child->next;
    if (strcmp(lhs->type, "Concat") == 0 && lhs->child && lhs->child->next) {
        codegen_expr(lhs->child, fout);
        fprintf(fout, " = (");
        codegen_expr(rhs, fout);
        fprintf(fout, ")>>8;\n");

        for (int i=0; i<indent; i++) fprintf(fout, " ");
        codegen_expr(lhs->child->next, fout);
        fprintf(fout, " = (");
        codegen_expr(rhs, fout);
        fprintf(fout, ")&0xFF;\n");
    } else if (lhs->type && strcmp(lhs->type, "Term") == 0) {
        char *name = lhs->val;
        int is_macro_lvalue = (name[0] == '`');
        if (is_macro_lvalue) name = name + 1;
        if (strcmp(name, "M") == 0) {
            fprintf(fout, "m[C] = ");
            codegen_expr(rhs, fout);
            fprintf(fout, " >> 8;\n");
            for (int i=0; i<indent; i++) fprintf(fout, " ");
            fprintf(fout, "m[C+1] = ");
            codegen_expr(rhs, fout);
            fprintf(fout, " & 0xFF;\n");
        } else if (strcmp(name, "N") == 0) {
            fprintf(fout, "SW = (SW & ~0x8000) | (((");
            codegen_expr(rhs, fout);
            fprintf(fout, ") & 1) << 15);\n");
        } else if (strcmp(name, "Z") == 0) {
            fprintf(fout, "SW = (SW & ~0x4000) | (((");
            codegen_expr(rhs, fout);
            fprintf(fout, ") & 1) << 14);\n");
        } else {
            codegen_expr(lhs, fout);
            fprintf(fout, " = ");
            codegen_expr(rhs, fout);
            fprintf(fout, ";\n");
        }
    } else {
        codegen_expr(lhs, fout);
        fprintf(fout, " = ");
        codegen_expr(rhs, fout);
        fprintf(fout, ";\n");
    }
    } else if (strcmp(n->type, "If") == 0) {
        fprintf(fout, "if (");
        codegen_expr(n->child, fout);
        fprintf(fout, ") ");
        if (strcmp(n->child->next->type, "Block") != 0 && strcmp(n->child->next->type, "Assign") != 0) {
            fprintf(fout, "{\n");
            codegen_stmt(n->child->next, fout, indent + 1);
            for (int i=0; i<indent; i++) fprintf(fout, "    ");
            fprintf(fout, "}\n");
        } else {
            fprintf(fout, "\n");
            codegen_stmt(n->child->next, fout, indent + 1);
        }
    } else if (strcmp(n->type, "Case") == 0) {
        fprintf(fout, "switch (");
        codegen_expr(n->child, fout);
        fprintf(fout, ") {\n");
        Node *ci = n->child->next;
        while(ci) {
            for (int i=0; i<indent+1; i++) fprintf(fout, "    ");
            fprintf(fout, "case ");
            codegen_expr(ci->child, fout);
            fprintf(fout, ":\n");
            codegen_stmt(ci->child->next, fout, indent + 2);
            for (int i=0; i<indent+2; i++) fprintf(fout, "    ");
            fprintf(fout, "break;\n");
            ci = ci->next;
        }
        for (int i=0; i<indent; i++) fprintf(fout, "    ");
        fprintf(fout, "}\n");
    } else if (strcmp(n->type, "SystemTask") == 0) {
        if (strcmp(n->val, "$display") == 0) {
            Node* fmt = n->child;
            if (fmt) {
                // translate format string
                char new_fmt[512] = "";
                int len = strlen(fmt->val);
                int j = 0;
                for (int i=0; i<len; i++) {
                    if (fmt->val[i] == '"' && i == len-1) {
                        new_fmt[j++] = '\\'; new_fmt[j++] = 'n'; new_fmt[j++] = '"';
                    } else if (fmt->val[i] == '%' && fmt->val[i+1] == 'x') {
                        new_fmt[j++] = '%'; new_fmt[j++] = '0'; new_fmt[j++] = '4'; new_fmt[j++] = 'X'; i++;
                    } else if (fmt->val[i] == '%' && fmt->val[i+1] == 'b') {
                        new_fmt[j++] = '%'; new_fmt[j++] = 'd'; i++;
                    } else {
                        new_fmt[j++] = fmt->val[i];
                    }
                }
                new_fmt[j] = '\0';
                fprintf(fout, "printf(%s", new_fmt);
                Node* arg = fmt->next;
                while(arg) {
                    fprintf(fout, ", ");
                    codegen_expr(arg, fout);
                    arg = arg->next;
                }
                fprintf(fout, ");\n");
            }
        } else if (strcmp(n->val, "$readmemh") == 0) {
            fprintf(fout, "{\n      int _idx=0; int h1,h2; char _l[256];\n");
            fprintf(fout, "      FILE *_f=fopen(%s,\"r\");\n", n->child->val);
            fprintf(fout, "      if(_f) while(fgets(_l, 256, _f)){\n");
            fprintf(fout, "        if (_l[0]=='/' || _l[0]=='\\n') continue;\n");
            fprintf(fout, "        if(sscanf(_l,\"%%x %%x\",&h1,&h2)==2){ ");
            codegen_expr(n->child->next, fout);
            fprintf(fout, "[_idx++]=h1; ");
            codegen_expr(n->child->next, fout);
            fprintf(fout, "[_idx++]=h2; }\n");
            fprintf(fout, "        else if(sscanf(_l,\"%%x\",&h1)==1){ ");
            codegen_expr(n->child->next, fout);
            fprintf(fout, "[_idx++]=h1>>8; ");
            codegen_expr(n->child->next, fout);
            fprintf(fout, "[_idx++]=h1&0xff; }\n");
            fprintf(fout, "      }\n      if(_f) fclose(_f);\n    }\n");
} else if (strcmp(n->val, "$finish") == 0) {
        // Don't call exit here - let the testbench loop handle termination
    }
    } else if (strcmp(n->type, "Delay") == 0) {
        if (n->child) {
            // inside a testbench, #10 implies advancing clock and evaluating logic
            fprintf(fout, "curr_time += %s;\n", n->val + 1);
            fprintf(fout, "    eval_all();\n");
            codegen_stmt(n->child, fout, indent);
        } else {
            fprintf(fout, "curr_time += %s;\n", n->val + 1);
            fprintf(fout, "    eval_all();\n");
        }
    } else if (strcmp(n->type, "For") == 0) {
        fprintf(fout, "for (");
        codegen_expr(n->child->child, fout);
        fprintf(fout, "=");
        codegen_expr(n->child->child->next, fout);
        fprintf(fout, "; ");
        codegen_expr(n->child->next, fout);
        fprintf(fout, "; ");
        codegen_expr(n->child->next->next->child, fout);
        fprintf(fout, "=");
        codegen_expr(n->child->next->next->child->next, fout);
        fprintf(fout, ") {\n");
        codegen_stmt(n->child->next->next->next, fout, indent + 1);
        for (int i=0; i<indent; i++) fprintf(fout, "    ");
        fprintf(fout, "}\n");
    }
}

void generate_c_code(Node* root, FILE* fout) {
    gen_count = 0;
    fprintf(fout, "/* Generated by AST Verilog0C transpiler */\n");
    fprintf(fout, "#include <stdio.h>\n#include <stdint.h>\n#include <stdlib.h>\n\n");
    fprintf(fout, "uint8_t clock = 0;\nint curr_time = 0;\n");
    fprintf(fout, "void eval_all();\nvoid eval_always_posedge();\n\n");

Node* def = root->child;
    while(def) {
        if (strcmp(def->type, "Define") == 0) {
            if (def->child && strcmp(def->child->type, "Slice") == 0) {
                char *base = def->child->val;
                Node *idx = def->child->child;
                if (idx && idx->next) {
                    int msb = atoi(idx->val);
                    int lsb = atoi(idx->next->val);
                    int width = msb - lsb + 1;
                    if (def->val && def->val[0] == '`') {
                        fprintf(fout, "#define %s (((%s) >> %d) & 0x%X)\n", def->val + 1, base, lsb, (1 << width) - 1);
                    } else if (def->val) {
                        fprintf(fout, "#define %s (((%s) >> %d) & 0x%X)\n", def->val, base, lsb, (1 << width) - 1);
                    }
                } else {
                    int bit = idx ? atoi(idx->val) : 0;
                    if (def->val && def->val[0] == '`') {
                        fprintf(fout, "#define %s (((%s) >> %d) & 1)\n", def->val + 1, base, bit);
                    } else if (def->val) {
                        fprintf(fout, "#define %s (((%s) >> %d) & 1)\n", def->val, base, bit);
                    }
                }
            } else if (def->child && strcmp(def->child->type, "Concat") == 0) {
                if (def->val && def->val[0] == '`') {
                    fprintf(fout, "#define %s ", def->val + 1);
                } else if (def->val) {
                    fprintf(fout, "#define %s ", def->val);
                }
                codegen_expr(def->child, fout);
                fprintf(fout, "\n");
            } else {
                if (def->val && def->val[0] == '`') {
                    fprintf(fout, "#define %s ", def->val + 1);
                } else if (def->val) {
                    fprintf(fout, "#define %s ", def->val);
                }
                codegen_expr(def->child, fout);
                fprintf(fout, "\n");
            }
        }
        def = def->next;
    }
    fprintf(fout, "\n");

    Node* mod = root->child;
    char assignments_buffer[8192] = "";
    char always_buffer[8192] = "";
    char initial_buffer[8192] = "";

    FILE *f_asn = tmpfile();
    FILE *f_alw = tmpfile();
    FILE *f_ini = tmpfile();

    while(mod) {
        if (strcmp(mod->type, "Module") == 0) {
            Node* item = mod->child;
            while(item) {
                if (strcmp(item->type, "VarDecl") == 0) {
                    char type[64] = "uint8_t";
                    Node* t = item->child;
                    int is_signed = 0, is_16 = 0, is_array = 0;
                    char vname[64] = "";
                    while(t) {
                        if (strcmp(t->val, "signed")==0) is_signed = 1;
                        if (strcmp(t->val, "[")==0) {
                            if (t->next && strcmp(t->next->val, "15")==0) is_16 = 1;
                        }
                        if (strcmp(t->type, "Token") == 0 && t->val[0] != '[' && t->val[0] != ']' && t->val[0] != ':') {
                            if (!is_signed && !is_16 && strcmp(t->val, "m")==0) { } // it's vname
                            if (strcmp(t->val, "signed")!=0 && !isdigit(t->val[0])) strcpy(vname, t->val);
                        }
                        t = t->next;
                    }
                    if (is_16) strcpy(type, is_signed ? "int16_t" : "uint16_t");
                    if (strcmp(item->val, "integer")==0) strcpy(type, "int");
                    
                    if (strcmp(vname, "m") == 0) fprintf(fout, "uint8_t m[33];\n");
                    else if (strlen(vname)>0) fprintf(fout, "%s %s;\n", type, vname);
                }
                else if (strcmp(item->type, "ParamDecl") == 0) {
                    fprintf(fout, "enum { ");
                    Node* t = item->child;
                    while(t) {
                        if(strcmp(t->val, "parameter")!=0 && strcmp(t->val, "[")!=0 && strcmp(t->val, "3")!=0 && strcmp(t->val, ":")!=0 && strcmp(t->val, "0")!=0 && strcmp(t->val, "]")!=0) {
                            if (strchr(t->val, '\'')) { // 4'h0
                                fprintf(fout, "%c", t->val[strlen(t->val)-1]);
                            } else {
                                fprintf(fout, "%s ", t->val);
                            }
                        }
                        t = t->next;
                    }
                    fprintf(fout, "};\n");
                }
                else if (strcmp(item->type, "CombAssign") == 0) {
                    codegen_expr(item->child, f_asn);
                    fprintf(f_asn, " = ");
                    codegen_expr(item->child->next, f_asn);
                    fprintf(f_asn, ";\n");
                }
                else if (strcmp(item->type, "Always") == 0) {
                    if (item->child && strcmp(item->child->type, "Event") == 0) {
                        // posedge clock
                        codegen_stmt(item->child->next->next, f_alw, 1); // skip Event, Event
                    } else if (item->child && item->child->type && strcmp(item->child->type, "Delay") == 0) {
                        // always #10 clock = ~clock
                        in_testbench_loop = 1;
                    }
                }
                else if (strcmp(item->type, "Initial") == 0) {
                    codegen_stmt(item->child, f_ini, 1);
                }
                item = item->next;
            }
        }
        mod = mod->next;
    }

    fprintf(fout, "\nvoid eval_all() {\n");
    rewind(f_asn);
    char buf[1024];
    while(fgets(buf, 1024, f_asn)) fprintf(fout, "    %s", buf);
    fprintf(fout, "}\n\n");

    fprintf(fout, "void eval_always_posedge() {\n");
    rewind(f_alw);
    while(fgets(buf, 1024, f_alw)) fprintf(fout, "%s", buf);
    fprintf(fout, "}\n\n");

    fprintf(fout, "int main() {\n");
    rewind(f_ini);
    while(fgets(buf, 1024, f_ini)) fprintf(fout, "%s", buf);
    
    if (in_testbench_loop) {
        fprintf(fout, "    while(curr_time <= 2000) {\n");
        fprintf(fout, "        if(curr_time>0 && curr_time %% 10 == 0) {\n");
        fprintf(fout, "            clock = !clock;\n");
        fprintf(fout, "            if (clock) eval_always_posedge();\n");
        fprintf(fout, "        }\n");
        fprintf(fout, "        eval_all();\n");
        fprintf(fout, "        curr_time += 10;\n");
        fprintf(fout, "    }\n");
    } else {
        fprintf(fout, "    eval_all();\n"); // needed for simple testbenches
    }
    fprintf(fout, "    return 0;\n}\n");

    fclose(f_asn); fclose(f_alw); fclose(f_ini);
}
