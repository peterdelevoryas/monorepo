#ifndef VEC_HH
#define VEC_HH
#include <cstdlib>
#include <cstdint>
#include <cassert>

template<typename T>
struct Vec {
  T* data = nullptr;
  uint32_t len = 0;
  uint32_t cap = 0;

  void reserve(uint32_t n)
  {
    assert(n >= cap);
    auto p = realloc(data, n * sizeof(T));
    data = static_cast<T*>(p);
    cap = n;
  }

  void append(T x)
  {
    assert(len < cap);
    data[len] = x;
    len += 1;
  }

  T& operator[](uint32_t i)
  {
    assert(i < len);
    return data[i];
  }

  const T& operator[](uint32_t i) const
  {
    assert(i < len);
    return data[i];
  }

  void free()
  {
    ::free(data);
  }

  const T* begin() const
  {
    return data;
  }

  const T* end() const
  {
    return data + len;
  }

  T* begin()
  {
    return data;
  }

  T* end()
  {
    return data + len;
  }
};

#endif
