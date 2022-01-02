#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;

static u8* align_address(u8* address, i64 alignment) {
  assert(alignment >= 1);
  i64 x = i64(address);
  x += -x & (alignment - 1);
  return reinterpret_cast<u8*>(x);
}

template<typename T, int N>
struct SmallVector {
  int len = 0;
  T buf[N];

  void push(const T& x) {
    assert(len < N);
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
};

template<typename T>
struct Vector {
  T* buf = nullptr;
  int cap = 0;
  int len = 0;

  void push(const T& x) {
    if (len >= cap) {
      int cap_ = cap ? cap + cap / 2 : 16;
      buf = static_cast<T*>(realloc(buf, cap_ * sizeof(T)));
      cap = cap_;
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
};

template<typename A, typename B>
struct Pair {
  A x;
  B y;
};

static Pair<void*, size_t> mmap_file(const char* path) {
  int fd = open(path, O_RDONLY);
  assert(fd != -1);

  struct stat st;
  assert(fstat(fd, &st) == 0);
  size_t size = static_cast<size_t>(st.st_size);

  void* addr = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
  assert(addr != MAP_FAILED);

  close(fd);
  return {addr, size};
}

struct f32x3 { f32 x, y, z; };
struct u16x3 { u16 x, y, z; };

struct Obj {
  Vector<f32x3> vertices;
  Vector<u16x3> faces;
};

struct Parser {
  const char* s;

  void skip_space() {
  start:
    switch (*s) {
      case ' ':
      case '\t':
      case '\r':
        s++;
        goto start;
      default:
        break;
    }
  }

  void skip_to_next_line() {
  start:
    switch (*s) {
      case '\0':
        break;
      case '\n':
        s++;
        break;
      default:
        s++;
        goto start;
    }
  }

  f32 parse_f32() {
    char* end;
    f32 x = strtof(s, &end);
    assert(end != s);
    s = end;
    return x;
  }

  u16 parse_u16() {
    char* end;
    unsigned long x = strtoul(s, &end, 10);
    assert(end != s);
    assert(x <= UINT16_MAX);
    s = end;
    return u16(x);
  }
};

static Obj load_obj(const char* path) {
  Obj obj;

  auto [addr, size] = mmap_file(path);
  auto s = static_cast<const char*>(addr);
  auto p = Parser{s};
  for (;;) {
    switch (*p.s) {
      case 'v':
        p.s++;
        switch (*p.s) {
          case 'n':
            break;
          case 't':
            break;
          default: {
            f32 v[3];
            for (int i = 0; i < 3; i++) {
              p.skip_space();
              v[i] = p.parse_f32();
            }
            obj.vertices.push({v[0], v[1], v[2]});
            break;
          }
        }
        break;
      case 'f':
        p.s++;
        break;
    }
    p.skip_to_next_line();
  }
  munmap(addr, size);

  return obj;
}

int main(int argc, char** argv) {
  (void)align_address;

  Obj obj = load_obj("head.obj");
  (void)obj;
}
