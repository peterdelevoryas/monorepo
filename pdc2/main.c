#include <stdio.h>
#include "common.h"
#include "parser.h"

static void usage() {
    printf("usage: pdc [-h] file...\n");
}

static void compile(const char *path) {
    Parser p;

    parser_init(&p, path);
    parser_debug_tokens(&p);
    parser_free(&p);
}

int main(int argc, char **argv) {
    const char *s;
    int i;

    if (argc < 2) {
        usage();
        return 0;
    }
    for (i = 1; i < argc; i++) {
        s = argv[i];
        if (strings_equal(s, "-h")) {
            usage();
            return 0;
        }
        compile(s);
    }
}
