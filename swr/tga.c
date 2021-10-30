#include <stdlib.h>
#include <assert.h>
#include "tga.h"

Tga tga_init(u16 width, u16 height) {
    Tga self = {};

    self.image = malloc(width * height * 3);
    assert(self.image);

    self.width = width;
    self.height = height;

    return self;
}

void tga_free(Tga* self) {
    free(self->image);
}
