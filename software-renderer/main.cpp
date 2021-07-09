#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <array>

struct Pixel {
  uint8_t b, g, r;
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
};

int main() {
  auto image = Image::zeros(1920, 1080);
  for (uint32_t i = 0; i < image.height; i++) {
    image.pixels[i * image.width + image.width / 2].r = 0xFF;
  }
  for (uint32_t i = 0; i < image.width; i++) {
    image.pixels[image.height * 3 * image.width / 4 + i].r = 0xFF;
  }
  image.write_tga_file("out.tga");
}
