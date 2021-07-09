#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <array>

struct Pixel {
  uint8_t b, g, r;
};

struct Index2 {
  uint32_t x, y;
};

struct Float2 {
  float x, y;
};

struct Image {
  Pixel* pixels;
  uint32_t width;
  uint32_t height;

  static Image zeros(uint32_t width, uint32_t height) {
    auto pixels_size = sizeof(Pixel) * width * height;
    auto pixels = static_cast<Pixel*>(malloc(pixels_size));
    memset(pixels, 0, pixels_size);
    return {pixels, width, height};
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
    fwrite(pixels, sizeof(Pixel), width * height, file);
    fclose(file);
  }

  Pixel& at(uint32_t x, uint32_t y) {
    return pixels[y * width + x];
  }

  void draw_line(Index2 a, Index2 b, Pixel color) {
    auto dx = float(b.x - a.x) * 0.01f;
    auto dy = float(b.y - a.y) * 0.01f;
    auto p = Float2{float(a.x), float(a.y)};
    for (int i = 0; i < 100; i++) {
      auto x = uint32_t(p.x);
      auto y = uint32_t(p.y);
      this->at(x, y) = color;
      p.x += dx;
      p.y += dy;
    }
  }
};

int main() {
  auto image = Image::zeros(100, 100);
  image.draw_line({10, 10}, {80, 32}, {0xFF, 0xFF, 0xFF});
  image.write_tga_file("out.tga");
}
