# pragma once

#include "vec3.h"

typedef struct Ray {
    Point3 origin;
    Vec3 direction;
} Ray;

Point3 at(const Ray *r, double t) {
    Vec3 scaled_dir = scale_vec3(r->direction, t);
    return add_vec3(r->origin, scaled_dir);
}

double origin_dim(const Ray *r, int axis) {
    switch (axis) {
        case 0:
            return r->origin.x;
            break;
        case 1: 
            return r->origin.y;
            break;
        case 2:
            return r->origin.z;
            break;
    }

    return 0;
}

double dir_dim(const Ray *r, int axis) {
    switch (axis) {
        case 0:
            return r->direction.x;
            break;
        case 1: 
            return r->direction.y;
            break;
        case 2:
            return r->direction.z;
            break;
    }

    return 0;
}
