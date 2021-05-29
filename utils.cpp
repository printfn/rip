#include "utils.h"
#include <cstdio>
#include <iostream>

float deg2rad(float degrees) {
    return degrees * 3.141592653589793238 / 180;
}

void vec3_rotate_x(vec3 v, float angle) {
    vec3 result;
    result[0] = v[0];
    result[1] = cos(angle) * v[1] - sin(angle) * v[2];
    result[2] = sin(angle) * v[1] + cos(angle) * v[2];
    v[0] = result[0];
    v[1] = result[1];
    v[2] = result[2];
}

void vec3_rotate_y(vec3 v, float angle) {
    vec3 result;
    result[0] = cos(angle) * v[0] + sin(angle) * v[2];
    result[1] = v[1];
    result[2] = -sin(angle) * v[0] + cos(angle) * v[2];
    v[0] = result[0];
    v[1] = result[1];
    v[2] = result[2];
}
