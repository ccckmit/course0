// Rust 有限狀態機範例
// 展示 DFA、NFA、狀態轉換圖

use std::collections::HashMap;
use std::collections::HashSet;

// ============ 1. 有限狀態機 (FSM) 基本結構 ============
#[derive(Debug, Clone, PartialEq)]
enum State {
    Normal(String),
    Accept(String),
    Trap(String),
}

#[derive(Debug, Clone)]
struct FSM {
    states: HashSet<String>,
    alphabet: HashSet<char>,
    transitions: HashMap<(String, char), String>,
    start_state: String,
    accept_states: HashSet<String>,
}

impl FSM {
    fn new() -> Self {
        FSM {
            states: HashSet::new(),
            alphabet: HashSet::new(),
            transitions: HashMap::new(),
            start_state: String::new(),
            accept_states: HashSet::new(),
        }
    }

    fn add_state(&mut self, name: &str, is_accept: bool) {
        self.states.insert(name.to_string());
        if is_accept {
            self.accept_states.insert(name.to_string());
        }
    }

    fn add_transition(&mut self, from: &str, to: &str, symbol: char) {
        self.transitions
            .insert((from.to_string(), symbol), to.to_string());
        self.alphabet.insert(symbol);
    }

    fn set_start(&mut self, state: &str) {
        self.start_state = state.to_string();
    }

    fn process(&self, input: &str) -> bool {
        let mut current = &self.start_state;

        for ch in input.chars() {
            if let Some(next) = self.transitions.get(&(current.clone(), ch)) {
                current = next;
            } else {
                return false;
            }
        }

        self.accept_states.contains(current)
    }

    fn simulate(&self, input: &str) -> Vec<String> {
        let mut current = &self.start_state;
        let mut path = vec![current.clone()];

        for ch in input.chars() {
            if let Some(next) = self.transitions.get(&(current.clone(), ch)) {
                current = next;
                path.push(current.clone());
            }
        }

        path
    }
}

// ============ 2. NFA (非確定有限狀態機) ============
#[derive(Debug, Clone)]
struct NFA {
    states: HashSet<String>,
    epsilon_transitions: HashMap<String, Vec<String>>,
    transitions: HashMap<(String, char), Vec<String>>,
    start_state: String,
    accept_states: HashSet<String>,
}

impl NFA {
    fn new() -> Self {
        NFA {
            states: HashSet::new(),
            epsilon_transitions: HashMap::new(),
            transitions: HashMap::new(),
            start_state: String::new(),
            accept_states: HashSet::new(),
        }
    }

    fn add_state(&mut self, name: &str, is_accept: bool) {
        self.states.insert(name.to_string());
        if is_accept {
            self.accept_states.insert(name.to_string());
        }
    }

    fn add_transition(&mut self, from: &str, to: &str, symbol: char) {
        self.transitions
            .entry((from.to_string(), symbol))
            .or_insert_with(Vec::new)
            .push(to.to_string());
    }

    fn add_epsilon(&mut self, from: &str, to: &str) {
        self.epsilon_transitions
            .entry(from.to_string())
            .or_insert_with(Vec::new)
            .push(to.to_string());
    }

    fn set_start(&mut self, state: &str) {
        self.start_state = state.to_string();
    }

    fn epsilon_closure(&self, states: &HashSet<String>) -> HashSet<String> {
        let mut closure = states.clone();
        let mut stack: Vec<String> = states.iter().cloned().collect();

        while let Some(state) = stack.pop() {
            if let Some(next_states) = self.epsilon_transitions.get(&state) {
                for next in next_states {
                    if !closure.contains(next) {
                        closure.insert(next.clone());
                        stack.push(next.clone());
                    }
                }
            }
        }

        closure
    }

    fn process(&self, input: &str) -> bool {
        let mut current = self.epsilon_closure(&{
            let mut s = HashSet::new();
            s.insert(self.start_state.clone());
            s
        });

        for ch in input.chars() {
            let mut next_states = HashSet::new();

            for state in &current {
                if let Some(destinations) = self.transitions.get(&(state.clone(), ch)) {
                    for dest in destinations {
                        next_states.insert(dest.clone());
                    }
                }
            }

            current = self.epsilon_closure(&next_states);

            if current.is_empty() {
                return false;
            }
        }

        for state in &current {
            if self.accept_states.contains(state) {
                return true;
            }
        }

        false
    }
}

// ============ 3. 狀態轉換圖 ============
struct StateDiagram {
    nodes: HashMap<String, String>,
    edges: Vec<(String, String, String)>,
}

impl StateDiagram {
    fn new() -> Self {
        StateDiagram {
            nodes: HashMap::new(),
            edges: Vec::new(),
        }
    }

    fn add_state(&mut self, name: &str, label: &str) {
        self.nodes.insert(name.to_string(), label.to_string());
    }

    fn add_edge(&mut self, from: &str, to: &str, label: &str) {
        self.edges
            .push((from.to_string(), to.to_string(), label.to_string()));
    }

    fn to_mermaid(&self) -> String {
        let mut md = String::from("stateDiagram-v2\n");

        for (name, label) in &self.nodes {
            if self.edges.iter().any(|(f, t, _)| f == name || t == name) {
                md.push_str(&format!("    {}: {}\n", name, label));
            }
        }

        for (from, to, label) in &self.edges {
            md.push_str(&format!("    {} --> {}: {}\n", from, to, label));
        }

        md
    }
}

// ============ 4. 正規表達式轉 FSM ============
struct RegexToFSM;

impl RegexToFSM {
    fn from_literal(c: char) -> FSM {
        let mut fsm = FSM::new();
        fsm.add_state("q0", false);
        fsm.add_state("q1", true);
        fsm.add_transition("q0", "q1", c);
        fsm.set_start("q0");
        fsm
    }

    fn concatenate(fsm1: &FSM, fsm2: &FSM) -> FSM {
        let mut fsm = FSM::new();

        for state in &fsm1.states {
            fsm.add_state(state, false);
        }
        for state in &fsm2.states {
            fsm.add_state(state, fsm2.accept_states.contains(state));
        }

        for (key, to) in &fsm1.transitions {
            let (from, symbol) = key;
            fsm.add_transition(from, to, *symbol);
        }
        for (key, to) in &fsm2.transitions {
            let (from, symbol) = key;
            fsm.add_transition(from, to, *symbol);
        }

        fsm.set_start(&fsm1.start_state);

        for accept in &fsm1.accept_states {
            fsm.add_transition(accept, &fsm2.start_state, 'ε');
        }

        fsm
    }

    fn union(fsm1: &FSM, fsm2: &FSM) -> FSM {
        let mut fsm = FSM::new();

        fsm.add_state("start", false);
        fsm.add_state("new_accept", true);

        for state in &fsm1.states {
            fsm.add_state(state, fsm1.accept_states.contains(state));
        }
        for state in &fsm2.states {
            fsm.add_state(state, fsm2.accept_states.contains(state));
        }

        for (key, to) in &fsm1.transitions {
            let (from, symbol) = key;
            fsm.add_transition(from, to, *symbol);
        }
        for (key, to) in &fsm2.transitions {
            let (from, symbol) = key;
            fsm.add_transition(from, to, *symbol);
        }

        fsm.add_transition("start", &fsm1.start_state, 'ε');
        fsm.add_transition("start", &fsm2.start_state, 'ε');

        for accept in &fsm1.accept_states {
            fsm.add_transition(accept, "new_accept", 'ε');
        }
        for accept in &fsm2.accept_states {
            fsm.add_transition(accept, "new_accept", 'ε');
        }

        fsm.set_start("start");

        fsm
    }

    fn kleene_star(fsm: &FSM) -> FSM {
        let mut result = FSM::new();

        result.add_state("start", false);
        result.add_state("new_accept", true);

        for state in &fsm.states {
            result.add_state(state, state == &fsm.start_state);
        }

        for (key, to) in &fsm.transitions {
            let (from, symbol) = key;
            result.add_transition(from, to, *symbol);
        }

        result.add_transition("start", "new_accept", 'ε');
        result.add_transition("start", &fsm.start_state, 'ε');

        for accept in &fsm.accept_states {
            result.add_transition(accept, "new_accept", 'ε');
            result.add_transition(accept, &fsm.start_state, 'ε');
        }

        result.set_start("start");

        result
    }
}

// ============ 5. 範例 FSM ============
fn demo_binary_divisible_by_3() {
    println!("=== 1. 二進位可被 3 整除的 FSM ===");

    let mut fsm = FSM::new();

    // 狀態: r0 (餘數0), r1 (餘數1), r2 (餘數2)
    fsm.add_state("r0", true);
    fsm.add_state("r1", false);
    fsm.add_state("r2", false);

    // 轉換
    // r0 -> r0: 0, r0 -> r1: 1
    fsm.add_transition("r0", "r0", '0');
    fsm.add_transition("r0", "r1", '1');

    // r1 -> r2: 0, r1 -> r0: 1
    fsm.add_transition("r1", "r2", '0');
    fsm.add_transition("r1", "r0", '1');

    // r2 -> r1: 0, r2 -> r2: 1
    fsm.add_transition("r2", "r1", '0');
    fsm.add_transition("r2", "r2", '1');

    fsm.set_start("r0");

    // 測試
    let tests = vec!["0", "11", "110", "1001", "1010", "1111"];

    for test in tests {
        let result = fsm.process(test);
        println!("  '{}' 可被 3 整除: {}", test, result);
    }
    println!();
}

fn demo_binary_ends_with_01() {
    println!("=== 2. 二進位以 01 結尾 ===");

    let mut fsm = FSM::new();

    fsm.add_state("q0", false); // 初始
    fsm.add_state("q1", false); // 讀到 0
    fsm.add_state("q2", true); // 讀到 01

    fsm.add_transition("q0", "q0", '0');
    fsm.add_transition("q0", "q1", '1');
    fsm.add_transition("q1", "q2", '1');
    fsm.add_transition("q1", "q0", '0');
    fsm.add_transition("q2", "q0", '0');
    fsm.add_transition("q2", "q1", '1');

    fsm.set_start("q0");

    let tests = vec!["01", "001", "101", "1101", "010", "11", "100"];

    for test in tests {
        let result = fsm.process(test);
        println!("  '{}' 以 01 結尾: {}", test, result);
    }
    println!();
}

fn demo_nfa_example() {
    println!("=== 3. NFA 範例 (包含 'ab' 或 'ba') ===");

    let mut nfa = NFA::new();

    // q0: 起始
    // q1: 讀到 a
    // q2: 讀到 b
    // q3: 讀到 ab
    // q4: 讀到 ba
    // q5: 接受

    nfa.add_state("q0", false);
    nfa.add_state("q1", false);
    nfa.add_state("q2", false);
    nfa.add_state("q3", true);
    nfa.add_state("q4", true);
    nfa.add_state("q5", true);

    nfa.set_start("q0");

    // 轉換
    nfa.add_transition("q0", "q1", 'a');
    nfa.add_transition("q0", "q2", 'b');
    nfa.add_transition("q1", "q3", 'b');
    nfa.add_transition("q2", "q4", 'a');
    nfa.add_transition("q3", "q5", 'ε');
    nfa.add_transition("q4", "q5", 'ε');

    let tests = vec!["ab", "ba", "aab", "bba", "a", "b", "aba", "bab"];

    for test in tests {
        let result = nfa.process(test);
        println!("  '{}' 包含 ab 或 ba: {}", test, result);
    }
    println!();
}

fn demo_state_diagram() {
    println!("=== 4. 狀態轉換圖 ===");

    let mut diagram = StateDiagram::new();

    diagram.add_state("q0", "初始");
    diagram.add_state("q1", "讀到 0");
    diagram.add_state("q2", "接受");

    diagram.add_edge("q0", "q0", "0");
    diagram.add_edge("q0", "q1", "1");
    diagram.add_edge("q1", "q2", "1");

    println!("Mermaid 格式:");
    println!("{}", diagram.to_mermaid());
    println!();
}

fn demo_regex_to_fsm() {
    println!("=== 5. 正規表達式轉 FSM ===");

    // a* 的 FSM
    let a = RegexToFSM::from_literal('a');
    let a_star = RegexToFSM::kleene_star(&a);

    println!("a* 測試:");
    for test in vec!["", "a", "aa", "aaa", "ba"] {
        let result = a_star.process(test);
        println!("  '{}': {}", test, result);
    }

    // ab 的 FSM
    let b = RegexToFSM::from_literal('b');
    let ab = RegexToFSM::concatenate(&a, &b);

    println!("\nab 測試:");
    for test in vec!["ab", "aab", "abb", "b", "a"] {
        let result = ab.process(test);
        println!("  '{}': {}", test, result);
    }
    println!();
}

fn demo_fsm_simulation() {
    println!("=== 6. FSM 模擬路徑 ===");

    let mut fsm = FSM::new();

    fsm.add_state("start", false);
    fsm.add_state("middle", false);
    fsm.add_state("end", true);

    fsm.add_transition("start", "middle", 'a');
    fsm.add_transition("start", "end", 'b');
    fsm.add_transition("middle", "end", 'c');
    fsm.add_transition("middle", "middle", 'a');

    fsm.set_start("start");

    let test = "aac";
    let path = fsm.simulate(test);

    println!("輸入: {}", test);
    println!("路徑: {:?}", path);
    println!();
}

fn demo_minimization() {
    println!("=== 7. 狀態最小化概念 ===");

    println!("狀態最小化步驟:");
    println!("1. 區分接受/非接受狀態");
    println!("2. 對於每個區塊，根據輸出行為細分");
    println!("3. 重複直到沒有新區塊");
    println!("4. 合併等價狀態");
    println!();

    // 展示等價狀態概念
    println!("範例: 兩個狀態 'q0' 和 'q1' 等價如果:");
    println!("  - 對所有輸入字元，轉移到相同區塊");
    println!("  - 都是接受狀態或都不是接受狀態");
    println!();
}

fn main() {
    println!("=== Rust 有限狀態機範例 ===\n");

    demo_binary_divisible_by_3();
    demo_binary_ends_with_01();
    demo_nfa_example();
    demo_state_diagram();
    demo_regex_to_fsm();
    demo_fsm_simulation();
    demo_minimization();

    println!("FSM 範例完成!");
}

// 輔助巨集
macro_rules! hashset {
    ($($item:expr),*) => {{
        let mut set = std::collections::HashSet::new();
        $(set.insert($item);)*
        set
    }};
}
