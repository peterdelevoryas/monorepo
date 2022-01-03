#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

template<typename T>
struct Vector {
  T* buf = nullptr;
  int len = 0;
  int cap = 0;

  void Push(const T& x) {
    if (len >= cap) {
      int new_cap = cap ? cap + cap / 2 : 16;
      void* new_buf = realloc(buf, new_cap * sizeof(T));
      cap = new_cap;
      buf = static_cast<T*>(new_buf);
    }
    buf[len++] = x;
  }

  T& operator[](int i) {
    assert(i < len);
    return buf[i];
  }

  const T& operator[](int i) const {
    assert(i < len);
    return buf[i];
  }

  void Destroy() {
    free(buf);
  }
};

template<typename T, typename U>
struct Pair { T x; U y; };

static Pair<const char*, size_t> MmapFileReadOnly(const char* file_path) {
  int fd = open(file_path, O_RDONLY | O_CLOEXEC);
  assert(fd != -1);

  struct stat st;
  assert(fstat(fd, &st) == 0);
  size_t size = st.st_size;

  void* addr = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
  assert(addr != MAP_FAILED);
  const char* file = static_cast<const char*>(addr);

  close(fd);
  return {file, size};
}

enum class Token {
  Function,
  Let,
  Return,
  I8,
  I32,
  NrKeywords,
  Ident,
  Int,
  Lparen,
  Rparen,
  Lbrace,
  Rbrace,
  Colon,
  Semicolon,
  Comma,
  Star,
  Arrow,
  Minus,
  Plus,
  Eq,
  Space,
  Newline,
  Error,
  Eof,
};

static const char* TokenString(Token t) {
  switch (t) {
    case Token::Function: return "function";
    case Token::Let: return "let";
    case Token::Return: return "return";
    case Token::I8: return "i8";
    case Token::I32: return "i32";
    case Token::Lparen: return "(";
    case Token::Rparen: return ")";
    case Token::Lbrace: return "{";
    case Token::Rbrace: return "}";
    case Token::Eq: return "=";
    case Token::Colon: return ":";
    case Token::Eof: return nullptr;
    case Token::NrKeywords: return nullptr;
    case Token::Ident: return "<ident>";
    case Token::Int: return "<int>";
    case Token::Error: return nullptr;
    case Token::Space: return nullptr;
    case Token::Newline: return nullptr;
    case Token::Comma: return ",";
    case Token::Star: return "*";
    case Token::Arrow: return "->";
    case Token::Plus: return "+";
    case Token::Minus: return "-";
    case Token::Semicolon: return ";";
  }
}

static Token CharToToken(char c) {
  switch (c) {
    case 'a'...'z':
    case 'A'...'Z':
    case '_':
      return Token::Ident;
    case '0'...'9':
      return Token::Int;
    case '(':
      return Token::Lparen;
    case ')':
      return Token::Rparen;
    case '{':
      return Token::Lbrace;
    case '}':
      return Token::Rbrace;
    case '=':
      return Token::Eq;
    case ':':
      return Token::Colon;
    case ',':
      return Token::Comma;
    case '*':
      return Token::Star;
    case '+':
      return Token::Plus;
    case '-':
      return Token::Minus;
    case ';':
      return Token::Semicolon;
    case ' ':
    case '\t':
    case '\r':
      return Token::Space;
    case '\n':
      return Token::Newline;
    default:
      return Token::Error;
  }
}

struct Parser {
  const char* file_path;
  const char* file;
  size_t size;
  Token token;
  int start;
  int end;
  int line_no;

  static Parser Create(const char* file_path) {
    auto [file, size] = MmapFileReadOnly(file_path);
    return {
      .file_path = file_path,
      .file = file,
      .size = size,
      .token = Token::Eof,
      .start = 0,
      .end = 0,
      .line_no = 1,
    };
  }

  void ParseIdent() {
    for (; end < size; end++) {
      char c = file[end];
      switch (c) {
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
    const char* ident = file + start;
    size_t len = end - start;
    for (int i = 0; i < int(Token::NrKeywords); i++) {
      const char* s = TokenString(Token(i));
      size_t n = strlen(s);
      if (n != len) {
        continue;
      }
      if (memcmp(s, ident, len) != 0) {
        continue;
      }
      token = Token(i);
      break;
    }
  }

  void ParseInt() {
    for (; end < size; end++) {
      char c = file[end];
      switch (c) {
        case '0'...'9':
          continue;
        default:
          break;
      }
      break;
    }
  }

  void Bump() {
    for (;;) {
      start = end;
      if (end >= size) {
        token = Token::Eof;
        break;
      }
      char c = file[end];
      token = CharToToken(c);
      switch (token) {
        case Token::Ident:
          ParseIdent();
          break;
        case Token::Int:
          ParseInt();
          break;
        case Token::Space:
          end++;
          continue;
        case Token::Newline:
          end++;
          line_no++;
          continue;
        case Token::Minus:
          end++;
          if (end < size && file[end] == '>') {
            end++;
            token = Token::Arrow;
          }
          break;
        case Token::Lparen:
        case Token::Rparen:
        case Token::Lbrace:
        case Token::Rbrace:
        case Token::Eq:
        case Token::Colon:
        case Token::Comma:
        case Token::Star:
        case Token::Plus:
        case Token::Semicolon:
          end++;
          break;
        case Token::Arrow:
        case Token::Eof:
        case Token::Function:
        case Token::Let:
        case Token::Return:
        case Token::I8:
        case Token::I32:
        case Token::NrKeywords:
          printf("This should be unreachable\n");
          abort();
        case Token::Error:
          printf("Parse error on '%c'\n", c);
          exit(1);
      }
      break;
    }
  }

  void Destroy() {
    char* file = const_cast<char*>(this->file);
    void* addr = reinterpret_cast<void*>(file);
    munmap(addr, size);
  }
};

struct timespec operator-(struct timespec x, struct timespec y) {
  return {x.tv_sec - y.tv_sec, x.tv_nsec - y.tv_nsec};
}

static double TimespecToDouble(struct timespec x) {
  double nsec = double(x.tv_nsec) * 1e-9;
  return double(x.tv_sec) + nsec;
}

static void Compile(const char* path) {
  printf("Compiling '%s'...", path);

  struct timespec t0;
  clock_gettime(CLOCK_MONOTONIC, &t0);

  Vector<Token> tokens;

  auto p = Parser::Create(path);
  for (p.Bump(); p.token != Token::Eof; p.Bump()) {
    tokens.Push(p.token);
  }
  p.Destroy();

  struct timespec t1;
  clock_gettime(CLOCK_MONOTONIC, &t1);
  printf("done: %f seconds\n", TimespecToDouble(t1 - t0));

  for (int i = 0; i < tokens.len; i++) {
    printf("%s ", TokenString(tokens[i]));
  }
  printf("\n");
  tokens.Destroy();
}

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("usage: %s [-h] file...\n", argv[0]);
    return 0;
  }
  for (int i = 1; i < argc; i++) {
    Compile(argv[i]);
  }
}
