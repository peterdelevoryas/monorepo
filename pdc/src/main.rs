use std::env;
use std::fs;

fn main() {
    let args: Vec<String> = env::args().collect();
    let path = match args.get(1) {
        Some(s) => s,
        None => {
            println!("USAGE: {} FILE", args[0]);
            return;
        }
    };
    let text = match fs::read_to_string(&path) {
        Ok(s) => s,
        Err(e) => {
            println!("Unable to open {:?}: {}", path, e);
            return;
        }
    };
    let mut parser = Parser::new(&text);
    while parser.token != Token::Eof {
        println!("{:?} ", parser.token);
        parser.next_token();
    }
}

#[derive(Debug)]
struct Parser<'a> {
    text: &'a str,
    start: usize,
    end: usize,
    token: Token,
}

#[derive(Debug, Copy, Clone, PartialEq)]
enum Token {
    Fn,
    Let,
    Ret,
    I8,
    I32,
    Lparen,
    Lbrace,
    Rparen,
    Rbrace,
    Colon,
    Semicolon,
    Comma,
    Star,
    Eq,
    Eof,
}

impl<'a> Parser<'a> {
    fn new(text: &'a str) -> Self {
        let mut parser = Self {
            text: text,
            start: 0,
            end: 0,
            token: Token::Eof,
        };
        parser.next_token();
        parser
    }

    fn next_token(&mut self) {
        self.start = self.end;
        loop {
            break;
        }
    }
}
