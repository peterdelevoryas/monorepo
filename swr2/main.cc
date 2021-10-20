#include <cstdlib>
#include <unistd.h>
#include <math.h>
#include "math.hh"
#include "tga.hh"
#include "obj.hh"

using obj::Obj;
using obj::Vertex;
using tga::Image;
using tga::Pixel;

template<typename T>
T max(T a, T b)
{
  return a > b ? a : b;
}

template<typename T>
T min(T a, T b)
{
  return a < b ? a : b;
}

struct NDC {
  int x;
  int y;
  int z;
};

static void draw_triangle(Image& image, NDC a, NDC b, NDC c, Pixel color)
{
  auto min_x = min(min(a.x, b.x), c.x);
  auto max_x = max(max(a.x, b.x), c.x);
  auto min_y = min(min(a.y, b.y), c.y);
  auto max_y = max(max(a.y, b.y), c.y);
  for (int x = min_x; x <= max_x; x++) {
    if (x >= image.width) {
      continue;
    }
    for (int y = min_y; y <= max_y; y++) {
      if (y >= image.height) {
        continue;
      }
      auto p = float3(c.x - a.x, b.x - a.x, a.x - x);
      auto q = float3(c.y - a.y, b.y - a.y, a.y - y);
      auto s = p.cross(q);
      if (fabsf(s.z) < 1.0f) {
        continue;
      }
      auto t = float3(1.0f - (s.x + s.y) / s.z, s.y / s.z, s.x / s.z);
      if (t.x < 0.0f || t.y < 0.0f || t.z < 0.0f) {
        continue;
      }
      image.pixels[y * image.width + x] = color;
    }
  }
}

static NDC ndc(Vertex v, uint16_t width, uint16_t height)
{
  int x = (v.x + 1.0f) * float(width / 2);
  int y = (v.y + 1.0f) * float(height / 2);
  return {x, y, int(v.z)};
}

static void draw_obj(const Obj& obj, Image& image)
{
  for (auto& f : obj.faces) {
    auto v0 = obj.vertices[f.v0];
    auto v1 = obj.vertices[f.v1];
    auto v2 = obj.vertices[f.v2];
    auto n0 = ndc(v0, image.width, image.height);
    auto n1 = ndc(v1, image.width, image.height);
    auto n2 = ndc(v2, image.width, image.height);
    uint8_t r = rand() % 255;
    uint8_t g = rand() % 255;
    uint8_t b = rand() % 255;
    draw_triangle(image, n0, n1, n2, {b, g, r});
  }
}

int main(int argc, char** argv) {
  auto obj = Obj::from_file("head.obj");
  auto image = Image::allocate(1000, 1000);
  draw_obj(obj, image);
  obj.free();
  image.write("out.tga");
  image.free();

  execlp("open", "open", "out.tga", NULL);
}
