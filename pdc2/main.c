#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <assert.h>

#define loop for (;;)
#define int(x) ((int)(x))

typedef int32_t     i32;
typedef int64_t     i64;
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef const char* String;

static u8* file_mmap(String path, u64* size)
{
    struct stat st;
    int fd, err;
    u8* addr;

    fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd == -1) {
        return NULL;
    }
    err = stat(path, &st);
    if (err != 0) {
        goto error;
    }
    addr = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        goto error;
    }
    close(fd);
    *size = st.st_size;
    return addr;
error:
    close(fd);
    return NULL;
}

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

static void parse_ident(Parser* p)
{
    loop {
        p->token_end += 1;
        if (p->token_end >= p->text_size) {
            break;
        }
        switch (p->text[p->token_end]) {
            case '_':
            case 'a'...'z':
            case 'A'...'Z':
            case '0'...'9':
                continue;
        }
        break;
    }
}

static void parse_int(Parser* p)
{
    loop {
        p->token_end += 1;
        if (p->token_end >= p->text_size) {
            break;
        }
        switch (p->text[p->token_end]) {
            case '0'...'9':
                continue;
        }
        break;
    }
}

static void parse_string(Parser* p)
{
    bool escaped = false;

    assert(p->text[p->token_end] == '"');
    loop {
        p->token_end += 1;
        if (p->token_end >= p->text_size) {
            printf("unmatched '\"'\n");
            p->token = TOKEN_ERROR;
            break;
        }
        u8 b = p->text[p->token_end];
        if (b == '\\' && !escaped) {
            escaped = true;
            continue;
        }
        if (b == '"' && !escaped) {
            break;
        }
        escaped = false;
    }
    assert(p->text[p->token_end] == '"');
    p->token_end += 1;
}

static Token byte_to_token(u8 b)
{
    switch (b) {
        case '(':       return TOKEN_LPAREN;
        case ')':       return TOKEN_RPAREN;
        case '{':       return TOKEN_LBRACE;
        case '}':       return TOKEN_RBRACE;
        case '=':       return TOKEN_EQ;
        case '*':       return TOKEN_STAR;
        case '+':       return TOKEN_PLUS;
        case ',':       return TOKEN_COMMA;
        case ':':       return TOKEN_COLON;
        case ';':       return TOKEN_SEMICOLON;
        case '.':       return TOKEN_ELLIPSIS;
        case '-':       return TOKEN_ARROW;
        case 'a'...'z':
        case 'A'...'Z':
        case '_':       return TOKEN_IDENT;
        case '0'...'9': return TOKEN_INT;
        case '"':       return TOKEN_STRING;
        case '\n':
        case '\t':
        case '\r':
        case ' ':       return TOKEN_SPACE;
        default:        return TOKEN_ERROR;
    }
}

static void parse_token(Parser* p)
{
    loop {
        p->token_start = p->token_end;
        if (p->token_start >= p->text_size) {
            p->token = TOKEN_EOF;
            return;
        }
        u8 b = p->text[p->token_end];
        p->token = byte_to_token(b);
        switch (p->token) {
            case TOKEN_FN:
            case TOKEN_LET:
            case TOKEN_RETURN:
            case TOKEN_I8:
            case TOKEN_I16:
            case TOKEN_I32:
            case TOKEN_I64:
            case TOKEN_NUM_KEYWORDS:
            case TOKEN_DOT:
            case TOKEN_MINUS:
                __builtin_unreachable();
            case TOKEN_LPAREN:
            case TOKEN_RPAREN:
            case TOKEN_LBRACE:
            case TOKEN_RBRACE:
            case TOKEN_EQ:
            case TOKEN_STAR:
            case TOKEN_PLUS:
            case TOKEN_COMMA:
            case TOKEN_COLON:
            case TOKEN_SEMICOLON:
                p->token_end += 1;
                break;
            case TOKEN_ELLIPSIS:
                if (p->token_end + 2 <= p->text_size &&
                    p->text[p->token_end + 1] == '.' &&
                    p->text[p->token_end + 2] == '.')
                {
                    p->token_end += 3;
                    break;
                }
                p->token = TOKEN_DOT;
                p->token_end += 1;
                break;
            case TOKEN_ARROW:
                if (p->token_end + 1 <= p->text_size &&
                    p->text[p->token_end + 1] == '>')
                {
                    p->token_end += 2;
                    break;
                }
                p->token = TOKEN_MINUS;
                p->token_end += 1;
                break;
            case TOKEN_IDENT:
                parse_ident(p);
                break;
            case TOKEN_INT:
                parse_int(p);
                break;
            case TOKEN_STRING:
                parse_string(p);
                break;
            case TOKEN_SPACE:
                p->token_end += 1;
                continue;
            case TOKEN_ERROR:
                p->token = TOKEN_EOF;
                printf("unexpected character: '%c', 0x%02x\n", b, b);
                break;
            case TOKEN_EOF:
                break;
        }
        break;
    }
}

static Parser parser_init(String path)
{
    Parser p = {};
    p.text = file_mmap(path, &p.text_size);
    if (!p.text) {
        printf("unable to mmap '%s': %s\n", path, strerror(errno));
        return p;
    }
    p.line_no     = 1;
    p.token       = TOKEN_EOF;
    p.token_start = 0;
    p.token_end   = 0;
    parse_token(&p);
    return p;
}

static void parser_free(Parser* p)
{
    munmap(p->text, p->text_size);
}

static void parser_debug_tokens(Parser* p)
{
    loop {
        parse_token(p);
        if (p->token == TOKEN_EOF) {
            break;
        }
        printf("%.*s ", int(p->token_end - p->token_start),
               &p->text[p->token_start]);
    }
    printf("\n");
}

static void compile(String path)
{
    Parser p = parser_init(path);
    parser_debug_tokens(&p);
    parser_free(&p);
}

static void usage()
{
    printf("usage: pdc [-h] file...\n");
}

static bool string_equals(String a, String b)
{
    return strcmp(a, b) == 0;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        usage();
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        if (string_equals(argv[i], "-h")) {
            usage();
            return 0;
        }
        compile(argv[i]);
    }
}
