#pragma once
#include <stdint.h>
#include "types.h"
#include "vector.h"

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
    string  text;
    u64     text_size;
    u64     line_no;
    u64     token_start;
    u64     token_end;
    Token   token;
};

void*       mmap_file(string path, u64* size);
Parser      parser_init(string path, string text, u64 text_size);

void        print_tokens(Parser* p);

typedef struct Function Function;
typedef struct FunctionType FunctionType;
typedef struct FunctionParam FunctionParam;
typedef struct Type Type;
typedef enum TypeKind TypeKind;

struct FunctionType {
    Vector(FunctionParam)   params;
    Type*                   return_type;
};

enum TypeKind {
    TYPE_NONE,
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,
    TYPE_POINTER,
    TYPE_FUNCTION,
};

struct Type {
    TypeKind        kind;
    Type*           pointer_type;
    FunctionType    function_type;
};

struct FunctionParam {
    string  name;
    Type    type;
};

struct Function {
    string          name;
    FunctionType    type;
};

Function parse_function(Parser* p);
