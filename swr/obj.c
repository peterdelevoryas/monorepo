#include <stdio.h>
#include <assert.h>
#include "obj.h"

Obj obj_load(String path) {
    let self = (Obj){};
    let file = fopen(path, "r");
    assert(file);

    char line[64];
    while (fgets(line, sizeof(line), file)) {
        switch (line[0]) {
            case 'v': {
                f32 x, y, z;
                let n = sscanf(line, "v %f %f %f", &x, &y, &z);
                if (n != 3) {
                    continue;
                }
                vector_append(self.vertices, f32x3(x, y, z));
                break;
            }
            case 'f': {
                u16 x, y, z, _;
                let n = sscanf(line, "f %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
                               &x, &_, &_, &y, &_, &_, &z, &_, &_);
                if (n != 9) {
                    continue;
                }
                x--;
                y--;
                z--;
                vector_append(self.faces, u16x3(x, y, z));
                break;
            }
        }
    }

    fclose(file);

    return self;
}

void obj_free(Obj* self) {
    vector_free(self->vertices);
    vector_free(self->faces);
}
