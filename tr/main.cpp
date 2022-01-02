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
  int i;

  char bump() {
    return s[i++];
  }

  void skip_space() {
    for (;;) {
      switch (s[i]) {
        case ' ':
        case '\t':
        case '\r':
          i++;
          continue;
        default:
          break;
      }
      break;
    }
  }

  void skip_to_space() {
    for (;;) {
      switch (s[i]) {
        case '\0':
        case ' ':
        case '\t':
        case '\r':
        case '\n':
          break;
        default:
          i++;
          continue;
      }
      break;
    }
  }

  void skip_to_next_line() {
    for (;;) {
      switch (s[i]) {
        case '\0':
          break;
        case '\n':
          i++;
          break;
        default:
          i++;
          continue;
      }
      break;
    }
  }

  f32 parse_f32() {
    char* end;
    f32 x = strtof(&s[i], &end);
    assert(end != s);
    i = end - s;
    return x;
  }

  u16 parse_u16() {
    char* end;
    unsigned long x = strtoul(&s[i], &end, 10);
    assert(end != s);
    assert(x <= UINT16_MAX);
    i = end - s;
    return u16(x);
  }
};

static Obj load_obj(const char* path) {
  Obj obj;

  auto [addr, size] = mmap_file(path);
  auto s = static_cast<const char*>(addr);
  auto p = Parser{s, 0};
  for (;;) {
    f32 v[3];
    u16 u[3];
    switch (p.bump()) {
      case '\0':
        goto done;
      case 'v':
        switch (p.bump()) {
          case 'n':
            printf("vn ? ? ?\n");
            break;
          case 't':
            printf("vt ? ? ?\n");
            break;
          default:
            for (int i = 0; i < 3; i++) {
              p.skip_space();
              v[i] = p.parse_f32();
            }
            obj.vertices.push({v[0], v[1], v[2]});
            printf("v %f %f %f\n", v[0], v[1], v[2]);
            break;
        }
        break;
      case 'f':
        for (int i = 0; i < 3; i++) {
          p.skip_space();
          u[i] = p.parse_u16();
          p.skip_to_space();
        }
        obj.faces.push({u[0], u[1], u[2]});
        printf("f %hu %hu %hu\n", u[0], u[1], u[2]);
        break;
    }
    p.skip_to_next_line();
  }
done:
  munmap(addr, size);

  return obj;
}

struct Image {
  struct Pixel { u8 b, g, r; };

  Pixel* pixels;
  int width;
  int height;

  template<int W, int H>
  static Image from_array(Pixel (&array)[H][W]) {
    return {&array[0][0], W, H};
  }
};

int main(int argc, char** argv) {
  auto obj = load_obj("head.obj");
  (void)obj;

  static Image::Pixel image_memory[1000][1000];
  auto image = Image::from_array(image_memory);
  (void)image;
}
