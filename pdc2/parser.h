#pragma once
#include "common.h"

typedef enum Token Token;
typedef struct Parser Parser;

enum Token {
    TOKEN_FN,
    TOKEN_LET,
    TOKEN_RETURN,
    TOKEN_I8,
    TOKEN_I16,
    TOKEN_I32,
    TOKEN_I64,
    TOKEN_NUM_KEYWORDS,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_EQ,
    TOKEN_STAR,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_DOT,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_ELLIPSIS,
    TOKEN_ARROW,
    TOKEN_IDENT,
    TOKEN_INT,
    TOKEN_STRING,
    TOKEN_SPACE,
    TOKEN_ERROR,
    TOKEN_EOF,
};

struct Parser {
    u8* text;
    u64 text_size;
    u64 line_no;
    u64 token_start;
    u64 token_end;
    Token token;
};

Parser parser_init(String path);
void parser_free(Parser* p);
void parser_debug_tokens(Parser* p);
