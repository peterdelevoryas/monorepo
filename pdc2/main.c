#include <stdio.h>
#include <string.h>

#define USAGE "Usage: pdc [options] file...\n" \
              "Options:\n" \
              "  --help             Print this message.\n"

static void compile(const char *file) {
    FILE *f = fopen(file, "r");
    if (!f) {

    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("%s", USAGE);
        return 1;
    }
    const char *files[16];
    int num_files = 0;
    for (int i = 1; i < argc; i++) {
        const char *file = argv[i];
        if (strcmp(file, "-h") == 0 || strcmp(file, "--help") == 0) {
            printf("%s", USAGE);
            return 1;
        }
        files[num_files++] = file;
    }
    for (int i = 0; i < num_files; i++) {
        compile(files[i]);
    }
}
