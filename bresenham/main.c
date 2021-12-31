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

struct f32x3 { f32 x, y, z; };
struct u16x3 { u16 x, y, z; };

struct Obj {
    struct f32x3 *v;
    struct u16x3 *f;
    int v_n, v_i;
    int f_n, f_i;
};

struct Obj Obj_Load(const char *path)
{
    struct Obj obj;
    struct MemoryMappedFile f;
    const char *s;
    struct f32x3 v;
    struct u16x3 u;
    u16 d;
    size_t i;

    memset(&obj, 0, sizeof(obj));
    f = Memory_MapFile(path);
    s = f.addr;
    for (i = 0; i < f.size; i++) {
        switch (s[i]) {
        case 'v':
            switch (s[i + 1]) {
            case ' ':
                if (sscanf(&s[i], "v %f %f %f", &v.x, &v.y, &v.z) == 3) {
                    Reserve(obj.v, obj.v_n, obj.v_i + 1, B);
                    obj.v[obj.v_i++] = v;
                }
                break;
            }
            break;
        case 'f':
            if (sscanf(&s[i], "f %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
                       &u.x, &d, &d, &u.y, &d, &d, &u.z, &d, &d) == 9) {
                Reserve(obj.f, obj.f_n, obj.f_i + 1, B);
                obj.f[obj.f_i++] = u;
            }
            break;
        }
    }
    munmap(f.addr, f.size);

    return obj;
}

static void Image_SaveAsTgaFile(const u8 *im, u16 w, u16 h, const char *out_path)
{
    FILE *f;
    u8 header[18];

    memset(&header, 0, sizeof(header));
    header[2] = 2;
    header[12] = (w & 0x00FF) >> 0;
    header[13] = (w & 0xFF00) >> 8;
    header[14] = (h & 0x00FF) >> 0;
    header[15] = (h & 0xFF00) >> 8;
    header[16] = 24;

    f = fopen(out_path, "w");
    assert(f);
    fwrite(header, sizeof(header), 1, f);
    fwrite(im, 
    fclose(f);
}

int main(int argc, char **argv)
{
    u8 *im;
    f32 *z;
    u16 w, h;
    struct Image im;
    struct Obj obj;

    w = 1000;
    h = 1000;
    im = Arena_AllocateAligned(&A, w * h * 3, 16);
    z = Arena_AllocateAligned(&A, w * h * sizeof(*z), 16);

    obj = Obj_Load("head.obj");

    Image_SaveAsTgaFile(im, w, h, "out.tga");
}
