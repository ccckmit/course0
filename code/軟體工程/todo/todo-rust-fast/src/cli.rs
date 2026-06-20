use clap::{Parser, Subcommand};
use std::sync::Arc;
use crate::db::Store;

#[derive(Parser)]
#[command(name = "todo-fast", about = "Ultra-fast Todo CLI")]
struct Cli {
    #[command(subcommand)]
    command: Command,
}

#[derive(Subcommand)]
enum Command {
    Add { text: String },
    List,
    Done { id: u64 },
    Delete { id: u64 },
}

pub fn run(args: &[String], store: Arc<Store>) {
    let cli = Cli::parse_from(args);
    match cli.command {
        Command::Add { text } => match store.add(&text) {
            Ok(id) => println!("Added: #{} {}", id, text),
            Err(e) => eprintln!("Error: {}", e),
        },
        Command::List => match store.list(false) {
            Ok(todos) => {
                if todos.is_empty() {
                    println!("No todos");
                } else {
                    for t in &todos {
                        println!("{}. {}", t.id, t.text);
                    }
                }
            }
            Err(e) => eprintln!("Error: {}", e),
        },
        Command::Done { id } => match store.set_done(id) {
            Ok(true) => println!("Done: #{}", id),
            Ok(false) => println!("Not found: #{}", id),
            Err(e) => eprintln!("Error: {}", e),
        },
        Command::Delete { id } => match store.delete(id) {
            Ok(true) => println!("Deleted: #{}", id),
            Ok(false) => println!("Not found: #{}", id),
            Err(e) => eprintln!("Error: {}", e),
        },
    }
}
