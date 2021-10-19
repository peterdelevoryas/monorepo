#ifndef TGA_H
#define TGA_H
#include <cstdint>
#include <string_view>

namespace tga {
  struct Image {
    uint8_t* data;
    uint32_t width;
    uint32_t height;

    explicit Image(uint32_t width, uint32_t height) noexcept;
    ~Image() noexcept;
    void write_file(std::string_view path);
  };
};

#endif
