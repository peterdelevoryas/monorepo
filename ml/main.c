// w = theta
// h_theta(x) = f(w, x)
// J = Residual sum of squares (1/2m * sum((h_theta(x[i]) - y[i])^2 for i in 0..m))
// Error = J = cost function

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define Len(a) (sizeof(a) / sizeof(a[0]))

// This generates some input data from the following function: f(x) = x
// It just adds some random noise to each point to make it harder to
// train a model. But if you train a model, if should produce a line
// roughly equivalent to "y = x".
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

static
float DotProduct(int n, float a[n], float b[n])
{
  float product = 0.0f;
  for (int i = 0; i < n; i++)
  {
    product += a[i] * b[i];
  }
  return product;
}

// Gradient descent using a modified residual sum of squares as the error function.
// Uses the full training set for each parameter update.
static
void BatchGradientDescent(int n, float w[n], int m, float x[m][n], float y[m], float a)
{
  int i, j, k;

  // Initialize parameters to zero to start with.
  memset(w, 0, sizeof(*w) * n);

  float* w_ = malloc(sizeof(*w) * n);
  memcpy(w_, w, sizeof(*w) * n);

  for (k = 0; k < 10000; k++)
  {
    // Update the parameters.
    for (i = 0; i < n; i++)
    {
      // g = the gradient at point "w".
      float g = 0.0f;
      for (j = 0; j < m; j++)
      {
        g += (DotProduct(n, w, x[j]) - y[j]) * x[j][i];
      }
      g /= m;
      w_[i] = w[i] - a * g;
    }

    // Check for convergence.
    if (memcmp(w, w_, sizeof(*w) * n) == 0)
      break;
    memcpy(w, w_, sizeof(*w) * n);
  }

  // Print out the number of iterations, so we can adjust the learning rate
  // statically.
  // printf("%s: stopped at %d iterations\n", __func__, k);
}

int main(int argc, char** argv)
{
  float x[32][2];
  float y[32];
  float w[2];
  float a = 0.01f;
  int m = Len(x);
  int n = Len(w);
  int i;
  assert(Len(y) == m);
  assert(Len(x[0]) == n);

  GenerateData(m, x, y);

  // Print out the generated data set.
  printf("x, y\n");
  for (i = 0; i < m; i++)
  {
    printf("%6.2f, %6.2f\n", x[i][1], y[i]);
  }

  BatchGradientDescent(n, w, m, x, y, a);

  // Print out the parameters after training.
  // printf("w\n");
  // for (i = 0; i < n; i++)
  // {
  //   printf("%6.2f\n", w[i]);
  // }

  // Print out the predictions from the model.
  printf("x, predicted_y\n");
  for (i = 0; i < m; i++)
  {
    float prediction = DotProduct(n, w, x[i]);
    printf("%6.2f, %6.2f\n", x[i][1], prediction);
  }
}
