#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdalign.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define Panic(...) \
    do { \
        int e; \
        e = errno; \
        printf("%s:%s:%d: ", __FILE__, __func__, __LINE__); \
        printf(__VA_ARGS__); \
        if (e) { \
            printf(": %s", strerror(e)); \
        } \
        printf("\n"); \
        exit(1); \
    } while (0)

#define Reserve(p, n, i, a) \
    do { \
        size_t m, l, z; \
        if (i < n) { \
            break; \
        } \
        m = n ? n + n / 2 : 16; \
        z = sizeof(*p); \
        l = alignof(typeof(*p)); \
        p = Arena_ResizeAllocation(&a, p, n * z, m * z, l); \
        n = m; \
    } while (0)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

struct Arena {
    void *p;
    size_t n, i;
};

static void *AlignAddress(void *p, size_t n)
{
    intptr_t i;

    i = (intptr_t)p;
    i += -i & (intptr_t)(n - 1);
    return (void *)i;
}

static void *Arena_AllocateAligned(struct Arena *a, size_t n, size_t l)
{
    void *p;

    p = a->p + a->i;
    p = AlignAddress(p, l);
    a->i = p + n - a->p;

    if (a->i > a->n) {
        Panic("Exceeded arena capacity: %lu > %lu", a->i, a->n);
    }
    return p;
}

static void *Arena_ResizeAllocation(struct Arena *a, void *p, size_t n, size_t m, size_t l)
{
    void *r;

    assert(n < m);

    if (a->p + a->i == p + n) {
        if (p + m > a->p + a->n) {
            Panic("Exceed arena capacity: %lu > %lu", p + m - a->p, a->n);
        }
        a->i += m - n;
        return p;
    }

    r = Arena_AllocateAligned(a, m, l);
    memcpy(r, p, n);
    return r;
}

static u8 A_[1024 * 1024 * 1024];
static u8 B_[1024 * 1024];
static struct Arena A = {&A_, sizeof(A_), 0};
static struct Arena B = {&B_, sizeof(B_), 0};

struct MemoryMappedFile {
    void *addr;
    u64 size;
};

static struct MemoryMappedFile Memory_MapFile(const char *path)
{
    struct MemoryMappedFile f;
    int fd;
    struct stat st;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        Panic("unable to open '%s'", path);
    }

    if (fstat(fd, &st) != 0) {
        Panic("unable to fstat '%s'", path);
    }
    f.size = st.st_size;

    f.addr = mmap(NULL, f.size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (f.addr == MAP_FAILED) {
        Panic("unable to mmap '%s'", path);
    }
    close(fd);

    return f;
}

typedef f32 f32x3[3];
typedef u16 u16x3[3];

struct Obj {
    f32x3 *v;
    u16x3 *f;
    int vn, vi;
    int fn, fi;
};

struct Obj Obj_Load(const char *path)
{
    struct Obj o;
    struct MemoryMappedFile f;
    const char *s;
    f32 x, y, z;
    u16 a, b, c, d;
    size_t i;

    memset(&o, 0, sizeof(o));
    f = Memory_MapFile(path);
    s = f.addr;
    for (i = 0; i < f.size; i++) {
        switch (s[i]) {
        case 'v':
            switch (s[i + 1]) {
            case ' ':
                if (sscanf(&s[i], "v %f %f %f", &x, &y, &z) == 3) {
                    Reserve(o.v, o.vn, o.vi + 1, B);
                    printf("o.v = %p\n", o.v);
                    o.v[o.vi][0] = x;
                    o.v[o.vi][1] = y;
                    o.v[o.vi][2] = z;
                    o.vi++;
                }
                break;
            }
            break;
        case 'f':
            if (sscanf(&s[i], "f %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
                       &a, &d, &d, &b, &d, &d, &c, &d, &d) == 9) {
                Reserve(o.f, o.fn, o.fi + 1, B);
                o.f[o.fi][0] = a;
                o.f[o.fi][1] = b;
                o.f[o.fi][2] = c;
                o.fi++;
            }
            break;
        }
    }
    munmap(f.addr, f.size);

    return o;
}

int main(int argc, char **argv)
{
    uint8_t *im;
    float *z;
    int w, h, i;
    struct Obj obj;

    w = 1000;
    h = 1000;
    im = Arena_AllocateAligned(&A, w * h * 3, 16);
    z = Arena_AllocateAligned(&A, w * h * sizeof(float), 16);

    obj = Obj_Load("head.obj");

    for (i = 0; i < obj.vi; i++) {
        printf("v %f %f %f\n", obj.v[i][0], obj.v[i][1], obj.v[i][2]);
    }
    for (i = 0; i < obj.fi; i++) {
        printf("f %hu %hu %hu\n", obj.f[i][0], obj.f[i][1], obj.f[i][2]);
    }
}
