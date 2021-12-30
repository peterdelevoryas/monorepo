#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define panic(...) \
  do { \
    printf(__VA_ARGS__); \
    if (errno) { \
      printf(": %s\n", strerror(errno)); \
    } \
    exit(1); \
  } while (0)

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;

struct f32x3 {
  f32 x, y, z;
};

struct u16x3 {
  u16 x, y, z;
};

static u8* align_address(u8* addr, u64 alignment) {
  i64 x = i64(addr);
  x += -x & i64(alignment - 1);
  return reinterpret_cast<u8*>(x);
}

struct Arena {
  u8* ptr;
  u32 cap;
  u32 len;

  u8* aligned_alloc(u64 size, u64 alignment) {
    u8* p = align_address(&ptr[len], alignment);
    len = p + size - ptr;
    if (len > cap) {
      panic("Arena cap=%u exceeded: %u", cap, len);
    }
    return p;
  }

  u8* aligned_realloc(u8* old_mem, u64 old_size, u64 new_size, u64 alignment) {
    assert(new_size > old_size);

    u8* arena_end = ptr + cap;
    u8* remaining = ptr + len;
    u8* mem_end = old_mem + old_size;
    if (mem_end == remaining) {
      u8* new_end = old_mem + new_size;
      if (new_end > arena_end) {
        panic("Arena cap=%u exceeded: %lu", cap, new_end - ptr);
      }
      len = new_end - ptr;
      return old_mem;
    }

    u8* new_mem = aligned_alloc(new_size, alignment);
    memcpy(new_mem, old_mem, old_size);
    return new_mem;
  }

  void reset() {
    len = 0;
  }

  template<typename T>
  T* alloc_array(u64 n) {
    u8* p = aligned_alloc(n * sizeof(T), alignof(T));
    return reinterpret_cast<T*>(p);
  }

  template<typename T>
  T* realloc_array(T* p, u64 old_len, u64 new_len) {
    u8* p_ = reinterpret_cast<u8*>(p);
    u8* q = aligned_realloc(p_, old_len * sizeof(T), new_len * sizeof(T), alignof(T));
    return reinterpret_cast<T*>(q);
  }
};

static u8 g_memory_pool[1024 * 1024 * 1024]; // 1 GiB
static Arena g_arena = {g_memory_pool, sizeof(g_memory_pool), 0};

template<typename T>
struct Vec {
  T* ptr;
  u32 len;
  u32 cap;

  static Vec<T> with_capacity(u32 cap, Arena& arena) {
    T* ptr = arena.alloc_array<T>(cap);
    return {ptr, 0, cap};
  }

  void push(const T& x, Arena& arena) {
    if (len >= cap) {
      u32 new_cap = cap ? cap * 3 / 2 : 16;
      ptr = arena.realloc_array<T>(ptr, cap, new_cap);
      cap = new_cap;
    }
    ptr[len++] = x;
  }

  T& operator[](u32 i) {
    assert(i < len);
    return ptr[i];
  }

  const T& operator[](u32 i) const {
    assert(i < len);
    return ptr[i];
  }
};

int main(int argc, char** argv) {
  int fd = open("head.obj", O_RDONLY);
  if (fd < 0) {
    panic("Unable to open head.obj");
  }
  struct stat st;
  if (fstat(fd, &st) != 0) {
    panic("Unable to fstat head.obj");
  }
  void* addr = mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (addr == MAP_FAILED) {
    panic("Unable to mmap head.obj");
  }
  const char* text = static_cast<const char*>(addr);
  Vec<f32x3> vertices = {nullptr, 0, 0};
  Vec<u16x3> faces = {nullptr, 0, 0};
  for (size_t i = 0; i < st.st_size; i++) {
    switch (text[i]) {
      case 'v':
        switch (text[i + 1]) {
          case ' ': {
            f32x3 v;
            if (sscanf(&text[i], "v %f %f %f", &v.x, &v.y, &v.z) == 3) {
              vertices.push(v, g_arena);
            }
            break;
          }
        }
        break;
      case 'f': {
        u16x3 f;
        u16 d;
        if (sscanf(&text[i], "f %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
                   &f.x, &d, &d, &f.y, &d, &d, &f.z, &d, &d) == 9) {
          faces.push(f, g_arena);
        }
        break;
      }
    }
  }
  munmap(addr, st.st_size);
  close(fd);
}
