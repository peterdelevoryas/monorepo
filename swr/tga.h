#pragma once
#include "common.h"

typedef struct Tga Tga;

struct Tga {
    u8* image;
    u16 width;
    u16 height;
};

Tga tga_init(u16 width, u16 height);
void tga_free(Tga* self);
