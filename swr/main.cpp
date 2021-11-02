#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define let const auto
#define var auto

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
struct Vector3 {
  T x, y, z;
};

using f32x3 = Vector3<f32>;
using u16x3 = Vector3<u16>;

template<typename T>
struct Vector {
  T* data;
  u32 len;
  u32 capacity;

  void free() const {
    ::free(data);
  }

  static Vector<T> with_capacity(u32 capacity) {
    let data = malloc(sizeof(T) * capacity);
    let data_ = static_cast<T*>(data);
    assert(data_);
    return {data_, 0, capacity};
  }

  void reserve(u32 count) {
    if (count <= capacity) {
      return;
    }
    while (capacity < count) {
      capacity = capacity * 3 / 2;
    }
    let data_ = realloc(data, sizeof(T) * capacity);
    assert(data_);
    data = static_cast<T*>(data_);
  }

  void append(T elem) {
    reserve(len + 1);
    data[len] = elem;
    len += 1;
  }
};

struct Obj {
  Vector<f32x3> vertices;
  Vector<u16x3> faces;

  static Obj read_file(const char* path);
  void free() const;
};

Obj Obj::read_file(const char* path) {
  let f = fopen(path, "r");
  assert(f);

  var vertices = Vector<f32x3>::with_capacity(10);
  var faces = Vector<u16x3>::with_capacity(10);

  for (char line[64]; fgets(line, sizeof(line), f);) {
    switch (line[0]) {
      case 'v': {
        f32 x, y, z;
        let r = sscanf(line, "v %f %f %f", &x, &y, &z);
        if (r != 3) {
          continue;
        }
        printf("vertex %f %f %f\n", x, y, z);
        vertices.append({x, y, z});
        break;
      }
      case 'f': {
        u16 a, b, c, d;
        let r = sscanf(line, "f %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
                       &a, &d, &d, &b, &d, &d, &c, &d, &d);
        if (r != 9) {
          continue;
        }
        a--;
        b--;
        c--;
        printf("face %hu %hu %hu\n", a, b, c);
        faces.append({a, b, c});
        break;
      }
    }
  }
  fclose(f);

  return {vertices, faces};
}

void Obj::free() const {
  vertices.free();
  faces.free();
}

int main(int argc, char** argv) {
  let obj = Obj::read_file("head.obj");
  obj.free();
}
