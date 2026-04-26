import re

# --- Tokenizer ---
TOKEN_REGEX = [
    ('NUMBER',   r'\d+'),
    ('STRING',   r'".*?"|\'.*?\''),
    ('DEF',      r'\bdef\b'),
    ('IF',       r'\bif\b'),
    ('ELSE',     r'\belse\b'),
    ('RETURN',   r'\breturn\b'),
    ('AND',      r'\band\b'),
    ('OR',       r'\bor\b'),
    ('NOT',      r'\bnot\b'),
    ('IDENT',    r'[a-zA-Z_][a-zA-Z0-9_]*'),
    ('EQ',       r'=='),
    ('NEQ',      r'!='),
    ('LE',       r'<='),
    ('GE',       r'>='),
    ('LT',       r'<'),
    ('GT',       r'>'),
    ('ASSIGN',   r'='),
    ('PLUS',     r'\+'),
    ('MINUS',    r'-'),
    ('MUL',      r'\*'),
    ('DIV',      r'/'),
    ('MOD',      r'%'),
    ('LPAREN',   r'\('),
    ('RPAREN',   r'\)'),
    ('COLON',    r':'),
    ('COMMA',    r','),
    ('NEWLINE',  r'\n'),
    ('SKIP',     r'[ \t]+'),
    ('MISMATCH', r'.'),
]

class Token:
    def __init__(self, type, value, line):
        self.type = type
        self.value = value
        self.line = line
    def __repr__(self):
        return f"Token({self.type}, {repr(self.value)})"

def tokenize(code):
    tok_regex = '|'.join('(?P<%s>%s)' % pair for pair in TOKEN_REGEX)
    line_num = 1
    line_start = 0
    tokens = []
    indent_stack = [0]
    
    # Process line by line for Python-style indentation
    lines = code.split('\n')
    for line in lines:
        if not line.strip() or line.strip().startswith('#'):
            line_num += 1
            continue
            
        # Count leading spaces
        indent = len(line) - len(line.lstrip(' '))
        
        if indent > indent_stack[-1]:
            indent_stack.append(indent)
            tokens.append(Token('INDENT', '', line_num))
        else:
            while indent < indent_stack[-1]:
                indent_stack.pop()
                tokens.append(Token('DEDENT', '', line_num))
                
        for mo in re.finditer(tok_regex, line):
            kind = mo.lastgroup
            value = mo.group()
            if kind == 'SKIP' or kind == 'NEWLINE':
                continue
            elif kind == 'MISMATCH':
                raise RuntimeError(f'{value!r} unexpected on line {line_num}')
            tokens.append(Token(kind, value, line_num))
            
        tokens.append(Token('NEWLINE', '\n', line_num))
        line_num += 1

    while len(indent_stack) > 1:
        indent_stack.pop()
        tokens.append(Token('DEDENT', '', line_num))
        
    return tokens

# --- AST Nodes ---
class ASTNode: pass

class Module(ASTNode):
    def __init__(self, body): self.body = body

class FunctionDef(ASTNode):
    def __init__(self, name, args, body):
        self.name = name
        self.args = args # list of strings
        self.body = body

class If(ASTNode):
    def __init__(self, test, body, orelse):
        self.test = test
        self.body = body
        self.orelse = orelse

class Return(ASTNode):
    def __init__(self, value): self.value = value

class Assign(ASTNode):
    def __init__(self, targets, value):
        self.targets = targets
        self.value = value

class ExprStmt(ASTNode):
    def __init__(self, value): self.value = value

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

class Call(ASTNode):
    def __init__(self, func, args):
        self.func = func
        self.args = args

class Name(ASTNode):
    def __init__(self, id): self.id = id

class Constant(ASTNode):
    def __init__(self, value): self.value = value

# --- Parser ---
class Parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.pos = 0

    def curr(self):
        return self.tokens[self.pos] if self.pos < len(self.tokens) else None

    def match(self, *types):
        c = self.curr()
        if c and c.type in types:
            self.pos += 1
            return c
        return None

    def expect(self, type_val):
        c = self.match(type_val)
        if not c:
            found = self.curr().type if self.curr() else 'EOF'
            raise SyntaxError(f"Expected {type_val}, found {found} at line {self.curr().line if self.curr() else 'EOF'}")
        return c

    def skip_newlines(self):
        while self.match('NEWLINE'): pass

    def parse(self):
        statements = []
        self.skip_newlines()
        while self.curr() is not None:
            statements.append(self.parse_statement())
            self.skip_newlines()
        return Module(statements)

    def parse_statement(self):
        c = self.curr()
        if c.type == 'DEF': return self.parse_def()
        elif c.type == 'IF': return self.parse_if()
        elif c.type == 'RETURN': return self.parse_return()
        else:
            # Could be assignment or expr
            # Very basic lookahead to see if it's an assignment
            if self.pos + 1 < len(self.tokens) and self.tokens[self.pos+1].type == 'ASSIGN':
                return self.parse_assign()
            return ExprStmt(self.parse_expr())

    def parse_def(self):
        self.expect('DEF')
        name = self.expect('IDENT').value
        self.expect('LPAREN')
        args = []
        if self.curr() and self.curr().type == 'IDENT':
            args.append(self.expect('IDENT').value)
            while self.match('COMMA'):
                args.append(self.expect('IDENT').value)
        self.expect('RPAREN')
        self.expect('COLON')
        body = self.parse_block()
        return FunctionDef(name, args, body)

    def parse_if(self):
        self.expect('IF')
        test = self.parse_expr()
        self.expect('COLON')
        body = self.parse_block()
        orelse = []
        self.skip_newlines()
        if self.match('ELSE'):
            self.expect('COLON')
            orelse = self.parse_block()
        return If(test, body, orelse)

    def parse_return(self):
        self.expect('RETURN')
        val = self.parse_expr()
        self.match('NEWLINE')
        return Return(val)

    def parse_assign(self):
        target = self.parse_expr() # should be Name
        self.expect('ASSIGN')
        val = self.parse_expr()
        self.match('NEWLINE')
        return Assign([target], val)

    def parse_block(self):
        self.match('NEWLINE')
        self.expect('INDENT')
        stmts = []
        while self.curr() and self.curr().type != 'DEDENT':
            self.skip_newlines()
            if self.curr() and self.curr().type != 'DEDENT':
                stmts.append(self.parse_statement())
        self.expect('DEDENT')
        return stmts

    # --- Expressions ---
    # expr = logic_expr
    def parse_expr(self):
        return self.parse_logic_expr()

    # logic_expr = rel_expr (( 'and' | 'or' ) rel_expr)*
    def parse_logic_expr(self):
        node = self.parse_rel_expr()
        while True:
            op = self.match('AND', 'OR')
            if op:
                right = self.parse_rel_expr()
                node = BinOp(node, op.value, right) # Using BinOp for simplicity in interpreter mapping
            else:
                break
        return node

    # rel_expr = add_expr (( '==' | '!=' | '<' | '<=' | '>' | '>=' ) add_expr)*
    def parse_rel_expr(self):
        node = self.parse_add_expr()
        op = self.match('EQ', 'NEQ', 'LT', 'LE', 'GT', 'GE')
        if op:
            right = self.parse_add_expr()
            return Compare(node, op.value, right)
        return node

    # add_expr = mul_expr (( '+' | '-' ) mul_expr)*
    def parse_add_expr(self):
        node = self.parse_mul_expr()
        while True:
            op = self.match('PLUS', 'MINUS')
            if op:
                right = self.parse_mul_expr()
                node = BinOp(node, op.value, right)
            else:
                break
        return node

    # mul_expr = primary (( '*' | '/' | '%' ) primary)*
    def parse_mul_expr(self):
        node = self.parse_primary()
        while True:
            op = self.match('MUL', 'DIV', 'MOD')
            if op:
                right = self.parse_primary()
                node = BinOp(node, op.value, right)
            else:
                break
        return node

    # primary = id [ '(' [ expr_list ] ')' ] | num | str | '(' expr ')'
    def parse_primary(self):
        c = self.curr()
        if self.match('LPAREN'):
            node = self.parse_expr()
            self.expect('RPAREN')
            return node
        elif c.type == 'NUMBER':
            self.pos += 1
            return Constant(int(c.value))
        elif c.type == 'STRING':
            self.pos += 1
            # remove quotes
            return Constant(c.value[1:-1])
        elif c.type == 'IDENT':
            self.pos += 1
            name = Name(c.value)
            # Check for call
            if self.match('LPAREN'):
                args = []
                if self.curr() and self.curr().type != 'RPAREN':
                    args.append(self.parse_expr())
                    while self.match('COMMA'):
                        args.append(self.parse_expr())
                self.expect('RPAREN')
                return Call(name, args)
            return name
        else:
            raise SyntaxError(f"Unexpected token {c} in expression")

def parse(source):
    tokens = tokenize(source)
    parser = Parser(tokens)
    return parser.parse()
