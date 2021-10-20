#include <unistd.h>
#include "math.h"
#include "tga.h"

int main(int argc, char** argv) {
  TgaImage image = tga_image_init(1000, 1000);
  tga_image_write(&image, "out.tga");
  tga_image_free(&image);
  execlp("open", "open", "out.tga", NULL);
}
