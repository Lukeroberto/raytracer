# pragma once

#include "vec3.h"

typedef struct {
    Point3 origin;
    Vec3 direction;
} ray;

Point3 at(ray *r, double t) {
    Vec3 scaled_dir = scale_vec3(r->direction, t);
    return add_vec3(r->origin, scaled_dir);
}
