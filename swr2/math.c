#include "math.h"

float float3_dot(float3 a, float3 b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

float3 float3_cross(float3 a, float3 b)
{
  float x = a.y * b.z - a.z * b.y;
  float y = a.z * b.x - a.x * b.z;
  float z = a.x * b.y - a.y * b.x;
  return float3(x, y, z);
}


