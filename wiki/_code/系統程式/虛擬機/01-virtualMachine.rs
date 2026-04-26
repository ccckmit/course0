// Rust 虛擬機範例
// 展示 Stack VM、Bytecode 解釋器、暫存器 VM

// ============ 1. 指令集架構 (ISA) ============
#[derive(Debug, Clone)]
enum Instruction {
    // 算術指令
    Add,
    Sub,
    Mul,
    Div,
    Mod,

    // 比較指令
    Eq,
    Ne,
    Lt,
    Le,
    Gt,
    Ge,

    // 邏輯指令
    And,
    Or,
    Not,

    // 記憶體指令
    Load,  // Load from memory
    Store, // Store to memory
    Push,  // Push to stack
    Pop,   // Pop from stack
    Dup,   // Duplicate top

    // 暫存器指令
    Mov,      // Move to register
    LoadReg,  // Load from register
    StoreReg, // Store to register

    // 控制流指令
    Jump,
    JumpIf,
    Call,
    Ret,
    Halt,

    // 其他
    Nop,
    Print,
    Read,
}

impl Instruction {
    fn to_bytes(&self) -> Vec<u8> {
        match self {
            Instruction::Add => vec![0x01],
            Instruction::Sub => vec![0x02],
            Instruction::Mul => vec![0x03],
            Instruction::Div => vec![0x04],
            Instruction::Mod => vec![0x05],
            Instruction::Eq => vec![0x10],
            Instruction::Ne => vec![0x11],
            Instruction::Lt => vec![0x12],
            Instruction::Le => vec![0x13],
            Instruction::Gt => vec![0x14],
            Instruction::Ge => vec![0x15],
            Instruction::And => vec![0x20],
            Instruction::Or => vec![0x21],
            Instruction::Not => vec![0x22],
            Instruction::Load => vec![0x30],
            Instruction::Store => vec![0x31],
            Instruction::Push => vec![0x40],
            Instruction::Pop => vec![0x41],
            Instruction::Dup => vec![0x42],
            Instruction::Mov => vec![0x50],
            Instruction::LoadReg => vec![0x51],
            Instruction::StoreReg => vec![0x52],
            Instruction::Jump => vec![0x60],
            Instruction::JumpIf => vec![0x61],
            Instruction::Call => vec![0x62],
            Instruction::Ret => vec![0x63],
            Instruction::Halt => vec![0xFF],
            Instruction::Nop => vec![0x00],
            Instruction::Print => vec![0xE0],
            Instruction::Read => vec![0xE1],
        }
    }
}

// ============ 2. Stack VM (基於堆疊的虛擬機) ============
struct StackVM {
    memory: Vec<i64>,
    stack: Vec<i64>,
    pc: usize, // Program Counter
    sp: usize, // Stack Pointer
    running: bool,
    output: Vec<String>,
}

impl StackVM {
    fn new(memory_size: usize) -> Self {
        StackVM {
            memory: vec![0; memory_size],
            stack: Vec::new(),
            pc: 0,
            sp: 0,
            running: true,
            output: Vec::new(),
        }
    }

    fn push(&mut self, value: i64) {
        self.stack.push(value);
        self.sp += 1;
    }

    fn pop(&mut self) -> Option<i64> {
        if self.sp > 0 {
            self.sp -= 1;
            self.stack.pop()
        } else {
            None
        }
    }

    fn execute(&mut self, program: &[Instruction]) {
        while self.running && self.pc < program.len() {
            let instr = &program[self.pc];

            match instr {
                Instruction::Push => {
                    self.pc += 1;
                    if self.pc < program.len() {
                        if let Instruction::Load = program[self.pc].clone() {
                            self.pc += 1;
                            if self.pc < program.len() {
                                self.push(program[self.pc].clone() as i64);
                            }
                        } else {
                            self.push(0);
                        }
                    }
                }
                Instruction::Add => self.binary_op(|a, b| a + b),
                Instruction::Sub => self.binary_op(|a, b| a - b),
                Instruction::Mul => self.binary_op(|a, b| a * b),
                Instruction::Div => self.binary_op(|a, b| a / b),
                Instruction::Mod => self.binary_op(|a, b| a % b),
                Instruction::Eq => self.binary_op(|a, b| if a == b { 1 } else { 0 }),
                Instruction::Lt => self.binary_op(|a, b| if a < b { 1 } else { 0 }),
                Instruction::Gt => self.binary_op(|a, b| if a > b { 1 } else { 0 }),
                Instruction::Jump => {
                    self.pc += 1;
                    if let Instruction::Load = program[self.pc].clone() {
                        self.pc += 1;
                        self.pc = program[self.pc].clone() as usize;
                    }
                }
                Instruction::JumpIf => {
                    self.pc += 1;
                    if let Some(cond) = self.pop() {
                        if cond != 0 {
                            self.pc += 1;
                            if self.pc < program.len() {
                                self.pc = program[self.pc].clone() as usize;
                            }
                        } else {
                            self.pc += 2;
                        }
                    }
                }
                Instruction::Halt => self.running = false,
                Instruction::Print => {
                    if let Some(v) = self.pop() {
                        self.output.push(format!("{}", v));
                    }
                }
                Instruction::Nop => {}
                _ => {}
            }

            self.pc += 1;
        }
    }

    fn binary_op<F>(&mut self, op: F)
    where
        F: FnOnce(i64, i64) -> i64,
    {
        if let (Some(b), Some(a)) = (self.pop(), self.pop()) {
            self.push(op(a, b));
        }
    }
}

// ============ 3. Bytecode 解釋器 ============
struct BytecodeInterpreter {
    bytecode: Vec<u8>,
    ip: usize, // Instruction Pointer
}

impl BytecodeInterpreter {
    fn execute(&mut self) -> Vec<String> {
        let mut output = Vec::new();
        let mut stack: Vec<i64> = Vec::new();

        while self.ip < self.bytecode.len() {
            let opcode = self.bytecode[self.ip];

            match opcode {
                0x01 => {
                    // Add
                    let b = stack.pop().unwrap_or(0);
                    let a = stack.pop().unwrap_or(0);
                    stack.push(a + b);
                }
                0x02 => {
                    // Sub
                    let b = stack.pop().unwrap_or(0);
                    let a = stack.pop().unwrap_or(0);
                    stack.push(a - b);
                }
                0x03 => {
                    // Mul
                    let b = stack.pop().unwrap_or(0);
                    let a = stack.pop().unwrap_or(0);
                    stack.push(a * b);
                }
                0x04 => {
                    // Div
                    let b = stack.pop().unwrap_or(1);
                    let a = stack.pop().unwrap_or(0);
                    stack.push(a / b);
                }
                0x10 => {
                    // Eq
                    let b = stack.pop().unwrap_or(0);
                    let a = stack.pop().unwrap_or(0);
                    stack.push(if a == b { 1 } else { 0 });
                }
                0x40 => {
                    // Push immediate
                    self.ip += 1;
                    if self.ip < self.bytecode.len() {
                        stack.push(self.bytecode[self.ip] as i64);
                    }
                }
                0xFF => break, // Halt
                0xE0 => {
                    // Print
                    if let Some(v) = stack.last() {
                        output.push(format!("{}", v));
                    }
                }
                _ => {}
            }

            self.ip += 1;
        }

        output
    }
}

// ============ 4. 暫存器 VM (類 x86) ============
struct RegisterVM {
    registers: [i64; 8],
    memory: Vec<u8>,
    pc: usize,
    flags: u8, // Zero, Sign, Carry, Overflow
}

impl RegisterVM {
    const R0: usize = 0;
    const R1: usize = 1;
    const R2: usize = 2;
    const R3: usize = 3;
    const RIP: usize = 4;
    const RSP: usize = 5;
    const RBP: usize = 6;
    const FLAGS: usize = 7;

    fn new(memory_size: usize) -> Self {
        RegisterVM {
            registers: [0; 8],
            memory: vec![0; memory_size],
            pc: 0,
            flags: 0,
        }
    }

    fn set_flag(&mut self, flag: &str, value: bool) {
        match flag {
            "Z" => {
                if value {
                    self.flags |= 1
                } else {
                    self.flags &= !1
                }
            }
            "S" => {
                if value {
                    self.flags |= 2
                } else {
                    self.flags &= !2
                }
            }
            "C" => {
                if value {
                    self.flags |= 4
                } else {
                    self.flags &= !4
                }
            }
            "O" => {
                if value {
                    self.flags |= 8
                } else {
                    self.flags &= !8
                }
            }
            _ => {}
        }
    }

    fn execute_instruction(&mut self, opcode: u8) {
        match opcode {
            0xB8 => {
                // MOV reg, imm32
                self.registers[Self::R0] = 42;
            }
            0x01 => {
                // ADD reg, reg
                self.registers[Self::R0] += self.registers[Self::R1];
                self.set_flag("Z", self.registers[Self::R0] == 0);
                self.set_flag("S", self.registers[Self::R0] < 0);
            }
            0xFF => {
                // HLT
                self.registers[Self::RIP] = 0xFFFFFFFF;
            }
            _ => {}
        }
    }
}

// ============ 5. JIT 編譯概念 ============
struct JITCompiler;

impl JITCompiler {
    fn new() -> Self {
        JITCompiler
    }

    fn compile_ir_to_native(&self, _ir: &str) -> Vec<u8> {
        // 簡化的 JIT 編譯 - 將 IR 轉換為機器碼
        let mut code = Vec::new();

        // x86-64 return instruction
        code.push(0xC3); // ret

        code
    }

    fn execute_compiled(&self, code: &[u8]) {
        println!(
            "執行 JIT 編譯的程式碼: {:?} ...",
            &code[..10.min(code.len())]
        );
    }
}

fn demo_bytecode_vm() {
    println!("=== 1. Bytecode VM ===");

    let program = vec![
        0x40, 5, // PUSH 5
        0x40, 3,    // PUSH 3
        0x01, // ADD
        0xE0, // PRINT
        0xFF, // HALT
    ];

    let mut interpreter = BytecodeInterpreter {
        bytecode: program,
        ip: 0,
    };

    let output = interpreter.execute();
    println!("輸出: {:?}", output);
    println!();
}

fn demo_stack_vm() {
    println!("=== 2. Stack VM ===");

    let mut vm = StackVM::new(1024);

    // 5 + 3 * 2
    let program = vec![
        Instruction::Load,
        Instruction::Push, // 需要值
    ];

    vm.push(5);
    vm.push(3);
    vm.push(2);
    vm.execute(&[
        Instruction::Mul,
        Instruction::Add,
        Instruction::Print,
        Instruction::Halt,
    ]);

    println!("VM 輸出: {:?}", vm.output);
    println!();
}

fn demo_register_vm() {
    println!("=== 3. Register VM ===");

    let mut vm = RegisterVM::new(4096);

    vm.registers[RegisterVM::R0] = 10;
    vm.registers[RegisterVM::R1] = 20;

    println!(
        "初始化: R0 = {}, R1 = {}",
        vm.registers[RegisterVM::R0],
        vm.registers[RegisterVM::R1]
    );

    vm.execute_instruction(0x01); // ADD

    println!("執行 ADD 後: R0 = {}", vm.registers[RegisterVM::R0]);
    println!("Zero flag: {}", (vm.flags & 1) != 0);
    println!();
}

fn demo_jit() {
    println!("=== 4. JIT 編譯 ===");

    let jit = JITCompiler::new();

    let ir = "add r0, r1, r2";
    let native_code = jit.compile_ir_to_native(ir);

    println!("IR: {}", ir);
    println!(
        "編譯為: {:?} ...",
        &native_code[..native_code.len().min(10)]
    );

    jit.execute_compiled(&native_code);
    println!();
}

fn demo_instruction_encoding() {
    println!("=== 5. 指令編碼 ===");

    let instructions = vec![
        Instruction::Add,
        Instruction::Sub,
        Instruction::Mul,
        Instruction::Div,
        Instruction::Jump,
        Instruction::Call,
        Instruction::Halt,
    ];

    println!("指令 -> 位元組編碼:");
    for instr in &instructions {
        println!("  {:?} -> {:?}", instr, instr.to_bytes());
    }
    println!();
}

fn demo_stack_operations() {
    println!("=== 6. 堆疊操作 ===");

    let mut vm = StackVM::new(1024);

    // 模擬棧操作
    vm.push(10);
    vm.push(20);
    vm.push(30);

    println!("Push 10, 20, 30");
    println!("堆疊深度: {}", vm.sp);
    println!("Top: {:?}", vm.stack.last());

    if let Some(v) = vm.pop() {
        println!("Pop: {}", v);
    }

    println!("Pop 後堆疊深度: {}", vm.sp);
    println!();
}

fn demo_memory_access() {
    println!("=== 7. 記憶體訪問 ===");

    let mut vm = StackVM::new(1024);

    // 模擬記憶體載入/儲存
    let addr = 100;
    vm.memory[addr] = 42;

    println!("寫入記憶體位址 {}: {}", addr, vm.memory[addr]);
    println!("記憶體大小: {} bytes", vm.memory.len());
    println!();
}

fn main() {
    println!("=== Rust 虛擬機範例 ===\n");

    demo_bytecode_vm();
    demo_stack_vm();
    demo_register_vm();
    demo_jit();
    demo_instruction_encoding();
    demo_stack_operations();
    demo_memory_access();

    println!("虛擬機範例完成!");
}
