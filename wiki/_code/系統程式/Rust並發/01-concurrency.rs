// Rust 並發範例
// 展示執行緒、通道、同步原語

use std::collections::HashMap;
use std::sync::mpsc;
use std::sync::{Arc, Barrier, Condvar, Mutex, RwLock};
use std::thread;
use std::time::Duration;

// 1. 基本執行緒
fn basic_threading() {
    println!("=== 基本執行緒 ===");

    let handle = thread::spawn(|| {
        for i in 1..=5 {
            println!("執行緒: {}", i);
            thread::sleep(Duration::from_millis(100));
        }
    });

    handle.join().unwrap();
    println!("執行緒完成\n");
}

// 2. 使用 move 閉包
fn move_closure() {
    println!("=== Move 閉包 ===");

    let v = vec![1, 2, 3];

    let handle = thread::spawn(move || {
        println!("取得的向量: {:?}", v);
    });

    handle.join().unwrap();
    println!("\n");
}

// 3. 執行緒間通訊 - 通道
fn channel_communication() {
    println!("=== 通道通訊 ===");

    let (tx, rx) = mpsc::channel();

    // 生產者執行緒
    let tx_clone = tx.clone();
    thread::spawn(move || {
        for msg in vec!["Hello", "World", "Rust"] {
            tx_clone.send(msg).unwrap();
            thread::sleep(Duration::from_millis(100));
        }
    });

    // 消費者執行緒
    let handle = thread::spawn(move || {
        for received in rx {
            println!("收到: {}", received);
        }
    });

    handle.join().unwrap();
    println!();
}

// 4. 共享資料 - Arc 與 Mutex
fn shared_data() {
    println!("=== 共享資料 (Arc + Mutex) ===");

    let counter = Arc::new(Mutex::new(0));
    let mut handles = vec![];

    for _ in 0..10 {
        let counter = Arc::clone(&counter);
        let handle = thread::spawn(move || {
            let mut num = counter.lock().unwrap();
            *num += 1;
        });
        handles.push(handle);
    }

    for handle in handles {
        handle.join().unwrap();
    }

    println!("最終計數: {}", *counter.lock().unwrap());
    println!();
}

// 5. RwLock - 讀寫鎖
fn rw_lock() {
    println!("=== 讀寫鎖 ===");

    let data = Arc::new(RwLock::new(HashMap::new()));

    // 寫入執行緒
    let data_clone = Arc::clone(&data);
    thread::spawn(move || {
        let mut map = data_clone.write().unwrap();
        map.insert("key", "value");
        println!("寫入完成");
    })
    .join()
    .unwrap();

    // 讀取執行緒
    let data_clone = Arc::clone(&data);
    thread::spawn(move || {
        let map = data_clone.read().unwrap();
        println!("讀取: {:?}", map.get("key"));
    })
    .join()
    .unwrap();

    println!();
}

// 6. 條件變數 - Condition Variable
fn condition_variable() {
    println!("=== 條件變數 ===");

    let queue: Arc<Mutex<Vec<i32>>> = Arc::new(Mutex::new(Vec::new()));
    let not_empty = Arc::new(Condvar::new());

    // 生產者
    let queue_clone = Arc::clone(&queue);
    let not_empty_clone = Arc::clone(&not_empty);
    let producer = thread::spawn(move || {
        for i in 0..5 {
            thread::sleep(Duration::from_millis(100));
            let mut q = queue_clone.lock().unwrap();
            q.push(i);
            println!("生產: {}", i);
            not_empty_clone.notify_one();
        }
    });

    // 消費者
    let queue_clone = Arc::clone(&queue);
    let not_empty_clone = Arc::clone(&not_empty);
    let consumer = thread::spawn(move || {
        for _ in 0..5 {
            let mut q = queue_clone.lock().unwrap();
            while q.is_empty() {
                q = not_empty_clone.wait(q).unwrap();
            }
            let item = q.remove(0);
            println!("消費: {}", item);
        }
    });

    producer.join().unwrap();
    consumer.join().unwrap();
    println!();
}

// 7. 屏障 - Barrier
fn barrier_demo() {
    println!("=== 屏障 ===");

    let barrier = Arc::new(Barrier::new(3));
    let mut handles = vec![];

    for i in 0..3 {
        let barrier = Arc::clone(&barrier);
        let handle = thread::spawn(move || {
            println!("執行緒 {} 開始", i);
            barrier.wait();
            println!("執行緒 {} 通過屏障", i);
        });
        handles.push(handle);
    }

    for handle in handles {
        handle.join().unwrap();
    }
    println!();
}

// 8. 原子操作
fn atomic_operations() {
    use std::sync::atomic::{AtomicUsize, Ordering};

    println!("=== 原子操作 ===");

    let counter = Arc::new(AtomicUsize::new(0));

    let handles: Vec<_> = (0..10)
        .map(|_| {
            let counter = Arc::clone(&counter);
            thread::spawn(move || {
                for _ in 0..1000 {
                    counter.fetch_add(1, Ordering::SeqCst);
                }
            })
        })
        .collect();

    for handle in handles {
        handle.join().unwrap();
    }

    println!("原子計數: {}", counter.load(Ordering::SeqCst));
    println!();
}

// 9. 計數器訊號量實現
struct Semaphore {
    permits: Mutex<usize>,
    condvar: Condvar,
}

impl Semaphore {
    fn new(permits: usize) -> Self {
        Semaphore {
            permits: Mutex::new(permits),
            condvar: Condvar::new(),
        }
    }

    fn acquire(&self) {
        let mut permits = self.permits.lock().unwrap();
        while *permits == 0 {
            permits = self.condvar.wait(permits).unwrap();
        }
        *permits -= 1;
    }

    fn release(&self) {
        let mut permits = self.permits.lock().unwrap();
        *permits += 1;
        self.condvar.notify_one();
    }
}

// 10. 延遲初始化 - Once
fn once_initialization() {
    use std::sync::OnceLock;

    println!("=== 延遲初始化 ===");

    static INIT: OnceLock<String> = OnceLock::new();

    let handle1 = thread::spawn(|| {
        INIT.get_or_init(|| {
            println!("初始化中...");
            String::from("Hello, World!")
        })
    });

    let handle2 = thread::spawn(|| {
        INIT.get_or_init(|| {
            println!("初始化中...");
            String::from("Hello, World!")
        })
    });

    println!("結果: {}", INIT.get().unwrap());
    handle1.join().unwrap();
    handle2.join().unwrap();
}

fn main() {
    println!("=== Rust 並發範例 ===\n");

    basic_threading();
    move_closure();
    channel_communication();
    shared_data();
    rw_lock();
    condition_variable();
    barrier_demo();
    atomic_operations();
    once_initialization();

    println!("所有並發範例完成!");
}
