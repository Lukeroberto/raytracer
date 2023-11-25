#pragma once

#include "interval.h"
#include "utils.h"

typedef struct {
    interval x;
    interval y;
    interval z;
} aabb;

void print_aabb(aabb *bbox) {
    printf("bbox: (x[%f, %f], y[%f, %f], z[%f, %f])\n", bbox->x.min, bbox->x.max, bbox->y.min, bbox->y.max, bbox->z.min, bbox->z.max);
}

aabb create_empty_aabb() {
    return (aabb){0};
}

aabb create_aabb_for_point(point3 a, point3 b) {
    aabb bbox = {
        .x = (interval) {.min=fmin(a.x, b.x), .max=fmax(a.x, b.x)},
        .y = (interval) {.min=fmax(a.y, b.y), .max=fmax(a.y, b.y)},
        .z = (interval) {.min=fmax(a.z, b.z), .max=fmax(a.z, b.z)}
    };

    return bbox;
}

aabb create_aabb_for_aabb(aabb *a, aabb *b) {
   return (aabb) {
        .x = create_from_interval(a->x, b->x),
        .y = create_from_interval(a->y, b->y),
        .z = create_from_interval(a->z, b->z),
   };
}

interval get_axis_from_aabb(aabb *bbox, int n) {
    if (n == 1) return bbox->y;
    if (n == 2) return bbox->z;

    return bbox->x;
}

bool hit_aabb_axis(ray *ray, interval ray_t, aabb *bbox, int axis_n) {
    double ray_dir;
    double orig_axis;
    switch (axis_n) {
        case 0:
            ray_dir = ray->direction.x;
            orig_axis = ray->origin.x;
            break;

        case 1: 
            ray_dir = ray->direction.y;
            orig_axis = ray->origin.y;
            break;

        case 2:
            ray_dir = ray->direction.z;
            orig_axis = ray->origin.z;
            break;
    }

    double invDir = 1 / ray_dir;

    interval intvl = get_axis_from_aabb(bbox, axis_n);
    double t0 = (intvl.min - orig_axis) * invDir;
    double t1 = (intvl.max - orig_axis) * invDir;

    if (invDir < 0) {
        double temp = t0;
        t0 = t1;
        t1 = temp;
    }

    if (t0 > ray_t.min) ray_t.min = t0;
    if (t1 < ray_t.max) ray_t.max = t1;

    if (ray_t.max <= ray_t.min)
        return false;
    
    return true;
}

bool hit_aabb(ray *ray, interval ray_t, aabb *bbox) {
    for (int a = 0; a < 3; a++) {
        if (hit_aabb_axis(ray, ray_t, bbox, a)) {
            return true;
        }
    }

    return false;
}
