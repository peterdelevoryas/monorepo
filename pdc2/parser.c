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
#include "parser.h"

u8* mmap_file(string path, u64* size) {
    struct stat st;
    int fd;
    int r;
    u8* addr = NULL;
    u64 size_;

    fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd == -1) {
        goto done;
    }

    r = fstat(fd, &st);
    if (r != 0) {
        goto done;
    }
    size_ = st.st_size;
    *size = size_;

    addr = mmap(NULL, size_, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        goto done;
    }

done:
    if (fd != -1) {
        close(fd);
    }
    return addr;
}

static void parse_ident(Parser* p) {
    for (;;) {
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

static void parse_int(Parser* p) {
    for (;;) {
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

static void parse_string(Parser* p) {
    bool escaped = false;

    assert(p->text[p->token_end] == '"');
    for (;;) {
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

static Token byte_to_token(u8 b) {
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

static void parse_token(Parser* p) {
    for (;;) {
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

Parser parser_init(string path, u8* text, u64 text_size) {
    Parser p;

    p.text          = text;
    p.text_size     = text_size;
    p.line_no       = 1;
    p.token         = TOKEN_EOF;
    p.token_start   = 0;
    p.token_end     = 0;

    parse_token(&p);

    return p;
}

void print_tokens(Parser* p) {
    for (;;) {
        parse_token(p);
        if (p->token == TOKEN_EOF) {
            break;
        }
        printf("%.*s ", (int)(p->token_end - p->token_start),
               &p->text[p->token_start]);
    }
    printf("\n");
}
