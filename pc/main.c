#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define APPEND(v, x) \
    do { \
        if (v.len >= v.cap) { \
            v.cap = v.cap ? v.cap + v.cap / 2 : 16; \
            v.buf = realloc(v.buf, v.cap * sizeof(*v.buf)); \
        } \
        v.buf[v.len++] = x; \
    } while (0)

typedef enum Token Token;
typedef struct Parser Parser;
typedef struct MemoryMappedFile MemoryMappedFile;
typedef struct TokenArray TokenArray;

struct MemoryMappedFile {
    void *addr;
    size_t size;
};

static MemoryMappedFile mmap_file_read_only(const char *path)
{
    int fd;
    struct stat st;
    MemoryMappedFile f;

    fd = open(path, O_RDONLY | O_CLOEXEC);
    assert(fd != -1);
    assert(fstat(fd, &st) == 0);
    f.size = st.st_size;
    f.addr = mmap(NULL, f.size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(f.addr != MAP_FAILED);
    close(fd);
    return f;
}

enum Token {
    TOKEN_FUNCTION,
    TOKEN_LET,
    TOKEN_RETURN,
    TOKEN_I8,
    TOKEN_I32,
    TOKEN_NR_KEYWORDS,
    TOKEN_IDENT,
    TOKEN_INT,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_STAR,
    TOKEN_ARROW,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_EQ,
    TOKEN_SPACE,
    TOKEN_NEWLINE,
    TOKEN_ERROR,
    TOKEN_EOF,
    TOKEN_MAX,
};

const static uint8_t CHAR_TOKEN[256] = {
    ['('] = TOKEN_LPAREN,
    [')'] = TOKEN_RPAREN,
    ['{'] = TOKEN_LBRACE,
    ['}'] = TOKEN_RBRACE,
    [':'] = TOKEN_COLON,
    [';'] = TOKEN_SEMICOLON,
    [','] = TOKEN_COMMA,
    ['*'] = TOKEN_STAR,
    ['+'] = TOKEN_PLUS,
    ['='] = TOKEN_EQ,
};

static const char *const TOKEN_STRING[TOKEN_MAX] = {
    [TOKEN_FUNCTION] = "function",
    [TOKEN_LET] = "let",
    [TOKEN_RETURN] = "return",
    [TOKEN_I8] = "i8",
    [TOKEN_I32] = "i32",
    [TOKEN_NR_KEYWORDS] = NULL,
    [TOKEN_IDENT] = "<ident>",
    [TOKEN_INT] = "<int>",
    [TOKEN_LPAREN] = "(",
    [TOKEN_RPAREN] = ")",
    [TOKEN_LBRACE] = "{",
    [TOKEN_RBRACE] = "}",
    [TOKEN_COLON] = ":",
    [TOKEN_SEMICOLON] = ";",
    [TOKEN_COMMA] = ",",
    [TOKEN_STAR] = "*",
    [TOKEN_ARROW] = "->",
    [TOKEN_MINUS] = "-",
    [TOKEN_PLUS] = "+",
    [TOKEN_EQ] = "=",
    [TOKEN_SPACE] = NULL,
    [TOKEN_NEWLINE] = NULL,
    [TOKEN_ERROR] = NULL,
    [TOKEN_EOF] = NULL,
};

struct Parser {
    const char *path, *file;
    size_t size;
    Token token;
    int start, end, line_no;
};

static void parse_ident(Parser *p)
{
    int i, n1, n2;
    const char *s1, *s2;

    p->token = TOKEN_IDENT;
    for (; p->end < p->size; p->end++) {
        switch (p->file[p->end]) {
        case 'a'...'z':
        case 'A'...'Z':
        case '_':
        case '0'...'9':
            continue;
        default:
            break;
        }
        break;
    }

    n1 = p->end - p->start;
    s1 = p->file + p->start;
    for (i = 0; i < TOKEN_NR_KEYWORDS; i++) {
        s2 = TOKEN_STRING[i];
        n2 = strlen(s2);
        if (n1 == n2 && memcmp(s1, s2, n1) == 0) {
            p->token = i;
            break;
        }
    }
}

static void parse_int(Parser *p)
{
    p->token = TOKEN_INT;
    for (; p->end < p->size; p->end++) {
        switch (p->file[p->end]) {
        case '0'...'9':
            continue;
        default:
            break;
        }
        break;
    }
}

static void print_escaped_char(char c)
{
    const char *s;

    switch (c) {
    case '\n':
        s = "\\n";
        break;
    case '\t':
        s = "\\t";
        break;
    case '\r':
        s = "\\r";
        break;
    default:
        putchar(c);
        return;
    }
    puts(s);
}

static void bump(Parser *p)
{
    char c;

    for (;;) {
        p->start = p->end;
        if (p->end >= p->size) {
            p->token = TOKEN_EOF;
            break;
        }

        c = p->file[p->end];
        switch (c) {
        case '\n':
            p->line_no++;
            p->end++;
            continue;
        case ' ':
        case '\t':
        case '\r':
            p->end++;
            continue;
        case 'a'...'z':
        case 'A'...'Z':
        case '_':
            parse_ident(p);
            break;
        case '0'...'9':
            parse_int(p);
            break;
        case '-':
            p->end++;
            p->token = TOKEN_MINUS;
            if (p->end < p->size && p->file[p->end] == '>') {
                p->end++;
                p->token = TOKEN_ARROW;
            }
            break;
        case '(':
        case ')':
        case '{':
        case '}':
        case ':':
        case ';':
        case ',':
        case '*':
        case '+':
        case '=':
            p->token = CHAR_TOKEN[(int)c];
            p->end++;
            break;
        default:
            printf("Unexpected character: '");
            print_escaped_char(c);
            printf("'\n");
            exit(1);
        }
        break;
    }
}

struct TokenArray {
    Token *buf;
    int cap, len;
};

static TokenArray parse_file(const char *path)
{
    MemoryMappedFile f;
    Parser p;
    TokenArray t;

    f = mmap_file_read_only(path);
    p.path = path;
    p.file = f.addr;
    p.size = f.size;
    p.token = TOKEN_EOF;
    p.start = p.end = 0;
    p.line_no = 1;
    t.buf = NULL;
    t.cap = t.len = 0;
    for (bump(&p); p.token != TOKEN_EOF; bump(&p)) {
        APPEND(t, p.token);
    }

    munmap(f.addr, f.size);
    return t;
}

static struct timespec timespec_subtract(struct timespec a, struct timespec b)
{
    struct timespec dt;

    dt.tv_sec = a.tv_sec - b.tv_sec;
    dt.tv_nsec = a.tv_nsec - b.tv_nsec;

    return dt;
}

static double timespec_to_double(struct timespec t)
{
    return (double)t.tv_sec + (double)t.tv_nsec * 1.0e-9;
}

static void compile(const char *path)
{
    struct timespec t0, t1;
    double dt;
    TokenArray tokens;
    int i;

    printf("Compiling '%s'...", path);
    clock_gettime(CLOCK_MONOTONIC, &t0);

    tokens = parse_file(path);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    dt = timespec_to_double(timespec_subtract(t1, t0));
    printf("done: %f seconds\n", dt);

    printf("tokens: ");
    for (i = 0; i < tokens.len; i++) {
        printf("%s ", TOKEN_STRING[tokens.buf[i]]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    int i;

    if (argc < 2) {
        printf("Usage: %s [-h] file...\n", argv[0]);
        return 0;
    }
    for (i = 1; i < argc; i++) {
        compile(argv[i]);
    }
}
