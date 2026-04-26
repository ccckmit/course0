// Rust 作業系統概念範例
// 展示行程管理、執行緒、排程、訊號處理

use std::collections::VecDeque;
use std::sync::{Arc, Mutex};
use std::thread;
use std::time::{Duration, Instant};

// ============ 1. 程序 (Process) 模擬 ============
#[derive(Debug, Clone)]
struct Process {
    pid: u32,
    name: String,
    state: ProcessState,
    priority: i32,
    cpu_time: u64,
}

#[derive(Debug, Clone, PartialEq)]
enum ProcessState {
    Ready,
    Running,
    Waiting,
    Terminated,
}

impl Process {
    fn new(pid: u32, name: &str, priority: i32) -> Self {
        Process {
            pid,
            name: name.to_string(),
            state: ProcessState::Ready,
            priority,
            cpu_time: 0,
        }
    }
}

// ============ 2. 排程器 (Scheduler) ============
struct Scheduler {
    ready_queue: VecDeque<Process>,
    running: Option<Process>,
    terminated: Vec<Process>,
    time_slice: u64,
    current_time: u64,
}

impl Scheduler {
    fn new(time_slice: u64) -> Self {
        Scheduler {
            ready_queue: VecDeque::new(),
            running: None,
            terminated: vec![],
            time_slice,
            current_time: 0,
        }
    }

    fn add_process(&mut self, mut process: Process) {
        process.state = ProcessState::Ready;
        self.ready_queue.push_back(process);
    }

    fn schedule(&mut self) {
        if self.running.is_some() {
            return;
        }

        if let Some(mut process) = self.ready_queue.pop_front() {
            process.state = ProcessState::Running;
            self.running = Some(process);
        }
    }

    fn tick(&mut self) {
        self.current_time += 1;

        if let Some(ref mut process) = self.running {
            process.cpu_time += 1;

            if process.cpu_time >= self.time_slice {
                self.preempt();
            }
        }
    }

    fn preempt(&mut self) {
        if let Some(mut process) = self.running.take() {
            process.state = ProcessState::Ready;
            self.ready_queue.push_back(process);
        }
        self.schedule();
    }

    fn terminate(&mut self) {
        if let Some(mut process) = self.running.take() {
            process.state = ProcessState::Terminated;
            self.terminated.push(process);
        }
        self.schedule();
    }

    fn block(&mut self) {
        if let Some(mut process) = self.running.take() {
            process.state = ProcessState::Waiting;
            self.ready_queue.push_back(process);
        }
        self.schedule();
    }

    fn wake(&mut self, pid: u32) {
        for process in self.ready_queue.iter_mut() {
            if process.pid == pid {
                process.state = ProcessState::Ready;
                break;
            }
        }
    }
}

// ============ 3. 排程演算法 ============
struct FCFSScheduler;
struct PriorityScheduler;
struct RoundRobinScheduler;

impl FCFSScheduler {
    fn schedule(processes: &mut Vec<Process>) {
        processes.sort_by_key(|p| p.cpu_time);
    }
}

impl PriorityScheduler {
    fn schedule(processes: &mut Vec<Process>) {
        processes.sort_by(|a, b| b.priority.cmp(&a.priority));
    }
}

// ============ 4. 記憶體管理模擬 ============
struct MemoryManager {
    total: usize,
    used: usize,
    free_blocks: Vec<(usize, usize)>, // (offset, size)
}

impl MemoryManager {
    fn new(total: usize) -> Self {
        MemoryManager {
            total,
            used: 0,
            free_blocks: vec![(0, total)],
        }
    }

    fn alloc(&mut self, size: usize) -> Option<usize> {
        for (offset, free_size) in self.free_blocks.iter_mut() {
            if *free_size >= size {
                let result = *offset;
                *offset += size;
                *free_size -= size;
                self.used += size;
                return Some(result);
            }
        }
        None
    }

    fn free(&mut self, offset: usize, size: usize) {
        self.used -= size;

        // 合併相鄰的 free block
        let mut new_block = Some((offset, size));
        self.free_blocks.retain(|block| {
            if let Some((off, sz)) = new_block {
                if block.0 == off + sz || off == block.0 + block.1 {
                    new_block = None;
                    return false;
                }
            }
            true
        });

        if let Some((off, sz)) = new_block {
            self.free_blocks.push((off, sz));
        }
    }

    fn status(&self) {
        println!("記憶體使用: {}/{} bytes", self.used, self.total);
        println!("空閒區塊數: {}", self.free_blocks.len());
    }
}

// ============ 5. 訊號處理模擬 ============
#[derive(Debug, Clone)]
enum Signal {
    SIGINT,  // Ctrl+C
    SIGTERM, // 終止訊號
    SIGKILL, // 強制終止
    SIGCHLD, // 子程序結束
}

trait SignalHandler {
    fn handle(&self, signal: &Signal);
}

struct DefaultHandler;

impl SignalHandler for DefaultHandler {
    fn handle(&self, signal: &Signal) {
        match signal {
            Signal::SIGINT => println!("收到 Ctrl+C，準備退出..."),
            Signal::SIGTERM => println!("收到終止訊號"),
            Signal::SIGKILL => println!("被強制終止"),
            Signal::SIGCHLD => println!("子程序已結束"),
        }
    }
}

// ============ 6. 系統呼叫模擬 ============
mod syscalls {
    use super::*;

    pub fn fork() -> i32 {
        println!("[SYSCALL] fork()");
        0
    }

    pub fn exec(path: &str) -> i32 {
        println!("[SYSCALL] exec({})", path);
        0
    }

    pub fn wait() -> i32 {
        println!("[SYSCALL] wait()");
        0
    }

    pub fn read(fd: i32, buf: &mut [u8]) -> i32 {
        println!("[SYSCALL] read(fd={})", fd);
        buf.len() as i32
    }

    pub fn write(fd: i32, data: &[u8]) -> i32 {
        println!("[SYSCALL] write(fd={}, size={})", fd, data.len());
        data.len() as i32
    }
}

// ============ 7. 上下文切換模擬 ============
struct Context {
    registers: [u64; 16],
    pc: u64,
    sp: u64,
}

impl Context {
    fn new() -> Self {
        Context {
            registers: [0; 16],
            pc: 0,
            sp: 0,
        }
    }

    fn save(&self) -> Context {
        Context {
            registers: self.registers,
            pc: self.pc,
            sp: self.sp,
        }
    }

    fn restore(&mut self, ctx: &Context) {
        self.registers = ctx.registers;
        self.pc = ctx.pc;
        self.sp = ctx.sp;
    }
}

fn demo_process_creation() {
    println!("=== 1. 程序創建 ===");

    let mut scheduler = Scheduler::new(10);

    scheduler.add_process(Process::new(1, "init", 0));
    scheduler.add_process(Process::new(2, "bash", 1));
    scheduler.add_process(Process::new(3, "vim", 2));

    println!("就緒佇列:");
    for p in &scheduler.ready_queue {
        println!("  PID {}: {} (priority={})", p.pid, p.name, p.priority);
    }
    println!();
}

fn demo_scheduling() {
    println!("=== 2. 排程演算法 ===");

    let mut processes = vec![
        Process::new(1, "P1", 3),
        Process::new(2, "P2", 1),
        Process::new(3, "P3", 2),
    ];

    println!(
        "初始順序: {:?}",
        processes.iter().map(|p| &p.name).collect::<Vec<_>>()
    );

    // FCFS
    FCFSScheduler::schedule(&mut processes);
    println!(
        "FCFS 後: {:?}",
        processes.iter().map(|p| &p.name).collect::<Vec<_>>()
    );

    // Priority
    PriorityScheduler::schedule(&mut processes);
    println!(
        "Priority 後: {:?}",
        processes.iter().map(|p| &p.name).collect::<Vec<_>>()
    );
    println!();
}

fn demo_memory() {
    println!("=== 3. 記憶體管理 ===");

    let mut mm = MemoryManager::new(1024);

    let addr1 = mm.alloc(256);
    println!("配置 256 bytes: {:?}", addr1);

    let addr2 = mm.alloc(512);
    println!("配置 512 bytes: {:?}", addr2);

    mm.status();

    if let Some(addr) = addr1 {
        mm.free(addr, 256);
    }
    println!("釋放後:");
    mm.status();
    println!();
}

fn demo_signals() {
    println!("=== 4. 訊號處理 ===");

    let handler = DefaultHandler;

    let signals = vec![
        Signal::SIGINT,
        Signal::SIGTERM,
        Signal::SIGKILL,
        Signal::SIGCHLD,
    ];

    for sig in signals {
        handler.handle(&sig);
    }
    println!();
}

fn demo_syscalls() {
    println!("=== 5. 系統呼叫 ===");

    syscalls::fork();
    syscalls::exec("/bin/ls");
    syscalls::wait();

    let mut buf = [0u8; 1024];
    syscalls::read(0, &mut buf);

    syscalls::write(1, b"Hello, OS!\n");
    println!();
}

fn demo_context_switch() {
    println!("=== 6. 上下文切換 ===");

    let mut ctx1 = Context::new();
    ctx1.registers[0] = 100;
    ctx1.pc = 0x1000;
    ctx1.sp = 0xFFFF;

    let saved = ctx1.save();

    ctx1.registers[0] = 200;
    ctx1.pc = 0x2000;

    println!("執行序 1: registers[0] = {}", ctx1.registers[0]);

    ctx1.restore(&saved);
    println!("恢復後: registers[0] = {}", ctx1.registers[0]);
    println!();
}

fn demo_scheduler_simulation() {
    println!("=== 7. 排程器模擬 ===");

    let mut scheduler = Scheduler::new(3);

    scheduler.add_process(Process::new(1, "P1", 1));
    scheduler.add_process(Process::new(2, "P2", 1));
    scheduler.add_process(Process::new(3, "P3", 1));

    for tick in 0..10 {
        scheduler.tick();
        scheduler.schedule();

        if let Some(ref p) = scheduler.running {
            println!("Tick {}: Running PID {} ({})", tick, p.pid, p.name);
        }
    }

    println!("最終狀態:");
    println!("  Terminated: {}", scheduler.terminated.len());
    println!();
}

fn main() {
    println!("=== Rust 作業系統概念範例 ===\n");

    demo_process_creation();
    demo_scheduling();
    demo_memory();
    demo_signals();
    demo_syscalls();
    demo_context_switch();
    demo_scheduler_simulation();

    println!("作業系統範例完成!");
}
