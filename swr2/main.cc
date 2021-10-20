#include <cstdlib>
#include <unistd.h>
#include <math.h>
#include "math.hh"
#include "tga.hh"
#include "obj.hh"

using obj::Obj;
using tga::Image;
using tga::Pixel;

template<typename T>
inline T max(T a, T b)
{
  return a > b ? a : b;
}

template<typename T>
inline T min(T a, T b)
{
  return a < b ? a : b;
}

struct int3 {
  int x;
  int y;
  int z;
};

static void draw_triangle(Image& image, int3 a, int3 b, int3 c, Pixel color)
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
      auto p = float3{float(c.x - a.x), float(b.x - a.x), float(a.x - x)};
      auto q = float3{float(c.y - a.y), float(b.y - a.y), float(a.y - y)};
      auto s = cross(p, q);
      if (fabsf(s.z) < 1.0f) {
        continue;
      }
      auto t = float3{1.0f - (s.x + s.y) / s.z, s.y / s.z, s.x / s.z};
      if (t.x < 0.0f || t.y < 0.0f || t.z < 0.0f) {
        continue;
      }
      image.pixels[y * image.width + x] = color;
    }
  }
}

static int3 world_to_screen(float3 v, uint16_t width, uint16_t height)
{
  auto x = int((v.x + 1.0f) * float(width / 2));
  auto y = int((v.y + 1.0f) * float(height / 2));
  return {x, y, int(v.z)};
}

constexpr auto SPOTLIGHT = float3(0.0f, 0.0f, -1.0f);

static void draw_obj(const Obj& obj, Image& image)
{
  for (auto& f : obj.faces) {
    auto v0 = obj.vertices[f.v0];
    auto v1 = obj.vertices[f.v1];
    auto v2 = obj.vertices[f.v2];
    auto s0 = world_to_screen(v0, image.width, image.height);
    auto s1 = world_to_screen(v1, image.width, image.height);
    auto s2 = world_to_screen(v2, image.width, image.height);
    auto n = cross(v2 - v0, v1 - v0).normalize();
    auto I = n * SPOTLIGHT;
    if (I <= 0.0f) {
      continue;
    }
    auto r = uint8_t(I * 255.0f);
    auto g = uint8_t(I * 255.0f);
    auto b = uint8_t(I * 255.0f);
    draw_triangle(image, s0, s1, s2, {b, g, r});
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
