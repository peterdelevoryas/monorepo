#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>

#define swap(a, b)  \
do {                \
  auto tmp = a;     \
  a = b;            \
  b = tmp;          \
} while (0)         \

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

  static constexpr uint8_t UNCOMPRESSED_TRUE_COLOR_IMAGE = 2;
  static constexpr uint8_t ALPHA_CHANNEL_DEPTH = 0;
  static constexpr uint8_t LOWER_LEFT_HAND_ORIGIN = 0;

  void write_tga_file(const char* path) {
    uint8_t header[18] = {};
    header[2] = UNCOMPRESSED_TRUE_COLOR_IMAGE;
    header[12] = (width & 0x0000'00FF);
    header[13] = (width & 0x0000'FF00) >> 8;
    header[14] = (height & 0x0000'00FF);
    header[15] = (height & 0x0000'FF00) >> 8;
    header[16] = sizeof(Pixel) * 8; // Number of bits per pixel.
    header[17] = ALPHA_CHANNEL_DEPTH | (LOWER_LEFT_HAND_ORIGIN << 5);

    auto file = fopen(path, "w");
    fwrite(header, sizeof(header), 1, file);
    fwrite(data, sizeof(Pixel), width * height, file);
    fclose(file);
  }

  Pixel& at(int x, int y) {
    return data[y * width + x];
  }

  void draw_line(Point a, Point b, Pixel c) {
    //printf("draw_line (%u, %u) (%u, %u) #%02x%02x%02x\n", a.x, a.y, b.x, b.y, c.r, c.g, c.b);
    if (a.x > b.x) {
      swap(a, b);
    }

    bool steep = false;
    int dx = b.x - a.x;
    int dy = b.y - a.y;
    if (dy > dx) {
      swap(a.x, a.y);
      swap(b.x, b.y);
      steep = true;
    }

    int derr = dy * 2;
    int err = 0;

    for (int x = a.x, y = a.y; x <= b.x; x++) {
      int x_ = steep ? y : x;
      int y_ = steep ? x : y;
      data[x_ * width + y_] = c;
      err += derr;
      if (err < dx) {
        continue;
      }
      y +=
      err -= dx * 2;
    }
  }

  void free() {
    ::free(data);
  }
};

constexpr int N = 1'000'000;
constexpr int M = 100;

int main(int argc, char** argv) {
  auto image = Image::zeros(M, M);
  image.draw_line({0, 0}, {99, 99}, {0, 0, 0xFF});
  image.write_tga_file("out.tga");
  image.free();
}
