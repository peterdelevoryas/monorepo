// w = theta
// h_theta(x) = f(w, x)
// J = Residual sum of squares (1/2m * sum((h_theta(x[i]) - y[i])^2 for i in 0..m))
// Error = J = cost function

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#define len(a) (sizeof(a) / sizeof(a[0]))

// This generates some input data from the following function: f(x) = x
// It just adds some random noise to each point to make it harder to
// train a model. But if you train a model, if should produce a line
// roughly equivalent to "y = x".
static
void generate_data(int m, float x[m][2], float y[m]) {
  uint8_t* e = malloc(sizeof(uint8_t) * m);
  FILE* f = fopen("/dev/urandom", "r");
  assert(f);
  fread(e, 1, m, f);
  fclose(f);

  // f(x) = x
  // y[i] = f(x[i]) + e[i]
  for (int i = 0; i < m; i++) {
    float e_i = (float)(e[i] - 128) / 16.0f;
    x[i][0] = 1.0f;
    x[i][1] = -m / 2 + i;
    y[i] = x[i][1] + e_i;
  }

  free(e);
}

static float dot_product(int n, float a[n], float b[n]) {
  float product = 0.0f;
  for (int i = 0; i < n; i++) {
    product += a[i] * b[i];
  }
  return product;
}

// Gradient descent using a modified residual sum of squares as the error function.
// Uses the full training set for each parameter update.
static
void batch_gradient_descent(int n, float w[n], int m, float x[m][n], float y[m], float a) {
  int i, j, k;

  // Initialize parameters to zero to start with.
  memset(w, 0, sizeof(*w) * n);

  float* w_ = malloc(sizeof(*w) * n);
  memcpy(w_, w, sizeof(*w) * n);

  for (k = 0; k < 10000; k++) {
    // Update the parameters.
    for (i = 0; i < n; i++) {
      // g = the gradient at point "w".
      float g = 0.0f;
      for (j = 0; j < m; j++) {
        g += (dot_product(n, w, x[j]) - y[j]) * x[j][i];
      }
      g /= m;
      w_[i] = w[i] - a * g;
    }

    // Check for convergence.
    if (memcmp(w, w_, sizeof(*w) * n) == 0) {
      break;
    }
    memcpy(w, w_, sizeof(*w) * n);
  }

  // Print out the number of iterations, so we can adjust the learning rate
  // statically.
  // printf("%s: stopped at %d iterations\n", __func__, k);
}

static void linear_regression_test(void) {
  float x[32][2];
  float y[32];
  float w[2];
  float a = 0.01f;
  float p;
  int m = len(x);
  int n = len(w);
  int i;
  FILE* f;
  assert(len(y) == m);
  assert(len(x[0]) == n);

  generate_data(m, x, y);

  // Write the generated data set to a csv.
  f = fopen("in.csv", "w");
  fprintf(f, "x,y\n");
  for (i = 0; i < m; i++) {
    fprintf(f, "%f,%f\n", x[i][1], y[i]);
  }
  fclose(f);

  batch_gradient_descent(n, w, m, x, y, a);

  // Print out the parameters after training.
  // printf("w\n");
  // for (i = 0; i < n; i++) {
  //   printf("%6.2f\n", w[i]);
  // }

  // Write the predictions to a csv.
  f = fopen("out.csv", "w");
  fprintf(f, "x,y^\n");
  for (i = 0; i < m; i++) {
    p = dot_product(n, w, x[i]);
    fprintf(f, "%f,%f\n", x[i][1], p);
  }
  fclose(f);
}

static void tga_uncompressed_grayscale(const void* pixels, uint16_t height,
                                       uint16_t width, const char* path) {
  FILE* f;
  uint8_t header[18];

  header[0] = 0;              // No ID included.
  header[1] = 0;              // No color map included.
  header[2] = 3;              // Uncompressed grayscale image.
  header[3] = 0;              // First color map index (none), lower 8 bits.
  header[4] = 0;              // First color map index (none), upper 8 bits.
  header[5] = 0;              // Color map length (zero), lower 8 bits.
  header[6] = 0;              // Color map length (zero), upper 8 bits.
  header[7] = 0;              // Color map entry size (zero).
  header[8] = 0;              // X origin, lower 8 bits.
  header[9] = 0;              // X origin, upper 8 bits.
  header[10] = 0;             // Y origin, lower 8 bits.
  header[11] = 0;             // Y origin, upper 8 bits.
  header[12] = width & 0xFF;  // Width, lower 8 bits.
  header[13] = width >> 8;    // Width, upper 8 bits.
  header[14] = height & 0xFF; // Height, lower 8 bits.
  header[15] = height >> 8;   // Height, upper 8 bits.
  header[16] = 8;             // Pixel depth (bits per pixel).
  header[17] = 0x20;          // Alpha-channel depth and image orientation.

  f = fopen(path, "w");
  fwrite(header, sizeof(header), 1, f);
  fwrite(pixels, 1, height * width, f);
  fclose(f);
}

int main(int argc, char** argv) {
  uint32_t header[4];
  static uint8_t labels[60000];
  static uint8_t images[60000][28][28];
  static uint8_t tga[200][28][300][28];
  FILE* f;
  int m = len(labels);
  int h = len(images[0]);
  int w = len(images[0][0]);
  int i, j, k, l;

  assert(m == len(images));
  assert(sizeof(tga) == sizeof(images));

  f = fopen("train-labels-idx1-ubyte", "r");
  fread(header, sizeof(uint32_t), 2, f);
  header[0] = __builtin_bswap32(header[0]);
  header[1] = __builtin_bswap32(header[1]);
  assert(header[0] == 0x00000801);
  assert(header[1] == m);
  fread(labels, sizeof(uint8_t), m, f);
  fclose(f);

  f = fopen("train-images-idx3-ubyte", "r");
  fread(header, sizeof(uint32_t), 4, f);
  header[0] = __builtin_bswap32(header[0]);
  header[1] = __builtin_bswap32(header[1]);
  header[2] = __builtin_bswap32(header[2]);
  header[3] = __builtin_bswap32(header[3]);
  assert(header[0] == 0x00000803);
  assert(header[1] == m);
  assert(header[2] == h);
  assert(header[3] == h);
  fread(images, w * h, m, f);
  fclose(f);

  for (i = 0; i < 10; i++) {
    printf("%d ", labels[i]);
  }
  printf("\n");

  for (i = 0; i < 200; i++)
  for (j = 0; j < 28; j++)
  for (k = 0; k < 300; k++)
  for (l = 0; l < 28; l++) {
    tga[i][j][k][l] = images[i * 300 + k][j][l];
  }
  tga_uncompressed_grayscale(tga, 200 * 28, 300 * 28, "images.tga");

  //    0  ... 28 ... 56 ... 128
  // 0  +-----++-----+
  // 1  |     ||     |
  // 2  +-----++-----+
  // 3
  // 4

  // for (i = 0; i < 10; i++)
  //   printf("%d ", labels[i]);
  // printf("\n");
  // tga_uncompressed_grayscale(images, height * 10, width, "images.tga");
}
