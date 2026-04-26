use std::cell::RefCell;
use std::collections::HashMap;
use std::rc::Rc;

// ===== Types & Values =====

#[derive(Clone)]
pub enum Val {
    Undef,
    Null,
    Bool(bool),
    Num(f64),
    Str(Rc<String>),
    Obj(Rc<RefCell<Obj>>),
}

impl std::fmt::Debug for Val {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "ValError")
    }
}

#[derive(Clone)]
pub enum ObjType {
    Object,
    Array(Vec<Val>),
    Function(FuncData),
    Native(fn(Val, Vec<Val>, &mut Runtime) -> Result<Val, Val>),
    Bound(Val, Val, Vec<Val>),
}

pub struct Obj {
    pub props: HashMap<String, Val>,
    pub proto: Val,
    pub typ: ObjType,
}

#[derive(Clone)]
pub struct FuncData {
    pub name: Option<String>,
    pub params: Vec<String>,
    pub body: Vec<Node>,
    pub env: Rc<RefCell<Env>>,
}

#[derive(Clone)]
pub struct Runtime {
    pub object_proto: Val,
    pub array_proto: Val,
    pub func_proto: Val,
    pub string_proto: Val,
    pub error_proto: Val,
}

pub enum Signal {
    Return(Val),
    Break(Option<String>),
    Continue(Option<String>),
    Throw(Val),
}

pub struct Env {
    pub parent: Option<Rc<RefCell<Env>>>,
    pub vars: HashMap<String, Val>,
}

// ===== AST Nodes =====

#[derive(Debug, Clone)]
pub enum Node {
    Prog(Vec<Node>),
    Empty,
    ExprStmt(Box<Node>),
    Var { decls: Vec<(String, Option<Node>)> },
    FuncDecl { name: String, params: Vec<String>, body: Vec<Node> },
    Return(Option<Box<Node>>),
    If { test: Box<Node>, cons: Box<Node>, alt: Option<Box<Node>> },
    While { test: Box<Node>, body: Box<Node> },
    DoWhile { test: Box<Node>, body: Box<Node> },
    For { init: Option<Box<Node>>, test: Option<Box<Node>>, update: Option<Box<Node>>, body: Box<Node> },
    ForIn { kind: Option<String>, name: String, obj: Box<Node>, body: Box<Node> },
    ForOf { kind: Option<String>, name: String, iter: Box<Node>, body: Box<Node> },
    Break(Option<String>),
    Cont(Option<String>),
    Switch { disc: Box<Node>, cases: Vec<(Option<Box<Node>>, Vec<Node>)> },
    Throw(Box<Node>),
    Try { block: Vec<Node>, param: Option<String>, catch_body: Option<Vec<Node>>, fin: Option<Vec<Node>> },
    Label { label: String, body: Box<Node> },
    Block(Vec<Node>),

    Num(f64),
    Str(String),
    Bool(bool),
    LitNull,
    LitUndef,
    This,
    Id(String),
    Array(Vec<Option<Node>>),
    Object(Vec<(Box<Node>, Box<Node>, bool)>),
    FuncExpr { name: Option<String>, params: Vec<String>, body: Vec<Node> },
    Assign { op: String, left: Box<Node>, right: Box<Node> },
    Binary { op: String, left: Box<Node>, right: Box<Node> },
    Logical { op: String, left: Box<Node>, right: Box<Node> },
    Unary { op: String, arg: Box<Node>, prefix: bool },
    Update { op: String, arg: Box<Node>, prefix: bool },
    Cond { test: Box<Node>, cons: Box<Node>, alt: Box<Node> },
    Seq { left: Box<Node>, right: Box<Node> },
    Call { callee: Box<Node>, args: Vec<Node> },
    New { callee: Box<Node>, args: Vec<Node> },
    Member { obj: Box<Node>, prop: Box<Node>, computed: bool },
}

// ===== Tokenizer =====

#[derive(Debug, Clone)]
pub enum Token {
    N(f64),
    S(String),
    I(String),
    P(String),
    E,
}

fn tokenize(src: &str) -> Vec<Token> {
    let src: Vec<char> = src.chars().collect();
    let mut toks = Vec::new();
    let mut i = 0;
    let len = src.len();
    while i < len {
        let c = src[i];
        if " \t\r\n".contains(c) { i += 1; continue; }
        if c == '/' && i + 1 < len && src[i + 1] == '/' {
            i += 2; while i < len && src[i] != '\n' { i += 1; } continue;
        }
        if c == '/' && i + 1 < len && src[i + 1] == '*' {
            i += 2; while i + 1 < len && !(src[i] == '*' && src[i + 1] == '/') { i += 1; } i += 2; continue;
        }
        if c.is_ascii_digit() {
            let s = i;
            if c == '0' && i + 1 < len {
                let nx = src[i + 1];
                if nx == 'x' || nx == 'X' { i += 2; while i < len && src[i].is_ascii_hexdigit() { i += 1; } toks.push(Token::N(i64::from_str_radix(&src[s+2..i].iter().collect::<String>(), 16).unwrap_or(0) as f64)); continue; }
                if nx == 'b' || nx == 'B' { i += 2; while i < len && (src[i]=='0'||src[i]=='1') { i += 1; } toks.push(Token::N(i64::from_str_radix(&src[s+2..i].iter().collect::<String>(), 2).unwrap_or(0) as f64)); continue; }
                if nx == 'o' || nx == 'O' { i += 2; while i < len && src[i]>='0' && src[i]<='7' { i += 1; } toks.push(Token::N(i64::from_str_radix(&src[s+2..i].iter().collect::<String>(), 8).unwrap_or(0) as f64)); continue; }
            }
            while i < len && src[i].is_ascii_digit() { i += 1; }
            if i < len && src[i] == '.' && (i + 1 >= len || src[i + 1] != '.') { i += 1; while i < len && src[i].is_ascii_digit() { i += 1; } }
            if i < len && (src[i] == 'e' || src[i] == 'E') { i += 1; if i < len && (src[i] == '+' || src[i] == '-') { i += 1; } while i < len && src[i].is_ascii_digit() { i += 1; } }
            toks.push(Token::N(src[s..i].iter().collect::<String>().parse().unwrap_or(0.0))); continue;
        }
        if c == '"' || c == '\'' {
            i += 1; let mut s = String::new();
            while i < len && src[i] != c {
                if src[i] == '\\' {
                    i += 1; let e = src[i];
                    if e == 'n' { s.push('\n'); } 
                    else if e == 't' { s.push('\t'); } 
                    else if e == 'r' { s.push('\r'); } 
                    else if e == 'b' { s.push('\x08'); } 
                    else if e == 'f' { s.push('\x0C'); } 
                    else if e == '0' { s.push('\0'); } 
                    else if e == 'u' { i += 1; s.push(std::char::from_u32(u32::from_str_radix(&src[i..i+4].iter().collect::<String>(), 16).unwrap_or(0)).unwrap_or('\0')); i += 3; } 
                    else if e == 'x' { i += 1; s.push(std::char::from_u32(u32::from_str_radix(&src[i..i+2].iter().collect::<String>(), 16).unwrap_or(0)).unwrap_or('\0')); i += 1; } 
                    else { s.push(e); } 
                    i += 1;
                } else { s.push(src[i]); i += 1; }
            }
            i += 1; toks.push(Token::S(s)); continue;
        }
        if c.is_ascii_alphabetic() || c == '_' || c == '$' {
            let s = i; while i < len && (src[i].is_ascii_alphanumeric() || src[i] == '_' || src[i] == '$') { i += 1; }
            toks.push(Token::I(src[s..i].iter().collect())); continue;
        }
        let c3: String = if i + 2 < len { src[i..i+3].iter().collect() } else { "".into() };
        let c2: String = if i + 1 < len { src[i..i+2].iter().collect() } else { "".into() };
        if ["===", "!==", ">>>", "**=", "<<=", ">>="].contains(&c3.as_str()) { toks.push(Token::P(c3)); i += 3; continue; }
        if ["==", "!=", "<=", ">=", "&&", "||", "+=", "-=", "*=", "/=", "%=", "**", "++", "--", "=>", "<<", ">>", "&=", "|=", "^="].contains(&c2.as_str()) { toks.push(Token::P(c2)); i += 2; continue; }
        if "+-*/%=<>!&|^~?:.;,(){}[]".contains(c) { toks.push(Token::P(c.to_string())); i += 1; continue; }
        panic!("Unexpected char: {} at pos {}", c, i);
    }
    toks.push(Token::E); toks
}

// ===== Parser =====

struct Parser { toks: Vec<Token>, pos: usize }
impl Parser {
    fn pk(&self) -> &Token { &self.toks[self.pos] }
    fn nx(&mut self) -> Token { let t = self.toks[self.pos].clone(); self.pos += 1; t }
    fn is_p(&self, v: &str) -> bool { if let Token::P(s) = self.pk() { s == v } else { false } }
    fn is_i(&self, v: &str) -> bool { if let Token::I(s) = self.pk() { s == v } else { false } }
    fn is_e(&self) -> bool { matches!(self.pk(), Token::E) }
    fn eat_p(&mut self, v: &str) -> bool { if self.is_p(v) { self.pos += 1; true } else { false } }
    fn eat_i(&mut self, v: &str) -> bool { if self.is_i(v) { self.pos += 1; true } else { false } }
    fn ex_p(&mut self, v: &str) { if !self.eat_p(v) { panic!("Expected P {}", v); } }
    fn ex_i(&mut self, v: &str) { if !self.eat_i(v) { panic!("Expected I {}", v); } }
    fn ex_any_i(&mut self) -> String { if let Token::I(s) = self.nx() { s } else { panic!("Expected Identifier"); } }

    fn parse_program(&mut self) -> Node {
        let mut b = vec![]; while !self.is_e() { b.push(self.parse_stmt()); } Node::Prog(b)
    }

    fn parse_stmt(&mut self) -> Node {
        if self.is_p("{") { return self.parse_block(); }
        if self.eat_p(";") { return Node::Empty; }
        if self.is_i("var") || self.is_i("let") || self.is_i("const") { self.nx(); return self.parse_var_decl(true); }
        if self.eat_i("function") {
            let name = self.ex_any_i(); let params = self.parse_params();
            let body = if let Node::Block(b) = self.parse_block() { b } else { vec![] };
            return Node::FuncDecl { name, params, body };
        }
        if self.eat_i("return") {
            let a = if !self.is_p(";") && !self.is_p("}") && !self.is_e() { Some(Box::new(self.parse_expr())) } else { None };
            self.eat_p(";"); return Node::Return(a);
        }
        if self.eat_i("if") {
            self.ex_p("("); let t = self.parse_expr(); self.ex_p(")");
            let c = self.parse_stmt(); let a = if self.eat_i("else") { Some(Box::new(self.parse_stmt())) } else { None };
            return Node::If { test: Box::new(t), cons: Box::new(c), alt: a };
        }
        if self.eat_i("while") {
            self.ex_p("("); let t = self.parse_expr(); self.ex_p(")"); return Node::While { test: Box::new(t), body: Box::new(self.parse_stmt()) };
        }
        if self.eat_i("do") {
            let b = self.parse_stmt(); self.ex_i("while"); self.ex_p("("); let t = self.parse_expr(); self.ex_p(")"); self.eat_p(";");
            return Node::DoWhile { test: Box::new(t), body: Box::new(b) };
        }
        if self.eat_i("for") {
            self.ex_p("(");
            let saved = self.pos;
            if self.is_i("var") || self.is_i("let") || self.is_i("const") {
                let kind = if let Token::I(k) = self.nx() { k } else { "".into() };
                if let Token::I(name) = self.pk() {
                    let nm = name.clone(); self.pos += 1;
                    if self.eat_i("in") { let o = self.parse_expr(); self.ex_p(")"); return Node::ForIn { kind: Some(kind), name: nm, obj: Box::new(o), body: Box::new(self.parse_stmt()) }; }
                    if self.eat_i("of") { let o = self.parse_expr(); self.ex_p(")"); return Node::ForOf { kind: Some(kind), name: nm, iter: Box::new(o), body: Box::new(self.parse_stmt()) }; }
                }
                self.pos = saved;
            } else if let Token::I(nm) = self.pk() {
                let name = nm.clone(); self.pos += 1;
                if self.eat_i("in") { let o = self.parse_expr(); self.ex_p(")"); return Node::ForIn { kind: None, name, obj: Box::new(o), body: Box::new(self.parse_stmt()) }; }
                self.pos = saved;
            }
            let init = if !self.is_p(";") {
                if self.is_i("var") || self.is_i("let") || self.is_i("const") { self.nx(); Some(Box::new(self.parse_var_decl(false))) } else { Some(Box::new(self.parse_expr())) }
            } else { None };
            self.ex_p(";"); let test = if !self.is_p(";") { Some(Box::new(self.parse_expr())) } else { None };
            self.ex_p(";"); let update = if !self.is_p(")") { Some(Box::new(self.parse_expr())) } else { None };
            self.ex_p(")"); return Node::For { init, test, update, body: Box::new(self.parse_stmt()) };
        }
        if self.eat_i("break") { let l = if let Token::I(s) = self.pk() { let v=s.clone(); self.pos+=1; Some(v) } else { None }; self.eat_p(";"); return Node::Break(l); }
        if self.eat_i("continue") { let l = if let Token::I(s) = self.pk() { let v=s.clone(); self.pos+=1; Some(v) } else { None }; self.eat_p(";"); return Node::Cont(l); }
        if self.eat_i("switch") {
            self.ex_p("("); let d = self.parse_expr(); self.ex_p(")"); self.ex_p("{");
            let mut cases = vec![];
            while !self.is_p("}") {
                if self.eat_i("case") { let t = self.parse_expr(); self.ex_p(":"); let mut b = vec![]; while !self.is_p("}") && !self.is_i("case") && !self.is_i("default") { b.push(self.parse_stmt()); } cases.push((Some(Box::new(t)), b)); }
                else if self.eat_i("default") { self.ex_p(":"); let mut b = vec![]; while !self.is_p("}") && !self.is_i("case") && !self.is_i("default") { b.push(self.parse_stmt()); } cases.push((None, b)); }
            }
            self.ex_p("}"); return Node::Switch { disc: Box::new(d), cases };
        }
        if self.eat_i("throw") { let a = self.parse_expr(); self.eat_p(";"); return Node::Throw(Box::new(a)); }
        if self.eat_i("try") {
            let block = if let Node::Block(b) = self.parse_block() { b } else { vec![] };
            let mut param = None; let mut catch_body = None;
            if self.eat_i("catch") { if self.eat_p("(") { param = Some(self.ex_any_i()); self.ex_p(")"); } catch_body = Some(if let Node::Block(b) = self.parse_block() { b } else { vec![] }); }
            let fin = if self.eat_i("finally") { Some(if let Node::Block(b) = self.parse_block() { b } else { vec![] }) } else { None };
            return Node::Try { block, param, catch_body, fin };
        }
        if let Token::I(lb) = self.pk() {
            if self.pos + 1 < self.toks.length() && matches!(&self.toks[self.pos+1], Token::P(p) if p == ":") {
                let l = lb.clone(); self.pos += 2; return Node::Label { label: l, body: Box::new(self.parse_stmt()) };
            }
        }
        let expr = self.parse_expr(); self.eat_p(";"); Node::ExprStmt(Box::new(expr))
    }

    fn parse_block(&mut self) -> Node {
        self.ex_p("{"); let mut b = vec![]; while !self.is_p("}") { b.push(self.parse_stmt()); } self.ex_p("}"); Node::Block(b)
    }

    fn parse_var_decl(&mut self, semi: bool) -> Node {
        let mut decls = vec![];
        loop {
            let name = self.ex_any_i(); let init = if self.eat_p("=") { Some(self.parse_assign()) } else { None };
            decls.push((name, init)); if !self.eat_p(",") { break; }
        }
        if semi { self.eat_p(";"); } Node::Var { decls }
    }

    fn parse_params(&mut self) -> Vec<String> {
        self.ex_p("("); let mut p = vec![];
        if !self.is_p(")") { loop { p.push(self.ex_any_i()); if !self.eat_p(",") { break; } } }
        self.ex_p(")"); p
    }

    fn get_prec(&self) -> i32 {
        if let Token::P(v) = self.pk() {
            match v.as_str() {
                "||" => 1, "&&" => 2, "|" => 3, "^" => 4, "&" => 5,
                "==" | "!=" | "===" | "!==" => 6,
                "<" | ">" | "<=" | ">=" => 7,
                "<<" | ">>" | ">>>" => 8,
                "+" | "-" => 9,
                "*" | "/" | "%" => 10,
                "**" => 11,
                _ => -1
            }
        } else if let Token::I(v) = self.pk() {
            if v == "instanceof" || v == "in" { return 7; }
            -1
        } else {
            -1
        }
    }

    fn parse_expr(&mut self) -> Node {
        let mut e = self.parse_assign(); while self.eat_p(",") { e = Node::Seq { left: Box::new(e), right: Box::new(self.parse_assign()) }; } e
    }

    fn parse_assign(&mut self) -> Node {
        let left = self.parse_ternary();
        for op in ["=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", ">>>=", "&=", "|=", "^=", "**="] {
            if self.eat_p(op) { return Node::Assign { op: op.into(), left: Box::new(left), right: Box::new(self.parse_assign()) }; }
        }
        left
    }

    fn parse_ternary(&mut self) -> Node {
        let e = self.parse_bin(1);
        if self.eat_p("?") { let c = self.parse_assign(); self.ex_p(":"); return Node::Cond { test: Box::new(e), cons: Box::new(c), alt: Box::new(self.parse_assign()) }; }
        e
    }

    fn parse_bin(&mut self, min_p: i32) -> Node {
        let mut left = self.parse_unary();
        loop {
            let p = self.get_prec(); if p < min_p { break; }
            
            // 這裡修正了嚴重的 BUG：只提取一次 Token 避免吞掉兩次
            let tk = self.nx();
            let op = if let Token::P(v) = tk { v } else if let Token::I(v) = tk { v } else { unreachable!() };
            
            let right = self.parse_bin(p + if op == "**" { 0 } else { 1 });
            if op == "||" || op == "&&" { left = Node::Logical { op, left: Box::new(left), right: Box::new(right) }; }
            else { left = Node::Binary { op, left: Box::new(left), right: Box::new(right) }; }
        }
        left
    }

    fn parse_unary(&mut self) -> Node {
        for op in ["!", "-", "+", "~"] { if self.eat_p(op) { return Node::Unary { op: op.into(), arg: Box::new(self.parse_unary()), prefix: true }; } }
        for op in ["++", "--"] { if self.eat_p(op) { return Node::Update { op: op.into(), arg: Box::new(self.parse_unary()), prefix: true }; } }
        for op in ["typeof", "void", "delete"] { if self.eat_i(op) { return Node::Unary { op: op.into(), arg: Box::new(self.parse_unary()), prefix: true }; } }
        if self.eat_i("new") {
            if self.is_i("new") { return Node::New { callee: Box::new(self.parse_unary()), args: vec![] }; }
            let prim = self.parse_primary();
            let callee = self.parse_member_only(prim);
            let mut args = vec![];
            if self.eat_p("(") { if !self.is_p(")") { loop { args.push(self.parse_assign()); if !self.eat_p(",") { break; } } } self.ex_p(")"); }
            return Node::New { callee: Box::new(callee), args };
        }
        self.parse_postfix()
    }

    fn parse_member_only(&mut self, mut expr: Node) -> Node {
        loop {
            if self.eat_p(".") { expr = Node::Member { obj: Box::new(expr), prop: Box::new(Node::Id(self.ex_any_i())), computed: false }; }
            else if self.eat_p("[") { let p = self.parse_expr(); self.ex_p("]"); expr = Node::Member { obj: Box::new(expr), prop: Box::new(p), computed: true }; }
            else { break; }
        }
        expr
    }

    fn parse_postfix(&mut self) -> Node {
        let e = self.parse_call_member();
        for op in ["++", "--"] { if self.eat_p(op) { return Node::Update { op: op.into(), arg: Box::new(e), prefix: false }; } }
        e
    }

    fn parse_call_member(&mut self) -> Node {
        let mut e = self.parse_primary();
        loop {
            if self.eat_p("(") { let mut a = vec![]; if !self.is_p(")") { loop { a.push(self.parse_assign()); if !self.eat_p(",") { break; } } } self.ex_p(")"); e = Node::Call { callee: Box::new(e), args: a }; }
            else if self.eat_p(".") { e = Node::Member { obj: Box::new(e), prop: Box::new(Node::Id(self.ex_any_i())), computed: false }; }
            else if self.eat_p("[") { let p = self.parse_expr(); self.ex_p("]"); e = Node::Member { obj: Box::new(e), prop: Box::new(p), computed: true }; }
            else { break; }
        }
        e
    }

    fn parse_primary(&mut self) -> Node {
        if let Token::N(n) = self.pk() { let v=*n; self.pos+=1; return Node::Num(v); }
        if let Token::S(s) = self.pk() { let v=s.clone(); self.pos+=1; return Node::Str(v); }
        if self.eat_i("true") { return Node::Bool(true); } if self.eat_i("false") { return Node::Bool(false); }
        if self.eat_i("null") { return Node::LitNull; } if self.eat_i("undefined") { return Node::LitUndef; }
        if self.eat_i("this") { return Node::This; }
        if self.eat_i("function") {
            let name = if let Token::I(_) = self.pk() { Some(self.ex_any_i()) } else { None };
            let params = self.parse_params(); let body = if let Node::Block(b) = self.parse_block() { b } else { vec![] };
            return Node::FuncExpr { name, params, body };
        }
        if self.eat_p("[") {
            let mut el = vec![]; while !self.is_p("]") { if self.eat_p(",") { el.push(None); continue; } el.push(Some(self.parse_assign())); if !self.is_p("]") { self.ex_p(","); } }
            self.ex_p("]"); return Node::Array(el);
        }
        if self.eat_p("{") {
            let mut props = vec![];
            while !self.is_p("}") {
                let mut computed = false; let key_expr;
                if let Token::S(s) = self.pk() { key_expr = Node::Str(s.clone()); self.pos+=1; }
                else if let Token::N(n) = self.pk() { key_expr = Node::Str(n.to_string()); self.pos+=1; }
                else if self.eat_p("[") { key_expr = self.parse_assign(); self.ex_p("]"); computed = true; }
                else { key_expr = Node::Id(self.ex_any_i()); }
                if !computed && self.eat_p("(") {
                    let p = self.parse_params(); let b = if let Node::Block(b) = self.parse_block() { b } else { vec![] };
                    let n = if let Node::Id(s) = &key_expr { Some(s.clone()) } else { None };
                    props.push((Box::new(key_expr), Box::new(Node::FuncExpr { name: n, params: p, body: b }), false));
                } else { self.ex_p(":"); props.push((Box::new(key_expr), Box::new(self.parse_assign()), computed)); }
                if !self.is_p("}") { self.eat_p(","); }
            }
            self.ex_p("}"); return Node::Object(props);
        }
        if self.eat_p("(") { let e = self.parse_expr(); self.ex_p(")"); return e; }
        if let Token::I(s) = self.nx() { return Node::Id(s); }
        panic!("Unexpected token at pos {} {:?}", self.pos, self.pk());
    }
}

trait LengthHelper { fn length(&self) -> usize; }
impl LengthHelper for Vec<Token> { fn length(&self) -> usize { self.len() } }

// ===== Engine Helpers =====

impl Env {
    fn get(&self, name: &str) -> Result<Val, Val> {
        if let Some(v) = self.vars.get(name) { Ok(v.clone()) }
        else if let Some(p) = &self.parent { p.borrow().get(name) }
        else { Err(Val::Str(Rc::new(format!("{} is not defined", name)))) }
    }
    fn set(&mut self, name: &str, val: Val) -> Result<(), Val> {
        if self.vars.contains_key(name) { self.vars.insert(name.into(), val); Ok(()) }
        else if let Some(p) = &self.parent { p.borrow_mut().set(name, val) }
        else { Err(Val::Str(Rc::new(format!("{} is not defined", name)))) }
    }
    fn def(&mut self, name: &str, val: Val) { self.vars.insert(name.into(), val); }
    fn has(&self, name: &str) -> bool { self.vars.contains_key(name) || self.parent.as_ref().map_or(false, |p| p.borrow().has(name)) }
}

fn to_num(v: &Val) -> f64 {
    match v { Val::Num(n) => *n, Val::Bool(b) => if *b { 1.0 } else { 0.0 }, Val::Str(s) => s.trim().parse().unwrap_or(std::f64::NAN), Val::Null => 0.0, _ => std::f64::NAN }
}
fn to_int32(v: &Val) -> i32 { to_num(v) as i64 as i32 }
fn to_uint32(v: &Val) -> u32 { to_num(v) as i64 as u32 }
fn to_bool(v: &Val) -> bool {
    match v { Val::Bool(b) => *b, Val::Num(n) => *n != 0.0 && !n.is_nan(), Val::Str(s) => !s.is_empty(), Val::Obj(_) => true, _ => false }
}
fn to_string(v: &Val) -> String {
    match v {
        Val::Undef => "undefined".into(), Val::Null => "null".into(), Val::Bool(b) => b.to_string(),
        Val::Num(n) => if n.is_nan() { "NaN".into() } else if n.is_infinite() { if *n>0.0 { "Infinity".into() } else { "-Infinity".into() } } else if n.fract() == 0.0 { format!("{}", *n as i64) } else { format!("{}", n) },
        Val::Str(s) => s.to_string(), Val::Obj(o) => match o.borrow().typ { ObjType::Array(_) => "[Array]".into(), ObjType::Function(_) | ObjType::Native(_) | ObjType::Bound(..) => "[Function]".into(), _ => "[object Object]".into() }
    }
}
fn type_of(v: &Val) -> String {
    match v { Val::Undef => "undefined".into(), Val::Null => "object".into(), Val::Bool(_) => "boolean".into(), Val::Num(_) => "number".into(), Val::Str(_) => "string".into(), Val::Obj(o) => match o.borrow().typ { ObjType::Function(_) | ObjType::Native(_) | ObjType::Bound(..) => "function".into(), _ => "object".into() } }
}
fn is_eq(l: &Val, r: &Val, strict: bool) -> bool {
    match (l, r) {
        (Val::Undef, Val::Undef) | (Val::Null, Val::Null) => true,
        (Val::Undef, Val::Null) | (Val::Null, Val::Undef) => !strict,
        (Val::Num(a), Val::Num(b)) => a == b || (a.is_nan() && b.is_nan()),
        (Val::Str(a), Val::Str(b)) => a == b,
        (Val::Bool(a), Val::Bool(b)) => a == b,
        (Val::Obj(a), Val::Obj(b)) => Rc::ptr_eq(a, b),
        _ => false,
    }
}
fn cmp_lt(l: &Val, r: &Val) -> bool { if let (Val::Str(a), Val::Str(b)) = (l, r) { a < b } else { to_num(l) < to_num(r) } }
fn cmp_gt(l: &Val, r: &Val) -> bool { if let (Val::Str(a), Val::Str(b)) = (l, r) { a > b } else { to_num(l) > to_num(r) } }

fn get_prop(val: &Val, prop: &str, rt: &mut Runtime) -> Result<Val, Val> {
    if let Val::Str(s) = val {
        if prop == "length" { return Ok(Val::Num(s.chars().count() as f64)); }
        if let Ok(idx) = prop.parse::<usize>() { if let Some(c) = s.chars().nth(idx) { return Ok(Val::Str(Rc::new(c.to_string()))); } }
        return get_prop(&rt.string_proto.clone(), prop, rt);
    }
    if let Val::Obj(o) = val {
        let obj = o.borrow();
        if let ObjType::Array(ref arr) = obj.typ {
            if prop == "length" { return Ok(Val::Num(arr.len() as f64)); }
            if let Ok(idx) = prop.parse::<usize>() { if idx < arr.len() { return Ok(arr[idx].clone()); } else { return Ok(Val::Undef); } }
        }
        if let Some(v) = obj.props.get(prop) { return Ok(v.clone()); }
        if let Val::Obj(_) = obj.proto { drop(obj); return get_prop(&o.borrow().proto, prop, rt); }
    }
    Ok(Val::Undef)
}
fn set_prop(val: &Val, prop: &str, v: Val, _rt: &mut Runtime) -> Result<(), Val> {
    if let Val::Obj(o) = val {
        let mut obj = o.borrow_mut();
        if let ObjType::Array(ref mut arr) = obj.typ {
            if prop == "length" { if let Val::Num(n) = v { arr.resize(n as usize, Val::Undef); } return Ok(()); }
            if let Ok(idx) = prop.parse::<usize>() { if idx >= arr.len() { arr.resize(idx + 1, Val::Undef); } arr[idx] = v; return Ok(()); }
        }
        obj.props.insert(prop.into(), v);
    }
    Ok(())
}

fn call_func(func: &Val, this_val: Val, args: Vec<Val>, rt: &mut Runtime) -> Result<Val, Signal> {
    if let Val::Obj(o) = func {
        let typ = o.borrow().typ.clone();
        match typ {
            ObjType::Function(fd) => {
                let local_env = Rc::new(RefCell::new(Env { parent: Some(fd.env.clone()), vars: HashMap::new() }));
                local_env.borrow_mut().def("this", this_val);
                let args_obj = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.array_proto.clone(), typ: ObjType::Array(args.clone()) })));
                local_env.borrow_mut().def("arguments", args_obj);
                for (i, p) in fd.params.iter().enumerate() { local_env.borrow_mut().def(p, args.get(i).cloned().unwrap_or(Val::Undef)); }
                match eval_body(&fd.body, local_env, rt) { Err(Signal::Return(v)) => return Ok(v), Err(e) => return Err(e), Ok(_) => return Ok(Val::Undef) }
            },
            ObjType::Native(nf) => return nf(this_val, args, rt).map_err(Signal::Throw),
            ObjType::Bound(orig, bound_this, bound_args) => {
                let mut final_args = bound_args.clone(); final_args.extend(args);
                return call_func(&orig, bound_this.clone(), final_args, rt);
            },
            _ => return Err(Signal::Throw(Val::Str(Rc::new("Not a function".into())))),
        }
    }
    Err(Signal::Throw(Val::Str(Rc::new("Not a function".into()))))
}

fn construct(ctor: &Val, args: Vec<Val>, rt: &mut Runtime) -> Result<Val, Signal> {
    let mut actual_ctor = ctor.clone(); let mut final_args = args.clone();
    if let Val::Obj(o) = ctor {
        if let ObjType::Bound(orig, _, bargs) = &o.borrow().typ {
            actual_ctor = orig.clone(); final_args = bargs.clone(); final_args.extend(args); // override bound `this`
        }
    }
    let proto = get_prop(&actual_ctor, "prototype", rt).unwrap_or(rt.object_proto.clone());
    let new_obj = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto, typ: ObjType::Object })));
    let res = call_func(&actual_ctor, new_obj.clone(), final_args, rt)?;
    if let Val::Obj(_) = res { Ok(res) } else { Ok(new_obj) }
}

fn make_func(name: Option<String>, params: Vec<String>, body: Vec<Node>, env: Rc<RefCell<Env>>, rt: &mut Runtime) -> Val {
    let f = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.func_proto.clone(), typ: ObjType::Function(FuncData { name: name.clone(), params, body, env }) })));
    let proto = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.object_proto.clone(), typ: ObjType::Object })));
    set_prop(&f, "prototype", proto, rt).unwrap();
    if let Some(n) = name { set_prop(&f, "name", Val::Str(Rc::new(n)), rt).unwrap(); }
    f
}

// ===== Evaluator =====

fn eval_body(body: &Vec<Node>, env: Rc<RefCell<Env>>, rt: &mut Runtime) -> Result<(), Signal> {
    for n in body { if let Node::FuncDecl { name, params, body } = n { let fn_val = make_func(Some(name.clone()), params.clone(), body.clone(), env.clone(), rt); env.borrow_mut().def(name, fn_val); } }
    for n in body { eval_node(n, env.clone(), rt)?; } Ok(())
}

fn eval_node(node: &Node, env: Rc<RefCell<Env>>, rt: &mut Runtime) -> Result<(), Signal> {
    match node {
        Node::Prog(b) => eval_body(b, env, rt),
        Node::Block(b) => eval_body(b, Rc::new(RefCell::new(Env { parent: Some(env), vars: HashMap::new() })), rt),
        Node::Empty => Ok(()),
        Node::ExprStmt(e) => { eval_expr(e, env, rt)?; Ok(()) },
        Node::Var { decls } => { for (n, i) in decls { let v = if let Some(e) = i { eval_expr(e, env.clone(), rt)? } else { Val::Undef }; env.borrow_mut().def(n, v); } Ok(()) },
        Node::FuncDecl { .. } => Ok(()), // Handled in eval_body
        Node::Return(a) => Err(Signal::Return(if let Some(e) = a { eval_expr(e, env, rt)? } else { Val::Undef })),
        Node::If { test, cons, alt } => { if to_bool(&eval_expr(test, env.clone(), rt)?) { eval_node(cons, env, rt) } else if let Some(a) = alt { eval_node(a, env, rt) } else { Ok(()) } },
        Node::While { test, body } => { loop { if !to_bool(&eval_expr(test, env.clone(), rt)?) { break; } match eval_node(body, env.clone(), rt) { Ok(_) | Err(Signal::Continue(None)) => {}, Err(Signal::Break(None)) => break, Err(e) => return Err(e) } } Ok(()) },
        Node::DoWhile { test, body } => { loop { match eval_node(body, env.clone(), rt) { Ok(_) | Err(Signal::Continue(None)) => {}, Err(Signal::Break(None)) => break, Err(e) => return Err(e) } if !to_bool(&eval_expr(test, env.clone(), rt)?) { break; } } Ok(()) },
        Node::For { init, test, update, body } => {
            let fe = Rc::new(RefCell::new(Env { parent: Some(env), vars: HashMap::new() }));
            if let Some(i) = init { eval_node(i, fe.clone(), rt)?; }
            loop {
                if let Some(t) = test { if !to_bool(&eval_expr(t, fe.clone(), rt)?) { break; } }
                match eval_node(body, fe.clone(), rt) { Ok(_) | Err(Signal::Continue(None)) => {}, Err(Signal::Break(None)) => break, Err(e) => return Err(e) }
                if let Some(u) = update { eval_expr(u, fe.clone(), rt)?; }
            } Ok(())
        },
        Node::ForIn { kind, name, obj, body } => {
            let o = eval_expr(obj, env.clone(), rt)?;
            let mut keys = vec![];
            if let Val::Obj(obj_ref) = o {
                let ob = obj_ref.borrow();
                if let ObjType::Array(ref arr) = ob.typ { for i in 0..arr.len() { keys.push(i.to_string()); } }
                for k in ob.props.keys() { keys.push(k.clone()); }
            }
            for k in keys {
                let fe = if kind.is_some() { Rc::new(RefCell::new(Env { parent: Some(env.clone()), vars: HashMap::new() })) } else { env.clone() };
                if kind.is_some() { fe.borrow_mut().def(name, Val::Str(Rc::new(k))); } else { fe.borrow_mut().set(name, Val::Str(Rc::new(k))).unwrap(); }
                match eval_node(body, fe, rt) { Ok(_) | Err(Signal::Continue(None)) => {}, Err(Signal::Break(None)) => break, Err(e) => return Err(e) }
            } Ok(())
        },
        Node::ForOf { kind, name, iter, body } => {
            let o = eval_expr(iter, env.clone(), rt)?;
            if let Val::Obj(obj_ref) = o {
                if let ObjType::Array(ref arr) = obj_ref.borrow().typ {
                    for v in arr {
                        let fe = if kind.is_some() { Rc::new(RefCell::new(Env { parent: Some(env.clone()), vars: HashMap::new() })) } else { env.clone() };
                        if kind.is_some() { fe.borrow_mut().def(name, v.clone()); } else { fe.borrow_mut().set(name, v.clone()).unwrap(); }
                        match eval_node(body, fe, rt) { Ok(_) | Err(Signal::Continue(None)) => {}, Err(Signal::Break(None)) => break, Err(e) => return Err(e) }
                    }
                }
            } Ok(())
        },
        Node::Break(l) => Err(Signal::Break(l.clone())), Node::Cont(l) => Err(Signal::Continue(l.clone())),
        Node::Switch { disc, cases } => {
            let dv = eval_expr(disc, env.clone(), rt)?; let mut matched = false;
            for (t, b) in cases {
                if !matched { if let Some(tc) = t { if is_eq(&eval_expr(tc, env.clone(), rt)?, &dv, true) { matched = true; } } else { matched = true; } }
                if matched { match eval_body(b, env.clone(), rt) { Err(Signal::Break(None)) => return Ok(()), Err(e) => return Err(e), _ => {} } }
            } Ok(())
        },
        Node::Throw(a) => Err(Signal::Throw(eval_expr(a, env, rt)?)),
        Node::Try { block, param, catch_body, fin } => {
            let res = eval_body(block, env.clone(), rt);
            let mut throw_val = None;
            if let Err(sig) = res { match sig { Signal::Throw(v) => throw_val = Some(v), _ => { if let Some(f) = fin { eval_body(f, env, rt)?; } return Err(sig); } } }
            if let Some(v) = throw_val {
                if let Some(cb) = catch_body {
                    let ce = Rc::new(RefCell::new(Env { parent: Some(env.clone()), vars: HashMap::new() }));
                    if let Some(p) = param { ce.borrow_mut().def(p, v); }
                    if let Err(sig) = eval_body(cb, ce, rt) { if let Some(f) = fin { eval_body(f, env, rt)?; } return Err(sig); }
                }
            }
            if let Some(f) = fin { eval_body(f, env, rt)?; } Ok(())
        },
        Node::Label { label, body } => {
            match eval_node(body, env, rt) { Err(Signal::Break(Some(ref l))) if l == label => Ok(()), Err(Signal::Continue(Some(ref l))) if l == label => Ok(()), res => res }
        },
        _ => panic!("Unknown stmt"),
    }
}

fn eval_expr(node: &Node, env: Rc<RefCell<Env>>, rt: &mut Runtime) -> Result<Val, Signal> {
    match node {
        Node::Num(n) => Ok(Val::Num(*n)), Node::Str(s) => Ok(Val::Str(Rc::new(s.clone()))), Node::Bool(b) => Ok(Val::Bool(*b)), Node::LitNull => Ok(Val::Null), Node::LitUndef => Ok(Val::Undef),
        Node::This => Ok(env.borrow().get("this").unwrap_or(Val::Undef)),
        Node::Id(n) => env.borrow().get(n).map_err(Signal::Throw),
        Node::Array(el) => {
            let mut arr = vec![]; for e in el { if let Some(x) = e { arr.push(eval_expr(x, env.clone(), rt)?); } else { arr.push(Val::Undef); } }
            Ok(Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.array_proto.clone(), typ: ObjType::Array(arr) }))))
        },
        Node::Object(props) => {
            let obj = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.object_proto.clone(), typ: ObjType::Object })));
            for (ke, ve, comp) in props {
                let k = if *comp { to_string(&eval_expr(ke, env.clone(), rt)?) } else if let Node::Str(s) = &**ke { s.clone() } else if let Node::Id(s) = &**ke { s.clone() } else { "".into() };
                let v = eval_expr(ve, env.clone(), rt)?; set_prop(&obj, &k, v, rt).map_err(Signal::Throw)?;
            } Ok(obj)
        },
        Node::FuncExpr { name, params, body } => {
            let fn_val = make_func(name.clone(), params.clone(), body.clone(), env.clone(), rt);
            if let Some(n) = name { let fe = Rc::new(RefCell::new(Env { parent: Some(env), vars: HashMap::new() })); fe.borrow_mut().def(n, fn_val.clone()); }
            Ok(fn_val)
        },
        Node::Assign { op, left, right } => {
            let mut val = eval_expr(right, env.clone(), rt)?;
            if op != "=" {
                let cur = eval_lval(left, env.clone(), rt)?;
                if op == "+=" {
                    if let (Val::Str(s), _) = (&cur, &val) { val = Val::Str(Rc::new(format!("{}{}", s, to_string(&val)))); }
                    else if let (_, Val::Str(s)) = (&cur, &val) { val = Val::Str(Rc::new(format!("{}{}", to_string(&cur), s))); }
                    else { val = Val::Num(to_num(&cur) + to_num(&val)); }
                } else if op == "-=" { val = Val::Num(to_num(&cur) - to_num(&val)); } else if op == "*=" { val = Val::Num(to_num(&cur) * to_num(&val)); } else if op == "/=" { val = Val::Num(to_num(&cur) / to_num(&val)); } else if op == "%=" { val = Val::Num(to_num(&cur) % to_num(&val)); } else if op == "**=" { val = Val::Num(to_num(&cur).powf(to_num(&val))); } else if op == "<<=" { val = Val::Num((to_int32(&cur) << (to_uint32(&val) & 31)) as f64); } else if op == ">>=" { val = Val::Num((to_int32(&cur) >> (to_uint32(&val) & 31)) as f64); } else if op == ">>>=" { val = Val::Num((to_uint32(&cur) >> (to_uint32(&val) & 31)) as f64); } else if op == "&=" { val = Val::Num((to_int32(&cur) & to_int32(&val)) as f64); } else if op == "|=" { val = Val::Num((to_int32(&cur) | to_int32(&val)) as f64); } else if op == "^=" { val = Val::Num((to_int32(&cur) ^ to_int32(&val)) as f64); }
            }
            set_lval(left, env, val.clone(), rt)?; Ok(val)
        },
        Node::Binary { op, left, right } => {
            let l = eval_expr(left, env.clone(), rt)?; let r = eval_expr(right, env, rt)?;
            match op.as_str() {
                "+" => { if let (Val::Str(s), _) = (&l, &r) { Ok(Val::Str(Rc::new(format!("{}{}", s, to_string(&r))))) } else if let (_, Val::Str(s)) = (&l, &r) { Ok(Val::Str(Rc::new(format!("{}{}", to_string(&l), s)))) } else { Ok(Val::Num(to_num(&l) + to_num(&r))) } },
                "-" => Ok(Val::Num(to_num(&l) - to_num(&r))), "*" => Ok(Val::Num(to_num(&l) * to_num(&r))), "/" => Ok(Val::Num(to_num(&l) / to_num(&r))), "%" => Ok(Val::Num(to_num(&l) % to_num(&r))), "**" => Ok(Val::Num(to_num(&l).powf(to_num(&r)))),
                "==" => Ok(Val::Bool(is_eq(&l, &r, false))), "!=" => Ok(Val::Bool(!is_eq(&l, &r, false))), "===" => Ok(Val::Bool(is_eq(&l, &r, true))), "!==" => Ok(Val::Bool(!is_eq(&l, &r, true))),
                "<" => Ok(Val::Bool(cmp_lt(&l, &r))), ">" => Ok(Val::Bool(cmp_gt(&l, &r))), "<=" => Ok(Val::Bool(!cmp_gt(&l, &r))), ">=" => Ok(Val::Bool(!cmp_lt(&l, &r))),
                "<<" => Ok(Val::Num((to_int32(&l) << (to_uint32(&r) & 31)) as f64)), ">>" => Ok(Val::Num((to_int32(&l) >> (to_uint32(&r) & 31)) as f64)), ">>>" => Ok(Val::Num((to_uint32(&l) >> (to_uint32(&r) & 31)) as f64)),
                "&" => Ok(Val::Num((to_int32(&l) & to_int32(&r)) as f64)), "|" => Ok(Val::Num((to_int32(&l) | to_int32(&r)) as f64)), "^" => Ok(Val::Num((to_int32(&l) ^ to_int32(&r)) as f64)),
                "instanceof" => { let mut curr = l; let ctor_proto = get_prop(&r, "prototype", rt).unwrap_or(Val::Undef); for _ in 0..100 { if let Val::Obj(o) = curr { if is_eq(&o.borrow().proto, &ctor_proto, true) { return Ok(Val::Bool(true)); } curr = o.borrow().proto.clone(); } else { break; } } Ok(Val::Bool(false)) },
                "in" => { if let Val::Obj(o) = r { Ok(Val::Bool(o.borrow().props.contains_key(&to_string(&l)))) } else { Ok(Val::Bool(false)) } },
                _ => Err(Signal::Throw(Val::Str(Rc::new("Unknown bin op".into()))))
            }
        },
        Node::Logical { op, left, right } => {
            let l = eval_expr(left, env.clone(), rt)?;
            if op == "&&" { if !to_bool(&l) { return Ok(l); } return eval_expr(right, env, rt); }
            if op == "||" { if to_bool(&l) { return Ok(l); } return eval_expr(right, env, rt); } unreachable!()
        },
        Node::Unary { op, arg, .. } => {
            if op == "typeof" { if let Node::Id(n) = &**arg { return Ok(Val::Str(Rc::new(type_of(&env.borrow().get(n).unwrap_or(Val::Undef))))); } return Ok(Val::Str(Rc::new(type_of(&eval_expr(arg, env, rt)?)))); }
            let a = eval_expr(arg, env.clone(), rt)?;
            match op.as_str() { "!" => Ok(Val::Bool(!to_bool(&a))), "-" => Ok(Val::Num(-to_num(&a))), "+" => Ok(Val::Num(to_num(&a))), "~" => Ok(Val::Num(!to_int32(&a) as f64)), "void" => Ok(Val::Undef), "delete" => { if let Node::Member{obj, prop, computed} = &**arg { let o = eval_expr(obj, env.clone(), rt)?; let k = if *computed { eval_expr(prop, env, rt)? } else { if let Node::Id(n) = &**prop { Val::Str(Rc::new(n.clone())) } else { Val::Undef } }; if let Val::Obj(ob) = o { ob.borrow_mut().props.remove(&to_string(&k)); } } Ok(Val::Bool(true)) }, _ => unreachable!() }
        },
        Node::Update { op, arg, prefix } => {
            let cur = eval_lval(arg, env.clone(), rt)?; let nv = Val::Num(to_num(&cur) + if op == "++" { 1.0 } else { -1.0 });
            set_lval(arg, env, nv.clone(), rt)?; Ok(if *prefix { nv } else { cur })
        },
        Node::Cond { test, cons, alt } => if to_bool(&eval_expr(test, env.clone(), rt)?) { eval_expr(cons, env, rt) } else { eval_expr(alt, env, rt) },
        Node::Seq { left, right } => { eval_expr(left, env.clone(), rt)?; eval_expr(right, env, rt) },
        Node::Call { callee, args } => {
            let mut arg_vals = vec![]; for a in args { arg_vals.push(eval_expr(a, env.clone(), rt)?); }
            if let Node::Member { obj, prop, computed } = &**callee {
                let o = eval_expr(obj, env.clone(), rt)?;
                let k = if *computed { eval_expr(prop, env, rt)? } else { if let Node::Id(n) = &**prop { Val::Str(Rc::new(n.clone())) } else { Val::Undef } };
                let fn_val = get_prop(&o, &to_string(&k), rt).map_err(Signal::Throw)?;
                return call_func(&fn_val, o, arg_vals, rt);
            }
            let fn_val = eval_expr(callee, env, rt)?; call_func(&fn_val, Val::Undef, arg_vals, rt)
        },
        Node::New { callee, args } => {
            let ctor = eval_expr(callee, env.clone(), rt)?; let mut arg_vals = vec![]; for a in args { arg_vals.push(eval_expr(a, env.clone(), rt)?); }
            construct(&ctor, arg_vals, rt)
        },
        Node::Member { obj, prop, computed } => {
            let o = eval_expr(obj, env.clone(), rt)?; let k = if *computed { eval_expr(prop, env, rt)? } else { if let Node::Id(n) = &**prop { Val::Str(Rc::new(n.clone())) } else { Val::Undef } };
            get_prop(&o, &to_string(&k), rt).map_err(Signal::Throw)
        },
        _ => panic!("Unknown expr"),
    }
}

fn eval_lval(node: &Node, env: Rc<RefCell<Env>>, rt: &mut Runtime) -> Result<Val, Signal> {
    match node {
        Node::Id(n) => env.borrow().get(n).map_err(Signal::Throw),
        Node::Member { obj, prop, computed } => { let o = eval_expr(obj, env.clone(), rt)?; let k = if *computed { eval_expr(prop, env, rt)? } else { if let Node::Id(n) = &**prop { Val::Str(Rc::new(n.clone())) } else { Val::Undef } }; get_prop(&o, &to_string(&k), rt).map_err(Signal::Throw) },
        _ => panic!("Invalid lvalue")
    }
}
fn set_lval(node: &Node, env: Rc<RefCell<Env>>, val: Val, rt: &mut Runtime) -> Result<(), Signal> {
    match node {
        Node::Id(n) => { if env.borrow().has(n) { env.borrow_mut().set(n, val).unwrap(); } else { env.borrow_mut().def(n, val); } Ok(()) },
        Node::Member { obj, prop, computed } => { let o = eval_expr(obj, env.clone(), rt)?; let k = if *computed { eval_expr(prop, env, rt)? } else { if let Node::Id(n) = &**prop { Val::Str(Rc::new(n.clone())) } else { Val::Undef } }; set_prop(&o, &to_string(&k), val, rt).map_err(Signal::Throw)?; Ok(()) },
        _ => panic!("Invalid lvalue")
    }
}

// ===== Native Built-ins =====

fn make_native(func: fn(Val, Vec<Val>, &mut Runtime) -> Result<Val, Val>, rt: &Runtime) -> Val {
    Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.func_proto.clone(), typ: ObjType::Native(func) })))
}

fn native_dummy(this: Val, _a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> { Ok(this) }
fn native_log(_: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> { println!("{}", a.iter().map(to_string).collect::<Vec<_>>().join(" ")); Ok(Val::Undef) }
fn native_error(_: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> { eprintln!("{}", a.iter().map(to_string).collect::<Vec<_>>().join(" ")); Ok(Val::Undef) }
fn native_exit(_: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> { std::process::exit(to_int32(&a.get(0).cloned().unwrap_or(Val::Num(0.0)))); }
fn native_readfilesync(_: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> { Ok(Val::Str(Rc::new(std::fs::read_to_string(to_string(&a.get(0).cloned().unwrap_or(Val::Undef))).map_err(|e| Val::Str(Rc::new(e.to_string())))?))) }

fn native_substring(this: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> {
    let s = to_string(&this); let start = to_int32(&a.get(0).cloned().unwrap_or(Val::Num(0.0))).max(0) as usize; let end = to_int32(&a.get(1).cloned().unwrap_or(Val::Num(s.chars().count() as f64))).max(0) as usize;
    Ok(Val::Str(Rc::new(s.chars().skip(start).take(end.saturating_sub(start)).collect())))
}
fn native_indexof(this: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> {
    let s = to_string(&this); let sub = to_string(&a.get(0).cloned().unwrap_or(Val::Undef));
    Ok(Val::Num(if let Some(idx) = s.find(&sub) { s[..idx].chars().count() as f64 } else { -1.0 }))
}
fn native_push(this: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> {
    if let Val::Obj(o) = this { if let ObjType::Array(ref mut arr) = o.borrow_mut().typ { arr.extend(a); return Ok(Val::Num(arr.len() as f64)); } } Ok(Val::Num(0.0))
}
fn native_slice(this: Val, a: Vec<Val>, rt: &mut Runtime) -> Result<Val, Val> {
    let mut res = vec![];
    if let Val::Obj(o) = &this { if let ObjType::Array(ref arr) = o.borrow().typ {
        let start = to_int32(&a.get(0).cloned().unwrap_or(Val::Num(0.0))).max(0) as usize;
        let end = to_int32(&a.get(1).cloned().unwrap_or(Val::Num(arr.len() as f64))).max(0) as usize;
        for i in start..end.min(arr.len()) { res.push(arr[i].clone()); }
    } }
    Ok(Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.array_proto.clone(), typ: ObjType::Array(res) }))))
}
fn native_concat(this: Val, a: Vec<Val>, rt: &mut Runtime) -> Result<Val, Val> {
    let mut res = vec![];
    if let Val::Obj(o) = &this { if let ObjType::Array(ref arr) = o.borrow().typ { res.extend(arr.clone()); } }
    for item in a { if let Val::Obj(o) = &item { if let ObjType::Array(ref arr) = o.borrow().typ { res.extend(arr.clone()); continue; } } res.push(item); }
    Ok(Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.array_proto.clone(), typ: ObjType::Array(res) }))))
}
fn native_apply(this: Val, a: Vec<Val>, rt: &mut Runtime) -> Result<Val, Val> {
    let this_arg = a.get(0).cloned().unwrap_or(Val::Undef); let mut cargs = vec![];
    if let Some(Val::Obj(o)) = a.get(1) { if let ObjType::Array(ref arr) = o.borrow().typ { cargs = arr.clone(); } }
    call_func(&this, this_arg, cargs, rt).map_err(|e| if let Signal::Throw(v) = e { v } else { Val::Undef })
}
fn native_bind(this: Val, a: Vec<Val>, rt: &mut Runtime) -> Result<Val, Val> {
    let bthis = a.get(0).cloned().unwrap_or(Val::Undef); let bargs = a.into_iter().skip(1).collect();
    Ok(Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.func_proto.clone(), typ: ObjType::Bound(this, bthis, bargs) }))))
}
fn native_hasownprop(this: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> {
    if let Val::Obj(o) = this { let p = to_string(&a.get(0).cloned().unwrap_or(Val::Undef)); let ob = o.borrow(); if let ObjType::Array(ref arr) = ob.typ { if let Ok(i)=p.parse::<usize>(){if i<arr.len(){return Ok(Val::Bool(true));}} if p=="length"{return Ok(Val::Bool(true));} } return Ok(Val::Bool(ob.props.contains_key(&p))); } Ok(Val::Bool(false))
}
fn native_fromcharcode(_this: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> {
    Ok(Val::Str(Rc::new(a.iter().filter_map(|v| std::char::from_u32(to_num(v) as u32)).collect())))
}
fn native_parseint(_this: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> {
    let s = to_string(&a.get(0).cloned().unwrap_or(Val::Undef)); let r = to_uint32(&a.get(1).cloned().unwrap_or(Val::Num(10.0)));
    Ok(Val::Num(i64::from_str_radix(s.trim(), r).map(|x| x as f64).unwrap_or(std::f64::NAN)))
}
fn native_parsefloat(_this: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> { Ok(Val::Num(to_string(&a.get(0).cloned().unwrap_or(Val::Undef)).trim().parse().unwrap_or(std::f64::NAN))) }
fn native_isnan(_this: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> { Ok(Val::Bool(to_num(&a.get(0).cloned().unwrap_or(Val::Undef)).is_nan())) }
fn native_isfinite(_this: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> { Ok(Val::Bool(to_num(&a.get(0).cloned().unwrap_or(Val::Undef)).is_finite())) }
fn native_number(_this: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> { Ok(Val::Num(to_num(&a.get(0).cloned().unwrap_or(Val::Num(0.0))))) }
fn native_string(_this: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> { Ok(Val::Str(Rc::new(to_string(&a.get(0).cloned().unwrap_or(Val::Str(Rc::new("".into()))))))) }
fn native_error_ctor(_this: Val, a: Vec<Val>, rt: &mut Runtime) -> Result<Val, Val> {
    let o = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.error_proto.clone(), typ: ObjType::Object }))); set_prop(&o, "message", a.get(0).cloned().unwrap_or(Val::Undef), rt).unwrap(); Ok(o)
}
fn native_pow(_this: Val, a: Vec<Val>, _rt: &mut Runtime) -> Result<Val, Val> { Ok(Val::Num(to_num(&a.get(0).cloned().unwrap_or(Val::Num(0.0))).powf(to_num(&a.get(1).cloned().unwrap_or(Val::Num(0.0)))))) }
fn native_defprop(_this: Val, a: Vec<Val>, rt: &mut Runtime) -> Result<Val, Val> { let o = a.get(0).cloned().unwrap_or(Val::Undef); set_prop(&o, &to_string(&a.get(1).cloned().unwrap_or(Val::Undef)), get_prop(&a.get(2).cloned().unwrap_or(Val::Undef), "value", rt).unwrap_or(Val::Undef), rt).unwrap(); Ok(o) }
fn native_require(_this: Val, a: Vec<Val>, rt: &mut Runtime) -> Result<Val, Val> {
    let p = to_string(&a.get(0).cloned().unwrap_or(Val::Undef)); let o = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.object_proto.clone(), typ: ObjType::Object })));
    if p == "fs" { set_prop(&o, "readFileSync", make_native(native_readfilesync, rt), rt).unwrap(); } Ok(o)
}

fn create_runtime() -> Runtime {
    let objp = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: Val::Null, typ: ObjType::Object })));
    let funcp = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: objp.clone(), typ: ObjType::Object })));
    let arrp = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: objp.clone(), typ: ObjType::Object })));
    let strp = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: objp.clone(), typ: ObjType::Object })));
    let errp = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: objp.clone(), typ: ObjType::Object })));
    let mut rt = Runtime { object_proto: objp, array_proto: arrp, func_proto: funcp, string_proto: strp, error_proto: errp };
    set_prop(&rt.string_proto.clone(), "substring", make_native(native_substring, &rt), &mut rt).unwrap();
    set_prop(&rt.string_proto.clone(), "indexOf", make_native(native_indexof, &rt), &mut rt).unwrap();
    set_prop(&rt.array_proto.clone(), "push", make_native(native_push, &rt), &mut rt).unwrap();
    set_prop(&rt.array_proto.clone(), "slice", make_native(native_slice, &rt), &mut rt).unwrap();
    set_prop(&rt.array_proto.clone(), "concat", make_native(native_concat, &rt), &mut rt).unwrap();
    set_prop(&rt.func_proto.clone(), "apply", make_native(native_apply, &rt), &mut rt).unwrap();
    set_prop(&rt.func_proto.clone(), "bind", make_native(native_bind, &rt), &mut rt).unwrap();
    set_prop(&rt.object_proto.clone(), "hasOwnProperty", make_native(native_hasownprop, &rt), &mut rt).unwrap();
    rt
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 { eprintln!("Usage: js0i <script> [args...]"); std::process::exit(1); }
    let script_file = &args[1];
    let source = std::fs::read_to_string(script_file).unwrap();
    let mut js_argv = vec!["node".into()]; js_argv.extend(args.into_iter().skip(1));

    let mut rt = create_runtime();
    let env = Rc::new(RefCell::new(Env { parent: None, vars: HashMap::new() }));
    env.borrow_mut().def("undefined", Val::Undef); env.borrow_mut().def("null", Val::Null);
    env.borrow_mut().def("true", Val::Bool(true)); env.borrow_mut().def("false", Val::Bool(false));
    env.borrow_mut().def("NaN", Val::Num(std::f64::NAN)); env.borrow_mut().def("Infinity", Val::Num(std::f64::INFINITY));
    
    let scon = make_native(native_string, &rt); set_prop(&scon, "fromCharCode", make_native(native_fromcharcode, &rt), &mut rt).unwrap(); env.borrow_mut().def("String", scon);
    let ocon = make_native(native_dummy, &rt); set_prop(&ocon, "defineProperty", make_native(native_defprop, &rt), &mut rt).unwrap(); env.borrow_mut().def("Object", ocon);
    let econ = make_native(native_error_ctor, &rt); env.borrow_mut().def("Error", econ.clone()); env.borrow_mut().def("TypeError", econ.clone()); env.borrow_mut().def("SyntaxError", econ.clone()); env.borrow_mut().def("ReferenceError", econ.clone());
    env.borrow_mut().def("Number", make_native(native_number, &rt)); env.borrow_mut().def("Boolean", make_native(native_dummy, &rt)); env.borrow_mut().def("Array", make_native(native_dummy, &rt)); env.borrow_mut().def("Function", make_native(native_dummy, &rt));
    env.borrow_mut().def("parseInt", make_native(native_parseint, &rt)); env.borrow_mut().def("parseFloat", make_native(native_parsefloat, &rt)); env.borrow_mut().def("isNaN", make_native(native_isnan, &rt)); env.borrow_mut().def("isFinite", make_native(native_isfinite, &rt));
    env.borrow_mut().def("require", make_native(native_require, &rt));
    
    let console = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.object_proto.clone(), typ: ObjType::Object })));
    set_prop(&console, "log", make_native(native_log, &rt), &mut rt).unwrap(); set_prop(&console, "error", make_native(native_error, &rt), &mut rt).unwrap(); set_prop(&console, "warn", make_native(native_log, &rt), &mut rt).unwrap(); env.borrow_mut().def("console", console);
    
    let process = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.object_proto.clone(), typ: ObjType::Object })));
    let argv_arr = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.array_proto.clone(), typ: ObjType::Array(js_argv.into_iter().map(|s| Val::Str(Rc::new(s))).collect()) })));
    set_prop(&process, "argv", argv_arr, &mut rt).unwrap(); set_prop(&process, "exit", make_native(native_exit, &rt), &mut rt).unwrap(); env.borrow_mut().def("process", process);
    
    let math = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.object_proto.clone(), typ: ObjType::Object })));
    set_prop(&math, "pow", make_native(native_pow, &rt), &mut rt).unwrap(); env.borrow_mut().def("Math", math);

    // 補上 js0i.js makeGlobal 裡面會用到的各種原生 JS 全域變數
    let json_obj = Val::Obj(Rc::new(RefCell::new(Obj { props: HashMap::new(), proto: rt.object_proto.clone(), typ: ObjType::Object })));
    env.borrow_mut().def("JSON", json_obj);

    let dummy_fn = make_native(native_dummy, &rt);
    let missing_globals = vec![
        "RegExp", "Date", "RangeError", "URIError", "EvalError",
        "encodeURIComponent", "decodeURIComponent", "encodeURI", "decodeURI",
        "setTimeout", "clearTimeout", "setInterval", "clearInterval"
    ];
    for mg in missing_globals {
        env.borrow_mut().def(mg, dummy_fn.clone());
    }

    let mut parser = Parser { toks: tokenize(&source), pos: 0 };
    let ast = parser.parse_program();
    if let Err(e) = eval_node(&ast, env, &mut rt) {
        if let Signal::Throw(v) = e {
            eprintln!("Uncaught Runtime Error: {}", to_string(&v));
        }
    }
}