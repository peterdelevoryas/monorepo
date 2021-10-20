#ifndef OBJ_H
#define OBJ_H
#include <cstdint>
#include "vec.hh"

namespace obj {

struct Vertex {
  float x, y, z;
};

struct Face {
  uint16_t v0, v1, v2;
};

struct Obj {
  Vec<Vertex> vertices;
  Vec<Face> faces;

  static Obj from_file(const char* path);
  void free();
};

};

#endif
