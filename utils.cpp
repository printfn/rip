#include "utils.h"
#include <cstdio>
#include <cstdlib>

void fail(const char *message) {
    if (message) {
        fprintf(stderr, "%s\n", message);
    }
    abort();
}
