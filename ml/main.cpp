#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cassert>

static void linear_regression() {
  constexpr uint32_t m = 1024 * 1024;
  constexpr uint32_t n = 2;
  constexpr uint32_t mn = m * n;
  static float x[m * n];
  static float y[m];
  float w[n];

  auto urandom = fopen("/dev/urandom", "r");
  assert(urandom);
  for (uint32_t i = 0; i < m; i++) {
    int noise = fgetc(urandom);
    x[i * n + 0] = 1.0f;
    x[i * n + 1] = float(i);
    y[i] = 
  }
  fclose(urandom);
}

int main(int argc, char** argv) {
  linear_regression();
}
