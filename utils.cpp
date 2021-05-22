#include "utils.h"
#include <cstdio>
#include <cstdlib>

void fail(const char *message) {
    fprintf(stderr, "%s\n", message);
    abort();
}
