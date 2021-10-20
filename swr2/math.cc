#include "math.hh"

float3::float3(float x, float y, float z) noexcept
  : x(x), y(y), z(z)
{
}

float float3::operator*(float3 b) const noexcept
{
  auto a = *this;
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

float3 float3::operator^(float3 b) const noexcept
{
  auto a = *this;
  auto x = a.y * b.z - a.z * b.y;
  auto y = a.z * b.x - a.x * b.z;
  auto z = a.x * b.y - a.y * b.x;
  return float3(x, y, z);
}
