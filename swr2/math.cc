#include <math.h>
#include "math.hh"

float3 float3::normalize() const noexcept
{
  auto self = *this;
  auto n = 1 / sqrtf(self * self);
  return self * n;
}

float3 operator*(float3 a, float s)
{
  return float3(a.x * s, a.y * s, a.z * s);
}

float operator*(float3 a, float3 b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

float3 operator-(float3 a, float3 b)
{
  return float3(a.x - b.x, a.y - b.y, a.z - b.z);
}

float3 cross(float3 a, float3 b)
{
  auto x = a.y * b.z - a.z * b.y;
  auto y = a.z * b.x - a.x * b.z;
  auto z = a.x * b.y - a.y * b.x;
  return float3(x, y, z);
}
