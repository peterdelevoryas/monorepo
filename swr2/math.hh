#ifndef MATH_H
#define MATH_H

struct float3 {
  float x, y, z;

  float operator*(float3 b) const noexcept;
  float3 operator^(float3 b) const noexcept;
};

#endif
