#ifndef TGA_H
#define TGA_H
#include <cstdint>
#include <string_view>

namespace tga {

// A struct representing the layout of a pixel within a TGA image.
struct Pixel {
  // First byte is "blue".
  uint8_t b;
  // Second byte is "green".
  uint8_t g;
  // Third byte is "red".
  uint8_t r;
};

// A TGA image, containing a pixel array formatted for output to a file.
struct Image {
  // Array of pixels, len = width x height.
  Pixel* pixels;
  // Width of the image, in pixel units.
  uint32_t width;
  // Height of the image, in pixel units.
  uint32_t height;

  explicit Image(uint32_t width, uint32_t height) noexcept;
  void free();
  void write(std::string_view path);
};

};

#endif
