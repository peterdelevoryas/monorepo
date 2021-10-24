// w == theta
// h_theta(x) == f(w, x)
// J = Residual sum of squares (1/2m * sum((h_theta(x[i]) - y[i])^2 for i in 0..m))

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define Len(a) (sizeof(a) / sizeof(a[0]))

static
void GenerateData(int m, float x[m][2], float y[m])
{
  uint8_t* e = malloc(sizeof(uint8_t) * m);
  FILE* f = fopen("/dev/urandom", "r");
  assert(f);
  fread(e, 1, m, f);
  fclose(f);

  // f(x) = x
  // y[i] = f(x[i]) + e[i]
  for (int i = 0; i < m; i++)
  {
    float e_i = (float)(e[i] - 128) / 16.0f;
    x[i][0] = 1.0f;
    x[i][1] = -m / 2 + i;
    y[i] = x[i][1] + e_i;
  }

  free(e);
}

// Gradient descent using a modified residual sum of squares as the error function.
static
void GradientDescent(int n, float w[n], int m, float x[m][n], float y[m])
{
  // Initialize parameters to zero to start with.
  memset(w, 0, n * sizeof(w[0]));
}

int main(int argc, char** argv)
{
  float x[32][2];
  float y[32];
  float w[2];
  int m = Len(x);
  int n = Len(w);
  int i, j;
  assert(Len(y) == m);
  assert(Len(x[0]) == n);

  GenerateData(m, x, y);
  printf("x, y\n");
  for (i = 0; i < m; i++)
  {
    printf("[");
    for (j = 0; j < n; j++)
    {
      if (j)
        printf(" ");
      printf("%6.2f", x[i][j]);
    }
    printf("] %6.2f\n", y[i]);
  }

  GradientDescent(n, w, m, x, y);
  printf("w\n");
  for (i = 0; i < n; i++)
  {
    printf("%6.2f\n", w[i]);
  }
}
