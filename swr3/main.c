#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

int main(int argc, char **argv)
{
    int fd, i;
    struct stat st;
    size_t n;
    const char *s;
    f32 x, y, z;

    fd = open("head.obj", O_RDONLY);
    assert(fd != -1);
    assert(fstat(fd, &st) == 0);
    n = st.st_size;
    s = mmap(NULL, n, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(s != MAP_FAILED);

    for (i = 0; i < n;) {
        switch (s[i]) {
            case 'v':
                if (sscanf(&s[i], "v %f %f %f", &x, &y, &z) == 3) {
                    printf("x=%f y=%f z=%f\n", x, y, z);
                    break;
                }
                break;
            case 'f':
                break;
        }
        for (; s[i] != '\n' && i < n; i++);
        i++;
    }

    munmap((void *)s, n);
    close(fd);
}
