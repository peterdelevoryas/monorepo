#pragma once

#define Vector(T)   \
    struct {        \
        T*  data;   \
        u32 len;    \
        u32 cap;    \
    }
