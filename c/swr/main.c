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

void* reserve(void* array, 

void obj_init(Obj* self, const char *path)
{
  FILE* f = fopen(path, "r");
  if (!f) {
    const char* error = strerror(errno);
    printf("obj_init: unable to open '%s': %s\n", path, error);
  }
  char line[64];
  for (int line_no = 1;; line_no++) {
    if (!fgets(line, sizeof(line), f)) {
      const char* error = strerror(errno);
      printf("obj_init: fgets line %d: %s\n", line_no, error);
      break;
    }
    float v[3];
    uint16_t f[3];
    switch (line[0]) {
      case 'v':
        if (line[1] == 't') {
          break;
        }
        if (sscanf(line, "v %f %f %f", &v[0], &v[1], &v[2]) != 3) {
          printf("%s:%d unable to parse vertex: '%s'\n", path, line_no, line);
          break;
        }
        obj.vertices.append(v);
        break;
      case 'f':
        break;
      default:
        break;
    }
  }

  fclose(f);
}

void obj_free(Obj* self)
{
  free(self->vertices);
  free(self->triangles);
}

int main(int argc, char** argv) {
  Obj head_obj = {};

  obj_init(&head_obj, "head.obj");
  obj_free(&head_obj);
}
