#include <cstdio>
#include <cstdlib>

struct Parser {
  FILE* file;

  static Parser init(FILE* file) {
    return {
      .file = file,
    };
  }
};

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("USAGE: %s FILE\n", argv[0]);
    printf("Not enough arguments\n");
    return 1;
  }

  auto file = fopen(argv[1], "r");
  auto parser = Parser::init(file);

  fclose(file);
}
