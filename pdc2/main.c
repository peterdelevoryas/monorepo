#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include "parser.h"

#define USAGE_STRING "Usage: pdc [-h] file...\n"

static void compile_file(string path) {
    Parser p;
    Function f;
    void* addr;
    u64 size;

    addr = mmap_file(path, &size);
    if (!addr) {
        printf("unable to mmap '%s': %s\n", path, strerror(errno));
        return;
    }

    p = parser_init(path, addr, size);
    print_tokens(&p);

    p = parser_init(path, addr, size);
    f = parse_function(&p);
    munmap(addr, size);
}

int main(int argc, string argv[argc]) {
    int i;
    string arg;

    if (argc < 2) {
        printf(USAGE_STRING);
        return 0;
    }

    for (i = 1; i < argc; i++) {
        arg = argv[i];
        if (strcmp(arg, "-h") == 0) {
            printf(USAGE_STRING);
            return 0;
        }
        compile_file(arg);
    }
}
