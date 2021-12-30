#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <limits>
#include <math.h>
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

template<typename T>
T min(const T &x, const T &y) {
  return x > y ? y : x;
}

template<typename T>
T max(const T &x, const T &y) {
  return x < y ? y : x;
}

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

  f32 magnitude() const;
  f32x3 normalize() const;
};

struct u16x3 {
  u16 x, y, z;
};

static f32x3 cross(f32x3 a, f32x3 b) {
  f32 s0 = a.y * b.z - a.z * b.y;
  f32 s1 = a.z * b.x - a.x * b.z;
  f32 s2 = a.x * b.y - a.y * b.x;
  return {s0, s1, s2};
}

static f32 dot(f32x3 a, f32x3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

// static f32 operator*(f32x3 a, f32x3 b) {
//   return a.x * b.x + a.y * b.y + a.z * b.z;
// }

static f32x3 operator*(f32x3 a, f32 s) {
  return {a.x * s, a.y * s, a.z * s};
}

static f32x3 operator/(f32x3 a, f32 d) {
  return a * (1.0f / d);
}

// static f32x3 operator+(f32x3 a, f32x3 b) {
//   return {a.x + b.x, a.y + b.y, a.z + b.z};
// }

static f32x3 operator-(f32x3 a, f32x3 b) {
  return {a.x - b.x, a.y - b.y, a.z - b.z};
}

f32 f32x3::magnitude() const {
  return sqrtf(dot(*this, *this));
}

f32x3 f32x3::normalize() const {
  return (*this) / magnitude();
}

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
    assert(i < count);
    return data[i];
  }

  const T &operator[](u32 i) const {
    assert(i < count);
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
          a--;
          b--;
          c--;
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

struct Pixel {
  u8 b, g, r;
};

struct Image {
  Pixel *pixels;
  f32 *zbuffer;
  u32 width;
  u32 height;

  template<u32 N, u32 M>
  static Image from_arrays(Pixel (&pixels)[N][M], f32 (&zbuffer)[N][M]) {
    memset(pixels, 0, sizeof(pixels));
    for (u32 i = 0; i < N; i++) {
      for (u32 j = 0; j < M; j++) {
        zbuffer[i][j] = std::numeric_limits<f32>::max();
      }
    }
    return {&pixels[0][0], &zbuffer[0][0], M, N};
  }

  Pixel &at(u32 x, u32 y) {
    assert(x < width);
    assert(y < height);
    return pixels[y * width + x];
  }

  const Pixel &at(u32 x, u32 y) const {
    assert(x < width);
    assert(y < height);
    return pixels[y * width + x];
  }

  f32x3 screen_coord(const f32x3 &v) {
    f32 x = (v.x + 1.0f) * f32(width / 2);
    f32 y = (v.y + 1.0f) * f32(height / 2);
    return {x, y, v.z};
  }

  void draw_triangle(f32x3 a, f32x3 b, f32x3 c, Pixel color) {
    a = screen_coord(a);
    b = screen_coord(b);
    c = screen_coord(c);
    f32 min_x = min(min(a.x, b.x), c.x);
    f32 max_x = max(max(a.x, b.x), c.x);
    f32 min_y = min(min(a.y, b.y), c.y);
    f32 max_y = max(max(a.y, b.y), c.y);
    for (i32 x = min_x; x <= max_x; x++) {
      if (x >= width) {
        continue;
      }
      for (i32 y = min_y; y <= max_y; y++) {
        if (y >= height) {
          continue;
        }
        f32x3 p = {c.x - a.x, b.x - a.x, a.x - f32(x)};
        f32x3 q = {c.y - a.y, b.y - a.y, a.y - f32(y)};
        f32x3 s = cross(p, q);
        if (fabsf(s.z) < 1.0f) {
          continue;
        }
        f32x3 t = {1.0f - (s.x + s.y) / s.z, s.y / s.z, s.x / s.z};
        if (t.x < 0.0f || t.y < 0.0f || t.z < 0.0f) {
          continue;
        }
        at(x, y) = color;
      }
    }
  }

  void save_as_tga_file(const char *path) {
    assert(width <= UINT16_MAX);
    assert(height <= UINT16_MAX);

    constexpr u8 bytes_per_pixel = sizeof(pixels[0]);
    constexpr u8 bits_per_pixel = bytes_per_pixel * 8;

    u8 header[18] = {};
    header[2] = 2;
    header[12] = width & 0xFF;
    header[13] = (width & 0xFF00) >> 8;
    header[14] = height & 0xFF;
    header[15] = (height & 0xFF00) >> 8;
    header[16] = bits_per_pixel;

    FILE *f = fopen(path, "w");
    assert(f);
    fwrite(header, sizeof(header), 1, f);
    fwrite(pixels, bytes_per_pixel, width * height, f);
    fclose(f);
  }
};

int main(int argc, char **argv) {
  static Pixel pixels[1000][1000];
  static f32 zbuffer[1000][1000];
  Image image = Image::from_arrays(pixels, zbuffer);

  Obj obj = load_obj("head.obj");

  constexpr f32x3 spotlight = {0.0f, 0.0f, -1.0f};
  for (int i = 0; i < obj.faces.count; i++) {
    u16x3 f = obj.faces[i];
    f32x3 a = obj.vertices[f.x];
    f32x3 b = obj.vertices[f.y];
    f32x3 c = obj.vertices[f.z];
    f32x3 ab = b - a;
    f32x3 ac = c - a;
    f32x3 n = cross(ac, ab).normalize();
    f32 I = dot(n, spotlight);
    if (I <= 0.0f) {
      continue;
    }
    u8 p = I * 255.0f;
    image.draw_triangle(a, b, c, {p, p, p});
  }

  image.save_as_tga_file("out.tga");
}
