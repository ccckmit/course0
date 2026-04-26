// Rust 編譯器基礎範例
// 展示詞法分析、語法分析、抽象語法樹 (AST)

use std::collections::HashMap;

// ============ 1. Token 定義 ============
#[derive(Debug, Clone, PartialEq)]
enum Token {
    Number(i64),
    Identifier(String),
    Keyword(String),
    Plus,
    Minus,
    Star,
    Slash,
    Equal,
    LessThan,
    GreaterThan,
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Semicolon,
    EOF,
}

struct Lexer {
    input: Vec<char>,
    position: usize,
}

impl Lexer {
    fn new(source: &str) -> Self {
        Lexer {
            input: source.chars().collect(),
            position: 0,
        }
    }

    fn next_token(&mut self) -> Token {
        self.skip_whitespace();

        if self.position >= self.input.len() {
            return Token::EOF;
        }

        let ch = self.input[self.position];

        match ch {
            '(' => {
                self.position += 1;
                Token::LeftParen
            }
            ')' => {
                self.position += 1;
                Token::RightParen
            }
            '{' => {
                self.position += 1;
                Token::LeftBrace
            }
            '}' => {
                self.position += 1;
                Token::RightBrace
            }
            ';' => {
                self.position += 1;
                Token::Semicolon
            }
            '+' => {
                self.position += 1;
                Token::Plus
            }
            '-' => {
                self.position += 1;
                Token::Minus
            }
            '*' => {
                self.position += 1;
                Token::Star
            }
            '/' => {
                self.position += 1;
                Token::Slash
            }
            '=' => {
                self.position += 1;
                Token::Equal
            }
            '<' => {
                self.position += 1;
                Token::LessThan
            }
            '>' => {
                self.position += 1;
                Token::GreaterThan
            }
            '0'..='9' => self.read_number(),
            'a'..='z' | 'A'..='Z' | '_' => self.read_identifier(),
            _ => {
                self.position += 1;
                self.next_token()
            }
        }
    }

    fn skip_whitespace(&mut self) {
        while self.position < self.input.len() {
            let ch = self.input[self.position];
            if ch.is_whitespace() {
                self.position += 1;
            } else {
                break;
            }
        }
    }

    fn read_number(&mut self) -> Token {
        let mut num = 0i64;
        while self.position < self.input.len() {
            let ch = self.input[self.position];
            if let Some(digit) = ch.to_digit(10) {
                num = num * 10 + digit as i64;
                self.position += 1;
            } else {
                break;
            }
        }
        Token::Number(num)
    }

    fn read_identifier(&mut self) -> Token {
        let mut name = String::new();
        while self.position < self.input.len() {
            let ch = self.input[self.position];
            if ch.is_alphanumeric() || ch == '_' {
                name.push(ch);
                self.position += 1;
            } else {
                break;
            }
        }

        match name.as_str() {
            "let" | "fn" | "if" | "else" | "return" => Token::Keyword(name),
            _ => Token::Identifier(name),
        }
    }

    fn tokenize(&mut self) -> Vec<Token> {
        let mut tokens = vec![];
        loop {
            let token = self.next_token();
            if token == Token::EOF {
                tokens.push(token);
                break;
            }
            tokens.push(token);
        }
        tokens
    }
}

// ============ 2. AST 節點定義 ============
#[derive(Debug)]
enum ASTNode {
    Number(i64),
    Variable(String),
    Binary {
        op: String,
        left: Box<ASTNode>,
        right: Box<ASTNode>,
    },
    Assignment {
        name: String,
        value: Box<ASTNode>,
    },
    FunctionCall {
        name: String,
        args: Vec<ASTNode>,
    },
    IfStatement {
        condition: Box<ASTNode>,
        then_branch: Box<ASTNode>,
        else_branch: Option<Box<ASTNode>>,
    },
}

// ============ 3. 簡單 Parser ============
struct Parser {
    tokens: Vec<Token>,
    position: usize,
}

impl Parser {
    fn new(tokens: Vec<Token>) -> Self {
        Parser {
            tokens,
            position: 0,
        }
    }

    fn current(&self) -> &Token {
        &self.tokens[self.position.min(self.tokens.len() - 1)]
    }

    fn advance(&mut self) {
        if self.position < self.tokens.len() {
            self.position += 1;
        }
    }

    fn parse(&mut self) -> Vec<ASTNode> {
        let mut statements = vec![];

        while self.current() != &Token::EOF {
            statements.push(self.parse_statement());
        }

        statements
    }

    fn parse_statement(&mut self) -> ASTNode {
        match self.current() {
            Token::Keyword(ref k) if k == "let" => self.parse_let_statement(),
            _ => self.parse_expression(),
        }
    }

    fn parse_let_statement(&mut self) -> ASTNode {
        self.advance(); // skip 'let'

        if let Token::Identifier(name) = self.current().clone() {
            self.advance(); // skip name

            self.advance(); // skip '='

            let value = self.parse_expression();

            self.advance(); // skip ';'

            ASTNode::Assignment {
                name,
                value: Box::new(value),
            }
        } else {
            panic!("Expected identifier after 'let'");
        }
    }

    fn parse_expression(&mut self) -> ASTNode {
        let left = self.parse_primary();

        match self.current() {
            Token::Plus | Token::Minus | Token::Star | Token::Slash => {
                let op = match self.current() {
                    Token::Plus => "+",
                    Token::Minus => "-",
                    Token::Star => "*",
                    Token::Slash => "/",
                    _ => "",
                };
                self.advance();

                let right = self.parse_expression();

                ASTNode::Binary {
                    op: op.to_string(),
                    left: Box::new(left),
                    right: Box::new(right),
                }
            }
            _ => left,
        }
    }

    fn parse_primary(&mut self) -> ASTNode {
        match self.current().clone() {
            Token::Number(n) => {
                self.advance();
                ASTNode::Number(n)
            }
            Token::Identifier(name) => {
                self.advance();
                ASTNode::Variable(name)
            }
            Token::LeftParen => {
                self.advance();
                let expr = self.parse_expression();
                self.advance(); // skip ')'
                expr
            }
            _ => panic!("Unexpected token: {:?}", self.current()),
        }
    }
}

// ============ 4. 代碼生成 ============
struct CodeGenerator;

impl CodeGenerator {
    fn generate(node: &ASTNode) -> String {
        match node {
            ASTNode::Number(n) => format!("{}", n),
            ASTNode::Variable(name) => format!("{}", name),
            ASTNode::Binary { op, left, right } => {
                let l = Self::generate(left);
                let r = Self::generate(right);
                format!("({} {} {})", l, op, r)
            }
            ASTNode::Assignment { name, value } => {
                let v = Self::generate(value);
                format!("let {} = {};", name, v)
            }
            ASTNode::FunctionCall { name, args } => {
                let args_str: Vec<String> = args.iter().map(|a| Self::generate(a)).collect();
                format!("{}({})", name, args_str.join(", "))
            }
            _ => String::from(";"),
        }
    }

    fn generate_to_assembly(node: &ASTNode) -> String {
        let mut asm = String::from("; Generated Assembly\n");
        asm.push_str("section .text\n");
        asm.push_str("global _start\n");
        asm.push_str("_start:\n");

        match node {
            ASTNode::Number(n) => {
                asm.push_str(&format!("    mov eax, {}\n", n));
            }
            ASTNode::Binary { op, left, right } => {
                asm.push_str(&format!("    ; {} operation\n", op));
                // 簡化的 x86-64 assembly 生成
                asm.push_str(&format!(
                    "    ; left: {} right: {}\n",
                    Self::generate(left),
                    Self::generate(right)
                ));
            }
            _ => {}
        }

        asm.push_str("    ; syscall exit\n");
        asm.push_str("    mov eax, 60\n");
        asm.push_str("    xor edi, edi\n");
        asm.push_str("    syscall\n");

        asm
    }
}

// ============ 5. 最佳化示例 ============
struct Optimizer;

impl Optimizer {
    fn optimize(node: ASTNode) -> ASTNode {
        match node {
            ASTNode::Binary { op, left, right } => {
                let left_opt = Self::optimize(*left);
                let right_opt = Self::optimize(*right);

                // 常數折疊
                if let ASTNode::Number(l) = &left_opt {
                    if let ASTNode::Number(r) = &right_opt {
                        let result = match op.as_str() {
                            "+" => l + r,
                            "-" => l - r,
                            "*" => l * r,
                            "/" if *r != 0 => l / r,
                            _ => {
                                return ASTNode::Binary {
                                    op,
                                    left: Box::new(left_opt),
                                    right: Box::new(right_opt),
                                }
                            }
                        };
                        return ASTNode::Number(result);
                    }
                }

                ASTNode::Binary {
                    op,
                    left: Box::new(left_opt),
                    right: Box::new(right_opt),
                }
            }
            _ => node,
        }
    }
}

fn demo_lexer() {
    println!("=== 1. 詞法分析器 (Lexer) ===");

    let source = "let x = 42 + 10;";
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();

    println!("原始程式: {}", source);
    println!("Tokens:");
    for (i, token) in tokens.iter().enumerate() {
        println!("  {}: {:?}", i, token);
    }
    println!();
}

fn demo_parser() {
    println!("=== 2. 語法分析器 (Parser) ===");

    let source = "let x = 1 + 2 * 3;";
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();

    let mut parser = Parser::new(tokens);
    let ast = parser.parse();

    println!("原始程式: {}", source);
    println!("AST:");
    for node in &ast {
        println!("  {:?}", node);
    }
    println!();
}

fn demo_codegen() {
    println!("=== 3. 代碼生成 ===");

    let source = "let x = 10 + 20;";
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();

    let mut parser = Parser::new(tokens);
    let ast = parser.parse();

    println!("原始程式: {}", source);

    for node in &ast {
        let rust_code = CodeGenerator::generate(node);
        println!("生成的 Rust 代碼: {}", rust_code);

        let asm = CodeGenerator::generate_to_assembly(node);
        println!("生成的 Assembly:\n{}", asm);
    }
    println!();
}

fn demo_optimizer() {
    println!("=== 4. 代碼最佳化 ===");

    // 測試常數折疊
    let node = ASTNode::Binary {
        op: "+".to_string(),
        left: Box::new(ASTNode::Binary {
            op: "*".to_string(),
            left: Box::new(ASTNode::Number(2)),
            right: Box::new(ASTNode::Number(3)),
        }),
        right: Box::new(ASTNode::Number(4)),
    };

    println!("優化前: (2 * 3) + 4 = {:?}", node);

    let optimized = Optimizer::optimize(node);
    println!("優化後: {:?}", optimized);
    println!();
}

fn demo_expression_parser() {
    println!("=== 5. 表達式解析 ===");

    let test_cases = vec!["5", "x + y", "1 + 2 * 3", "(1 + 2) * 3"];

    for source in test_cases {
        let mut lexer = Lexer::new(source);
        let tokens = lexer.tokenize();

        let mut parser = Parser::new(tokens);
        let ast = parser.parse();

        let code = CodeGenerator::generate(&ast[0]);
        println!("'{}' => {}", source, code);
    }
}

fn main() {
    println!("=== Rust 編譯器基礎範例 ===\n");

    demo_lexer();
    demo_parser();
    demo_codegen();
    demo_optimizer();
    demo_expression_parser();

    println!("編譯器範例完成!");
}
