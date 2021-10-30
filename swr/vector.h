#pragma once
#include <stdlib.h>
#include "common.h"

#define Vector(T) struct { T* data; u32 len; u32 cap; }

#define vector_reserve(v, n)                \
    do {                                    \
        if (v.cap >= n) {                   \
            break;                          \
        }                                   \
        if (v.cap == 0) {                   \
            v.cap = 8;                      \
        }                                   \
        while (v.cap < n) {                 \
            v.cap = v.cap * 3 / 2;          \
        }                                   \
        let size = v.cap * sizeof(*v.data); \
        v.data = realloc(v.data, size);     \
    } while (0)

#define vector_append(v, x)                 \
    do {                                    \
        vector_reserve(v, v.len + 1);       \
        v.data[v.len] = x;                  \
        v.len += 1;                         \
    } while (0)

#define vector_free(v)                      \
    do {                                    \
        free(v.data);                       \
    } while (0)
