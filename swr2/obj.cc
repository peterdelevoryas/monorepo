#include <cstdio>
#include <cassert>
#include "obj.hh"

using namespace obj;

Obj Obj::from_file(const char* path)
{
  Vec<Vertex> vertices;
  Vec<Face> faces;

  vertices.reserve(16);
  faces.reserve(16);

  auto file = fopen(path, "r");
  assert(file);

  char line[64];
  while (fgets(line, sizeof(line), file)) {
    if (vertices.len >= vertices.cap) {
      vertices.reserve(vertices.cap * 3 / 2);
    }
    if (faces.len >= faces.cap) {
      faces.reserve(faces.cap * 3 / 2);
    }
    switch (line[0]) {
      case 'v': {
        Vertex v;
        auto n = sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
        if (n != 3) {
          continue;
        }
        vertices.append(v);
        break;
      }
      case 'f': {
        Face f;
        uint16_t discard;
        auto n = sscanf(line, "f %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
                        &f.v0, &discard, &discard, &f.v1, &discard, &discard,
                        &f.v2, &discard, &discard);
        if (n != 9) {
          continue;
        }
        f.v0--;
        f.v1--;
        f.v2--;
        faces.append(f);
        break;
      }
    }
  }

  fclose(file);

  return {vertices, faces};
}

void Obj::free()
{
  vertices.free();
  faces.free();
}
