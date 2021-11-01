#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define Vector(T)   \
    struct {        \
        T* data;    \
        u32 len;    \
        u32 cap;    \
    }

#define Vector3(T)  \
    struct {        \
        T x, y, z;  \
    }

#define f32x3(x, y, z)  ((f32x3){(x), (y), (z)})
#define u16x3(x, y, z)  ((u16x3){(x), (y), (z)})

typedef int8_t          i8;
typedef int16_t         i16;
typedef int32_t         i32;
typedef int64_t         i64;
typedef uint8_t         u8;
typedef uint16_t        u16;
typedef uint32_t        u32;
typedef uint64_t        u64;
typedef float           f32;
typedef double          f64;
typedef const char*     string;
typedef Vector3(f32)    f32x3;
typedef Vector3(u16)    u16x3;

struct Obj {
    Vector(f32x3) vertices;
    Vector(u16x3) faces;
};

static struct Obj load_obj(string path) {
    struct Obj obj;
    char line[64];
    FILE* f;
    int r;
    f32 x, y, z;
    u16 a, b, c, d;

    f = fopen(path, "r");
    assert(f);

    memset(&obj, 0, sizeof(obj));

    while (fgets(line, sizeof(line), f)) {
        switch (line[0]) {
            case 'v':
                r = sscanf(line, "v %f %f %f", &x, &y, &z);
                if (r != 3) {
                    continue;
                }
                printf("vertex %f %f %f\n", x, y, z);
                break;
            case 'f':
                r = sscanf(line, "f %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
                           &a, &d, &d, &b, &d, &d, &c, &d, &d);
                if (r != 9) {
                    continue;
                }
                a--;
                b--;
                c--;
                printf("face %hu %hu %hu\n", a, b, c);
                break;
        }
    }

    fclose(f);

    return obj;
}

int main(int argc, string argv[argc]) {
    struct Obj obj;

    obj = load_obj("head.obj");
}
