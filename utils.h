#ifndef HEADER_UTILS
#define HEADER_UTILS

typedef float vec3[3];

float deg2rad(float degrees);
void vec3_rotate_x(vec3 v, float angle);
void vec3_rotate_y(vec3 v, float angle);

#endif // HEADER_UTILS