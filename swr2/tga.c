#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "tga.h"

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

TgaImage tga_image_init(uint32_t width, uint32_t height)
{
  TgaPixel* pixels = malloc(width * height * sizeof(TgaPixel));
  return (TgaImage){pixels, width, height};
}

void tga_image_free(TgaImage* self)
{
  free(self->pixels);
}

void tga_image_write(TgaImage* self, const char* path)
{
  uint8_t header[18];
  memset(header, 0, sizeof(header));
  // Uncompressed true color image.
  header[2] = 2;
  // Width, little-endian byte-order.
  header[12] = self->width & 0xFF;
  header[13] = (self->width & 0xFF00) >> 8;
  // Height, little-endian byte-order.
  header[14] = self->height & 0xFF;
  header[15] = (self->height & 0xFF00) >> 8;
  // Bits-per-pixel.
  header[16] = sizeof(self->pixels[0]) * 8;

  FILE* file = fopen(path, "w");
  fwrite(header, sizeof(header), 1, file);
  fwrite(self->pixels, sizeof(self->pixels[0]),
         self->width * self->height, file);
  fclose(file);
}


