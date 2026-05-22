trait Summary {
    fn summarize(&self) -> String;
}

struct NewsArticle {
    headline: String,
    content: String,
}

impl Summary for NewsArticle {
    fn summarize(&self) -> String {
        // 修正：取前 10 個字元 (chars) 而非位元組 (bytes)
        let preview: String = self.content
            .chars()
            .take(10)
            .collect();
            
        format!("標題: {} (內容摘要: {}...)", self.headline, preview)
    }
}

fn main() {
    let article = NewsArticle {
        headline: String::from("Rust 1.0 發布"),
        content: String::from("Rust 是一門專注於安全與效能的程式語言..."),
    };

    println!("新文章摘要: {}", article.summarize());
}