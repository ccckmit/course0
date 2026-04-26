use std::collections::HashMap;
use std::convert::TryInto;
use std::fs::{File, OpenOptions};
use std::io::{self, Read, Seek, SeekFrom, Write};
use std::str;

const PAGE_SIZE: usize = 4096;
const MAX_KEYS: usize = 3;
const INVALID_PAGE_NUM: u32 = u32::MAX;

// ==========================================
// 序列化大小常數
// ==========================================
const ROW_SIZE: usize = 4 + 32 + 255; // id(4) + username(32) + email(255) = 291
// Node layout in PAGE_SIZE bytes:
// is_leaf(1) + num_keys(4) + parent_page_num(4) + next_page_num(4)
// + keys[(MAX_KEYS+1)*4] + children[(MAX_KEYS+2)*4]
// + rows[(MAX_KEYS+1)*ROW_SIZE]
const NODE_IS_LEAF_OFFSET: usize = 0;
const NODE_NUM_KEYS_OFFSET: usize = 1;
const NODE_PARENT_OFFSET: usize = 5;
const NODE_NEXT_OFFSET: usize = 9;
const NODE_KEYS_OFFSET: usize = 13;
const NODE_CHILDREN_OFFSET: usize = NODE_KEYS_OFFSET + (MAX_KEYS + 1) * 4;
const NODE_ROWS_OFFSET: usize = NODE_CHILDREN_OFFSET + (MAX_KEYS + 2) * 4;

// ==========================================
// 1. 資料結構 (Schema & Node)
// ==========================================
#[derive(Clone, Debug)]
struct Row {
    id: u32,
    username: [u8; 32],
    email: [u8; 255],
}

impl Row {
    fn new(id: u32, username: &str, email: &str) -> Self {
        let mut r = Row {
            id,
            username: [0; 32],
            email: [0; 255],
        };
        let u_bytes = username.as_bytes();
        let e_bytes = email.as_bytes();
        r.username[..u_bytes.len().min(32)].copy_from_slice(&u_bytes[..u_bytes.len().min(32)]);
        r.email[..e_bytes.len().min(255)].copy_from_slice(&e_bytes[..e_bytes.len().min(255)]);
        r
    }

    fn print(&self) {
        let uname = str::from_utf8(&self.username)
            .unwrap()
            .trim_matches(char::from(0));
        let email = str::from_utf8(&self.email)
            .unwrap()
            .trim_matches(char::from(0));
        println!("({}, '{}', '{}')", self.id, uname, email);
    }

    fn serialize(&self, buf: &mut [u8]) {
        buf[0..4].copy_from_slice(&self.id.to_le_bytes());
        buf[4..36].copy_from_slice(&self.username);
        buf[36..291].copy_from_slice(&self.email);
    }

    fn deserialize(buf: &[u8]) -> Self {
        let id = u32::from_le_bytes(buf[0..4].try_into().unwrap());
        let mut username = [0u8; 32];
        let mut email = [0u8; 255];
        username.copy_from_slice(&buf[4..36]);
        email.copy_from_slice(&buf[36..291]);
        Row { id, username, email }
    }
}

#[derive(Clone)]
struct Node {
    is_leaf: bool,
    num_keys: usize,
    parent_page_num: u32,
    next_page_num: u32,
    keys: [u32; MAX_KEYS + 1],
    children_page_nums: [u32; MAX_KEYS + 2],
    rows: Vec<Row>,
}

impl Node {
    fn new(is_leaf: bool) -> Self {
        Node {
            is_leaf,
            num_keys: 0,
            parent_page_num: INVALID_PAGE_NUM,
            next_page_num: INVALID_PAGE_NUM,
            keys: [0; MAX_KEYS + 1],
            children_page_nums: [INVALID_PAGE_NUM; MAX_KEYS + 2],
            rows: vec![Row::new(0, "", ""); MAX_KEYS + 1],
        }
    }

    fn serialize(&self, page: &mut [u8; PAGE_SIZE]) {
        page[NODE_IS_LEAF_OFFSET] = self.is_leaf as u8;
        page[NODE_NUM_KEYS_OFFSET..NODE_NUM_KEYS_OFFSET + 4]
            .copy_from_slice(&(self.num_keys as u32).to_le_bytes());
        page[NODE_PARENT_OFFSET..NODE_PARENT_OFFSET + 4]
            .copy_from_slice(&self.parent_page_num.to_le_bytes());
        page[NODE_NEXT_OFFSET..NODE_NEXT_OFFSET + 4]
            .copy_from_slice(&self.next_page_num.to_le_bytes());
        for i in 0..MAX_KEYS + 1 {
            let off = NODE_KEYS_OFFSET + i * 4;
            page[off..off + 4].copy_from_slice(&self.keys[i].to_le_bytes());
        }
        for i in 0..MAX_KEYS + 2 {
            let off = NODE_CHILDREN_OFFSET + i * 4;
            page[off..off + 4].copy_from_slice(&self.children_page_nums[i].to_le_bytes());
        }
        for i in 0..MAX_KEYS + 1 {
            let off = NODE_ROWS_OFFSET + i * ROW_SIZE;
            self.rows[i].serialize(&mut page[off..off + ROW_SIZE]);
        }
    }

    fn deserialize(page: &[u8; PAGE_SIZE]) -> Self {
        let is_leaf = page[NODE_IS_LEAF_OFFSET] != 0;
        let num_keys =
            u32::from_le_bytes(page[NODE_NUM_KEYS_OFFSET..NODE_NUM_KEYS_OFFSET + 4].try_into().unwrap()) as usize;
        let parent_page_num =
            u32::from_le_bytes(page[NODE_PARENT_OFFSET..NODE_PARENT_OFFSET + 4].try_into().unwrap());
        let next_page_num =
            u32::from_le_bytes(page[NODE_NEXT_OFFSET..NODE_NEXT_OFFSET + 4].try_into().unwrap());
        let mut keys = [0u32; MAX_KEYS + 1];
        for i in 0..MAX_KEYS + 1 {
            let off = NODE_KEYS_OFFSET + i * 4;
            keys[i] = u32::from_le_bytes(page[off..off + 4].try_into().unwrap());
        }
        let mut children_page_nums = [INVALID_PAGE_NUM; MAX_KEYS + 2];
        for i in 0..MAX_KEYS + 2 {
            let off = NODE_CHILDREN_OFFSET + i * 4;
            children_page_nums[i] = u32::from_le_bytes(page[off..off + 4].try_into().unwrap());
        }
        let mut rows = vec![Row::new(0, "", ""); MAX_KEYS + 1];
        for i in 0..MAX_KEYS + 1 {
            let off = NODE_ROWS_OFFSET + i * ROW_SIZE;
            rows[i] = Row::deserialize(&page[off..off + ROW_SIZE]);
        }
        Node {
            is_leaf,
            num_keys,
            parent_page_num,
            next_page_num,
            keys,
            children_page_nums,
            rows,
        }
    }
}

// ==========================================
// 2. Pager (Buffer Pool & Disk I/O)
// ==========================================
struct Pager {
    file: File,
    num_pages: u32,
    cache: HashMap<u32, Node>,
}

impl Pager {
    fn open(filename: &str) -> Self {
        let file = OpenOptions::new()
            .read(true)
            .write(true)
            .create(true)
            .open(filename)
            .expect("Cannot open database file");

        let file_length = file.metadata().unwrap().len() as u32;
        let num_pages = file_length / PAGE_SIZE as u32;

        Pager {
            file,
            num_pages,
            cache: HashMap::new(),
        }
    }

    fn get_new_page_num(&mut self) -> u32 {
        let num = self.num_pages;
        self.num_pages += 1;
        num
    }

    fn get_node(&mut self, page_num: u32) -> Node {
        if let Some(node) = self.cache.get(&page_num) {
            return node.clone();
        }
        // 從磁碟讀取
        let offset = page_num as u64 * PAGE_SIZE as u64;
        let mut page = [0u8; PAGE_SIZE];
        self.file.seek(SeekFrom::Start(offset)).unwrap();
        self.file.read_exact(&mut page).unwrap_or(());
        let node = Node::deserialize(&page);
        self.cache.insert(page_num, node.clone());
        node
    }

    fn write_node(&mut self, page_num: u32, node: Node) {
        self.cache.insert(page_num, node);
        if page_num >= self.num_pages {
            self.num_pages = page_num + 1;
        }
    }

    fn flush_all(&mut self) {
        // 將 cache 中所有 Node 序列化寫入磁碟
        let page_nums: Vec<u32> = self.cache.keys().cloned().collect();
        for page_num in page_nums {
            let node = self.cache[&page_num].clone();
            let mut page = [0u8; PAGE_SIZE];
            node.serialize(&mut page);
            let offset = page_num as u64 * PAGE_SIZE as u64;
            self.file.seek(SeekFrom::Start(offset)).unwrap();
            self.file.write_all(&page).unwrap();
        }
        self.file.flush().unwrap();
    }
}

// ==========================================
// 3. Meta Page (Page 0): 儲存 root_page_num
// ==========================================
const META_ROOT_OFFSET: usize = 0;

fn read_meta(file: &mut File) -> Option<u32> {
    let meta = file.metadata().unwrap();
    if meta.len() < PAGE_SIZE as u64 {
        return None;
    }
    let mut page = [0u8; PAGE_SIZE];
    file.seek(SeekFrom::Start(0)).unwrap();
    file.read_exact(&mut page).unwrap();
    let root = u32::from_le_bytes(page[META_ROOT_OFFSET..META_ROOT_OFFSET + 4].try_into().unwrap());
    if root == INVALID_PAGE_NUM {
        None
    } else {
        Some(root)
    }
}

fn write_meta(file: &mut File, root_page_num: u32) {
    let mut page = [0u8; PAGE_SIZE];
    page[META_ROOT_OFFSET..META_ROOT_OFFSET + 4].copy_from_slice(&root_page_num.to_le_bytes());
    file.seek(SeekFrom::Start(0)).unwrap();
    file.write_all(&page).unwrap();
}

// ==========================================
// 4. Database & B+ Tree Core
// ==========================================
struct Table {
    pager: Pager,
    root_page_num: u32,
}

impl Table {
    fn new(filename: &str) -> Self {
        let mut pager = Pager::open(filename);
        // Page 0 是 meta page，B+ tree 從 Page 1 開始
        if pager.num_pages == 0 {
            // 新資料庫：建立 meta page 佔位
            pager.num_pages = 1;
        }
        // 讀取 meta 取得 root
        let root_page_num = read_meta(&mut pager.file).unwrap_or(INVALID_PAGE_NUM);
        Table { pager, root_page_num }
    }

    fn close(&mut self) {
        self.pager.flush_all();
        // 更新 meta page
        write_meta(&mut self.pager.file, self.root_page_num);
        self.pager.file.flush().unwrap();
    }

    fn find_leaf_page(&mut self, key: u32) -> u32 {
        if self.root_page_num == INVALID_PAGE_NUM {
            return INVALID_PAGE_NUM;
        }
        let mut curr_page_num = self.root_page_num;
        let mut curr = self.pager.get_node(curr_page_num);
        while !curr.is_leaf {
            let mut i = 0;
            while i < curr.num_keys && key >= curr.keys[i] {
                i += 1;
            }
            curr_page_num = curr.children_page_nums[i];
            curr = self.pager.get_node(curr_page_num);
        }
        curr_page_num
    }

    fn insert_into_parent(&mut self, left_page_num: u32, key: u32, right_page_num: u32) {
        let left = self.pager.get_node(left_page_num);
        let mut right = self.pager.get_node(right_page_num);
        let parent_page_num = left.parent_page_num;

        if parent_page_num == INVALID_PAGE_NUM {
            let new_root_num = self.pager.get_new_page_num();
            let mut new_root = Node::new(false);
            new_root.keys[0] = key;
            new_root.children_page_nums[0] = left_page_num;
            new_root.children_page_nums[1] = right_page_num;
            new_root.num_keys = 1;

            let mut left2 = self.pager.get_node(left_page_num);
            left2.parent_page_num = new_root_num;
            right.parent_page_num = new_root_num;
            self.root_page_num = new_root_num;

            self.pager.write_node(left_page_num, left2);
            self.pager.write_node(right_page_num, right);
            self.pager.write_node(new_root_num, new_root);
            return;
        }

        let mut parent = self.pager.get_node(parent_page_num);
        let mut insert_idx = 0;
        while insert_idx < parent.num_keys && parent.keys[insert_idx] < key {
            insert_idx += 1;
        }

        for i in (insert_idx..parent.num_keys).rev() {
            parent.keys[i + 1] = parent.keys[i];
            parent.children_page_nums[i + 2] = parent.children_page_nums[i + 1];
        }

        parent.keys[insert_idx] = key;
        parent.children_page_nums[insert_idx + 1] = right_page_num;
        parent.num_keys += 1;
        right.parent_page_num = parent_page_num;

        self.pager.write_node(right_page_num, right);

        if parent.num_keys > MAX_KEYS {
            let new_internal_num = self.pager.get_new_page_num();
            let mut new_internal = Node::new(false);

            let split_idx = parent.num_keys / 2;
            let up_key = parent.keys[split_idx];

            new_internal.num_keys = parent.num_keys - split_idx - 1;
            for i in 0..new_internal.num_keys {
                new_internal.keys[i] = parent.keys[split_idx + 1 + i];
                new_internal.children_page_nums[i] =
                    parent.children_page_nums[split_idx + 1 + i];
            }
            new_internal.children_page_nums[new_internal.num_keys] =
                parent.children_page_nums[parent.num_keys];

            // 更新被移走的子節點之 parent 指標
            for i in 0..=new_internal.num_keys {
                let child_num = new_internal.children_page_nums[i];
                let mut child = self.pager.get_node(child_num);
                child.parent_page_num = new_internal_num;
                self.pager.write_node(child_num, child);
            }

            parent.num_keys = split_idx;
            self.pager.write_node(parent_page_num, parent);
            self.pager.write_node(new_internal_num, new_internal);

            self.insert_into_parent(parent_page_num, up_key, new_internal_num);
        } else {
            self.pager.write_node(parent_page_num, parent);
        }
    }

    fn insert(&mut self, row: Row) {
        let key = row.id;

        if self.root_page_num == INVALID_PAGE_NUM {
            let root_num = self.pager.get_new_page_num();
            let mut root = Node::new(true);
            root.keys[0] = key;
            root.rows[0] = row;
            root.num_keys = 1;
            self.root_page_num = root_num;
            self.pager.write_node(root_num, root);
            return;
        }

        let leaf_page_num = self.find_leaf_page(key);
        let mut leaf = self.pager.get_node(leaf_page_num);

        for i in 0..leaf.num_keys {
            if leaf.keys[i] == key {
                println!("Error: Duplicate primary key {}.", key);
                return;
            }
        }

        let mut insert_idx = 0;
        while insert_idx < leaf.num_keys && leaf.keys[insert_idx] < key {
            insert_idx += 1;
        }

        for i in (insert_idx..leaf.num_keys).rev() {
            leaf.keys[i + 1] = leaf.keys[i];
            leaf.rows[i + 1] = leaf.rows[i].clone();
        }
        leaf.keys[insert_idx] = key;
        leaf.rows[insert_idx] = row;
        leaf.num_keys += 1;

        if leaf.num_keys > MAX_KEYS {
            let new_leaf_num = self.pager.get_new_page_num();
            let mut new_leaf = Node::new(true);

            let split_idx = leaf.num_keys / 2;
            new_leaf.num_keys = leaf.num_keys - split_idx;

            for i in 0..new_leaf.num_keys {
                new_leaf.keys[i] = leaf.keys[split_idx + i];
                new_leaf.rows[i] = leaf.rows[split_idx + i].clone();
            }
            leaf.num_keys = split_idx;

            new_leaf.next_page_num = leaf.next_page_num;
            leaf.next_page_num = new_leaf_num;
            new_leaf.parent_page_num = leaf.parent_page_num;

            let up_key = new_leaf.keys[0];
            self.pager.write_node(leaf_page_num, leaf);
            self.pager.write_node(new_leaf_num, new_leaf);

            self.insert_into_parent(leaf_page_num, up_key, new_leaf_num);
        } else {
            self.pager.write_node(leaf_page_num, leaf);
        }
    }

    fn select_all(&mut self) {
        if self.root_page_num == INVALID_PAGE_NUM {
            println!("Empty table.");
            return;
        }

        // 走到最左葉節點
        let mut curr_page_num = self.root_page_num;
        let mut curr = self.pager.get_node(curr_page_num);
        while !curr.is_leaf {
            curr_page_num = curr.children_page_nums[0];
            curr = self.pager.get_node(curr_page_num);
        }

        let mut count = 0;
        while curr_page_num != INVALID_PAGE_NUM {
            curr = self.pager.get_node(curr_page_num);
            for i in 0..curr.num_keys {
                curr.rows[i].print();
                count += 1;
            }
            curr_page_num = curr.next_page_num;
        }
        println!("Total rows: {}", count);
    }

    fn select_by_key(&mut self, key: u32) {
        if self.root_page_num == INVALID_PAGE_NUM {
            println!("Empty table.");
            return;
        }
        let leaf_page_num = self.find_leaf_page(key);
        if leaf_page_num == INVALID_PAGE_NUM {
            println!("Row not found.");
            return;
        }
        let leaf = self.pager.get_node(leaf_page_num);
        for i in 0..leaf.num_keys {
            if leaf.keys[i] == key {
                leaf.rows[i].print();
                return;
            }
        }
        println!("Row not found.");
    }
}

// ==========================================
// 5. REPL
// ==========================================
fn main() {
    let mut table = Table::new("mydb.db");
    println!("Welcome to sql_rust.");

    loop {
        print!("sql_rust> ");
        io::stdout().flush().unwrap();

        let mut input = String::new();
        if io::stdin().read_line(&mut input).unwrap() == 0 {
            break;
        }
        let cmd = input.trim();

        if cmd == ".exit" {
            table.close();
            println!("Database saved. Bye.");
            break;
        } else if cmd.starts_with("insert") {
            let parts: Vec<&str> = cmd.split_whitespace().collect();
            if parts.len() == 4 {
                if let Ok(id) = parts[1].parse::<u32>() {
                    let row = Row::new(id, parts[2], parts[3]);
                    table.insert(row);
                    println!("Executed.");
                } else {
                    println!("Syntax error: id must be a positive integer.");
                }
            } else {
                println!("Syntax error. Usage: insert <id> <username> <email>");
            }
        } else if cmd.starts_with("select ") {
            let parts: Vec<&str> = cmd.split_whitespace().collect();
            if parts.len() == 2 {
                if let Ok(id) = parts[1].parse::<u32>() {
                    table.select_by_key(id);
                } else {
                    println!("Syntax error: id must be a positive integer.");
                }
            } else {
                println!("Syntax error. Usage: select <id>");
            }
        } else if cmd == "select_all" {
            table.select_all();
        } else if !cmd.is_empty() {
            println!("Unrecognized command.");
        }
    }
}