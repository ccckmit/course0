#include "../include/parser.h"

static void expect(Parser *parser, TokenType type);
static void advance(Parser *parser);
static void skip_newlines(Parser *parser);
static Expr *parse_equality(Parser *parser);
static Expr *parse_relational(Parser *parser);
static Expr *parse_additive(Parser *parser);
static Expr *parse_multiplicative(Parser *parser);
static Expr *parse_unary(Parser *parser);
static Expr *parse_primary(Parser *parser);

static void advance(Parser *parser) {
    parser->current = lexer_next(parser->lexer);
}

static void expect(Parser *parser, TokenType type) {
    if (parser->current->type != type) {
        fprintf(stderr, "Syntax error: expected %d but got %d at line %d\n", 
                type, parser->current->type, parser->current->line);
        exit(1);
    }
}

static void skip_newlines(Parser *parser) {
    while (parser->current->type == TOKEN_NEWLINE || 
           parser->current->type == TOKEN_INDENT ||
           parser->current->type == TOKEN_DEDENT) {
        advance(parser);
    }
}

void parser_init(Parser *parser, Lexer *lexer) {
    parser->lexer = lexer;
    parser->current = lexer_next(lexer);
}

Program *parse_program(Parser *parser) {
    Program *prog = create_program();
    
    while (parser->current->type != TOKEN_EOF) {
        skip_newlines(parser);
        if (parser->current->type == TOKEN_EOF) break;
        
        if (parser->current->type == TOKEN_DEF) {
            advance(parser);
            expect(parser, TOKEN_IDENT);
            char *name = strdup(parser->current->value);
            advance(parser);
            expect(parser, TOKEN_LPAREN);
            advance(parser);
            
            Function *func = create_function(name);
            free(name);
            
            while (parser->current->type == TOKEN_IDENT) {
                add_param(func, parser->current->value);
                advance(parser);
                if (parser->current->type == TOKEN_COMMA) {
                    advance(parser);
                }
            }
            expect(parser, TOKEN_RPAREN);
            advance(parser);
            expect(parser, TOKEN_COLON);
            advance(parser);
            
            func->body = parse_suite(parser);
            add_function(prog, func);
        }
    }
    return prog;
}

Suite *parse_suite(Parser *parser) {
    Suite *suite = create_suite();
    
    while (parser->current->type != TOKEN_DEDENT && 
           parser->current->type != TOKEN_EOF &&
           parser->current->type != TOKEN_DEF) {
        
        skip_newlines(parser);
        if (parser->current->type == TOKEN_DEDENT ||
            parser->current->type == TOKEN_EOF ||
            parser->current->type == TOKEN_DEF) break;
        
        Statement *stmt = NULL;
        
        if (parser->current->type == TOKEN_IDENT) {
            char *name = strdup(parser->current->value);
            advance(parser);
            if (parser->current->type == TOKEN_ASSIGN) {
                advance(parser);
                Expr *value = parse_expr(parser);
                stmt = create_assign_stmt(name, value);
            } else if (parser->current->type == TOKEN_LPAREN) {
                advance(parser);
                Expr **args = NULL;
                int arg_count = 0;
                if (parser->current->type != TOKEN_RPAREN) {
                    args = (Expr **)malloc(sizeof(Expr *));
                    args[arg_count++] = parse_expr(parser);
                    while (parser->current->type == TOKEN_COMMA) {
                        advance(parser);
                        args = (Expr **)realloc(args, sizeof(Expr *) * (arg_count + 1));
                        args[arg_count++] = parse_expr(parser);
                    }
                }
                expect(parser, TOKEN_RPAREN);
                advance(parser);
                stmt = create_expr_stmt(create_call(name, args, arg_count));
            } else {
                stmt = create_expr_stmt(create_identifier(name));
            }
            free(name);
        } else if (parser->current->type == TOKEN_RETURN) {
            advance(parser);
            Expr *value = parse_expr(parser);
            stmt = create_return_stmt(value);
        } else if (parser->current->type == TOKEN_PASS) {
            advance(parser);
            stmt = create_pass_stmt();
        } else if (parser->current->type == TOKEN_IF) {
            advance(parser);
            Expr *cond = parse_expr(parser);
            expect(parser, TOKEN_COLON);
            advance(parser);
            Suite *then_br = parse_suite(parser);
            Suite *else_br = NULL;
            if (parser->current->type == TOKEN_ELSE) {
                advance(parser);
                expect(parser, TOKEN_COLON);
                advance(parser);
                else_br = parse_suite(parser);
            }
            stmt = create_if_stmt(cond, then_br, else_br);
        } else if (parser->current->type == TOKEN_WHILE) {
            advance(parser);
            Expr *cond = parse_expr(parser);
            expect(parser, TOKEN_COLON);
            advance(parser);
            Suite *body = parse_suite(parser);
            stmt = create_while_stmt(cond, body);
        } else if (parser->current->type == TOKEN_NUMBER) {
            Expr *e = parse_expr(parser);
            stmt = create_expr_stmt(e);
        }
        
        if (stmt) {
            add_statement(suite, stmt);
        }
    }
    
    return suite;
}

Expr *parse_expr(Parser *parser) {
    return parse_equality(parser);
}

Expr *parse_equality(Parser *parser) {
    Expr *left = parse_relational(parser);
    
    while (parser->current->type == TOKEN_EQ || parser->current->type == TOKEN_NE) {
        char *op = strdup(parser->current->type == TOKEN_EQ ? "==" : "!=");
        advance(parser);
        Expr *right = parse_relational(parser);
        left = create_binary_op(op, left, right);
    }
    return left;
}

Expr *parse_relational(Parser *parser) {
    Expr *left = parse_additive(parser);
    
    while (parser->current->type == TOKEN_LT || parser->current->type == TOKEN_GT ||
           parser->current->type == TOKEN_LE || parser->current->type == TOKEN_GE) {
        char *op = NULL;
        switch (parser->current->type) {
            case TOKEN_LT: op = "<"; break;
            case TOKEN_GT: op = ">"; break;
            case TOKEN_LE: op = "<="; break;
            case TOKEN_GE: op = ">="; break;
            default: break;
        }
        char *op_str = strdup(op);
        advance(parser);
        Expr *right = parse_additive(parser);
        left = create_binary_op(op_str, left, right);
    }
    return left;
}

Expr *parse_additive(Parser *parser) {
    Expr *left = parse_multiplicative(parser);
    
    while (parser->current->type == TOKEN_PLUS || parser->current->type == TOKEN_MINUS) {
        char *op = strdup(parser->current->type == TOKEN_PLUS ? "+" : "-");
        advance(parser);
        Expr *right = parse_multiplicative(parser);
        left = create_binary_op(op, left, right);
    }
    return left;
}

Expr *parse_multiplicative(Parser *parser) {
    Expr *left = parse_unary(parser);
    
    while (parser->current->type == TOKEN_MUL || parser->current->type == TOKEN_DIV ||
           parser->current->type == TOKEN_MOD) {
        char *op = NULL;
        switch (parser->current->type) {
            case TOKEN_MUL: op = "*"; break;
            case TOKEN_DIV: op = "/"; break;
            case TOKEN_MOD: op = "%"; break;
            default: break;
        }
        char *op_str = strdup(op);
        advance(parser);
        Expr *right = parse_unary(parser);
        left = create_binary_op(op_str, left, right);
    }
    return left;
}

Expr *parse_unary(Parser *parser) {
    if (parser->current->type == TOKEN_PLUS || parser->current->type == TOKEN_MINUS) {
        char *op = strdup(parser->current->type == TOKEN_PLUS ? "+" : "-");
        advance(parser);
        Expr *operand = parse_unary(parser);
        return create_unary_op(op, operand);
    }
    return parse_primary(parser);
}

Expr *parse_primary(Parser *parser) {
    if (parser->current->type == TOKEN_NUMBER) {
        Expr *e = create_number(parser->current->num_value);
        advance(parser);
        return e;
    }
    
    if (parser->current->type == TOKEN_IDENT) {
        char *name = strdup(parser->current->value);
        advance(parser);
        
        if (parser->current->type == TOKEN_LPAREN) {
            advance(parser);
            Expr **args = NULL;
            int arg_count = 0;
            if (parser->current->type != TOKEN_RPAREN) {
                args = (Expr **)malloc(sizeof(Expr *));
                args[arg_count++] = parse_expr(parser);
                while (parser->current->type == TOKEN_COMMA) {
                    advance(parser);
                    args = (Expr **)realloc(args, sizeof(Expr *) * (arg_count + 1));
                    args[arg_count++] = parse_expr(parser);
                }
            }
            expect(parser, TOKEN_RPAREN);
            advance(parser);
            return create_call(name, args, arg_count);
        }
        return create_identifier(name);
    }
    
    if (parser->current->type == TOKEN_LPAREN) {
        advance(parser);
        Expr *e = parse_expr(parser);
        expect(parser, TOKEN_RPAREN);
        advance(parser);
        return e;
    }
    
    fprintf(stderr, "Syntax error: unexpected token %d at line %d\n", 
            parser->current->type, parser->current->line);
    exit(1);
    return NULL;
}
