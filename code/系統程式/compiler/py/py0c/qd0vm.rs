use std::collections::HashMap;
use std::env;
use std::fs;
use std::process;

// =========================================================================
// 型別與資料結構 (Types & Data Structures)
// =========================================================================

type NativeFunc = fn(&[Value]) -> Value;

#[derive(Clone)]
enum Value {
    None,
    Int(i64),
    Float(f64),
    Bool(bool),
    Str(String),
    List(Vec<Value>),
    Closure { label: String, env_id: usize },
    Native(NativeFunc),
}

impl Value {
    fn is_truthy(&self) -> bool {
        match self {
            Value::None => false,
            Value::Bool(b) => *b,
            Value::Int(i) => *i != 0,
            Value::Float(f) => *f != 0.0,
            Value::Str(s) => !s.is_empty(),
            Value::List(v) => !v.is_empty(),
            _ => true,
        }
    }

    fn print(&self) {
        match self {
            Value::None => print!("None"),
            Value::Int(i) => print!("{}", i),
            Value::Float(f) => {
                // 模擬 C 的 %g：去掉多餘的小數點零
                let s = format!("{}", f);
                // %g 風格：若沒小數點且是整數，直接印
                if s.contains('.') {
                    // 去掉尾巴的零
                    let trimmed = s.trim_end_matches('0').trim_end_matches('.');
                    print!("{}", trimmed);
                } else {
                    print!("{}", s);
                }
            }
            Value::Bool(b) => print!("{}", if *b { "True" } else { "False" }),
            Value::Str(s) => print!("{}", s),
            Value::List(v) => print!("<list size={}>", v.len()),
            Value::Closure { label, .. } => print!("<closure {}>", label),
            Value::Native(_) => print!("<native func>"),
        }
    }
}

// =========================================================================
// 環境 (Scope / Environment)
// =========================================================================

/// 以 Arena 管理多個 Env，避免指標迴圈
struct EnvArena {
    envs: Vec<HashMap<String, Value>>,
    parents: Vec<Option<usize>>,
}

impl EnvArena {
    fn new() -> Self {
        EnvArena {
            envs: Vec::new(),
            parents: Vec::new(),
        }
    }

    fn new_env(&mut self, parent: Option<usize>) -> usize {
        let id = self.envs.len();
        self.envs.push(HashMap::new());
        self.parents.push(parent);
        id
    }

    fn set(&mut self, env_id: usize, name: &str, val: Value) {
        if name == "_" {
            return;
        }
        self.envs[env_id].insert(name.to_string(), val);
    }

    fn get(&self, env_id: usize, name: &str) -> Value {
        let mut curr = Some(env_id);
        while let Some(id) = curr {
            if let Some(v) = self.envs[id].get(name) {
                return v.clone();
            }
            curr = self.parents[id];
        }
        eprintln!("Runtime Error: Undefined variable '{}'", name);
        process::exit(1);
    }
}

// =========================================================================
// 指令 (Instruction)
// =========================================================================

#[derive(Clone)]
struct Instruction {
    op: String,
    arg1: String,
    arg2: String,
    res: String,
    line_no: usize,
}

impl Instruction {
    fn blank(line_no: usize) -> Self {
        Instruction {
            op: "_".to_string(),
            arg1: "_".to_string(),
            arg2: "_".to_string(),
            res: "_".to_string(),
            line_no,
        }
    }
}

// =========================================================================
// 解析器 (Parser)
// =========================================================================

fn parse_line(line: &str, line_no: usize) -> Instruction {
    let mut ins = Instruction::blank(line_no);
    let targets = [&mut ins.op, &mut ins.arg1, &mut ins.arg2, &mut ins.res];
    let mut chars = line.chars().peekable();
    let mut p_idx = 0;

    while p_idx < 4 {
        // 跳過空白
        while chars.peek().map(|c| c.is_whitespace()).unwrap_or(false) {
            chars.next();
        }
        if chars.peek().is_none() {
            break;
        }

        let mut token = String::new();
        if chars.peek() == Some(&'"') {
            token.push(chars.next().unwrap()); // 開頭引號
            loop {
                match chars.next() {
                    None => break,
                    Some('"') => {
                        token.push('"');
                        break;
                    }
                    Some('\\') => {
                        token.push('\\');
                        if let Some(next) = chars.next() {
                            token.push(next);
                        }
                    }
                    Some(c) => token.push(c),
                }
            }
        } else {
            while let Some(&c) = chars.peek() {
                if c.is_whitespace() {
                    break;
                }
                token.push(c);
                chars.next();
            }
        }

        *targets[p_idx] = token;
        p_idx += 1;
    }

    ins
}

fn load_program(filename: &str) -> (Vec<Instruction>, HashMap<String, usize>) {
    let content = fs::read_to_string(filename).unwrap_or_else(|_| {
        eprintln!("Fatal Error: Cannot open input file");
        process::exit(1);
    });

    let mut prog: Vec<Instruction> = Vec::new();
    let mut labels: HashMap<String, usize> = HashMap::new();
    let mut line_no = 1usize;

    for raw in content.lines() {
        let trimmed = raw.trim();
        if trimmed.is_empty() || trimmed.starts_with('#') {
            line_no += 1;
            continue;
        }

        let ins = parse_line(trimmed, line_no);

        if ins.op == "LABEL" {
            labels.insert(ins.arg1.clone(), prog.len());
        }

        prog.push(ins);
        line_no += 1;
    }

    (prog, labels)
}

// =========================================================================
// 內建函數 (Built-ins)
// =========================================================================

fn native_print(args: &[Value]) -> Value {
    for (i, v) in args.iter().enumerate() {
        if i > 0 {
            print!(" ");
        }
        v.print();
    }
    println!();
    Value::None
}

fn native_len(args: &[Value]) -> Value {
    if args.len() != 1 {
        eprintln!("Fatal Error: len() takes exactly 1 argument");
        process::exit(1);
    }
    match &args[0] {
        Value::Str(s) => Value::Int(s.len() as i64),
        Value::List(v) => Value::Int(v.len() as i64),
        _ => {
            eprintln!("Fatal Error: Object of type has no len()");
            process::exit(1);
        }
    }
}

// =========================================================================
// 常數求值 (Constant Evaluation)
// =========================================================================

fn eval_const(s: &str) -> Value {
    if s == "None" {
        return Value::None;
    }
    if s == "True" {
        return Value::Bool(true);
    }
    if s == "False" {
        return Value::Bool(false);
    }
    if s.starts_with('"') {
        let inner = &s[1..s.rfind('"').unwrap_or(s.len())];
        let mut result = String::new();
        let mut chars = inner.chars();
        while let Some(c) = chars.next() {
            if c == '\\' {
                match chars.next() {
                    Some('n') => result.push('\n'),
                    Some('t') => result.push('\t'),
                    Some('\\') => result.push('\\'),
                    Some('"') => result.push('"'),
                    Some(other) => {
                        result.push('\\');
                        result.push(other);
                    }
                    None => {}
                }
            } else {
                result.push(c);
            }
        }
        return Value::Str(result);
    }
    if s.contains('.') {
        if let Ok(f) = s.parse::<f64>() {
            return Value::Float(f);
        }
    }
    if let Ok(i) = s.parse::<i64>() {
        return Value::Int(i);
    }
    eprintln!("Fatal Error: Cannot parse constant '{}'", s);
    process::exit(1);
}

// =========================================================================
// 呼叫疊 (Call Stack)
// =========================================================================

struct CallFrame {
    return_pc: usize,
    return_var: String,
    saved_env: usize,
}

// =========================================================================
// 執行引擎 (VM Execution)
// =========================================================================

fn resolve_label(labels: &HashMap<String, usize>, name: &str) -> usize {
    match labels.get(name) {
        Some(&pc) => pc,
        None => {
            eprintln!("Error: Unknown label '{}'", name);
            process::exit(1);
        }
    }
}

fn run(prog: &[Instruction], labels: &HashMap<String, usize>) {
    let mut arena = EnvArena::new();
    let global_env = arena.new_env(None);

    // 註冊內建函數
    arena.set(global_env, "print", Value::Native(native_print));
    arena.set(global_env, "len", Value::Native(native_len));

    let mut pc = 0usize;
    let mut env = global_env;
    let mut call_stack: Vec<CallFrame> = Vec::new();
    let mut arg_buffer: Vec<Value> = vec![Value::None; 256];

    while pc < prog.len() {
        let ins = &prog[pc];

        match ins.op.as_str() {
            "LOAD_CONST" => {
                let val = eval_const(&ins.arg1);
                arena.set(env, &ins.res, val);
            }
            "LOAD_NAME" => {
                let val = arena.get(env, &ins.arg1);
                arena.set(env, &ins.res, val);
            }
            "STORE" => {
                let val = arena.get(env, &ins.arg1);
                arena.set(env, &ins.res, val);
            }
            "ADD" => {
                let a = arena.get(env, &ins.arg1);
                let b = arena.get(env, &ins.arg2);
                let result = match (&a, &b) {
                    (Value::Int(x), Value::Int(y)) => Value::Int(x + y),
                    (Value::Float(x), Value::Float(y)) => Value::Float(x + y),
                    (Value::Int(x), Value::Float(y)) => Value::Float(*x as f64 + y),
                    (Value::Float(x), Value::Int(y)) => Value::Float(x + *y as f64),
                    (Value::Str(x), Value::Str(y)) => Value::Str(format!("{}{}", x, y)),
                    _ => {
                        eprintln!("Fatal Error: Unsupported operand types for ADD");
                        process::exit(1);
                    }
                };
                arena.set(env, &ins.res, result);
            }
            "SUB" => {
                let a = arena.get(env, &ins.arg1);
                let b = arena.get(env, &ins.arg2);
                let result = match (&a, &b) {
                    (Value::Int(x), Value::Int(y)) => Value::Int(x - y),
                    _ => {
                        eprintln!("Fatal Error: Unsupported operand types for SUB");
                        process::exit(1);
                    }
                };
                arena.set(env, &ins.res, result);
            }
            "MUL" => {
                let a = arena.get(env, &ins.arg1);
                let b = arena.get(env, &ins.arg2);
                let result = match (&a, &b) {
                    (Value::Int(x), Value::Int(y)) => Value::Int(x * y),
                    _ => {
                        eprintln!("Fatal Error: Unsupported operand types for MUL");
                        process::exit(1);
                    }
                };
                arena.set(env, &ins.res, result);
            }
            "CMP_GT" => {
                let a = arena.get(env, &ins.arg1);
                let b = arena.get(env, &ins.arg2);
                let result = match (&a, &b) {
                    (Value::Int(x), Value::Int(y)) => Value::Bool(x > y),
                    _ => {
                        eprintln!("Fatal Error: Type error in CMP_GT");
                        process::exit(1);
                    }
                };
                arena.set(env, &ins.res, result);
            }
            "CMP_LT" => {
                let a = arena.get(env, &ins.arg1);
                let b = arena.get(env, &ins.arg2);
                let result = match (&a, &b) {
                    (Value::Int(x), Value::Int(y)) => Value::Bool(x < y),
                    _ => {
                        eprintln!("Fatal Error: Type error in CMP_LT");
                        process::exit(1);
                    }
                };
                arena.set(env, &ins.res, result);
            }
            "CMP_GE" => {
                let a = arena.get(env, &ins.arg1);
                let b = arena.get(env, &ins.arg2);
                let result = match (&a, &b) {
                    (Value::Int(x), Value::Int(y)) => Value::Bool(x >= y),
                    _ => {
                        eprintln!("Fatal Error: Type error in CMP_GE");
                        process::exit(1);
                    }
                };
                arena.set(env, &ins.res, result);
            }
            "CMP_LE" => {
                let a = arena.get(env, &ins.arg1);
                let b = arena.get(env, &ins.arg2);
                let result = match (&a, &b) {
                    (Value::Int(x), Value::Int(y)) => Value::Bool(x <= y),
                    _ => {
                        eprintln!("Fatal Error: Type error in CMP_LE");
                        process::exit(1);
                    }
                };
                arena.set(env, &ins.res, result);
            }
            "CMP_EQ" => {
                let a = arena.get(env, &ins.arg1);
                let b = arena.get(env, &ins.arg2);
                let result = match (&a, &b) {
                    (Value::Int(x), Value::Int(y)) => Value::Bool(x == y),
                    (Value::Str(x), Value::Str(y)) => Value::Bool(x == y),
                    _ => Value::Bool(false),
                };
                arena.set(env, &ins.res, result);
            }
            "CMP_NE" => {
                let a = arena.get(env, &ins.arg1);
                let b = arena.get(env, &ins.arg2);
                let result = match (&a, &b) {
                    (Value::Int(x), Value::Int(y)) => Value::Bool(x != y),
                    (Value::Str(x), Value::Str(y)) => Value::Bool(x != y),
                    _ => Value::Bool(true),
                };
                arena.set(env, &ins.res, result);
            }
            "NEG" => {
                let a = arena.get(env, &ins.arg1);
                let result = match &a {
                    Value::Int(x) => Value::Int(-x),
                    _ => {
                        eprintln!("Fatal Error: Type error in NEG");
                        process::exit(1);
                    }
                };
                arena.set(env, &ins.res, result);
            }
            "JUMP" => {
                pc = resolve_label(labels, &ins.arg1);
                continue;
            }
            "BRANCH_IF_FALSE" => {
                let cond = arena.get(env, &ins.arg1);
                if !cond.is_truthy() {
                    pc = resolve_label(labels, &ins.res);
                    continue;
                }
            }
            "BRANCH_IF_TRUE" => {
                let cond = arena.get(env, &ins.arg1);
                if cond.is_truthy() {
                    pc = resolve_label(labels, &ins.res);
                    continue;
                }
            }
            "ARG_PUSH" => {
                let idx: usize = ins.arg2.parse().unwrap_or(0);
                let val = arena.get(env, &ins.arg1);
                if idx >= arg_buffer.len() {
                    arg_buffer.resize(idx + 1, Value::None);
                }
                arg_buffer[idx] = val;
            }
            "ARG_POP" => {
                let idx: usize = ins.arg2.parse().unwrap_or(0);
                let val = arg_buffer[idx].clone();
                arena.set(env, &ins.res, val);
            }
            "MAKE_CLOSURE" => {
                let val = Value::Closure {
                    label: ins.arg1.clone(),
                    env_id: env,
                };
                arena.set(env, &ins.res, val);
            }
            "CALL" => {
                let func = arena.get(env, &ins.arg1);
                let argc: usize = ins.arg2.parse().unwrap_or(0);

                match func {
                    Value::Native(f) => {
                        let args = arg_buffer[..argc].to_vec();
                        let ret = f(&args);
                        arena.set(env, &ins.res, ret);
                    }
                    Value::Closure { label, env_id: closure_env } => {
                        call_stack.push(CallFrame {
                            return_pc: pc + 1,
                            return_var: ins.res.clone(),
                            saved_env: env,
                        });
                        env = arena.new_env(Some(closure_env));
                        pc = resolve_label(labels, &label);
                        continue;
                    }
                    _ => {
                        eprintln!("Fatal Error: Attempt to call non-callable object");
                        process::exit(1);
                    }
                }
            }
            "RETURN" => {
                let ret = arena.get(env, &ins.arg1);
                if call_stack.is_empty() {
                    break;
                }
                let frame = call_stack.pop().unwrap();
                pc = frame.return_pc;
                env = frame.saved_env;
                arena.set(env, &frame.return_var, ret);
                continue;
            }
            "BUILD_LIST" => {
                let count: usize = ins.arg1.parse().unwrap_or(0);
                let items = arg_buffer[..count].to_vec();
                arena.set(env, &ins.res, Value::List(items));
            }
            "LOAD_SUBSCRIPT" => {
                let obj = arena.get(env, &ins.arg1);
                let idx_val = arena.get(env, &ins.arg2);
                match (&obj, &idx_val) {
                    (Value::List(v), Value::Int(i)) => {
                        let i = *i as usize;
                        if i >= v.len() {
                            eprintln!("Fatal Error: Index out of bounds");
                            process::exit(1);
                        }
                        let item = v[i].clone();
                        arena.set(env, &ins.res, item);
                    }
                    _ => {
                        eprintln!("Fatal Error: Type error in SUBSCRIPT");
                        process::exit(1);
                    }
                }
            }
            "STORE_SUBSCRIPT" => {
                let obj = arena.get(env, &ins.arg1);
                let idx_val = arena.get(env, &ins.arg2);
                let new_val = arena.get(env, &ins.res);
                match (&obj, &idx_val) {
                    (Value::List(_), Value::Int(i)) => {
                        let i = *i as usize;
                        // 取出 list，修改後存回
                        if let Value::List(mut v) = obj {
                            if i >= v.len() {
                                eprintln!("Fatal Error: Index out of bounds");
                                process::exit(1);
                            }
                            v[i] = new_val;
                            arena.set(env, &ins.arg1, Value::List(v));
                        }
                    }
                    _ => {
                        eprintln!("Fatal Error: Type error in STORE_SUBSCRIPT");
                        process::exit(1);
                    }
                }
            }
            "LABEL" => {
                // 已在 load_program 處理，執行時跳過
            }
            _ => {
                eprintln!("Unknown opcode: {} at line {}", ins.op, ins.line_no);
                process::exit(1);
            }
        }

        pc += 1;
    }
}

// =========================================================================
// 主程式 (Main)
// =========================================================================

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        println!("Usage: {} <program.qd>", args[0]);
        process::exit(1);
    }

    let (prog, labels) = load_program(&args[1]);
    run(&prog, &labels);
}