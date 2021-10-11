#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>

#ifdef __linux__
#define IMAGE_VIEWER "feh"
#endif

#ifdef __APPLE__
#define IMAGE_VIEWER "open"
#endif

#ifndef IMAGE_VIEWER
#error "Unsupported platform"
#endif

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

#define min(x, y)       \
  ({                    \
    __auto_type a = x;  \
    __auto_type b = y;  \
    a <= b ? a : b;     \
  })

#define max(x, y)       \
  ({                    \
    __auto_type a = x;  \
    __auto_type b = y;  \
    a >= b ? a : b;     \
  })

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

void image_free(struct image *self) {
  free(self->pixels);
}

void image_write_tga_file(struct image *self, FILE *file) {
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

  fwrite(header, sizeof(header), 1, file);
  fwrite(self->pixels, sizeof(self->pixels[0]), self->width * self->height, file);
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

void image_draw_triangle(struct image *self, int x0, int y0, int x1, int y1, int x2, int y2,
                         struct image_pixel pixel) {
  int min_x = min(min(x0, x1), x2);
  int min_y = min(min(y0, y1), y2);
  int max_x = max(max(x0, x1), x2);
  int max_y = max(max(y0, y1), y2);
  for (int x = min_x; x <= max_x; x++) {
    for (int y = min_y; y <= max_y; y++) {
      float a1 = x2 - x0;
      float a2 = x1 - x0;
      float a3 = x0 - x;
      float b1 = y2 - y0;
      float b2 = y1 - y0;
      float b3 = y0 - y;
      float s1 = a2 * b3 - a3 * b2;
      float s2 = a3 * b1 - a1 * b3;
      float s3 = a1 * b2 - a2 * b1;
      if (fabsf(s3) < 1.0f)
        continue;
      float t1 = 1.0f - (s1 + s2) / s3;
      float t2 = s2 / s3;
      float t3 = s1 / s3;
      if (t1 < 0.0f || t2 < 0.0f || t3 < 0.0f)
        continue;
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

void obj_free(struct obj *self) {
  free(self->vertices);
  free(self->faces);
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
    int r = rand() % 255;
    int g = rand() % 255;
    int b = rand() % 255;
    struct image_pixel pixel = {b, g, r};
    image_draw_triangle(image, x0, y0, x1, y1, x2, y2, pixel);
  }
}

int main() {
  struct obj obj = {};
  struct image image = {};
  FILE *out;

  obj_init(&obj, "head.obj");
  image_init(&image, 1000, 1000);

  image_draw_obj(&image, &obj);
  obj_free(&obj);

  out = fopen("out.tga", "w");
  image_write_tga_file(&image, out);
  fclose(out);

  image_free(&image);

  execlp(IMAGE_VIEWER, "open", "out.tga", NULL);
  perror("execlp");
}
