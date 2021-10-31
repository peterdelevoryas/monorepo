#include <stdio.h>
#include "common.h"
#include "parser.h"

static void usage() {
    printf("usage: pdc [-h] file...\n");
}

static void compile(const char *path) {

    let p = setup_parser(path);
    debug_tokens(&p);
    reset_parser(&p);

    let f = expect_function(&p);

    munmap(p.text, p.text_size);
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
