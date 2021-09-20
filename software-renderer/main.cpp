#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <utility>
#include <string>
#include <string_view>
#include <sys/mman.h>

template<typename T>
struct Vec {
  T *ptr;
  uint32_t len, cap;

  ~Vec() {
    free(ptr);
  }

  void push(T v) {
    if (len >= cap) {
      cap = cap ? cap * 3 / 2 : 8;
      ptr = static_cast<T*>(realloc(ptr, sizeof(T) * cap));
      assert(ptr);
    }
    ptr[len++] = std::move(v);
  }

  const T &operator[](uint32_t i) const {
    assert(i < len);
    return ptr[i];
  }

  T &operator[](uint32_t i) {
    assert(i < len);
    return ptr[i];
  }

  const T *begin() const {
    return ptr;
  }

  const T *end() const {
    return ptr + len;
  }

  T *begin() {
    return ptr;
  }

  T *end() {
    return ptr + len;
  }
};

struct Image {
  struct Pixel {
    uint8_t b, g, r;
  };
  Pixel *data;
  int width, height;

  static Image init(int width, int height) {
    auto data = static_cast<Pixel*>(calloc(sizeof(Pixel), width * height));
    return {data, width, height};
  }

  ~Image() {
    free(data);
  }

  void write_tga_file(const char *path) {
    uint8_t header[18] = {};
    // Uncompressed true color image.
    header[2] = 2;
    // Width, little-endian byte-order.
    header[12] = width & 0xFF;
    header[13] = (width & 0xFF00) >> 8;
    // Height, little-endian byte-order.
    header[14] = height & 0xFF;
    header[15] = (height & 0xFF00) >> 8;
    // Bits-per-pixel.
    header[16] = sizeof(Pixel) * 8;

    auto file = fopen(path, "w");
    fwrite(header, sizeof(header), 1, file);
    fwrite(data, sizeof(Pixel), width * height, file);
    fclose(file);
  }

  void draw_line(int x0, int y0, int x1, int y1) {
    bool steep = false;
    if (std::abs(y1 - y0) > std::abs(x1 - x0)) {
      std::swap(x0, y0);
      std::swap(x1, y1);
      steep = true;
    }
    if (x1 < x0) {
      std::swap(x0, x1);
      std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int de = std::abs(dy);
    int e = 0;
    int y = y0;
    int y_step = y0 < y1 ? 1 : -1;
    for (int x = x0; x <= x1; x++) {
      auto i = steep ? x * width + y : y * width + x;
      if (0 < i && i < width * height) {
        data[i] = {0, 0, 0xFF};
      }
      // printf("(%d, %d) e=%d\n", x, y, e);
      e += de;
      if (2 * e >= dx) {
        y += y_step;
        e -= dx;
      }
    }
  }
};

struct Obj {
  struct Vertex {
    float x;
    float y;
    float z;
  };
  struct Triangle {
    uint16_t vertex_index[3];
  };

  Vec<Vertex> vertices;
  Vec<Triangle> triangles;
};

static Obj::Vertex parse_vertex(FILE *f) {
  auto v = Obj::Vertex{};
  if (fscanf(f, "%f %f %f\n", &v.x, &v.y, &v.z) != 3) {
    printf("Error parsing vertex\n");
    exit(1);
  }
  return v;
}

static Obj::Triangle parse_triangle(FILE *f) {
  auto t = Obj::Triangle{};
  uint16_t unused = 0;
  if (fscanf(f, " %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu\n",
         &t.vertex_index[0], &unused, &unused,
         &t.vertex_index[1], &unused, &unused,
         &t.vertex_index[2], &unused, &unused) != 9) {
    printf("Error parsing face\n");
    exit(1);
  }
  return t;
}

Obj read_obj_file(const char *path) {
  auto f = fopen(path, "r");
  auto obj = Obj{};

  while (!feof(f)) {
    int c = getc(f);
    switch (c) {
      case 'v': {
        c = getc(f);
        if (c != ' ') {
          break;
        }
        auto v = parse_vertex(f);
        obj.vertices.push(v);
        break;
      }
      case 'f': {
        auto t = parse_triangle(f);
        obj.triangles.push(t);
        break;
      }
      default:
        while (c != '\n') c = getc(f);
        break;
    }
  }

  fclose(f);
  return obj;
}

int main() {
  auto image = Image::init(800, 600);

  for (int x = 0; x < image.width; x += 100) {
    image.draw_line(x, 0, x, image.height - 1);
  }
  image.draw_line(image.width - 1, 0, image.width - 1, image.height - 1);
  for (int y = 0; y < image.height; y += 100) {
    image.draw_line(0, y, image.width - 1, y);
  }
  image.draw_line(0, image.height - 1, image.width - 1, image.height - 1);

  auto obj = read_obj_file("head.obj");
  for (const auto& t : obj.triangles) {
    for (int i = 0; i < 3; i++) {
      auto a = obj.vertices[t.vertex_index[i] - 1];
      auto b = obj.vertices[t.vertex_index[(i + 1) % 3] - 1];
      int x0 = (a.x + 1.0f) * (image.width / 2);
      int y0 = (a.y + 1.0f) * (image.height / 2);
      int x1 = (b.x + 1.0f) * (image.width / 2);
      int y1 = (b.y + 1.0f) * (image.height / 2);
      image.draw_line(x0, y0, x1, y1);
    }
  }

  image.write_tga_file("out.tga");
}
