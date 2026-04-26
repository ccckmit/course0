// Rust 記憶體管理範例
// 展示 Box, Rc, Arc, Cell, RefCell, 智慧指標

use std::cell::{Ref, RefCell, RefMut};
use std::rc::Rc;
use std::sync::{Arc, Mutex};
use std::thread;

// 1. Box - 堆配置
fn box_example() {
    println!("=== Box - 堆配置 ===");

    // 在堆上分配資料
    let b = Box::new(5);
    println!("Box 內容: {}", b);

    // 遞迴資料結構
    #[derive(Debug)]
    enum List {
        Cons(i32, Box<List>),
        Nil,
    }

    let list = List::Cons(1, Box::new(List::Cons(2, Box::new(List::Nil))));
    println!("遞迴列舉: {:?}", list);
    println!();
}

// 2. Rc - 參考計數
fn rc_example() {
    println!("=== Rc - 參考計數 ===");

    let data = Rc::new(vec![1, 2, 3]);
    println!("初始參考計數: {}", Rc::strong_count(&data));

    {
        let clone1 = Rc::clone(&data);
        println!("clone1 後計數: {}", Rc::strong_count(&data));

        let clone2 = Rc::clone(&data);
        println!("clone2 後計數: {}", Rc::strong_count(&data));
    }

    println!("離開作用域後計數: {}", Rc::strong_count(&data));
    println!("資料: {:?}", data);
    println!();
}

// 3. RefCell - 內部可變性
fn refcell_example() {
    println!("=== RefCell - 內部可變性 ===");

    let data = RefCell::new(vec![1, 2, 3]);

    // 不可變借用
    let borrows: Ref<Vec<i32>> = data.borrow();
    println!("借用資料: {:?}", borrows);
    drop(borrows);

    // 可變借用
    let mut borrows = data.borrow_mut();
    borrows.push(4);
    println!("修改後資料: {:?}", borrows);
    println!();
}

// 4. Rc + RefCell 組合
fn rc_refcell_example() {
    println!("=== Rc + RefCell 組合 ===");

    #[derive(Debug)]
    struct Node {
        value: i32,
        next: RefCell<Option<Rc<Node>>>,
    }

    let node1 = Rc::new(Node {
        value: 1,
        next: RefCell::new(None),
    });

    let node2 = Rc::new(Node {
        value: 2,
        next: RefCell::new(None),
    });

    // node1 -> node2
    let mut next = node1.next.borrow_mut();
    *next = Some(Rc::clone(&node2));

    println!("node1: {:?}", node1);
    println!("node2: {:?}", node2);
    println!();
}

// 5. Arc - 原子參考計數（多用於執行緒間）
fn arc_example() {
    println!("=== Arc - 原子參考計數 ===");

    use std::sync::atomic::{AtomicUsize, Ordering};

    let counter = Arc::new(AtomicUsize::new(0));

    let handles: Vec<_> = (0..5)
        .map(|_| {
            let counter = Arc::clone(&counter);
            thread::spawn(move || {
                for _ in 0..100 {
                    counter.fetch_add(1, Ordering::SeqCst);
                }
            })
        })
        .collect();

    for handle in handles {
        handle.join().unwrap();
    }

    println!("Arc 原子計數: {}", counter.load(Ordering::SeqCst));
    println!();
}

// 6. Mutex 與 Arc 組合
fn mutex_example() {
    println!("=== Mutex 與 Arc ===");

    let counter = Arc::new(Mutex::new(0));
    let mut handles = vec![];

    for _ in 0..5 {
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

    println!("Mutex 計數: {}", *counter.lock().unwrap());
    println!();
}

// 7. Cell - 簡單內部可變性（僅適用於 Copy 類型）
fn cell_example() {
    println!("=== Cell - 內部可變性 ===");

    use std::cell::Cell;

    let cell = Cell::new(5);
    println!("Cell 內容: {}", cell.get());

    cell.set(10);
    println!("Cell 修改後: {}", cell.get());

    // 取出再放入
    let old = cell.replace(20);
    println!("舊值: {}, 新值: {}", old, cell.get());
    println!();
}

// 8. 可變借用規則與借用檢查
fn borrowing_rules() {
    println!("=== 借用規則 ===");

    let mut value = 10;

    // 不可變借用
    let r1 = &value;
    println!("r1: {}", r1);

    // 可變借用
    let r2 = &mut value;
    *r2 += 1;
    println!("修改後: {}", value);

    println!();
}

// 9. Drop 特徵 - 自訂釋放邏輯
fn drop_trait() {
    println!("=== Drop 特徵 ===");

    struct MyStruct {
        name: String,
    }

    impl Drop for MyStruct {
        fn drop(&mut self) {
            println!("釋放: {}", self.name);
        }
    }

    {
        let _a = MyStruct {
            name: String::from("A"),
        };
        let _b = MyStruct {
            name: String::from("B"),
        };
    }

    println!("離開作用域");
    println!();
}

// 10. 記憶體安全範例
fn memory_safety() {
    println!("=== 記憶體安全 ===");

    // Rust 防止緩衝區溢出
    let arr = [1, 2, 3, 4, 5];
    println!("陣列: {:?}", arr);

    // 防止空指標
    let option: Option<&i32> = Some(&arr[0]);
    println!("Optional: {:?}", option);

    let option: Option<&i32> = arr.get(10);
    println!("越界訪問: {:?}", option);

    // 防止 Use-After-Free
    let owner = String::from("owned");
    let _borrow = &owner;
    // 在借用仍然有效時使用
    println!("借用仍然有效: {}", _borrow);
}

fn main() {
    println!("=== Rust 記憶體管理範例 ===\n");

    box_example();
    rc_example();
    refcell_example();
    rc_refcell_example();
    arc_example();
    mutex_example();
    cell_example();
    borrowing_rules();
    drop_trait();
    memory_safety();

    println!("範例完成!");
}
