#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include "tga.h"

using namespace tga;

Image::Image(uint32_t width, uint32_t height) noexcept
  : width(width), height(height)
{
  auto size = width * height * 3;
  auto data_ = malloc(size);
  data = static_cast<uint8_t*>(data_);
  if (!data) {
    auto error = strerror(errno);
    printf("%s: unable to allocate %u: %s\n", __func__, size, error);
  }
}

Image::~Image() noexcept {
  free(data);
}

void Image::write_file(std::string_view path) {
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
  header[16] = 24;

  auto file = fopen(path.data(), "w");
  fwrite(header, sizeof(header), 1, file);
  auto size = width * height * 3;
  fwrite(data, size, 1, file);
}
