#pragma once
#include <stdint.h>

#define let         __auto_type
#define loop        for (;;)

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef float       f32;
typedef double      f64;

typedef const char* String;

typedef struct f32x3 f32x3;
typedef struct u16x3 u16x3;

struct f32x3 {
    f32 x;
    f32 y;
    f32 z;
};

struct u16x3 {
    u16 x;
    u16 y;
    u16 z;
};

#define f32x3(x, y, z) ((f32x3){x, y, z})
#define u16x3(x, y, z) ((u16x3){x, y, z})
