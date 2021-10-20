#ifndef OBJ_HH
#define OBJ_HH
#include <cstdint>
#include "vec.hh"

struct float3;

namespace obj {

struct Face {
  uint16_t v0, v1, v2;
};

struct Obj {
  Vec<float3> vertices;
  Vec<Face> faces;

  static Obj from_file(const char* path);
  void free();
};

};

#endif
