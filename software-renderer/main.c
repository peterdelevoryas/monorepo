#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define array(T) struct { T *data; int len; int cap }

struct vertex {
  float x, y, z;
};

struct triangle {
  uint16_t a, b, c;
};

struct global_data {
  array(struct vertex) vertices;
  array(struct triangle) triangles;
};

struct global_data gd;

void *reserve(void *p, int capacity, int count, int element_size) {
  if (count <= capacity)
    return p;
  if (capacity == 0)
    capacity = 8;
  while (count > capacity)
    capacity *= 2;
  return realloc(p, capacity * element_size);
}

#define OBJ_TAG(a, b) ((uint16_t)a | ((uint16_t)b) << 8)
#define OBJ_VERTEX    OBJ_TAG('v', ' ')
#define OBJ_FACE      OBJ_TAG('f', ' ')

static void read_obj(const char *path) {
  printf("read_obj '%s'\n", path);

  FILE *f = fopen(path, "r");
  if (!f) {
    perror("fopen");
    return;
  }

  char line[512];
  float x, y, z;
  uint16_t v0, v1, v2, unused;
  while (fgets(line, sizeof(line), f)) {
    if (line[0] == '\n')
      continue;
    switch (*(uint16_t*)&line[0]) {
      case OBJ_VERTEX:
        sscanf(&line[2], "%f %f %f", &x, &y, &z);
        gd.vertices = reserve(gd.vertices, gd.vertex_capacity,
                              gd.vertex_count + 3, sizeof(float));
        break;
      case OBJ_FACE:
        sscanf(&line[2], "%hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
               &v0, &unused, &unused,
               &v1, &unused, &unused,
               &v2, &unused, &unused);
        gd.triangles = reserve(gd.triangles, gd.triangle_capacity,
                               gd.triangle_count + 3
        break;
    }
  }

  fclose(f);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("usage: %s file...\n", argv[0]);
    return 1;
  }
  for (int i = 1; i < argc; i++) {
    read_obj(argv[i]);
  }
}
