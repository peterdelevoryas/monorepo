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

#define Reserve(v, n) \
    do { \
        if (v.len + n > v.cap) { \
            v.cap = v.cap ? v.cap + v.cap / 2 : 16; \
            v.buf = realloc(v.buf, v.cap * sizeof(*v.buf)); \
        } \
        assert(v.len + n <= v.cap); \
    } while (0)

#define Append(v, x) \
    do { \
        Reserve(v, 1); \
        v.buf[v.len++] = x; \
    } while (0)

struct MemoryMappedFile {
    void *addr;
    size_t size;
};

static struct MemoryMappedFile Mmap(const char *path)
{
    int fd;
    struct stat st;
    struct MemoryMappedFile f;

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

const static uint8_t char_to_token[256] = {
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

static const char *const token_to_string[TOKEN_MAX] = {
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
    enum Token token;
    int start, end, line_no;
};

static void ParseIdent(struct Parser *p)
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
        s2 = token_to_string[i];
        n2 = strlen(s2);
        if (n1 == n2 && memcmp(s1, s2, n1) == 0) {
            p->token = i;
            break;
        }
    }
}

static void ParseInt(struct Parser *p)
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

static void PrintEscapedChar(char c)
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

static void Bump(struct Parser *p)
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
                ParseIdent(p);
                break;
            case '0'...'9':
                ParseInt(p);
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
                p->token = char_to_token[(int)c];
                p->end++;
                break;
            default:
                printf("Unexpected character: '");
                PrintEscapedChar(c);
                printf("'\n");
                exit(1);
        }
        break;
    }
}

struct TokenArray {
    struct {
        enum Token token;
        int start, end;
    } *buf;
    int cap, len;
};

static struct TokenArray ParseFile(const char *path)
{
    struct MemoryMappedFile f;
    struct Parser p;
    struct TokenArray t;

    f = Mmap(path);
    p.path = path;
    p.file = f.addr;
    p.size = f.size;
    p.token = TOKEN_EOF;
    p.start = p.end = 0;
    p.line_no = 1;
    t.buf = NULL;
    t.cap = t.len = 0;
    for (Bump(&p); p.token != TOKEN_EOF; Bump(&p)) {
        Reserve(t, 1);
        t.buf[t.len].token = p.token;
        t.buf[t.len].start = p.start;
        t.buf[t.len].end = p.end;
        t.len++;
    }

    munmap(f.addr, f.size);
    return t;
}

static struct timespec TimespecSubtract(struct timespec a, struct timespec b)
{
    struct timespec dt;

    dt.tv_sec = a.tv_sec - b.tv_sec;
    dt.tv_nsec = a.tv_nsec - b.tv_nsec;

    return dt;
}

static double TimespecToDouble(struct timespec t)
{
    return (double)t.tv_sec + (double)t.tv_nsec * 1.0e-9;
}

static void Compile(const char *path)
{
    struct timespec t0, t1;
    double dt;
    int i;
    struct TokenArray tokens;
    struct MemoryMappedFile f;
    enum Token t;
    int start, end;
    const char *s;
    int n;

    printf("Compiling '%s'...", path);
    clock_gettime(CLOCK_MONOTONIC, &t0);

    tokens = ParseFile(path);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    dt = TimespecToDouble(TimespecSubtract(t1, t0));
    printf("done: %f seconds\n", dt);

    f = Mmap(path);
    for (i = 0; i < tokens.len; i++) {
        t = tokens.buf[i].token;
        start = tokens.buf[i].start;
        end = tokens.buf[i].end;
        s = &f.addr[start];
        n = end - start;
        switch (t) {
            case TOKEN_IDENT:
            case TOKEN_INT:
                printf("'%.*s' ", n, s);
                break;
            default:
                printf("%s ", token_to_string[t]);
                break;
        }
    }
    printf("\n");
    munmap(f.addr, f.size);
}

int main(int argc, char **argv)
{
    int i;

    if (argc < 2) {
        printf("Usage: %s [-h] file...\n", argv[0]);
        return 0;
    }
    for (i = 1; i < argc; i++) {
        Compile(argv[i]);
    }
}
