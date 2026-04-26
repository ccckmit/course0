use std::cell::RefCell;
use std::collections::HashMap;
use std::env;
use std::fmt;
use std::fs;
use std::process;
use std::rc::Rc;

// =========================================================================
// Tokens & Lexer
// =========================================================================

#[derive(Debug, Clone, PartialEq)]
enum TokenKind {
    Eof, Newline, Indent, Dedent, Name(String), Int(i64), Float(f64), String(String),
    Def, If, Else, While, Return, Pass, Lparen, Rparen, Lbracket, Rbracket,
    Comma, Colon, Dot, Plus, Minus, Star, Slash, Percent, Equal, Eqeq, Ne,
    Lt, Le, Gt, Ge,
}

#[derive(Debug, Clone)]
struct Token {
    kind: TokenKind,
    line: usize,
    col: usize,
}

fn lex_source(source: &str) -> Result<Vec<Token>, String> {
    let mut tokens = Vec::new();
    let mut indent_stack = vec![0];
    let mut line_no = 1;

    for line in source.lines() {
        let mut col = 0;
        let mut indent = 0;
        let chars: Vec<char> = line.chars().collect();

        while col < chars.len() && (chars[col] == ' ' || chars[col] == '\t') {
            indent += if chars[col] == '\t' { 4 } else { 1 };
            col += 1;
        }

        let is_blank = col == chars.len() || chars[col] == '#';
        if is_blank {
            line_no += 1;
            continue;
        }

        let top = *indent_stack.last().unwrap();
        if indent > top {
            indent_stack.push(indent);
            tokens.push(Token { kind: TokenKind::Indent, line: line_no, col: 1 });
        } else {
            while indent < *indent_stack.last().unwrap() {
                indent_stack.pop();
                tokens.push(Token { kind: TokenKind::Dedent, line: line_no, col: 1 });
            }
            if indent != *indent_stack.last().unwrap() {
                return Err(format!("inconsistent indentation at line {}", line_no));
            }
        }

        let mut i = col;
        while i < chars.len() {
            let c = chars[i];
            if c == '#' { break; }
            if c.is_ascii_whitespace() { i += 1; continue; }

            if c.is_ascii_alphabetic() || c == '_' {
                let start = i;
                while i < chars.len() && (chars[i].is_ascii_alphanumeric() || chars[i] == '_') { i += 1; }
                let text: String = chars[start..i].iter().collect();
                let kind = match text.as_str() {
                    "def" => TokenKind::Def, "if" => TokenKind::If, "else" => TokenKind::Else,
                    "while" => TokenKind::While, "return" => TokenKind::Return, "pass" => TokenKind::Pass,
                    _ => TokenKind::Name(text),
                };
                tokens.push(Token { kind, line: line_no, col: start + 1 });
                continue;
            }

            if c.is_ascii_digit() {
                let start = i;
                let mut is_float = false;
                while i < chars.len() && chars[i].is_ascii_digit() { i += 1; }
                if i < chars.len() && chars[i] == '.' {
                    is_float = true; i += 1;
                    while i < chars.len() && chars[i].is_ascii_digit() { i += 1; }
                }
                let text: String = chars[start..i].iter().collect();
                let kind = if is_float {
                    TokenKind::Float(text.parse().unwrap())
                } else {
                    TokenKind::Int(text.parse().unwrap())
                };
                tokens.push(Token { kind, line: line_no, col: start + 1 });
                continue;
            }

            if c == '\'' || c == '"' {
                let quote = c;
                let start = i;
                let mut val = String::new();
                i += 1;
                while i < chars.len() && chars[i] != quote {
                    if chars[i] == '\\' {
                        i += 1;
                        if i == chars.len() { break; }
                        match chars[i] {
                            'n' => val.push('\n'), 't' => val.push('\t'),
                            '\\' => val.push('\\'), '\'' => val.push('\''),
                            '"' => val.push('"'), _ => val.push(chars[i]),
                        }
                    } else {
                        val.push(chars[i]);
                    }
                    i += 1;
                }
                if i == chars.len() || chars[i] != quote {
                    return Err(format!("unterminated string at line {}", line_no));
                }
                i += 1;
                tokens.push(Token { kind: TokenKind::String(val), line: line_no, col: start + 1 });
                continue;
            }

            let start = i;
            let (kind, step) = if i + 1 < chars.len() && chars[i] == '=' && chars[i+1] == '=' { (TokenKind::Eqeq, 2) }
            else if i + 1 < chars.len() && chars[i] == '!' && chars[i+1] == '=' { (TokenKind::Ne, 2) }
            else if i + 1 < chars.len() && chars[i] == '<' && chars[i+1] == '=' { (TokenKind::Le, 2) }
            else if i + 1 < chars.len() && chars[i] == '>' && chars[i+1] == '=' { (TokenKind::Ge, 2) }
            else {
                let k = match c {
                    '(' => TokenKind::Lparen, ')' => TokenKind::Rparen,
                    '[' => TokenKind::Lbracket, ']' => TokenKind::Rbracket,
                    ',' => TokenKind::Comma, ':' => TokenKind::Colon, '.' => TokenKind::Dot,
                    '+' => TokenKind::Plus, '-' => TokenKind::Minus, '*' => TokenKind::Star,
                    '/' => TokenKind::Slash, '%' => TokenKind::Percent, '=' => TokenKind::Equal,
                    '<' => TokenKind::Lt, '>' => TokenKind::Gt,
                    _ => return Err(format!("unexpected character '{}' at line {}", c, line_no)),
                };
                (k, 1)
            };
            tokens.push(Token { kind, line: line_no, col: start + 1 });
            i += step;
        }
        tokens.push(Token { kind: TokenKind::Newline, line: line_no, col: chars.len() + 1 });
        line_no += 1;
    }

    while indent_stack.len() > 1 {
        indent_stack.pop();
        tokens.push(Token { kind: TokenKind::Dedent, line: line_no, col: 1 });
    }
    tokens.push(Token { kind: TokenKind::Eof, line: line_no, col: 1 });

    Ok(tokens)
}

// =========================================================================
// AST Nodes
// =========================================================================

#[derive(Debug, Clone, Copy, PartialEq)]
enum Op { Add, Sub, Mul, Div, Mod, Eq, Ne, Lt, Le, Gt, Ge, Neg }

#[derive(Debug, Clone)]
enum Expr {
    Name(String),
    Int(i64),
    Float(f64),
    String(String),
    BinOp(Op, Box<Expr>, Box<Expr>),
    UnaryOp(Op, Box<Expr>),
    Compare(Op, Box<Expr>, Box<Expr>),
    Call(Box<Expr>, Vec<Expr>),
    Attribute(Box<Expr>, String),
    Subscript(Box<Expr>, Box<Expr>),
}

#[derive(Debug, Clone)]
enum Stmt {
    Expr(Expr),
    Assign(String, Expr),
    If(Expr, Vec<Stmt>, Vec<Stmt>),
    While(Expr, Vec<Stmt>),
    FunctionDef(String, Vec<String>, Vec<Stmt>),
    Return(Option<Expr>),
    Pass,
}

// =========================================================================
// Parser
// =========================================================================

struct Parser<'a> {
    tokens: &'a [Token],
    pos: usize,
    filename: &'a str,
}

impl<'a> Parser<'a> {
    fn new(tokens: &'a [Token], filename: &'a str) -> Self {
        Self { tokens, pos: 0, filename }
    }

    fn peek(&self) -> &Token { &self.tokens[self.pos] }
    fn prev(&self) -> &Token { &self.tokens[self.pos - 1] }

    fn match_token(&mut self, kind: &TokenKind) -> bool {
        if core::mem::discriminant(&self.peek().kind) == core::mem::discriminant(kind) {
            self.pos += 1; true
        } else { false }
    }

    fn expect(&mut self, kind: TokenKind, msg: &str) -> Result<&Token, String> {
        if core::mem::discriminant(&self.peek().kind) != core::mem::discriminant(&kind) {
            let tok = self.peek();
            Err(format!("{}:{}:{}: {}", self.filename, tok.line, tok.col, msg))
        } else {
            self.pos += 1;
            Ok(self.prev())
        }
    }

    fn skip_newlines(&mut self) {
        while self.match_token(&TokenKind::Newline) {}
    }

    fn parse_primary(&mut self) -> Result<Expr, String> {
        let tok = self.peek().clone();
        match &tok.kind {
            TokenKind::Int(v) => { self.pos += 1; Ok(Expr::Int(*v)) }
            TokenKind::Float(v) => { self.pos += 1; Ok(Expr::Float(*v)) }
            TokenKind::String(v) => { self.pos += 1; Ok(Expr::String(v.clone())) }
            TokenKind::Name(n) => {
                self.pos += 1;
                let mut expr = Expr::Name(n.clone());
                self.parse_postfix(&mut expr)?;
                Ok(expr)
            }
            TokenKind::Lparen => {
                self.pos += 1;
                let mut expr = self.parse_expr()?;
                self.expect(TokenKind::Rparen, "expected ')'")?;
                self.parse_postfix(&mut expr)?;
                Ok(expr)
            }
            _ => Err(format!("{}:{}:{}: expected expression", self.filename, tok.line, tok.col)),
        }
    }

    fn parse_postfix(&mut self, expr: &mut Expr) -> Result<(), String> {
        loop {
            if self.match_token(&TokenKind::Lparen) {
                let mut args = Vec::new();
                if !self.match_token(&TokenKind::Rparen) {
                    loop {
                        args.push(self.parse_expr()?);
                        if !self.match_token(&TokenKind::Comma) { break; }
                    }
                    self.expect(TokenKind::Rparen, "expected ')'")?;
                }
                *expr = Expr::Call(Box::new(expr.clone()), args);
            } else if self.match_token(&TokenKind::Dot) {
                if let TokenKind::Name(n) = &self.expect(TokenKind::Name(String::new()), "expected attribute name")?.kind {
                    *expr = Expr::Attribute(Box::new(expr.clone()), n.clone());
                }
            } else if self.match_token(&TokenKind::Lbracket) {
                let index = self.parse_expr()?;
                self.expect(TokenKind::Rbracket, "expected ']'")?;
                *expr = Expr::Subscript(Box::new(expr.clone()), Box::new(index));
            } else {
                break;
            }
        }
        Ok(())
    }

    fn parse_unary(&mut self) -> Result<Expr, String> {
        if self.match_token(&TokenKind::Minus) {
            Ok(Expr::UnaryOp(Op::Neg, Box::new(self.parse_unary()?)))
        } else {
            self.parse_primary()
        }
    }

    fn parse_factor(&mut self) -> Result<Expr, String> {
        let mut expr = self.parse_unary()?;
        loop {
            let op = if self.match_token(&TokenKind::Star) { Op::Mul }
            else if self.match_token(&TokenKind::Slash) { Op::Div }
            else if self.match_token(&TokenKind::Percent) { Op::Mod }
            else { break };
            expr = Expr::BinOp(op, Box::new(expr), Box::new(self.parse_unary()?));
        }
        Ok(expr)
    }

    fn parse_term(&mut self) -> Result<Expr, String> {
        let mut expr = self.parse_factor()?;
        loop {
            let op = if self.match_token(&TokenKind::Plus) { Op::Add }
            else if self.match_token(&TokenKind::Minus) { Op::Sub }
            else { break };
            expr = Expr::BinOp(op, Box::new(expr), Box::new(self.parse_factor()?));
        }
        Ok(expr)
    }

    fn parse_comparison(&mut self) -> Result<Expr, String> {
        let mut expr = self.parse_term()?;
        loop {
            let op = if self.match_token(&TokenKind::Eqeq) { Op::Eq }
            else if self.match_token(&TokenKind::Ne) { Op::Ne }
            else if self.match_token(&TokenKind::Lt) { Op::Lt }
            else if self.match_token(&TokenKind::Le) { Op::Le }
            else if self.match_token(&TokenKind::Gt) { Op::Gt }
            else if self.match_token(&TokenKind::Ge) { Op::Ge }
            else { break };
            expr = Expr::Compare(op, Box::new(expr), Box::new(self.parse_term()?));
        }
        Ok(expr)
    }

    fn parse_expr(&mut self) -> Result<Expr, String> {
        self.parse_comparison()
    }

    fn parse_block(&mut self) -> Result<Vec<Stmt>, String> {
        self.expect(TokenKind::Newline, "expected newline after ':'")?;
        self.expect(TokenKind::Indent, "expected indented block")?;
        self.skip_newlines();
        let mut body = Vec::new();
        while self.peek().kind != TokenKind::Dedent && self.peek().kind != TokenKind::Eof {
            body.push(self.parse_stmt()?);
            self.skip_newlines();
        }
        self.expect(TokenKind::Dedent, "expected dedent")?;
        Ok(body)
    }

    fn parse_stmt(&mut self) -> Result<Stmt, String> {
        if self.match_token(&TokenKind::Def) {
            let name = if let TokenKind::Name(n) = &self.expect(TokenKind::Name(String::new()), "expected function name")?.kind { n.clone() } else { unreachable!() };
            self.expect(TokenKind::Lparen, "expected '('")?;
            let mut params = Vec::new();
            if !self.match_token(&TokenKind::Rparen) {
                loop {
                    if let TokenKind::Name(n) = &self.expect(TokenKind::Name(String::new()), "expected param name")?.kind { params.push(n.clone()); }
                    if !self.match_token(&TokenKind::Comma) { break; }
                }
                self.expect(TokenKind::Rparen, "expected ')'")?;
            }
            self.expect(TokenKind::Colon, "expected ':'")?;
            return Ok(Stmt::FunctionDef(name, params, self.parse_block()?));
        }
        if self.match_token(&TokenKind::If) {
            let test = self.parse_expr()?;
            self.expect(TokenKind::Colon, "expected ':'")?;
            let body = self.parse_block()?;
            self.skip_newlines();
            let orelse = if self.match_token(&TokenKind::Else) {
                self.expect(TokenKind::Colon, "expected ':'")?;
                self.parse_block()?
            } else { Vec::new() };
            return Ok(Stmt::If(test, body, orelse));
        }
        if self.match_token(&TokenKind::While) {
            let test = self.parse_expr()?;
            self.expect(TokenKind::Colon, "expected ':'")?;
            return Ok(Stmt::While(test, self.parse_block()?));
        }
        if self.match_token(&TokenKind::Return) {
            if self.match_token(&TokenKind::Newline) { return Ok(Stmt::Return(None)); }
            let expr = self.parse_expr()?;
            self.expect(TokenKind::Newline, "expected newline")?;
            return Ok(Stmt::Return(Some(expr)));
        }
        if self.match_token(&TokenKind::Pass) {
            self.expect(TokenKind::Newline, "expected newline")?;
            return Ok(Stmt::Pass);
        }

        if let TokenKind::Name(name) = &self.peek().kind {
            if self.tokens[self.pos + 1].kind == TokenKind::Equal {
                let n = name.clone();
                self.pos += 2; // skip name and '='
                let value = self.parse_expr()?;
                self.expect(TokenKind::Newline, "expected newline after assignment")?;
                return Ok(Stmt::Assign(n, value));
            }
        }

        let expr = self.parse_expr()?;
        self.expect(TokenKind::Newline, "expected newline after expression")?;
        Ok(Stmt::Expr(expr))
    }

    fn parse_module(&mut self) -> Result<Vec<Stmt>, String> {
        let mut body = Vec::new();
        self.skip_newlines();
        while self.peek().kind != TokenKind::Eof {
            body.push(self.parse_stmt()?);
            self.skip_newlines();
        }
        Ok(body)
    }
}

// =========================================================================
// Environment & Values
// =========================================================================

#[derive(Clone)]
enum PyValue {
    None,
    Bool(bool),
    Int(i64),
    Float(f64),
    Str(String),
    List(Rc<RefCell<Vec<PyValue>>>),
    Sys(Rc<RefCell<Vec<PyValue>>>),
    Function {
        name: String,
        params: Vec<String>,
        body: Rc<Vec<Stmt>>,
        closure: Rc<RefCell<Env>>,
    },
    Builtin(String, Rc<dyn Fn(&mut Runtime, Vec<PyValue>) -> Result<PyValue, String>>),
}

impl PartialEq for PyValue {
    fn eq(&self, other: &Self) -> bool {
        match (self, other) {
            (PyValue::None, PyValue::None) => true,
            (PyValue::Bool(a), PyValue::Bool(b)) => a == b,
            (PyValue::Int(a), PyValue::Int(b)) => a == b,
            (PyValue::Float(a), PyValue::Float(b)) => a == b,
            (PyValue::Str(a), PyValue::Str(b)) => a == b,
            _ => false,
        }
    }
}

impl fmt::Display for PyValue {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            PyValue::None => write!(f, "None"),
            PyValue::Bool(b) => write!(f, "{}", if *b { "True" } else { "False" }),
            PyValue::Int(i) => write!(f, "{}", i),
            PyValue::Float(n) => write!(f, "{}", n),
            PyValue::Str(s) => write!(f, "{}", s),
            PyValue::List(_) => write!(f, "<list>"),
            PyValue::Sys(_) => write!(f, "<sys>"),
            PyValue::Function { .. } => write!(f, "<function>"),
            PyValue::Builtin(name, _) => write!(f, "<builtin-function {}>", name),
        }
    }
}

impl PyValue {
    fn is_truthy(&self) -> bool {
        match self {
            PyValue::None => false,
            PyValue::Bool(b) => *b,
            PyValue::Int(i) => *i != 0,
            PyValue::Float(f) => *f != 0.0,
            PyValue::Str(s) => !s.is_empty(),
            PyValue::List(l) => !l.borrow().is_empty(),
            _ => true,
        }
    }

    fn as_number(&self) -> Result<f64, String> {
        match self {
            PyValue::Int(i) => Ok(*i as f64),
            PyValue::Float(f) => Ok(*f),
            PyValue::Bool(b) => Ok(if *b { 1.0 } else { 0.0 }),
            _ => Err("expected number".into()),
        }
    }
}

struct Env {
    parent: Option<Rc<RefCell<Env>>>,
    vars: HashMap<String, PyValue>,
}

impl Env {
    fn new(parent: Option<Rc<RefCell<Env>>>) -> Rc<RefCell<Self>> {
        Rc::new(RefCell::new(Env { parent, vars: HashMap::new() }))
    }

    fn set(&mut self, name: &str, value: PyValue) {
        self.vars.insert(name.to_string(), value);
    }

    fn assign(&mut self, name: &str, value: PyValue) {
        if self.vars.contains_key(name) {
            self.vars.insert(name.to_string(), value);
            return;
        }
        if let Some(parent) = &self.parent {
            if parent.borrow().get_opt(name).is_some() {
                parent.borrow_mut().assign(name, value);
                return;
            }
        }
        self.vars.insert(name.to_string(), value);
    }

    fn get_opt(&self, name: &str) -> Option<PyValue> {
        if let Some(val) = self.vars.get(name) {
            return Some(val.clone());
        }
        if let Some(parent) = &self.parent {
            return parent.borrow().get_opt(name);
        }
        None
    }

    fn get(&self, name: &str) -> Result<PyValue, String> {
        self.get_opt(name).ok_or_else(|| format!("name '{}' is not defined", name))
    }
}

// =========================================================================
// Evaluator
// =========================================================================

struct Runtime {
    globals: Rc<RefCell<Env>>,
    sys_value: PyValue,
}

enum ExecStatus {
    Continue,
    Return(PyValue),
}

fn eval_expr(rt: &mut Runtime, env: &Rc<RefCell<Env>>, expr: &Expr) -> Result<PyValue, String> {
    match expr {
        Expr::Name(name) => env.borrow().get(name),
        Expr::Int(v) => Ok(PyValue::Int(*v)),
        Expr::Float(v) => Ok(PyValue::Float(*v)),
        Expr::String(v) => Ok(PyValue::Str(v.clone())),
        Expr::BinOp(op, left, right) => {
            let l = eval_expr(rt, env, left)?;
            let r = eval_expr(rt, env, right)?;
            apply_binop(*op, l, r)
        }
        Expr::UnaryOp(op, operand) => {
            let v = eval_expr(rt, env, operand)?;
            match op {
                Op::Neg => match v {
                    PyValue::Int(i) => Ok(PyValue::Int(-i)),
                    _ => Ok(PyValue::Float(-v.as_number()?)),
                },
                _ => Err("unsupported unary operator".into()),
            }
        }
        Expr::Compare(op, left, right) => {
            let l = eval_expr(rt, env, left)?;
            let r = eval_expr(rt, env, right)?;
            apply_compare(*op, l, r)
        }
        Expr::Call(func_expr, args_exprs) => {
            let func = eval_expr(rt, env, func_expr)?;
            let mut args = Vec::new();
            for arg_expr in args_exprs {
                args.push(eval_expr(rt, env, arg_expr)?);
            }
            call_function(rt, func, args)
        }
        Expr::Attribute(val_expr, attr) => {
            let val = eval_expr(rt, env, val_expr)?;
            if let PyValue::Sys(sys_list) = val {
                if attr == "argv" { return Ok(PyValue::List(sys_list)); }
            }
            Err(format!("unsupported attribute access: {}", attr))
        }
        Expr::Subscript(val_expr, idx_expr) => {
            let val = eval_expr(rt, env, val_expr)?;
            let idx = eval_expr(rt, env, idx_expr)?;
            let i = match idx { PyValue::Int(i) => i, _ => return Err("subscript index must be int".into()) };
            
            match val {
                PyValue::List(list) => {
                    let l = list.borrow();
                    if i < 0 || i as usize >= l.len() { Err("list index out of range".into()) }
                    else { Ok(l[i as usize].clone()) }
                }
                PyValue::Str(s) => {
                    if i < 0 || i as usize >= s.len() { Err("string index out of range".into()) }
                    else { Ok(PyValue::Str(s.chars().nth(i as usize).unwrap().to_string())) }
                }
                _ => Err("unsupported subscript target".into()),
            }
        }
    }
}

fn apply_binop(op: Op, left: PyValue, right: PyValue) -> Result<PyValue, String> {
    if op == Op::Add {
        if let (PyValue::Str(l), PyValue::Str(r)) = (&left, &right) {
            return Ok(PyValue::Str(format!("{}{}", l, r)));
        }
    }
    if let (PyValue::Int(a), PyValue::Int(b)) = (&left, &right) {
        return match op {
            Op::Add => Ok(PyValue::Int(a + b)),
            Op::Sub => Ok(PyValue::Int(a - b)),
            Op::Mul => Ok(PyValue::Int(a * b)),
            Op::Div => Ok(PyValue::Float((*a as f64) / (*b as f64))),
            Op::Mod => Ok(PyValue::Int(a % b)),
            _ => Err("unsupported binop".into()),
        };
    }
    let a = left.as_number()?;
    let b = right.as_number()?;
    match op {
        Op::Add => Ok(PyValue::Float(a + b)),
        Op::Sub => Ok(PyValue::Float(a - b)),
        Op::Mul => Ok(PyValue::Float(a * b)),
        Op::Div => Ok(PyValue::Float(a / b)),
        Op::Mod => Ok(PyValue::Float((a as i64 % b as i64) as f64)),
        _ => Err("unsupported binop".into()),
    }
}

fn apply_compare(op: Op, left: PyValue, right: PyValue) -> Result<PyValue, String> {
    if let (PyValue::Str(a), PyValue::Str(b)) = (&left, &right) {
        return Ok(PyValue::Bool(match op {
            Op::Eq => a == b, Op::Ne => a != b,
            Op::Lt => a < b, Op::Le => a <= b,
            Op::Gt => a > b, Op::Ge => a >= b,
            _ => false,
        }));
    }
    let a = left.as_number()?;
    let b = right.as_number()?;
    Ok(PyValue::Bool(match op {
        Op::Eq => a == b, Op::Ne => a != b,
        Op::Lt => a < b, Op::Le => a <= b,
        Op::Gt => a > b, Op::Ge => a >= b,
        _ => false,
    }))
}

fn exec_stmt(rt: &mut Runtime, env: &Rc<RefCell<Env>>, stmt: &Stmt) -> Result<ExecStatus, String> {
    match stmt {
        Stmt::Expr(expr) => { eval_expr(rt, env, expr)?; Ok(ExecStatus::Continue) }
        Stmt::Assign(name, val_expr) => {
            let val = eval_expr(rt, env, val_expr)?;
            env.borrow_mut().assign(name, val);
            Ok(ExecStatus::Continue)
        }
        Stmt::If(test, body, orelse) => {
            let cond = eval_expr(rt, env, test)?;
            let block = if cond.is_truthy() { body } else { orelse };
            exec_block(rt, env, block)
        }
        Stmt::While(test, body) => {
            while eval_expr(rt, env, test)?.is_truthy() {
                if let ExecStatus::Return(v) = exec_block(rt, env, body)? {
                    return Ok(ExecStatus::Return(v));
                }
            }
            Ok(ExecStatus::Continue)
        }
        Stmt::FunctionDef(name, params, body) => {
            let func = PyValue::Function {
                name: name.clone(), params: params.clone(),
                body: Rc::new(body.clone()), closure: Rc::clone(env),
            };
            env.borrow_mut().set(name, func);
            Ok(ExecStatus::Continue)
        }
        Stmt::Return(expr_opt) => {
            let val = if let Some(expr) = expr_opt { eval_expr(rt, env, expr)? } else { PyValue::None };
            Ok(ExecStatus::Return(val))
        }
        Stmt::Pass => Ok(ExecStatus::Continue),
    }
}

fn exec_block(rt: &mut Runtime, env: &Rc<RefCell<Env>>, stmts: &[Stmt]) -> Result<ExecStatus, String> {
    for stmt in stmts {
        if let ExecStatus::Return(v) = exec_stmt(rt, env, stmt)? {
            return Ok(ExecStatus::Return(v));
        }
    }
    Ok(ExecStatus::Continue)
}

fn call_function(rt: &mut Runtime, func: PyValue, args: Vec<PyValue>) -> Result<PyValue, String> {
    match func {
        PyValue::Builtin(_, fn_ptr) => fn_ptr(rt, args),
        PyValue::Function { name, params, body, closure } => {
            if args.len() != params.len() {
                return Err(format!("{}() expected {} arguments, got {}", name, params.len(), args.len()));
            }
            let local_env = Env::new(Some(closure));
            for (param, arg) in params.iter().zip(args) {
                local_env.borrow_mut().set(param, arg);
            }
            match exec_block(rt, &local_env, &body)? {
                ExecStatus::Return(v) => Ok(v),
                ExecStatus::Continue => Ok(PyValue::None),
            }
        }
        _ => Err("object is not callable".into()),
    }
}

// =========================================================================
// Builtins & Runtime
// =========================================================================

fn builtin_print(_rt: &mut Runtime, args: Vec<PyValue>) -> Result<PyValue, String> {
    let s = args.iter().map(|v| v.to_string()).collect::<Vec<_>>().join(" ");
    println!("{}", s);
    Ok(PyValue::None)
}

fn builtin_len(_rt: &mut Runtime, args: Vec<PyValue>) -> Result<PyValue, String> {
    if args.len() != 1 { return Err("len() expects 1 argument".into()); }
    match &args[0] {
        PyValue::Str(s) => Ok(PyValue::Int(s.len() as i64)),
        PyValue::List(l) => Ok(PyValue::Int(l.borrow().len() as i64)),
        _ => Err("len() unsupported for this type".into()),
    }
}

fn builtin_int(_rt: &mut Runtime, args: Vec<PyValue>) -> Result<PyValue, String> {
    if args.len() != 1 { return Err("int() expects 1 argument".into()); }
    match args[0] {
        PyValue::Int(i) => Ok(PyValue::Int(i)),
        PyValue::Float(f) => Ok(PyValue::Int(f as i64)),
        PyValue::Bool(b) => Ok(PyValue::Int(if b { 1 } else { 0 })),
        _ => Err("int() unsupported for this type".into()),
    }
}

fn builtin_float(_rt: &mut Runtime, args: Vec<PyValue>) -> Result<PyValue, String> {
    if args.len() != 1 { return Err("float() expects 1 argument".into()); }
    Ok(PyValue::Float(args[0].as_number()?))
}

fn builtin_str(_rt: &mut Runtime, args: Vec<PyValue>) -> Result<PyValue, String> {
    if args.len() != 1 { return Err("str() expects 1 argument".into()); }
    Ok(PyValue::Str(args[0].to_string()))
}

fn builtin_bool(_rt: &mut Runtime, args: Vec<PyValue>) -> Result<PyValue, String> {
    if args.len() != 1 { return Err("bool() expects 1 argument".into()); }
    Ok(PyValue::Bool(args[0].is_truthy()))
}

fn builtin_import(rt: &mut Runtime, args: Vec<PyValue>) -> Result<PyValue, String> {
    if args.len() != 1 { return Err("__import__() expects one string argument".into()); }
    if let PyValue::Str(name) = &args[0] {
        if name == "sys" { return Ok(rt.sys_value.clone()); }
        return Err(format!("unsupported import: {}", name));
    }
    Err("__import__() expects one string argument".into())
}

fn builtin_run_path(rt: &mut Runtime, args: Vec<PyValue>) -> Result<PyValue, String> {
    if args.len() != 1 { return Err("run_path() expects one string argument".into()); }
    if let PyValue::Str(path) = &args[0] {
        let source = fs::read_to_string(path).map_err(|_| format!("cannot open {}", path))?;
        let tokens = lex_source(&source)?;
        let mut parser = Parser::new(&tokens, path);
        let module = parser.parse_module()?;

        // Save old sys.argv
        let old_argv = if let PyValue::Sys(sys_list) = &rt.sys_value {
            sys_list.borrow().clone()
        } else { vec![] };

        // Construct new argv = old_argv[1:]
        let new_argv = old_argv.iter().skip(1).cloned().collect();
        if let PyValue::Sys(sys_list) = &rt.sys_value {
            *sys_list.borrow_mut() = new_argv;
        }

        let module_env = Env::new(Some(Rc::clone(&rt.globals)));
        module_env.borrow_mut().set("__name__", PyValue::Str("__main__".into()));
        module_env.borrow_mut().set("__file__", PyValue::Str(path.clone()));

        exec_block(rt, &module_env, &module)?;

        // Restore old sys.argv
        if let PyValue::Sys(sys_list) = &rt.sys_value {
            *sys_list.borrow_mut() = old_argv;
        }

        return Ok(PyValue::None);
    }
    Err("run_path() expects one string argument".into())
}

fn install_builtins(globals: &Rc<RefCell<Env>>) {
    let mut env = globals.borrow_mut();
    env.set("print", PyValue::Builtin("print".into(), Rc::new(builtin_print)));
    env.set("len", PyValue::Builtin("len".into(), Rc::new(builtin_len)));
    env.set("int", PyValue::Builtin("int".into(), Rc::new(builtin_int)));
    env.set("float", PyValue::Builtin("float".into(), Rc::new(builtin_float)));
    env.set("str", PyValue::Builtin("str".into(), Rc::new(builtin_str)));
    env.set("bool", PyValue::Builtin("bool".into(), Rc::new(builtin_bool)));
    env.set("__import__", PyValue::Builtin("__import__".into(), Rc::new(builtin_import)));
    env.set("run_path", PyValue::Builtin("run_path".into(), Rc::new(builtin_run_path)));
}

// =========================================================================
// Main
// =========================================================================

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: ./py0i <script.py>");
        process::exit(1);
    }

    let script_path = &args[1];
    let source = match fs::read_to_string(script_path) {
        Ok(src) => src,
        Err(_) => {
            eprintln!("cannot open {}", script_path);
            process::exit(1);
        }
    };

    let globals = Env::new(None);
    install_builtins(&globals);

    // Setup sys.argv
    let sys_args: Vec<PyValue> = args[1..].iter().map(|s| PyValue::Str(s.clone())).collect();
    let sys_value = PyValue::Sys(Rc::new(RefCell::new(sys_args)));
    globals.borrow_mut().set("sys", sys_value.clone());
    globals.borrow_mut().set("__name__", PyValue::Str("__main__".into()));
    globals.borrow_mut().set("__file__", PyValue::Str(script_path.clone()));

    let mut rt = Runtime { globals: Rc::clone(&globals), sys_value };

    // Lex
    let tokens = match lex_source(&source) {
        Ok(t) => t,
        Err(e) => { eprintln!("Lexer Error: {}", e); process::exit(1); }
    };

    // Parse
    let mut parser = Parser::new(&tokens, script_path);
    let module = match parser.parse_module() {
        Ok(m) => m,
        Err(e) => { eprintln!("Parse Error: {}", e); process::exit(1); }
    };

    // Execute
    if let Err(e) = exec_block(&mut rt, &globals, &module) {
        eprintln!("Runtime Error: {}", e);
        process::exit(1);
    }
}