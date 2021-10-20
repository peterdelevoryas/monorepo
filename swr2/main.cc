#include <unistd.h>
#include "math.hh"
#include "tga.hh"
#include "obj.hh"

int main(int argc, char** argv) {
  //auto obj = obj_init("head.obj");
  //

  auto image = tga::Image(1000, 1000);
  image.write("out.tga");
  image.free();

  execlp("open", "open", "out.tga", NULL);
}
