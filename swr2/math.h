#ifndef MATH_H
#define MATH_H

typedef struct float3 float3;
struct float3 {
  float x, y, z;
};

#define float3(x, y, z) ((float3){x, y, z})

float float3_dot(float3 a, float3 b);
float3 float3_cross(float3 a, float3 b);

#endif
