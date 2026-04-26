use libc;
use std::env;
use std::fs;
use std::io::{self, Write};
use std::mem::MaybeUninit;
use std::time::Duration;

#[derive(PartialEq)]
enum Mode {
    Command,
    Insert,
}

// 終端機 Raw Mode 控制模組
struct TerminalGuard {
    orig_termios: Option<libc::termios>,
}

impl TerminalGuard {
    fn enable_raw_mode() -> Self {
        unsafe {
            let mut term = MaybeUninit::<libc::termios>::uninit();
            libc::tcgetattr(libc::STDIN_FILENO, term.as_mut_ptr());
            let mut term = term.assume_init();
            let orig = term;

            term.c_lflag &= !(libc::ICANON | libc::ECHO);
            libc::tcsetattr(libc::STDIN_FILENO, libc::TCSAFLUSH, &term);

            TerminalGuard {
                orig_termios: Some(orig),
            }
        }
    }

    fn kbhit(&self) -> bool {
        unsafe {
            let mut tv = libc::timeval { tv_sec: 0, tv_usec: 0 };
            let mut fds: libc::fd_set = std::mem::zeroed();
            libc::FD_ZERO(&mut fds);
            libc::FD_SET(libc::STDIN_FILENO, &mut fds);
            libc::select(
                libc::STDIN_FILENO + 1,
                &mut fds,
                std::ptr::null_mut(),
                std::ptr::null_mut(),
                &mut tv,
            ) > 0
        }
    }
}

impl Drop for TerminalGuard {
    fn drop(&mut self) {
        if let Some(orig) = self.orig_termios {
            unsafe {
                libc::tcsetattr(libc::STDIN_FILENO, libc::TCSAFLUSH, &orig);
            }
        }
        print!("\x1b[2J\x1b[H");
    }
}

// 不依賴 Rust 的 Stdout 緩衝，直接透過 libc 讀取，避免與 select(kbhit) 發生衝突
fn read_char() -> Option<u8> {
    let mut buf = [0u8; 1];
    let n = unsafe { libc::read(libc::STDIN_FILENO, buf.as_mut_ptr() as *mut libc::c_void, 1) };
    if n == 1 {
        Some(buf[0])
    } else {
        None
    }
}

struct Editor {
    lines: Vec<Vec<char>>,
    cursor_row: usize,
    cursor_col: usize,
    filename: String,
    modified: bool,
    mode: Mode,
}

impl Editor {
    fn new() -> Self {
        Editor {
            lines: vec![vec![]],
            cursor_row: 0,
            cursor_col: 0,
            filename: String::new(),
            modified: false,
            mode: Mode::Command,
        }
    }

    fn open(&mut self, filename: &str) {
        self.filename = filename.to_string();
        if let Ok(content) = fs::read_to_string(filename) {
            self.lines.clear();
            for line in content.lines() {
                self.lines.push(line.chars().collect());
            }
            if self.lines.is_empty() {
                self.lines.push(vec![]);
            }
        } else {
            self.lines = vec![vec![]];
        }
        self.cursor_row = 0;
        self.cursor_col = 0;
        self.modified = false;
    }

    fn save(&mut self) -> bool {
        if self.filename.is_empty() {
            return false;
        }
        let mut content = String::new();
        for line in &self.lines {
            let s: String = line.iter().collect();
            content.push_str(&s);
            content.push('\n');
        }
        if fs::write(&self.filename, content).is_ok() {
            self.modified = false;
            true
        } else {
            false
        }
    }

    fn insert_char(&mut self, c: char) {
        self.lines[self.cursor_row].insert(self.cursor_col, c);
        self.cursor_col += 1;
        self.modified = true;
    }

    fn backspace(&mut self) {
        if self.cursor_col > 0 {
            self.lines[self.cursor_row].remove(self.cursor_col - 1);
            self.cursor_col -= 1;
            self.modified = true;
        } else if self.cursor_row > 0 {
            let mut current_line = self.lines.remove(self.cursor_row);
            self.cursor_row -= 1;
            self.cursor_col = self.lines[self.cursor_row].len();
            self.lines[self.cursor_row].append(&mut current_line);
            self.modified = true;
        }
    }

    fn newline(&mut self) {
        let current_line = &mut self.lines[self.cursor_row];
        let new_line = current_line.split_off(self.cursor_col);
        self.lines.insert(self.cursor_row + 1, new_line);
        self.cursor_row += 1;
        self.cursor_col = 0;
        self.modified = true;
    }

    fn move_cursor(&mut self, direction: u8) {
        match direction {
            0 => { // Left
                if self.cursor_col > 0 { self.cursor_col -= 1; }
            }
            1 => { // Right
                if self.cursor_col < self.lines[self.cursor_row].len() { self.cursor_col += 1; }
            }
            2 => { // Up
                if self.cursor_row > 0 {
                    self.cursor_row -= 1;
                    self.cursor_col = self.cursor_col.min(self.lines[self.cursor_row].len());
                }
            }
            3 => { // Down
                if self.cursor_row < self.lines.len() - 1 {
                    self.cursor_row += 1;
                    self.cursor_col = self.cursor_col.min(self.lines[self.cursor_row].len());
                }
            }
            _ => {}
        }
    }

    fn execute_command(&mut self, cmd: &str) -> bool {
        let parts: Vec<&str> = cmd.split_whitespace().collect();
        if parts.is_empty() { return false; }

        match parts[0] {
            "wq" => { self.save(); return true; }
            "w" => {
                if parts.len() > 1 { self.filename = parts[1].to_string(); }
                self.save();
            }
            "q" => { if !self.modified { return true; } }
            "q!" => return true,
            _ => {}
        }
        false
    }

    fn render(&self) {
        // 使用 String 收集所有要印出的畫面，最後一次輸出，避免螢幕閃爍 (雙緩衝概念)
        let mut out = String::new();
        out.push_str("\x1b[2J\x1b[H"); // 清空畫面並回到左上角

        // 簡易的視角滾動 (Viewport)，防止檔案行數過多時終端機捲動造成游標亂跑
        let display_lines = 20;
        let start_row = if self.cursor_row >= display_lines {
            self.cursor_row - display_lines + 1
        } else {
            0
        };

        for i in 0..display_lines {
            let line_idx = start_row + i;
            if line_idx < self.lines.len() {
                let s: String = self.lines[line_idx].iter().collect();
                out.push_str(&s);
            } else {
                out.push('~');
            }
            out.push_str("\x1b[K\r\n"); // \x1b[K 清除該行殘影, \r\n 確保不會產生階梯狀對齊
        }

        let status = if self.mode == Mode::Insert {
            "-- INSERT --".to_string()
        } else {
            let fname = if self.filename.is_empty() { "[No Name]" } else { &self.filename };
            let modified = if self.modified { " [Modified]" } else { "" };
            format!("\"{}\"{} {}/{} -- COMMAND --", fname, modified, self.cursor_row + 1, self.lines.len())
        };

        // 寫入狀態列 (固定在第 21 行)
        out.push_str(&format!("\x1b[21;1H\x1b[K\x1b[7m{}\x1b[0m", status));
        
        // 將游標放置到實際螢幕的對應位置 (+1 因為終端機座標是從 1 開始)
        let screen_cursor_row = self.cursor_row - start_row;
        out.push_str(&format!("\x1b[{};{}H", screen_cursor_row + 1, self.cursor_col + 1));
        
        print!("{}", out);
        io::stdout().flush().unwrap();
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let mut ed = Editor::new();
    
    if args.len() > 1 {
        ed.open(&args[1]);
    }

    let term = TerminalGuard::enable_raw_mode();

    loop {
        ed.render();

        let Some(c) = read_char() else { break; };

        if ed.mode == Mode::Command {
            match c {
                b'i' => ed.mode = Mode::Insert,
                27 => { // ESC
                    std::thread::sleep(Duration::from_millis(10));
                    if term.kbhit() {
                        if let Some(b1) = read_char() {
                            if b1 == b'[' {
                                if let Some(b2) = read_char() {
                                    match b2 {
                                        b'A' => ed.move_cursor(2), // Up
                                        b'B' => ed.move_cursor(3), // Down
                                        b'C' => ed.move_cursor(1), // Right
                                        b'D' => ed.move_cursor(0), // Left
                                        _ => {}
                                    }
                                }
                            }
                        }
                    }
                }
                b'h' => ed.move_cursor(0),
                b'l' => ed.move_cursor(1),
                b'k' => ed.move_cursor(2),
                b'j' => ed.move_cursor(3),
                b':' => {
                    let mut cmd = String::new();
                    // 定位至 21 行並清除狀態列內容
                    print!("\x1b[21;1H\x1b[K:\x1b[K");
                    io::stdout().flush().unwrap();

                    let mut is_esc = false;
                    loop {
                        if let Some(cc) = read_char() {
                            if cc == b'\n' || cc == b'\r' { break; }
                            if cc == 27 { is_esc = true; break; }
                            
                            cmd.push(cc as char);
                            print!("{}", cc as char);
                            io::stdout().flush().unwrap();
                        } else {
                            break;
                        }
                    }
                    if is_esc { continue; }
                    
                    if ed.execute_command(&cmd) {
                        return; // 離開程式
                    }
                }
                _ => {}
            }
        } else {
            match c {
                27 => { // ESC
                    std::thread::sleep(Duration::from_millis(10));
                    if term.kbhit() {
                        if let Some(b1) = read_char() {
                            if b1 == b'[' {
                                if let Some(b2) = read_char() {
                                    match b2 {
                                        b'A' => ed.move_cursor(2),
                                        b'B' => ed.move_cursor(3),
                                        b'C' => ed.move_cursor(1),
                                        b'D' => ed.move_cursor(0),
                                        _ => {}
                                    }
                                }
                            }
                        }
                    }
                    ed.mode = Mode::Command;
                }
                127 | 8 => ed.backspace(), // Backspace
                b'\n' | b'\r' => ed.newline(), // 處理 Enter 鍵
                c if c >= 32 => ed.insert_char(c as char), // 正常可印出字元
                _ => {}
            }
        }
    }
}