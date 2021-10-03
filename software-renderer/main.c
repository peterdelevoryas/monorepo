#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define swap(x, y)        \
  do {                    \
    __auto_type tmp = x;  \
    x = y;                \
    y = tmp;              \
  } while (0)

#define panic(...)        \
  do {                    \
    printf(__VA_ARGS__);  \
    abort();              \
  } while (0)

#define append(data, count, capacity, value)            \
  do {                                                  \
    if (count >= capacity) {                            \
      capacity = capacity ? capacity * 3 / 2 : 8;       \
      data = realloc(data, sizeof(data[0]) * capacity); \
      if (!data) {                                      \
        panic("realloc: count %d capacity %d: %s\n",    \
              count, capacity, strerror(errno));        \
      }                                                 \
    }                                                   \
    data[count++] = value;                              \
  } while (0)

struct image_pixel {
  uint8_t b;
  uint8_t g;
  uint8_t r;
};

struct image {
  struct image_pixel *pixels;
  int width;
  int height;
};

void image_init(struct image *self, int width, int height) {
  self->pixels = calloc(sizeof(self->pixels[0]), width * height);
  if (!self->pixels) {
    panic("Out-of-memory: image %dx%d: %s\n", width, height, strerror(errno));
  }
  self->width = width;
  self->height = height;
}

void image_write_tga_file(struct image *self, const char *path) {
  uint8_t header[18] = {};
  // Uncompressed true color image.
  header[2] = 2;
  // Width, little-endian byte-order.
  header[12] = self->width & 0xFF;
  header[13] = (self->width & 0xFF00) >> 8;
  // Height, little-endian byte-order.
  header[14] = self->height & 0xFF;
  header[15] = (self->height & 0xFF00) >> 8;
  // Bits-per-pixel.
  header[16] = sizeof(self->pixels[0]) * 8;

  FILE *file = fopen(path, "w");
  fwrite(header, sizeof(header), 1, file);
  fwrite(self->pixels, sizeof(self->pixels[0]), self->width * self->height, file);
  fclose(file);
}

void image_set_pixel(struct image *self, int x, int y, struct image_pixel value) {
  int i = y * self->width + x;
  if (i < 0 || i >= self->width * self->height) {
    return;
  }
  self->pixels[i] = value;
}

void image_draw_line(struct image *self, int x0, int y0, int x1, int y1, uint32_t rgb) {
  uint8_t r = (rgb & 0xFF0000) >> 16;
  uint8_t g = (rgb & 0xFF00) >> 8;
  uint8_t b = rgb & 0xFF;
  struct image_pixel pixel = {b, g, r};
  int steep = 0;
  if (abs(y1 - y0) > abs(x1 - x0)) {
    swap(x0, y0);
    swap(x1, y1);
    steep = 1;
  }
  if (x1 < x0) {
    swap(x0, x1);
    swap(y0, y1);
  }
  int dx = x1 - x0;
  int dy = y1 - y0;
  int de = abs(dy);
  int e = 0;
  int y = y0;
  int y_step = y0 < y1 ? 1 : -1;
  for (int x = x0; x <= x1; x++) {
    if (steep) {
      image_set_pixel(self, x, y, pixel);
    } else {
      image_set_pixel(self, y, x, pixel);
    }
    e += de;
    if (2 * e >= dx) {
      y += y_step;
      e -= dx;
    }
  }
}

void image_draw_triangle(struct image *self, int x0, int y0, int x1, int y1, int x2, int y2) {
  struct image_pixel pixel = {0xFF, 0x00, 0x00};

  printf("image_draw_triangle (%d, %d) (%d, %d) (%d, %d)\n", x0, y0, x1, y1, x2, y2);

  if (y0 == y1 && y0 == y2) {
    return;
  }
  if (y0 > y1) {
    swap(x0, x1);
    swap(y0, y1);
  }
  if (y0 > y2) {
    swap(x0, x2);
    swap(y0, y2);
  }
  if (y1 > y2) {
    swap(x1, x2);
    swap(y1, y2);
  }
  int total_height = y2 - y0;
  int lower_half_height = y1 - y0 + 1;
  int upper_half_height = y2 - y1 + 1;
  // Lower half.
  for (int y = y0; y <= y1; y++) {
    float a = (float)(y - y0) / total_height;
    float b = (float)(y - y0) / lower_half_height;
    int ax = (x2 - x0) * a + x0;
    int ay = (y2 - y0) * a + y0;
    int bx = (x1 - x0) * b + x0;
    int by = (y1 - y0) * b + y0;
    if (ax > bx) {
      swap(ax, bx);
      swap(ay, by);
    }
    for (int x = ax; x <= bx; x++) {
      image_set_pixel(self, x, y, pixel);
    }
  }
  // Upper half.
  for (int y = y1; y <= y2; y++) {
    float a = (float)(y - y0) / total_height;
    float b = (float)(y - y1) / upper_half_height;
    int ax = (x2 - x0) * a + x0;
    int ay = (y2 - y0) * a + y0;
    int bx = (x2 - x1) * b + x1;
    int by = (y2 - y1) * b + y1;
    if (ax > bx) {
      swap(ax, bx);
      swap(ay, by);
    }
    for (int x = ax; x <= bx; x++) {
      image_set_pixel(self, x, y, pixel);
    }
  }
}

struct obj_vertex {
  float x;
  float y;
  float z;
};

struct obj_face {
  uint16_t vertex_index[4];
};

struct obj {
  struct obj_vertex *vertices;
  struct obj_face *faces;
  int vertex_count;
  int face_count;
};

void obj_init(struct obj *self, const char *path) {
  FILE *f = fopen(path, "r");
  int vertex_capacity = 0;
  int face_capacity = 0;
  struct obj_vertex vertex;
  struct obj_face face;
  uint16_t _unused;
  int c;

  do {
    if (fscanf(f, "v %f %f %f\n", &vertex.x, &vertex.y, &vertex.z) == 3) {
      append(self->vertices, self->vertex_count, vertex_capacity, vertex);
      continue;
    }
    if (fscanf(f, "f %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu\n",
               &face.vertex_index[0], &_unused, &_unused,
               &face.vertex_index[1], &_unused, &_unused,
               &face.vertex_index[2], &_unused, &_unused) == 9) {
      face.vertex_index[0]--;
      face.vertex_index[1]--;
      face.vertex_index[2]--;
      append(self->faces, self->face_count, face_capacity, face);
      continue;
    }
    for (c = getc(f); c != '\n' && c != EOF; c = getc(f));
  } while (c != EOF);
}

void image_draw_obj(struct image *image, const struct obj *obj) {
  for (int i = 0; i < obj->face_count; i++) {
    const struct obj_face *f = &obj->faces[i];
    const struct obj_vertex *v0 = &obj->vertices[f->vertex_index[0]];
    const struct obj_vertex *v1 = &obj->vertices[f->vertex_index[1]];
    const struct obj_vertex *v2 = &obj->vertices[f->vertex_index[2]];
    int x0 = (v0->x + 1.0f) * (image->width / 2);
    int x1 = (v1->x + 1.0f) * (image->width / 2);
    int x2 = (v2->x + 1.0f) * (image->width / 2);
    int y0 = (v0->y + 1.0f) * (image->height / 2);
    int y1 = (v1->y + 1.0f) * (image->height / 2);
    int y2 = (v2->y + 1.0f) * (image->height / 2);
    image_draw_triangle(image, x0, y0, x1, y1, x2, y2);
  }
}

int main() {
  struct obj obj = {};
  struct image image = {};

  obj_init(&obj, "head.obj");
  
  image_init(&image, 1000, 1000);
  image_draw_obj(&image, &obj);
  image_write_tga_file(&image, "out.tga");
}
