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
    printf("%s:%s:%d: ", __FILE__, __func__, __LINE__); \
    printf(__VA_ARGS__); \
    if (errno) { \
      printf(": %s", strerror(errno)); \
    } \
    printf("\n"); \
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

struct MemoryMappedFile {
  void *addr;
  size_t size;
};

static MemoryMappedFile mmap_read_only(const char *path) {
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    panic("unable to open '%s'", path);
  }

  struct stat st;
  if (fstat(fd, &st) != 0) {
    panic("unable to fstat '%s'", path);
  }
  size_t size = st.st_size;

  void *addr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (addr == MAP_FAILED) {
    panic("unable to mmap head.obj");
  }
  close(fd);

  return {addr, size};
}

struct f32x3 {
  f32 x, y, z;
};

struct u16x3 {
  u16 x, y, z;
};

// static f32 dot(f32x3 a, f32x3 b) {
//   return a.x * b.x + a.y * b.y + a.z * b.z;
// }

static u8 *align_address(u8 *addr, u64 alignment) {
  i64 x = i64(addr);
  x += -x & i64(alignment - 1);
  return reinterpret_cast<u8 *>(x);
}

struct Arena {
  u8 *data;
  u32 capacity;
  u32 pos;

  template<u32 N>
  constexpr static Arena from_array(u8 (&array)[N]) {
    return {array, sizeof(array), 0};
  }

  void reset() {
    pos = 0;
  }

  u8 *aligned_alloc(u64 size, u64 alignment) {
    u8 *ret = data + pos;
    ret = align_address(ret, alignment);
    u32 new_pos = ret + size - data;

    if (new_pos > capacity) {
      panic("Exceeded arena capacity: %u > %u", new_pos, capacity);
    }
    pos = new_pos;

    return ret;
  }

  u8 *aligned_realloc(u8 *ptr, u64 size, u64 new_size, u64 alignment) {
    u8 *ret = &data[pos];
    if (ret == ptr + size) {
      u32 new_pos = ptr + new_size - data;
      if (new_pos > capacity) {
        panic("Exceeded arena capacity: %u > %u", new_pos, capacity);
      }
      pos = new_pos;
      return ptr;
    }

    ret = aligned_alloc(new_size, alignment);
    memcpy(ret, ptr, size);
    return ret;
  }

  template<typename T>
  T *alloc_array(u32 count) {
    return reinterpret_cast<T *>(aligned_alloc(count * sizeof(T), alignof(T)));
  }

  template<typename T>
  T *realloc_array(T *data, u32 count, u32 new_count) {
    u64 size = count * sizeof(T);
    u64 new_size = new_count * sizeof(T);
    u64 alignment = alignof(T);
    u8 *p = reinterpret_cast<u8 *>(data);
    p = aligned_realloc(p, size, new_size, alignment);
    return reinterpret_cast<T *>(p);
  }
};

static u32 next_power_of_two(u32 x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

template<typename T>
struct Vector {
  T *data = nullptr;
  u32 capacity = 0;
  u32 count = 0;

  void push(const T &x, Arena& arena) {
    if (count >= capacity) {
      u32 new_capacity = capacity ? capacity + capacity / 2 : 32;
      data = arena.realloc_array(data, capacity, new_capacity);
      capacity = new_capacity;
    }
    data[count++] = x;
  }

  Vector<T> clone_in(Arena& arena) {
    u32 n = next_power_of_two(count);
    T *p = arena.alloc_array<T>(n);
    memcpy(p, data, count * sizeof(T));
    return {p, n, count};
  }

  T &operator[](u32 i) {
    return data[i];
  }

  const T &operator[](u32 i) const {
    return data[i];
  }
};

static u8 g_tmp_memory[1024 * 1024];
static u8 g_memory[1024 * 1024];
static Arena g_tmp_arena = Arena::from_array(g_tmp_memory);
static Arena g_arena = Arena::from_array(g_memory);

struct Obj {
  Vector<f32x3> vertices;
  Vector<u16x3> faces;
};

static Obj load_obj(const char *path) {
  MemoryMappedFile file = mmap_read_only("head.obj");
  const char *s = static_cast<const char *>(file.addr);

  Vector<f32x3> vertices;
  Vector<u16x3> faces;
  for (size_t i = 0; i < file.size;) {
    f32 x, y, z;
    u16 a, b, c, d;
    switch (s[i]) {
      case 'v':
        switch (s[i + 1]) {
          case ' ':
            if (sscanf(&s[i], "v %f %f %f", &x, &y, &z) == 3) {
              vertices.push({x, y, z}, g_tmp_arena);
            }
            break;
        }
        break;
      case 'f':
        if (sscanf(&s[i], "f %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
                   &a, &d, &d, &b, &d, &d, &c, &d, &d) == 9) {
          faces.push({a, b, c}, g_tmp_arena);
        }
        break;
    }
    for (; i < file.size && s[i] != '\n'; i++);
    i++;
  }
  munmap(file.addr, file.size);

  vertices = vertices.clone_in(g_arena);
  faces = faces.clone_in(g_arena);
  g_tmp_arena.reset();

  return {vertices, faces};
}

int main(int argc, char **argv) {
  Obj obj = load_obj("head.obj");

  for (int i = 0; i < obj.vertices.count; i++) {
    f32x3 v = obj.vertices[i];
    printf("v %f %f %f\n", v.x, v.y, v.z);
  }

  for (int i = 0; i < obj.faces.count; i++) {
    u16x3 u = obj.faces[i];
    printf("f %hu %hu %hu\n", u.x, u.y, u.z);
  }
}
