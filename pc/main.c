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

#define ArrayLen(x) (sizeof(x) / sizeof((x)[0]))
#define Zero(x) memset(&x, 0, sizeof(x))
#define ForEach(p, v) for (p = v.buf; p < v.buf + v.len; p++)
#define ForEachReverse(p, v) for (p = v.buf + v.len - 1; p >= v.buf; p--)

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

#define FixedAppend(v, x) \
    do { \
        assert(v.len < ArrayLen(v.buf)); \
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

enum SymbolKind {
    SYMBOL_UNDEFINED,
    SYMBOL_FUNCTION,
    SYMBOL_TYPE,
    SYMBOL_VARIABLE,
    SYMBOL_PARAM,
};

struct Symbol {
    enum SymbolKind kind;
    const char *value;
    void *definition;
};

enum TypeKind {
    TYPE_I8,
    TYPE_I32,
    TYPE_POINTER,
    TYPE_FUNCTION,
    TYPE_SYMBOL,
};

struct Type {
    enum TypeKind kind;
    struct FunctionType *function;
    struct Type *pointer_value_type;
    struct Symbol symbol;
};

struct Param {
    const char *name;
    struct Type type;
};

struct FunctionType {
    struct {
        struct Param buf[32];
        int len;
    } params;
    struct Type return_type;
};

struct StmtArray {
    struct Stmt *buf;
    int len, cap;
};

struct Block {
    struct StmtArray stmts;
};

enum ExprKind {
    EXPR_INT_LITERAL,
    EXPR_USE_SYMBOL,
};

struct Expr {
    enum ExprKind kind;
    const char *int_literal;
    struct Symbol use_symbol;
    struct Type *type;
};

enum StmtKind {
    STMT_LET,
    STMT_RETURN,
};

struct Let {
    const char *name;
    struct Expr rhs;
};

struct Return {
    struct Expr value;
};

struct Stmt {
    enum StmtKind kind;
    struct Let let;
    struct Return ret;
};

struct Function {
    const char *name;
    struct FunctionType type;
    struct Block body;
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
        s2 = TOKEN_STRING[i];
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
                p->token = CHAR_TOKEN[(int)c];
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

static void Expect(struct Parser *p, enum Token t)
{
    if (p->token == t) {
        Bump(p);
        return;
    }
    printf("Expected %s, got %s: '%.*s'\n",
           TOKEN_STRING[t], TOKEN_STRING[p->token],
           p->end - p->start, &p->file[p->start]);
    abort();
}

static const char *ExpectString(struct Parser *p, enum Token t)
{
    const char *s;
    int n;

    s = &p->file[p->start];
    n = p->end - p->start;
    Expect(p, t);

    return strndup(s, n);
}

static struct Symbol ParseSymbol(struct Parser *p)
{
    struct Symbol symbol;

    Zero(symbol);
    symbol.kind = SYMBOL_UNDEFINED;
    symbol.value = ExpectString(p, TOKEN_IDENT);
    symbol.definition = NULL;

    return symbol;
}

static struct Type ParseType(struct Parser *p)
{
    struct Type t;

    Zero(t);
    switch (p->token) {
        case TOKEN_IDENT:
            t.kind = TYPE_SYMBOL;
            t.symbol = ParseSymbol(p);
            break;
        case TOKEN_STAR:
            Bump(p);
            t.kind = TYPE_POINTER;
            t.pointer_value_type = malloc(sizeof(*t.pointer_value_type));
            *t.pointer_value_type = ParseType(p);
            break;
        default:
            printf("Expected type, got %s\n", TOKEN_STRING[p->token]);
            abort();
    }

    return t;
}

static struct Param ParseParam(struct Parser *p)
{
    struct Param param;

    param.name = ExpectString(p, TOKEN_IDENT);
    Expect(p, TOKEN_COLON);
    param.type = ParseType(p);
    return param;
}

static struct FunctionType ParseFunctionType(struct Parser *p)
{
    struct FunctionType f;

    Zero(f);
    Expect(p, TOKEN_LPAREN);
    while (p->token != TOKEN_RPAREN) {
        FixedAppend(f.params, ParseParam(p));
        if (p->token != TOKEN_COMMA) {
            break;
        }
        Bump(p);
    }
    Expect(p, TOKEN_RPAREN);
    Expect(p, TOKEN_ARROW);
    f.return_type = ParseType(p);

    return f;
}

static struct Expr ParseExpr(struct Parser *p)
{
    struct Expr e;

    Zero(e);
    switch (p->token) {
        case TOKEN_INT:
            e.kind = EXPR_INT_LITERAL;
            e.int_literal = ExpectString(p, TOKEN_INT);
            break;
        case TOKEN_IDENT:
            e.kind = EXPR_USE_SYMBOL;
            e.use_symbol = ParseSymbol(p);
            break;
        default:
            printf("Expected expression, got '%s'\n", TOKEN_STRING[p->token]);
            abort();
    }
    return e;
}

static struct Stmt ParseStmt(struct Parser *p)
{
    struct Stmt stmt;

    Zero(stmt);
    switch (p->token) {
        case TOKEN_LET:
            Bump(p);
            stmt.kind = STMT_LET;
            stmt.let.name = ExpectString(p, TOKEN_IDENT);
            Expect(p, TOKEN_EQ);
            stmt.let.rhs = ParseExpr(p);
            Expect(p, TOKEN_SEMICOLON);
            break;
        case TOKEN_RETURN:
            Bump(p);
            stmt.kind = STMT_RETURN;
            stmt.ret.value = ParseExpr(p);
            Expect(p, TOKEN_SEMICOLON);
            break;
        default:
            printf("Expected statement, got %s\n", TOKEN_STRING[p->token]);
            abort();
    }

    return stmt;
}

static struct Block ParseBlock(struct Parser *p)
{
    struct Block b;

    Zero(b);
    Expect(p, TOKEN_LBRACE);
    while (p->token != TOKEN_RBRACE) {
        Append(b.stmts, ParseStmt(p));
    }
    Expect(p, TOKEN_RBRACE);

    return b;
}

static struct Function ParseFunction(struct Parser *p)
{
    struct Function f;

    Zero(f);
    Expect(p, TOKEN_FUNCTION);
    f.name = ExpectString(p, TOKEN_IDENT);
    f.type = ParseFunctionType(p);
    f.body = ParseBlock(p);

    return f;
}

static struct Parser CreateParser(const char *path)
{
    struct MemoryMappedFile f;
    struct Parser p;

    f = Mmap(path);
    p.path = path;
    p.file = f.addr;
    p.size = f.size;
    p.token = TOKEN_EOF;
    p.start = p.end = 0;
    p.line_no = 1;
    Bump(&p);

    return p;
}

static void DestroyParser(struct Parser *p)
{
    munmap((void *)p->file, p->size);
}

struct FunctionArray {
    struct Function *buf;
    int len, cap;
};

struct File {
    struct FunctionArray functions;
};

static struct File ParseFile(const char *path)
{
    struct Parser p;
    struct File f;

    Zero(f);
    p = CreateParser(path);

    while (p.token != TOKEN_EOF) {
        Append(f.functions, ParseFunction(&p));
    }

    DestroyParser(&p);
    return f;
}

struct TypeArray {
    struct Type *buf;
    int len, cap;
};

struct SymbolTable {
    struct Symbol *buf;
    int len, cap;
};

struct Globals {
    struct TypeArray types;
    struct SymbolTable symbol_table;
    struct Function *current_function;
};

static const char *const PRIMITIVE_TYPE_NAMES[] = {
    [TYPE_I8] = "i8",
    [TYPE_I32] = "i32",
};

static struct Globals CreateGlobals() {
    struct Globals g;
    struct Type t;
    struct Symbol s;
    int i, n;

    Zero(g);

    n = ArrayLen(PRIMITIVE_TYPE_NAMES);
    Reserve(g.types, n);
    Reserve(g.symbol_table, n);
    g.types.len = n;
    g.symbol_table.len = n;
    for (i = 0; i < n; i++) {
        Zero(t);
        t.kind = i;
        g.types.buf[i] = t;

        s.kind = SYMBOL_TYPE;
        s.value = PRIMITIVE_TYPE_NAMES[i];
        s.definition = &g.types.buf[i];
        g.symbol_table.buf[i] = s;
    }

    return g;
}

static struct Symbol CreateSymbol(enum SymbolKind kind, const char *value, void *definition)
{
    struct Symbol s;

    s.kind = kind;
    s.value = value;
    s.definition = definition;
    return s;
}

static void ResolveSymbol(struct Globals *g, struct Symbol *symbol)
{
    struct Symbol *def;
    int n, m;

    assert(symbol->kind == SYMBOL_UNDEFINED);
    ForEachReverse(def, g->symbol_table) {
        n = strlen(def->value);
        m = strlen(symbol->value);
        if (n == m && memcmp(def->value, symbol->value, n) == 0) {
            *symbol = *def;
            return;
        }
    }
    printf("Undefined symbol: '%s'\n", symbol->value);
    abort();
}

static int CreateScope(struct Globals *g)
{
    return g->symbol_table.len;
}

static void DestroyScope(struct Globals *g, int scope)
{
    assert(0 <= scope && scope < g->symbol_table.len);
    g->symbol_table.len = scope;
}

static void ResolveSymbolsInType(struct Globals *g, struct Type *t)
{
    switch (t->kind) {
        case TYPE_I8:
        case TYPE_I32:
            // Nothing to do here.
            break;
        case TYPE_POINTER:
            ResolveSymbolsInType(g, t->pointer_value_type);
            break;
        case TYPE_FUNCTION:
            printf("unimplemented\n");
            abort();
        case TYPE_SYMBOL:
            ResolveSymbol(g, &t->symbol);
            break;
    }
}

static void ResolveSymbolsInFunctionType(struct Globals *g, struct FunctionType *f)
{
    struct Param *p;

    ForEach(p, f->params) {
        ResolveSymbolsInType(g, &p->type);
        Append(g->symbol_table, CreateSymbol(SYMBOL_PARAM, p->name, p));
    }
}

static void ResolveSymbolsInExpr(struct Globals *g, struct Expr *e)
{
    switch (e->kind) {
        case EXPR_INT_LITERAL:
            // Nothing to do for literals.
            break;
        case EXPR_USE_SYMBOL:
            ResolveSymbol(g, &e->use_symbol);
            break;
    }
}

static void ResolveSymbolsInStmt(struct Globals *g, struct Stmt *stmt)
{
    switch (stmt->kind) {
        case STMT_LET:
            ResolveSymbolsInExpr(g, &stmt->let.rhs);
            Append(g->symbol_table, CreateSymbol(SYMBOL_VARIABLE, stmt->let.name, &stmt->let));
            break;
        case STMT_RETURN:
            ResolveSymbolsInExpr(g, &stmt->ret.value);
            break;
    }
}

static void ResolveSymbolsInBlock(struct Globals *g, struct Block *b)
{
    struct Stmt *stmt;
    int scope;

    scope = CreateScope(g);
    ForEach(stmt, b->stmts) {
        ResolveSymbolsInStmt(g, stmt);
    }
    DestroyScope(g, scope);
}

static void ResolveSymbolsInFunction(struct Globals *g, struct Function *f)
{
    int scope;

    scope = CreateScope(g);
    ResolveSymbolsInFunctionType(g, &f->type);
    ResolveSymbolsInBlock(g, &f->body);
    DestroyScope(g, scope);
}

static void ResolveSymbols(struct Globals *g, struct File *f)
{
    struct Function *fn;

    // Add all function symbols before entering each function.
    ForEach(fn, f->functions) {
        Append(g->symbol_table, CreateSymbol(SYMBOL_FUNCTION, fn->name, fn));
    }

    ForEach(fn, f->functions) {
        ResolveSymbolsInFunction(g, fn);
    }
}

static void TypeCheckExpr(struct Globals *g, struct Expr *e, struct Type *expected)
{
}

static void TypeCheckStmt(struct Globals *g, struct Stmt *stmt)
{
    switch (stmt->kind) {
        case STMT_LET:
            break;
        case STMT_RETURN:
            assert(g->current_function);
            TypeCheckExpr(g, &stmt->ret.value, &g->current_function->type.return_type);
            break;
    }
}

static void TypeCheckBlock(struct Globals *g, struct Block *b)
{
    struct Stmt *stmt;

    ForEach(stmt, b->stmts) {
        TypeCheckStmt(g, stmt);
    }
}

static void TypeCheckFunction(struct Globals *g, struct Function *f)
{
    g->current_function = f;

    TypeCheckBlock(g, &f->body);
}

static void TypeCheck(struct Globals *g, struct File *f)
{
    struct Function *fn;

    ForEach(fn, f->functions) {
        TypeCheckFunction(g, fn);
    }
    g->current_function = NULL;
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
    struct Globals g;
    struct File f;

    clock_gettime(CLOCK_MONOTONIC, &t0);

    g = CreateGlobals();
    f = ParseFile(path);
    ResolveSymbols(&g, &f);
    TypeCheck(&g, &f);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    dt = TimespecToDouble(TimespecSubtract(t1, t0));
    printf("Compiling '%s' took %f seconds.\n", path, dt);
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
