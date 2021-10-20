#ifndef OBJ_H
#define OBJ_H
#include <stdint.h>

typedef struct ObjVertex ObjVertex;
struct ObjVertex {
    float x, y, z;
};

typedef struct ObjFace ObjFace;
struct ObjFace {
    uint16_t v0, v1, v2;
};

typedef struct Obj Obj;
struct Obj {
    ObjVertex* vertices;
    ObjFace* faces;
    int vertex_count;
    int vertex_capacity;
    int face_count;
    int face_capacity;
};

#endif
