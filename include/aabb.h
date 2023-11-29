#pragma once

#include <stdio.h>

#include "interval.h"
#include "ray.h"

typedef struct AABB{
    Interval x;
    Interval y;
    Interval z;
} AABB;

void print_aabb(const AABB *bbox) {
    printf("bbox: (x[%f, %f], y[%f, %f], z[%f, %f])\n", bbox->x.min, bbox->x.max, bbox->y.min, bbox->y.max, bbox->z.min, bbox->z.max);
}

AABB create_empty_aabb() {
    return (AABB){0};
}

AABB create_aabb_for_point(Point3 a, Point3 b) {
    AABB bbox = {
        .x = (Interval) {.min=fmin(a.x, b.x), .max=fmax(a.x, b.x)},
        .y = (Interval) {.min=fmax(a.y, b.y), .max=fmax(a.y, b.y)},
        .z = (Interval) {.min=fmax(a.z, b.z), .max=fmax(a.z, b.z)}
    };

    return bbox;
}

AABB create_aabb_for_aabb(const AABB *a, const AABB *b) {
   return (AABB) {
        .x = create_from_interval(a->x, b->x),
        .y = create_from_interval(a->y, b->y),
        .z = create_from_interval(a->z, b->z),
   };
}

Interval get_axis_from_aabb(const AABB *bbox, int n) {
    if (n == 1) return bbox->y;
    if (n == 2) return bbox->z;

    return bbox->x;
}

bool hit_aabb_axis(const Ray *ray, Interval ray_t, const AABB *bbox, int axis_n) {
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

    Interval intvl = get_axis_from_aabb(bbox, axis_n);
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

bool hit_aabb(const Ray *ray, Interval ray_t, const AABB *bbox) {
    for (int a = 0; a < 3; a++) {
        if (hit_aabb_axis(ray, ray_t, bbox, a)) {
            return true;
        }
    }

    return false;
}
