#include <string.h>
#include "common.h"

bool strings_equal(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}
