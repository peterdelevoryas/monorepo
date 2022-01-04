#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Analysis.h>

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

typedef struct MemoryMappedFile MemoryMappedFile;
typedef struct Symbol Symbol;
typedef struct Type Type;
typedef struct Param Param;
typedef struct FunctionType FunctionType;
typedef struct StmtArray StmtArray;
typedef struct Block Block;
typedef struct Stmt Stmt;
typedef struct Expr Expr;
typedef struct Let Let;
typedef struct Return Return;
typedef struct Function Function;
typedef struct Parser Parser;
typedef struct FunctionArray FunctionArray;
typedef struct File File;
typedef struct TypeArray TypeArray;
typedef struct SymbolTable SymbolTable;
typedef struct Globals Globals;
typedef enum TypeKind TypeKind;
typedef enum ExprKind ExprKind;
typedef enum StmtKind StmtKind;
typedef enum SymbolKind SymbolKind;
typedef enum Token Token;

struct MemoryMappedFile {
    void *addr;
    size_t size;
};

static MemoryMappedFile Mmap(const char *path)
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
    TOKEN_I8,
    TOKEN_I32,
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

enum SymbolKind {
    SYMBOL_UNDEFINED,
    SYMBOL_FUNCTION,
    SYMBOL_TYPE,
    SYMBOL_VARIABLE,
    SYMBOL_PARAM,
};

struct Symbol {
    SymbolKind kind;
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

// Maybe just delete this and cast Token to TypeKind?
static const TypeKind TOKEN_TYPE[] = {
    [TOKEN_I8] = TYPE_I8,
    [TOKEN_I32] = TYPE_I32,
};

struct Type {
    TypeKind kind;
    union {
        FunctionType *function;
        Type *pointer_value_type;
        Symbol symbol;
    };
};

static void PrintType(const Type *t)
{
    switch (t->kind) {
        case TYPE_I8:
            printf("i8");
            break;
        case TYPE_I32:
            printf("i32");
            break;
        case TYPE_POINTER:
            printf("*");
            PrintType(t->pointer_value_type);
            break;
        case TYPE_FUNCTION:
            printf("unimplemented: function type printing\n");
            abort();
        case TYPE_SYMBOL:
            printf("%s", t->symbol.value);
            break;
    }
}

struct Param {
    const char *name;
    Type type;
};

struct FunctionType {
    struct {
        Param buf[32];
        int len;
    } params;
    Type return_type;
};

struct StmtArray {
    Stmt *buf;
    int len, cap;
};

struct Block {
    StmtArray stmts;
};

enum ExprKind {
    EXPR_INT_LITERAL,
    EXPR_USE_SYMBOL,
};

struct Expr {
    ExprKind kind;
    const char *int_literal;
    Symbol use_symbol;
    Type type;
};

enum StmtKind {
    STMT_LET,
    STMT_RETURN,
};

struct Let {
    const char *name;
    Expr rhs;
    Type type;
};

struct Return {
    Expr value;
};

struct Stmt {
    StmtKind kind;
    Let let;
    Return ret;
};

struct Function {
    const char *name;
    FunctionType type;
    Block body;
};

struct Parser {
    const char *path, *file;
    size_t size;
    Token token;
    int start, end, line_no;
};

static void ParseIdent(Parser *p)
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

static void ParseInt(Parser *p)
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

static void Bump(Parser *p)
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

static void Expect(Parser *p, Token t)
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

static const char *ExpectString(Parser *p, Token t)
{
    const char *s;
    int n;

    s = &p->file[p->start];
    n = p->end - p->start;
    Expect(p, t);

    return strndup(s, n);
}

static Symbol ParseSymbol(Parser *p)
{
    Symbol symbol;

    Zero(symbol);
    symbol.kind = SYMBOL_UNDEFINED;
    symbol.value = ExpectString(p, TOKEN_IDENT);
    symbol.definition = NULL;

    return symbol;
}

static Type ParseType(Parser *p)
{
    Type t;

    Zero(t);
    switch (p->token) {
        case TOKEN_I8:
        case TOKEN_I32:
            t.kind = TOKEN_TYPE[p->token];
            Bump(p);
            break;
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

static Param ParseParam(Parser *p)
{
    Param param;

    param.name = ExpectString(p, TOKEN_IDENT);
    Expect(p, TOKEN_COLON);
    param.type = ParseType(p);
    return param;
}

static FunctionType ParseFunctionType(Parser *p)
{
    FunctionType f;

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

static Expr ParseExpr(Parser *p)
{
    Expr e;

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

static Stmt ParseStmt(Parser *p)
{
    Stmt stmt;

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

static Block ParseBlock(Parser *p)
{
    Block b;

    Zero(b);
    Expect(p, TOKEN_LBRACE);
    while (p->token != TOKEN_RBRACE) {
        Append(b.stmts, ParseStmt(p));
    }
    Expect(p, TOKEN_RBRACE);

    return b;
}

static Function ParseFunction(Parser *p)
{
    Function f;

    Zero(f);
    Expect(p, TOKEN_FUNCTION);
    f.name = ExpectString(p, TOKEN_IDENT);
    f.type = ParseFunctionType(p);
    f.body = ParseBlock(p);

    return f;
}

static Parser CreateParser(const char *path)
{
    MemoryMappedFile f;
    Parser p;

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

static void DestroyParser(Parser *p)
{
    munmap((void *)p->file, p->size);
}

struct FunctionArray {
    Function *buf;
    int len, cap;
};

struct File {
    const char *path;
    FunctionArray functions;
};

static File ParseFile(const char *path)
{
    Parser p;
    File f;

    Zero(f);
    f.path = path;
    p = CreateParser(path);

    while (p.token != TOKEN_EOF) {
        Append(f.functions, ParseFunction(&p));
    }

    DestroyParser(&p);
    return f;
}

struct TypeArray {
    Type *buf;
    int len, cap;
};

struct SymbolTable {
    Symbol *buf;
    int len, cap;
};

struct Globals {
    SymbolTable symbol_table;
    Function *current_function;
    LLVMBuilderRef builder;
    LLVMModuleRef module;
};

static Symbol CreateSymbol(SymbolKind kind, const char *value, void *definition)
{
    Symbol s;

    s.kind = kind;
    s.value = value;
    s.definition = definition;
    return s;
}

static void ResolveSymbol(Globals *g, Symbol *symbol)
{
    Symbol *def;
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

static int CreateScope(Globals *g)
{
    return g->symbol_table.len;
}

static void DestroyScope(Globals *g, int scope)
{
    assert(0 <= scope && scope < g->symbol_table.len);
    g->symbol_table.len = scope;
}

static void ResolveSymbolsInType(Globals *g, Type *t)
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

static void ResolveSymbolsInFunctionType(Globals *g, FunctionType *f)
{
    Param *p;

    ForEach(p, f->params) {
        ResolveSymbolsInType(g, &p->type);
        Append(g->symbol_table, CreateSymbol(SYMBOL_PARAM, p->name, p));
    }
}

static void ResolveSymbolsInExpr(Globals *g, Expr *e)
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

static void ResolveSymbolsInStmt(Globals *g, Stmt *stmt)
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

static void ResolveSymbolsInBlock(Globals *g, Block *b)
{
    Stmt *stmt;
    int scope;

    scope = CreateScope(g);
    ForEach(stmt, b->stmts) {
        ResolveSymbolsInStmt(g, stmt);
    }
    DestroyScope(g, scope);
}

static void ResolveSymbolsInFunction(Globals *g, Function *f)
{
    int scope;

    scope = CreateScope(g);
    ResolveSymbolsInFunctionType(g, &f->type);
    ResolveSymbolsInBlock(g, &f->body);
    DestroyScope(g, scope);
}

static void ResolveSymbols(Globals *g, File *f)
{
    Function *fn;

    // Add all function symbols before entering each function.
    ForEach(fn, f->functions) {
        Append(g->symbol_table, CreateSymbol(SYMBOL_FUNCTION, fn->name, fn));
    }

    ForEach(fn, f->functions) {
        ResolveSymbolsInFunction(g, fn);
    }
}

static bool TypesEqual(const Type *a, const Type *b)
{
    if (a->kind != b->kind) {
        PrintType(a);
        printf(" != ");
        PrintType(b);
        printf("\n");
        return false;
    }
    return true;
}

static Type TypeCheckSymbol(Globals *g, const Symbol *symbol)
{
    switch (symbol->kind) {
        case SYMBOL_UNDEFINED:
            printf("TypeCheck: symbol undefined: '%s'\n", symbol->value);
            abort();
        case SYMBOL_FUNCTION:
            printf("type checking function symbol unimplemented\n");
            abort();
        case SYMBOL_TYPE:
            printf("type checking type symbol unimplemented\n");
            abort();
        case SYMBOL_VARIABLE:
            return ((Let *)symbol->definition)->type;
        case SYMBOL_PARAM:
            return ((Param *)symbol->definition)->type;
    }
}

static void TypeCheckExpr(Globals *g, Expr *e, Type *expected)
{
    switch (e->kind) {
        case EXPR_INT_LITERAL:
            if (expected) {
                e->type = *expected;
            } else {
                e->type.kind = TYPE_I32;
            }
            break;
        case EXPR_USE_SYMBOL:
            e->type = TypeCheckSymbol(g, &e->use_symbol);
            break;
    }

    if (expected && !TypesEqual(&e->type, expected)) {
        printf("Type mismatch\n");
        abort();
    }
}

static void TypeCheckStmt(Globals *g, Stmt *stmt)
{
    switch (stmt->kind) {
        case STMT_LET:
            TypeCheckExpr(g, &stmt->let.rhs, NULL);
            stmt->let.type = stmt->let.rhs.type;
            break;
        case STMT_RETURN:
            assert(g->current_function);
            TypeCheckExpr(g, &stmt->ret.value, &g->current_function->type.return_type);
            break;
    }
}

static void TypeCheckBlock(Globals *g, Block *b)
{
    Stmt *stmt;

    ForEach(stmt, b->stmts) {
        TypeCheckStmt(g, stmt);
    }
}

static void TypeCheckFunction(Globals *g, Function *f)
{
    g->current_function = f;

    TypeCheckBlock(g, &f->body);
}

static void TypeCheck(Globals *g, File *f)
{
    Function *fn;

    ForEach(fn, f->functions) {
        TypeCheckFunction(g, fn);
    }
    g->current_function = NULL;
}

static void CodegenFunction(Globals *g, Function *f)
{
    
}

static void Codegen(Globals *g, File *f)
{
    char *triple, *error;
    LLVMTargetRef target;
    LLVMTargetMachineRef machine;
    LLVMTargetDataRef layout;
    LLVMModuleRef module;
    Function *fn;

    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86Target();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86AsmPrinter();

    triple = LLVMGetDefaultTargetTriple();
    if (LLVMGetTargetFromTriple(triple, &target, &error) != 0) {
        printf("LLVMGetTargetFromTriple: %s\n", error);
        abort();
    }

    machine = LLVMCreateTargetMachine(target, triple, "generic", "",
                                      LLVMCodeGenLevelNone,
                                      LLVMRelocDefault,
                                      LLVMCodeModelDefault);
    layout = LLVMCreateTargetDataLayout(machine);
    module = LLVMModuleCreateWithName(f->path);
    LLVMSetModuleDataLayout(module, layout);
    LLVMSetTarget(module, triple);

    g->builder = LLVMCreateBuilder();
    g->module = module;
    ForEach(fn, f->functions) {
        CodegenFunction(g, fn);
    }

    LLVMDisposeMessage(triple);
    LLVMDisposeMessage(error);
    LLVMDisposeTargetMachine(machine);
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
    Globals g;
    File f;

    clock_gettime(CLOCK_MONOTONIC, &t0);

    Zero(g);
    f = ParseFile(path);
    ResolveSymbols(&g, &f);
    TypeCheck(&g, &f);
    Codegen(&g, &f);

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
