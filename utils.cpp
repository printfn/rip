#include "utils.h"
#include <cstdio>
#include <iostream>

void fail(const char *message) {
    if (message) {
        std::cerr << message << std::endl;
    }
    abort();
}
