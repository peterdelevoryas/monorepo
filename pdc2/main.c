#include <stdio.h>
#include "common.h"
#include "parser.h"

static void usage()
{
    printf("usage: pdc [-h] file...\n");
}

static void compile(String path)
{
    Parser p = parser_init(path);
    parser_debug_tokens(&p);
    parser_free(&p);
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        usage();
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        if (string_equals(argv[i], "-h")) {
            usage();
            return 0;
        }
        compile(argv[i]);
    }
}
