use clap::{Parser, Subcommand};
use crate::database;

#[derive(Parser)]
#[command(name = "todo", about = "Todo CLI")]
struct Cli {
    #[command(subcommand)]
    command: Command,
}

#[derive(Subcommand)]
enum Command {
    Add { text: String },
    List,
    Done { id: i64 },
    Delete { id: i64 },
}

pub fn run(args: &[String]) {
    let cli = Cli::parse_from(args);
    let _ = database::init_db();
    match cli.command {
        Command::Add { text } => match database::add(&text) {
            Ok(id) => println!("Added: #{} {}", id, text),
            Err(e) => eprintln!("Error: {}", e),
        },
        Command::List => match database::list(false) {
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
        Command::Done { id } => match database::set_done(id) {
            Ok(true) => println!("Done: #{}", id),
            Ok(false) => println!("Not found: #{}", id),
            Err(e) => eprintln!("Error: {}", e),
        },
        Command::Delete { id } => match database::delete(id) {
            Ok(true) => println!("Deleted: #{}", id),
            Ok(false) => println!("Not found: #{}", id),
            Err(e) => eprintln!("Error: {}", e),
        },
    }
}
