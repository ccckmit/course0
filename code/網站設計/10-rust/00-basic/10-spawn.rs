use std::thread;

fn main() {
    let v = vec![1, 2, 3];

    // 使用 move 將所有權移入執行緒
    let handle = thread::spawn(move || {
        println!("來自執行緒的向量: {:?}", v);
    });

    handle.join().unwrap();
}