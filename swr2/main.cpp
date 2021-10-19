#include <cstdio>
#include <cstdlib>
#include "tga.h"

int main() {
  auto image = tga::Image(1000, 1000);
  image.write_file("out.tga");
}
