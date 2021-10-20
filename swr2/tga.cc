#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include "tga.hh"

using namespace tga;

#define panic(...) \
    do { \
        printf("%s: ", __func__); \
        printf(__VA_ARGS__); \
        if (errno) { \
            printf(": %s", strerror(errno)); \
        } \
        printf("\n"); \
        abort(); \
    } while (0)

Image::Image(uint32_t width, uint32_t height) noexcept
  : width(width), height(height)
{
  auto p = malloc(width * height * sizeof(pixels[0]));
  pixels = static_cast<Pixel*>(p);
}

void Image::free()
{
  ::free(pixels);
}

void Image::write(std::string_view path)
{
    uint8_t header[18];
    memset(header, 0, sizeof(header));
    // Uncompressed true color image.
    header[2] = 2;
    // Width, little-endian byte-order.
    header[12] = width & 0xFF;
    header[13] = (width & 0xFF00) >> 8;
    // Height, little-endian byte-order.
    header[14] = height & 0xFF;
    header[15] = (height & 0xFF00) >> 8;
    // Bits-per-pixel.
    header[16] = sizeof(pixels[0]) * 8;

    auto file = fopen(path.data(), "w");
    fwrite(header, sizeof(header), 1, file);
    fwrite(pixels, sizeof(pixels[0]), width * height, file);
    fclose(file);
}
