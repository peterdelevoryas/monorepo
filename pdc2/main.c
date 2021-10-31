#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include "parser.h"

static void print_usage() {
    printf("usage: pdc [-h] file...\n");
}

static void compile_file(string path) {
    Parser p;
    u8* addr;
    u64 size;

    addr = mmap_file(path, &size);
    if (!addr) {
        return;
    }

    p = parser_init(path, addr, size);
    print_tokens(&p);

    p = parser_init(path, addr, size);
    munmap(addr, size);
}

int main(int argc, string argv[argc]) {
    int i;
    string arg;

    if (argc < 2) {
        print_usage();
        return 0;
    }
    for (i = 1; i < argc; i++) {
        arg = argv[i];
        if (strcmp(arg, "-h") == 0) {
            print_usage();
            return 0;
        }
        compile_file(arg);
    }
}
