#pragma once
#include "common.h"
#include "vector.h"

typedef struct Obj Obj;

struct Obj {
    Vector(f32x3) vertices;
    Vector(u16x3) faces;
};

Obj obj_load(String path);
void obj_free(Obj* self);
