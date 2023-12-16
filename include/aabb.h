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
        .y = (Interval) {.min=fmin(a.y, b.y), .max=fmax(a.y, b.y)},
        .z = (Interval) {.min=fmin(a.z, b.z), .max=fmax(a.z, b.z)}
    };

    return bbox;
}

const double EPSILON = 0.0001;

AABB create_aabb_for_point3(Point3 a, Point3 b, Point3 c) {
    AABB bbox = {
        .x = (Interval) {.min=fmin(fmin(a.x, b.x), c.x) - EPSILON, .max=fmax(fmax(a.x, b.x), c.x) + EPSILON},
        .y = (Interval) {.min=fmin(fmin(a.y, b.y), c.y) - EPSILON, .max=fmax(fmax(a.y, b.y), c.y) + EPSILON},
        .z = (Interval) {.min=fmin(fmin(a.z, b.z), c.z) - EPSILON, .max=fmax(fmax(a.z, b.z), c.z)+ EPSILON},
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

bool hit_aabb(const Ray *ray, Interval ray_t, const AABB *bbox) {
    for (int a = 0; a < 3; a++) {
        double axis_ratio_min = (get_axis_from_aabb(bbox, a).min - origin_dim(ray, a)) / dir_dim(ray, a);
        double axis_ratio_max = (get_axis_from_aabb(bbox, a).max - origin_dim(ray, a)) / dir_dim(ray, a);

        double t0 = fmin(axis_ratio_min, axis_ratio_max);
        double t1 = fmax(axis_ratio_min, axis_ratio_max);

        ray_t.min = fmax(t0, ray_t.min);
        ray_t.max = fmin(t1, ray_t.max);

        if (ray_t.max <= ray_t.min) {
            return false;
        }
    }

    return true;
}
