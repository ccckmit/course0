import sys
import re
import traceback  # 用於顯示完整錯誤堆疊

# ==========================================
# 1. Token 定義
# ==========================================
class Token:
    def __init__(self, type_, value, line=None):
        self.type = type_
        self.value = value
        self.line = line

    def __repr__(self):
        return f"Token({self.type}, {self.value!r}, line={self.line})"

# Token 類型
TT_DEF = 'DEF'
TT_IF = 'IF'
TT_ELSE = 'ELSE'
TT_RETURN = 'RETURN'
TT_PRINT = 'PRINT'
TT_ID = 'ID'
TT_INT = 'INT'
TT_STR = 'STR'
TT_OP = 'OP'
TT_LPAREN = 'LPAREN'
TT_RPAREN = 'RPAREN'
TT_COLON = 'COLON'
TT_NEWLINE = 'NEWLINE'
TT_INDENT = 'INDENT'
TT_DEDENT = 'DEDENT'
TT_EOF = 'EOF'

# ==========================================
# 2. Lexer
# ==========================================
class Lexer:
    def __init__(self, text, debug=False):
        self.text = text
        self.pos = 0
        self.current_char = self.text[self.pos] if self.text else None
        self.line = 1
        self.indent_level = 0
        self.indent_stack = [0]
        self.debug = debug

    def advance(self):
        self.pos += 1
        if self.pos < len(self.text):
            self.current_char = self.text[self.pos]
            if self.current_char == '\n':
                self.line += 1
        else:
            self.current_char = None

    def peek(self, offset=1):
        pos = self.pos + offset
        return self.text[pos] if pos < len(self.text) else None

    def get_next_token(self):
        while self.current_char is not None:
            if self.current_char == '\n':
                self.advance()
                while self.current_char == '\n':
                    self.advance()
                
                if self.current_char is None:
                    break

                indent = 0
                while self.current_char in ' \t':
                    indent += 1
                    self.advance()
                
                if self.current_char == '#':
                    while self.current_char and self.current_char != '\n':
                        self.advance()
                    continue

                if indent > self.indent_stack[-1]:
                    self.indent_stack.append(indent)
                    tok = Token(TT_INDENT, indent, self.line)
                    if self.debug: print(f"[LEX] {tok}")
                    yield tok
                elif indent < self.indent_stack[-1]:
                    while self.indent_stack and indent < self.indent_stack[-1]:
                        self.indent_stack.pop()
                        tok = Token(TT_DEDENT, indent, self.line)
                        if self.debug: print(f"[LEX] {tok}")
                        yield tok
                continue

            if self.current_char == '"':
                tok = self.read_string()
                if self.debug: print(f"[LEX] {tok}")
                return tok

            if self.current_char.isdigit():
                tok = self.read_number()
                if self.debug: print(f"[LEX] {tok}")
                return tok

            if self.current_char.isalpha() or self.current_char == '_':
                tok = self.read_identifier()
                if self.debug: print(f"[LEX] {tok}")
                return tok

            if self.current_char == '(':
                self.advance()
                tok = Token(TT_LPAREN, '(', self.line)
                if self.debug: print(f"[LEX] {tok}")
                return tok
            if self.current_char == ')':
                self.advance()
                tok = Token(TT_RPAREN, ')', self.line)
                if self.debug: print(f"[LEX] {tok}")
                return tok
            if self.current_char == ':':
                self.advance()
                tok = Token(TT_COLON, ':', self.line)
                if self.debug: print(f"[LEX] {tok}")
                return tok
            
            if self.current_char in '=!<>+-*/':
                op = self.current_char
                self.advance()
                if self.current_char == '=' and op in '=!<>':
                    op += '='
                    self.advance()
                tok = Token(TT_OP, op, self.line)
                if self.debug: print(f"[LEX] {tok}")
                return tok

            if self.current_char in ' \t\r':
                self.advance()
                continue
            
            raise SyntaxError(f"Unknown character: {self.current_char} at line {self.line}")

        while len(self.indent_stack) > 1:
            self.indent_stack.pop()
            tok = Token(TT_DEDENT, 0, self.line)
            if self.debug: print(f"[LEX] {tok}")
            yield tok
            
        tok = Token(TT_EOF, None, self.line)
        if self.debug: print(f"[LEX] {tok}")
        yield tok

    def read_number(self):
        result = ''
        while self.current_char is not None and self.current_char.isdigit():
            result += self.current_char
            self.advance()
        return Token(TT_INT, int(result), self.line)

    def read_string(self):
        self.advance()
        result = ''
        while self.current_char is not None and self.current_char != '"':
            result += self.current_char
            self.advance()
        self.advance()
        return Token(TT_STR, result, self.line)

    def read_identifier(self):
        result = ''
        while self.current_char is not None and (self.current_char.isalnum() or self.current_char == '_'):
            result += self.current_char
            self.advance()
        
        keywords = {
            'def': TT_DEF, 'if': TT_IF, 'else': TT_ELSE,
            'return': TT_RETURN, 'or': TT_OP, 'print': TT_PRINT
        }
        type_ = keywords.get(result, TT_ID)
        return Token(type_, result, self.line)

    def tokenize(self):
        return list(self.get_next_token())

# ==========================================
# 3. AST Nodes
# ==========================================
class ASTNode:
    pass

class Program(ASTNode):
    def __init__(self, statements):
        self.statements = statements

class FunctionDef(ASTNode):
    def __init__(self, name, params, body):
        self.name = name
        self.params = params
        self.body = body

class IfStmt(ASTNode):
    def __init__(self, condition, true_block, false_block=None):
        self.condition = condition
        self.true_block = true_block
        self.false_block = false_block

class ReturnStmt(ASTNode):
    def __init__(self, value):
        self.value = value

class Assignment(ASTNode):
    def __init__(self, name, value):
        self.name = name
        self.value = value

class Call(ASTNode):
    def __init__(self, name, args):
        self.name = name
        self.args = args

class BinOp(ASTNode):
    def __init__(self, left, op, right):
        self.left = left
        self.op = op
        self.right = right

class Compare(ASTNode):
    def __init__(self, left, op, right):
        self.left = left
        self.op = op
        self.right = right

class BoolOp(ASTNode):
    def __init__(self, left, op, right):
        self.left = left
        self.op = op
        self.right = right

class Literal(ASTNode):
    def __init__(self, value):
        self.value = value

class Identifier(ASTNode):
    def __init__(self, name):
        self.name = name

# ==========================================
# 4. Parser
# ==========================================
class Parser:
    def __init__(self, tokens, debug=False):
        self.tokens = tokens
        self.pos = 0
        self.current_token = self.tokens[self.pos] if tokens else Token(TT_EOF, None)
        self.debug = debug

    def eat(self, token_type):
        if self.current_token.type == token_type:
            old_token = self.current_token
            self.pos += 1
            self.current_token = self.tokens[self.pos] if self.pos < len(self.tokens) else Token(TT_EOF, None)
            if self.debug: print(f"[PARSE] Eat {token_type}: {old_token}")
        else:
            raise SyntaxError(
                f"Expected {token_type}, got {self.current_token.type} ({self.current_token.value!r}) "
                f"at line {self.current_token.line}"
            )

    def parse(self):
        statements = []
        while self.current_token.type != TT_EOF:
            statements.append(self.parse_statement())
        return Program(statements)

    def parse_statement(self):
        token = self.current_token
        if self.debug: print(f"[PARSE] Statement: {token}")
        
        if token.type == TT_DEF:
            return self.parse_function_def()
        elif token.type == TT_IF:
            return self.parse_if_stmt()
        elif token.type == TT_RETURN:
            return self.parse_return_stmt()
        elif token.type == TT_PRINT:
            return self.parse_call_stmt()
        elif token.type == TT_ID:
            if self.peek().type == TT_OP and self.peek().value == '=':
                return self.parse_assignment()
            else:
                return self.parse_call_stmt()
        else:
            raise SyntaxError(f"Unexpected token {token.type} at line {token.line}")

    def peek(self):
        idx = self.pos + 1
        return self.tokens[idx] if idx < len(self.tokens) else Token(TT_EOF, None)

    def parse_function_def(self):
        self.eat(TT_DEF)
        name = self.current_token.value
        self.eat(TT_ID)
        self.eat(TT_LPAREN)
        params = []
        if self.current_token.type == TT_ID:
            params.append(self.current_token.value)
            self.eat(TT_ID)
        self.eat(TT_RPAREN)
        self.eat(TT_COLON)
        self.eat(TT_NEWLINE)
        self.eat(TT_INDENT)
        body = []
        while self.current_token.type != TT_DEDENT:
            body.append(self.parse_statement())
        self.eat(TT_DEDENT)
        return FunctionDef(name, params, body)

    def parse_if_stmt(self):
        self.eat(TT_IF)
        condition = self.parse_expression()
        self.eat(TT_COLON)
        self.eat(TT_NEWLINE)
        self.eat(TT_INDENT)
        true_block = []
        while self.current_token.type != TT_DEDENT and self.current_token.type != TT_ELSE:
            true_block.append(self.parse_statement())
        self.eat(TT_DEDENT)
        
        false_block = None
        if self.current_token.type == TT_ELSE:
            self.eat(TT_ELSE)
            self.eat(TT_COLON)
            self.eat(TT_NEWLINE)
            self.eat(TT_INDENT)
            false_block = []
            while self.current_token.type != TT_DEDENT:
                false_block.append(self.parse_statement())
            self.eat(TT_DEDENT)
            
        return IfStmt(condition, true_block, false_block)

    def parse_return_stmt(self):
        self.eat(TT_RETURN)
        value = None
        if self.current_token.type not in [TT_NEWLINE, TT_DEDENT, TT_EOF]:
            value = self.parse_expression()
        return ReturnStmt(value)

    def parse_assignment(self):
        name = self.current_token.value
        self.eat(TT_ID)
        self.eat(TT_OP)
        value = self.parse_expression()
        return Assignment(name, value)

    def parse_call_stmt(self):
        node = self.parse_expression()
        return node

    def parse_expression(self):
        return self.parse_logical_or()

    def parse_logical_or(self):
        left = self.parse_comparison()
        while self.current_token.type == TT_OP and self.current_token.value == 'or':
            op = self.current_token.value
            self.eat(TT_OP)
            right = self.parse_comparison()
            left = BoolOp(left, op, right)
        return left

    def parse_comparison(self):
        left = self.parse_additive()
        while self.current_token.type == TT_OP and self.current_token.value in ['==', '!=', '<', '>']:
            op = self.current_token.value
            self.eat(TT_OP)
            right = self.parse_additive()
            left = Compare(left, op, right)
        return left

    def parse_additive(self):
        left = self.parse_multiplicative()
        while self.current_token.type == TT_OP and self.current_token.value in ['+', '-']:
            op = self.current_token.value
            self.eat(TT_OP)
            right = self.parse_multiplicative()
            left = BinOp(left, op, right)
        return left

    def parse_multiplicative(self):
        left = self.parse_primary()
        while self.current_token.type == TT_OP and self.current_token.value in ['*', '/']:
            op = self.current_token.value
            self.eat(TT_OP)
            right = self.parse_primary()
            left = BinOp(left, op, right)
        return left

    def parse_primary(self):
        token = self.current_token
        if self.debug: print(f"[PARSE] Primary: {token}")
        
        if token.type == TT_INT:
            self.eat(TT_INT)
            return Literal(token.value)
        elif token.type == TT_STR:
            self.eat(TT_STR)
            return Literal(token.value)
        elif token.type == TT_ID:
            name = token.value
            self.eat(TT_ID)
            if self.current_token.type == TT_LPAREN:
                self.eat(TT_LPAREN)
                args = []
                if self.current_token.type != TT_RPAREN:
                    args.append(self.parse_expression())
                    while self.current_token.type == TT_OP and self.current_token.value == ',':
                        self.eat(TT_OP)
                        args.append(self.parse_expression())
                self.eat(TT_RPAREN)
                return Call(name, args)
            else:
                return Identifier(name)
        elif token.type == TT_LPAREN:
            self.eat(TT_LPAREN)
            expr = self.parse_expression()
            self.eat(TT_RPAREN)
            return expr
        else:
            raise SyntaxError(f"Unexpected token in expression: {token.type} at line {token.line}")

# ==========================================
# 5. Interpreter
# ==========================================
class ReturnException(Exception):
    def __init__(self, value):
        self.value = value

class Interpreter:
    def __init__(self, debug=False):
        self.global_env = {}
        self.builtins = {'print': self.builtin_print}
        self.debug = debug

    def builtin_print(self, *args):
        output = " ".join(str(arg) for arg in args)
        print(output)

    def interpret(self, node):
        self.visit(node)

    def visit(self, node):
        method_name = f'visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self.generic_visit)
        if self.debug: print(f"[INTERP] Visiting {type(node).__name__}")
        return visitor(node)

    def generic_visit(self, node):
        raise Exception(f"No visitor method for {type(node).__name__}")

    def visit_Program(self, node):
        for stmt in node.statements:
            self.visit(stmt)

    def visit_FunctionDef(self, node):
        self.global_env[node.name] = node
        if self.debug: print(f"[INTERP] Defined function: {node.name}")

    def visit_Assignment(self, node):
        value = self.visit(node.value)
        self.global_env[node.name] = value
        if self.debug: print(f"[INTERP] Assigned {node.name} = {value}")

    def visit_IfStmt(self, node):
        cond = self.visit(node.condition)
        if self.debug: print(f"[INTERP] If condition: {cond}")
        if cond:
            for stmt in node.true_block:
                self.visit(stmt)
        elif node.false_block:
            for stmt in node.false_block:
                self.visit(stmt)

    def visit_ReturnStmt(self, node):
        val = self.visit(node.value) if node.value else None
        if self.debug: print(f"[INTERP] Return: {val}")
        raise ReturnException(val)

    def visit_Call(self, node):
        if node.name in self.builtins:
            args = [self.visit(arg) for arg in node.args]
            if self.debug: print(f"[INTERP] Calling builtin: {node.name}({args})")
            return self.builtins[node.name](*args)
        
        if node.name in self.global_env:
            func_def = self.global_env[node.name]
            args = [self.visit(arg) for arg in node.args]
            if self.debug: print(f"[INTERP] Calling function: {node.name}({args})")
            return self.call_function(func_def, args)
        
        raise NameError(f"Function '{node.name}' is not defined")

    def call_function(self, func_def, args):
        if len(args) != len(func_def.params):
            raise TypeError(f"Function {func_def.name} expects {len(func_def.params)} arguments, got {len(args)}")
        
        local_env = dict(self.global_env)
        for param, arg in zip(func_def.params, args):
            local_env[param] = arg
        
        return self.execute_block(func_def.body, local_env)

    def execute_block(self, statements, env):
        old_global = self.global_env
        self.global_env = env
        
        try:
            for stmt in statements:
                self.visit(stmt)
            return None
        except ReturnException as e:
            return e.value
        finally:
            self.global_env = old_global

    def visit_BinOp(self, node):
        left = self.visit(node.left)
        right = self.visit(node.right)
        if self.debug: print(f"[INTERP] BinOp: {left} {node.op} {right}")
        if node.op == '+': return left + right
        if node.op == '-': return left - right
        if node.op == '*': return left * right
        if node.op == '/': return left / right
        raise ValueError(f"Unknown op {node.op}")

    def visit_Compare(self, node):
        left = self.visit(node.left)
        right = self.visit(node.right)
        if self.debug: print(f"[INTERP] Compare: {left} {node.op} {right}")
        if node.op == '==': return left == right
        if node.op == '!=': return left != right
        if node.op == '<': return left < right
        if node.op == '>': return left > right
        raise ValueError(f"Unknown op {node.op}")

    def visit_BoolOp(self, node):
        left = self.visit(node.left)
        if self.debug: print(f"[INTERP] BoolOp: {left} {node.op} ...")
        if node.op == 'or':
            if left: return True
            return self.visit(node.right)
        raise ValueError(f"Unknown op {node.op}")

    def visit_Literal(self, node):
        return node.value

    def visit_Identifier(self, node):
        if node.name in self.global_env:
            val = self.global_env[node.name]
            if self.debug: print(f"[INTERP] Identifier: {node.name} = {val}")
            return val
        raise NameError(f"Name '{node.name}' is not defined")

# ==========================================
# 6. Main
# ==========================================
def main():
    import argparse
    parser = argparse.ArgumentParser(description='py0i - Mini Python Interpreter')
    parser.add_argument('script', help='Script file to run')
    parser.add_argument('--debug', '-d', action='store_true', help='Enable debug mode')
    parser.add_argument('--tokens', '-t', action='store_true', help='Show tokens only')
    parser.add_argument('--ast', '-a', action='store_true', help='Show AST only')
    args = parser.parse_args()

    try:
        with open(args.script, 'r', encoding='utf-8') as f:
            source = f.read()
    except FileNotFoundError:
        print(f"Error: File '{args.script}' not found.")
        sys.exit(1)

    try:
        if not source.endswith('\n'):
            source += '\n'
            
        lexer = Lexer(source, debug=args.debug)
        tokens = lexer.tokenize()
        
        if args.tokens:
            print("=== TOKENS ===")
            for tok in tokens:
                print(tok)
            return
        
        parser = Parser(tokens, debug=args.debug)
        ast = parser.parse()
        
        if args.ast:
            print("=== AST ===")
            print(ast)
            return
        
        interpreter = Interpreter(debug=args.debug)
        interpreter.interpret(ast)
        
    except Exception as e:
        print(f"\n{'='*50}")
        print(f"Runtime Error: {e}")
        print(f"{'='*50}\n")
        traceback.print_exc()
        sys.exit(1)

if __name__ == '__main__':
    main()