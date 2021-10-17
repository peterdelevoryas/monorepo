#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

typedef struct Obj Obj;
struct Obj {
    float (*vertices)[3];
    float (*triangles)[3];
    int vertices_capacity;
    int vertices_count;
    int triangles_capacity;
    int triangles_count;
};

static void obj_append_vertex(Obj* self, float v[3]) {
}

static void obj_parse_line(Obj* self, const char* path,
                           int line_no, const char* line) {
    float v[3];
    uint16_t f[3];
    uint16_t _;

    switch (line[0]) {
    case 'v':
        if (line[1] != ' ') {
            break;
        }
        if (sscanf(line, "v %f %f %f", &v[0], &v[1], &v[2]) != 3) {
            printf("%s:%d: unable to parse vertex\n", path, line_no);
            printf("\n    %s\n", line);
            break;
        }
        obj_append_vertex(self, v);
        break;
    case 'f':
        if (sscanf(line, "f %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
                   &f[0], &_, &_, &f[1], &_, &_, &f[2], &_, &_) != 9) {
            printf("%s:%d: unable to parse face\n", path, line_no);
            printf("\n    %s\n", line);
            break;
        }
        break;
    default:
        break;
    }
}

static void obj_init(Obj* self, const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) {
        const char* error = strerror(errno);
        printf("obj_init: unable to open '%s': %s\n", path, error);
    }
    char line[64];
    int line_no = 1;
    for (; fgets(line, sizeof(line), f); line_no++) {
        obj_parse_line(self, path, line_no, line);
    }
    if (ferror(f)) {
        const char* error = strerror(errno);
        printf("%s:%d: %s\n", path, line_no, error);
    }
    fclose(f);
}

void obj_free(Obj* self) {
    free(self->vertices);
    free(self->triangles);
}

int main(int argc, char** argv) {
    Obj obj = {};

    obj_init(&obj, "head.obj");
    obj_free(&obj);
}
