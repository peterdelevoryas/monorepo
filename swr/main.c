#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#define panic(...) \
    ({ \
        printf("%s:%d: ", __func__, __LINE__); \
        printf(__VA_ARGS__); \
        if (errno) \
        { \
            printf(": %s", strerror(errno)); \
        } \
        printf("\n"); \
    })

#ifdef DEBUG
#define debug_assert(cond) \
    do \
    { \
        if (cond) \
            break; \
        panic("debug_assert(%s)", #cond); \
    } while (0)
#else
#define debug_assert(cond) cond
#endif

#define array_count(array) (sizeof(array) / sizeof(array[0]))

typedef struct arena arena_t;
struct arena
{
    void* data;
    int size;
    int used;
};

typedef int arena_index_t;

static
arena_index_t arena_allocate(arena_t* arena, int size)
{
    if (arena->used + size > arena->size)
    {
    }
    arena_index_t index = arena->used;
    arena->used += size;
    return index;
}

arena_t global_arena;

typedef struct obj obj_t;
struct obj
{
    arena_index_t vertices_start;
    arena_index_t faces_start;
    int vertex_count;
    int face_count;
};

static
obj_t load_obj(const char* path)
{
    obj_t obj = {};

    FILE* f = fopen(path, "r");
    debug_assert(f);

    char line[64];
    while (fgets(line, sizeof(line), f))
    {
        float x, y, z;
        uint16_t v0, v1, v2, discard;
        int n;
        switch (line[0])
        {
            case 'v':
                n = sscanf(line, "v %f %f %f", &x, &y, &z);
                if (n != 3)
                    break;
                vertices[0] = x;
                vertices[1] = y;
                vertices[2] = z;
                vertices += 3;
                break;
            case 'f':
                n = sscanf(line, "f %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
                           &v0, &discard, &discard,
                           &v1, &discard, &discard,
                           &v2, &discard, &discard);
                if (n != 9)
                    break;
                faces[0] = v0;
                faces[1] = v1;
                faces[2] = v2;
                faces += 3;
                break;
        }
    }

    debug_assert(!ferror(f));
    debug_assert(feof(f));
    fclose(f);

    return obj;
}

int main(int argc, char** argv)
{
    obj_t obj = load_obj("head.obj");
}
