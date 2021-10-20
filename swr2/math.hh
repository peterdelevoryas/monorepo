#ifndef MATH_H
#define MATH_H

struct float3 {
  float x, y, z;

  constexpr explicit float3(float x, float y, float z) noexcept
    : x(x), y(y), z(z)
  {
    // Nothing to do.
  }
  float3 normalize() const noexcept;
};

float operator*(float3 a, float3 b);
float3 operator*(float3 a, float s);
float3 operator-(float3 a, float3 b);
float3 cross(float3 a, float3 b);

#endif
