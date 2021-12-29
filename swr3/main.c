#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

//static u8 image[1000][1000][3];
static f32 vertices[2000][3];
static u16 faces[2500][3];

int main(int argc, char **argv)
{
    int fd, i;
    struct stat st;
    size_t n;
    const char *s;
    f32 x, y, z;
    u16 a, b, c, d;
    int nr_vertices;
    int nr_faces;

    fd = open("head.obj", O_RDONLY);
    assert(fd != -1);
    assert(fstat(fd, &st) == 0);
    n = st.st_size;
    s = mmap(NULL, n, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(s != MAP_FAILED);

    nr_vertices = 0;
    nr_faces = 0;
    for (i = 0; i < n;) {
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
                faces[nr_faces][0] = a;
                faces[nr_faces][1] = b;
                faces[nr_faces][2] = c;
                nr_faces++;
            }
            break;
        }
        // Find newline (or the end), then skip it (or go past the end a little
        // further).
        for (; s[i] != '\n' && i < n; i++);
        i++;
    }

    munmap((void *)s, n);
    close(fd);
}
