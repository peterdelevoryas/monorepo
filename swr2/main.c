#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

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

typedef struct TgaPixel TgaPixel;
struct TgaPixel {
  uint8_t b, g, r;
};

typedef struct TgaImage TgaImage;
struct TgaImage {
  TgaPixel* pixels;
  uint32_t width, height;
};

static void tga_image_init(TgaImage* self, uint32_t width, uint32_t height)
{
  self->pixels = malloc(width * height * sizeof(self->pixels[0]));
  if (!self->pixels) {
    panic("unable to allocate %ux%u image", width, height);
  }
  self->width = width;
  self->height = height;
}

static void tga_image_free(TgaImage* self)
{
  free(self->pixels);
}

static void tga_image_write(TgaImage* self, const char* path)
{
  uint8_t header[18] = {};
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

int main(int argc, char** argv) {
  TgaImage image = {};
  
  tga_image_init(&image, 1000, 1000);
  tga_image_write(&image, "out.tga");
  tga_image_free(&image);
}
