#ifndef HEADER_UTILS
#define HEADER_UTILS

#include <algorithm>

typedef float vec3[3];

float deg2rad(float degrees);
void vec3_rotate_x(vec3 v, float angle);
void vec3_rotate_y(vec3 v, float angle);

template<typename Collection, typename T>
bool contains(const Collection &v, T item) {
    if (std::find(v.begin(), v.end(), item) != v.end()) {
        return true;
    }
    return false;
}

#endif // HEADER_UTILS
