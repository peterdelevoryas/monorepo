#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <array>

#define swap(a, b)  \
do {                \
  auto tmp = a;     \
  a = b;            \
  b = tmp;          \
} while (0)         \

template<typename T>
T abs(T x) {
  return x < 0 ? -x : x;
}

struct Image {
  struct Pixel {
    uint8_t b;
    uint8_t g;
    uint8_t r;
  };

  struct Point {
    int x;
    int y;
  };

  Pixel* data;
  int width;
  int height;

  static Image zeros(int width, int height) {
    auto size = sizeof(Pixel) * width * height;
    auto data = static_cast<Pixel*>(malloc(size));
    memset(data, 0, size);
    return {data, width, height};
  }

  void write_tga_file(const char* path) {
    auto file = fopen(path, "w");
    auto header = std::array<uint8_t, 18>{};
    header[2] = 2;
    header[12] = (width & 0x0000'00FF);
    header[13] = (width & 0x0000'FF00) >> 8;
    header[14] = (height & 0x0000'00FF);
    header[15] = (height & 0x0000'FF00) >> 8;
    header[16] = sizeof(Pixel) * 8; // Number of bits per pixel.
    header[17] = 0b0000'0000;
    fwrite(header.data(), header.size(), 1, file);
    fwrite(data, sizeof(Pixel), width * height, file);
    fclose(file);
  }

  Pixel& at(int x, int y) {
    return data[y * width + x];
  }

  void draw_line(Point a, Point b, Pixel color) {
    if (a.x > b.x) {
      swap(a, b);
    }

    auto steep = false;
    auto dx = b.x - a.x;
    auto dy = b.y - a.y;
    if (dy > dx) {
      swap(a.x, a.y);
      swap(b.x, b.y);
      steep = true;
    }

    for (int x = a.x; x <= b.x; x++) {
      auto dx = x - a.x;
      auto t = dx / float(b.x - a.x);
      auto y = a.y + int(float(b.y - a.y) * t);
      if (steep) {
        data[x * width + y] = color;
        continue;
      }
      data[y * width + x] = color;
    }
  }

  void free() {
    ::free(data);
  }
};

int main() {
  auto image = Image::zeros(100, 100);
  image.draw_line({10, 10}, {80, 32}, {0xFF, 0xFF, 0xFF});
  image.draw_line({20, 13}, {40, 80}, {0x00, 0x00, 0xFF});
  image.draw_line({80, 32}, {10, 10}, {0x00, 0x00, 0xFF});
  image.write_tga_file("out.tga");
  image.free();
}
