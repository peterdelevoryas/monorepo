#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

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
        data[x * width + y] = c;
        continue;
      }
      data[y * width + x] = c;
    }
  }

  void free() {
    ::free(data);
  }
};

constexpr int N = 1'000'000;
constexpr int M = 100;

int main(int argc, char** argv) {
  auto buf = static_cast<uint8_t*>(malloc(N * 4));
  auto dev_random = fopen("/dev/random", "r");
  fread(buf, N, 4, dev_random);
  fclose(dev_random);
  for (int i = 0; i < N * 4; i++) {
    auto b = buf[i];
    auto t = float(b) / 256.0;
    buf[i] = uint8_t(float(M) * t);
    assert(0 <= buf[i] && buf[i] < 100);
  }
  auto image = Image::zeros(M, M);
  for (int i = 0; i < N; i++) {
    auto p = &buf[i * 4];
    auto a = Image::Point(p[0], p[1]);
    auto b = Image::Point(p[2], p[3]);
    image.draw_line(a, b, {0, 0, 255});
  }
  image.write_tga_file("out.tga");
  image.free();
  free(buf);
}
