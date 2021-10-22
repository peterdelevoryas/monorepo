use std::fs;

#[derive(Debug, Copy, Clone, PartialEq)]
enum Token {
    Fn,
    Let,
    Ret,
    I8,
    I16,
    I32,
    I64,
    Lparen,
    Rparen,
    Lbrace,
    Rbrace,
    Colon,
    Comma,
    Semicolon,
    Minus,
    Plus,
    Eq,
    Arrow,
    Star,
    Dot,
    Ellipsis,
    Ident,
    Integer,
    Float,
    String,
    Eof,
}

struct Parser<'a> {
    text: &'a [u8],
    start: usize,
    end: usize,
    token: Token,
}

impl<'a> Parser<'a> {
    fn new(text: &'a [u8]) -> Self {
        Parser {
            text: text,
            start: 0,
            end: 0,
            token: Token::Eof,
        }
    }

    fn parse_token(&mut self) {
        loop {
            self.start = self.end;
            let remaining = &self.text[self.end..];
            if remaining.is_empty() {
                self.token = Token::Eof;
                break;
            }
            let c = remaining[0] as char;
            let (t, n) = match c {
                'a'..='z' | 'A'..='Z' | '_' => self.parse_ident(),
                '0'..='9' => self.parse_integer(),
                ' ' | '\n' | '\r' | '\t' => {
                    self.end += 1;
                    continue;
                }
                '-' => {
                    let c = remaining[1] as char;
                    match c {
                        '>' => (Token::Arrow, 2),
                        _ => (Token::Minus, 1),
                    }
                }
                '.' => {
                    if remaining[1] == b'.' && remaining[2] == b'.' {
                        (Token::Ellipsis, 3)
                    } else {
                        (Token::Dot, 1)
                    }
                }
                '(' => (Token::Lparen, 1),
                ')' => (Token::Rparen, 1),
                '{' => (Token::Lbrace, 1),
                '}' => (Token::Rbrace, 1),
                ':' => (Token::Colon, 1),
                ',' => (Token::Comma, 1),
                ';' => (Token::Semicolon, 1),
                '+' => (Token::Plus, 1),
                _ => panic!("unexpected character {:?}", c),
            };
            self.token = t;
            self.end += n;
            break;
        }
    }

    fn parse_ident(&mut self) -> (Token, usize) {
        unimplemented!()
    }

    fn parse_integer(&mut self) -> (Token, usize) {
        unimplemented!()
    }
}

fn main() {
    let s = fs::read_to_string("test.pd").unwrap();
    let s = s.as_bytes();
    let p = &mut Parser::new(s);
    loop {
        p.parse_token();
        if p.token == Token::Eof {
            break;
        }
        println!("{:?}", p.token);
    }
}
