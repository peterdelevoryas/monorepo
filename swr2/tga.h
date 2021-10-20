#ifndef TGA_H
#define TGA_H
#include <stdint.h>

typedef struct TgaPixel TgaPixel;
struct TgaPixel {
  uint8_t b, g, r;
};

typedef struct TgaImage TgaImage;
struct TgaImage {
  TgaPixel* pixels;
  uint32_t width, height;
};

TgaImage tga_image_init(uint32_t width, uint32_t height);
void tga_image_free(TgaImage* self);
void tga_image_write(TgaImage* self, const char* path);

#endif
