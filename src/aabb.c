#include <stdio.h>

#include "aabb.h"

void print_aabb(const AABB *bbox) {
    printf("bbox: (x[%f, %f], y[%f, %f], z[%f, %f])\n", bbox->x.min, bbox->x.max, bbox->y.min, bbox->y.max, bbox->z.min, bbox->z.max);
}

AABB create_empty_aabb() {
    return (AABB){0};
}

AABB create_aabb_for_interval(Interval x, Interval y, Interval z) {
    return (AABB) {
        .x = x,
        .y = y,
        .z = z
    };
}

AABB create_aabb_for_point(Point3 a, Point3 b) {
    AABB bbox = {
        .x = (Interval) {.min=fmin(a.x, b.x), .max=fmax(a.x, b.x)},
        .y = (Interval) {.min=fmin(a.y, b.y), .max=fmax(a.y, b.y)},
        .z = (Interval) {.min=fmin(a.z, b.z), .max=fmax(a.z, b.z)}
    };

    return bbox;
}

AABB create_aabb_for_point3(Point3 a, Point3 b, Point3 c) {
    AABB bbox = {
        .x = (Interval) {.min=fmin(fmin(a.x, b.x), c.x), .max=fmax(fmax(a.x, b.x), c.x)},
        .y = (Interval) {.min=fmin(fmin(a.y, b.y), c.y), .max=fmax(fmax(a.y, b.y), c.y)},
        .z = (Interval) {.min=fmin(fmin(a.z, b.z), c.z), .max=fmax(fmax(a.z, b.z), c.z)},
    };

    return pad(bbox);
}


AABB create_aabb_for_aabb(const AABB *a, const AABB *b) {
   return (AABB) {
        .x = create_from_interval(a->x, b->x),
        .y = create_from_interval(a->y, b->y),
        .z = create_from_interval(a->z, b->z),
   };
}

Point3 center_aabb(const AABB *bbox) {
    return (Point3) {
        .x = (bbox->x.max - bbox->x.min) / 2,
        .y = (bbox->y.max - bbox->y.min) / 2,
        .z = (bbox->z.max - bbox->z.min) / 2,
    };
}

Interval get_axis_from_aabb(const AABB *bbox, int n) {
    if (n == 1) return bbox->y;
    if (n == 2) return bbox->z;

    return bbox->x;
}

AABB pad(AABB aabb) {
    // Return an AABB that has no side narrower than some delta, padding if necessary.
    double delta = 0.0001;
    Interval new_x = (size_interval(aabb.x) >= delta) ? aabb.x : expand_interval(delta, aabb.x);
    Interval new_y = (size_interval(aabb.y) >= delta) ? aabb.y : expand_interval(delta, aabb.y);
    Interval new_z = (size_interval(aabb.z) >= delta) ? aabb.z : expand_interval(delta, aabb.z);

    return create_aabb_for_interval(new_x, new_y, new_z);
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
