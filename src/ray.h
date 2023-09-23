#ifndef RAY_H
#define RAY_H

#include "vec3.h"

typedef struct {
    point3 origin;
    vec3 direction;
} ray;

point3 at(ray *r, double t) {
    vec3 scaled_dir = mult(&r->direction, t);
    return add(&r->origin, &scaled_dir);
}

#endif
