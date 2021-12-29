#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <math.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define MIN(x, y) (x <= y ? x : y)
#define MAX(x, y) (x >= y ? x : y)

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

static u8 image[1000][1000][3];
static f32 vertices[2000][3];
static u16 faces[2500][3];

#define SPOTLIGHT_X 0.0f
#define SPOTLIGHT_Y 0.0f
#define SPOTLIGHT_Z -1.0f

static void draw_triangle(int w, int h, u8 image[h][w][3],
                          f32 a_x, f32 a_y, f32 a_z,
                          f32 b_x, f32 b_y, f32 b_z,
                          f32 c_x, f32 c_y, f32 c_z,
                          u8 r, u8 g, u8 b)
{
    // i32 d_x = (a_x + 1.0f) * (f32)(w / 2);
    // i32 d_y = (a_y + 1.0f) * (f32)(h / 2);
    // i32 d_z = a_z;

    // i32 e_x = (b_x + 1.0f) * (f32)(w / 2);
    // i32 e_y = (b_y + 1.0f) * (f32)(h / 2);
    // i32 e_z = b_z;

    // i32 f_x = (c_x + 1.0f) * (f32)(w / 2);
    // i32 f_y = (c_y + 1.0f) * (f32)(h / 2);
    // i32 f_z = c_z;

    // i32 min_x = MIN(MIN(d_x, e_x), f_x);
    // i32 min_y = MIN(MIN(d_y, e_y), f_y);
    // i32 max_x = MAX(MAX(d_x, e_x), f_x);
    // i32 max_y = MAX(MAX(d_y, e_y), f_y);
    // for (i32 x = min_x; x <= max_x; x++) {
    //     if (x >= w) {
    //         continue;
    //     }
    //     for (i32 y = min_y; y <= max_y; y++) {
    //         if (y >= h) {
    //             continue;
    //         }
    //         // f32 p_x = (f32)(f_x - d_x);
    //         // f32 p_y = (f32)(e_x - d_x);
    //         // f32 p_z = (f32)(d_x - x);

    //         // f32 q_x = (f32)(f_y - d_y);
    //         // f32 q_y = (f32)(e_y - d_y);
    //         // f32 q_z = (f32)(d_y - y);
    //     }
    // }
}

static void draw_model(int n, f32 vertices[n][3],
                       int m, u16 faces[m][3],
                       int w, int h, u8 image[h][w][3])
{
    for (int i = 0; i < m; i++) {
        u16 a = faces[i][0];
        u16 b = faces[i][1];
        u16 c = faces[i][2];

        f32 a_x = vertices[a][0];
        f32 a_y = vertices[a][1];
        f32 a_z = vertices[a][2];

        f32 b_x = vertices[b][0];
        f32 b_y = vertices[b][1];
        f32 b_z = vertices[b][2];

        f32 c_x = vertices[c][0];
        f32 c_y = vertices[c][1];
        f32 c_z = vertices[c][2];

        f32 ab_x = b_x - a_x;
        f32 ab_y = b_y - a_y;
        f32 ab_z = b_z - a_z;

        f32 ac_x = c_x - a_x;
        f32 ac_y = c_y - a_y;
        f32 ac_z = c_z - a_z;

        // cross product:
        // x = a_y * b_z - a_z * b_y;
        // y = a_z * b_x - a_x * b_z;
        // z = a_x * b_y - a_y * b_x;
        f32 s_x = ac_y * ab_z - ac_z * ab_y;
        f32 s_y = ac_z * ab_x - ac_x * ab_z;
        f32 s_z = ac_x * ab_y - ac_y * ab_x;
        f32 s_magnitude = sqrtf(s_x * s_x +
                                s_y * s_y +
                                s_z * s_z);
        f32 s_inverse_magnitude = 1 / s_magnitude;

        f32 n_x = s_x * s_inverse_magnitude;
        f32 n_y = s_y * s_inverse_magnitude;
        f32 n_z = s_z * s_inverse_magnitude;

        f32 I = n_x * SPOTLIGHT_X +
                n_y * SPOTLIGHT_Y +
                n_z * SPOTLIGHT_Z;
        if (I <= 0.0f) {
            continue;
        }

        u8 p = I * 255.0f;
        draw_triangle(w, h, image, a_x, a_y, a_z, b_x, b_y, b_z, c_x, c_y, c_z, p, p, p);
    }
}

int main(int argc, char **argv)
{
    int fd = open("head.obj", O_RDONLY);
    assert(fd != -1);
    struct stat st;
    assert(fstat(fd, &st) == 0);
    size_t n = st.st_size;
    const char *s = mmap(NULL, n, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(s != MAP_FAILED);

    int nr_vertices = 0;
    int nr_faces = 0;
    for (int i = 0; i < n;) {
        f32 x, y, z;
        u16 a, b, c, d;

        switch (s[i]) {
        case 'v':
            switch (s[i + 1]) {
            case ' ':
                if (sscanf(&s[i], "v %f %f %f", &x, &y, &z) == 3) {
                    assert(nr_vertices < ARRAY_SIZE(vertices));
                    vertices[nr_vertices][0] = x;
                    vertices[nr_vertices][1] = y;
                    vertices[nr_vertices][2] = z;
                    nr_vertices++;
                }
                break;
            }
            break;
        case 'f':
            if (sscanf(&s[i], "f %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
                       &a, &d, &d, &b, &d, &d, &c, &d, &d) == 9) {
                assert(nr_faces < ARRAY_SIZE(faces));
                faces[nr_faces][0] = a - 1;
                faces[nr_faces][1] = b - 1;
                faces[nr_faces][2] = c - 1;
                nr_faces++;
            }
            break;
        }
        // Find newline (or the end), then skip it (or go past the end a little
        // further).
        for (; s[i] != '\n' && i < n; i++);
        i++;
    }

    draw_model(nr_vertices, vertices, nr_faces, faces, 1000, 1000, image);

    munmap((void *)s, n);
    close(fd);
}
