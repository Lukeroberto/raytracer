#pragma once

#include "interval.h"
#include "ray.h"
#include "vec3.h"

typedef struct AABB{
    Interval x;
    Interval y;
    Interval z;
} AABB;

void print_aabb(const AABB *bbox);

AABB pad(AABB bbox);

AABB create_empty_aabb();
AABB create_aabb_for_interval(Interval x, Interval y, Interval z);
AABB create_aabb_for_point(Point3 a, Point3 b);
AABB create_aabb_for_point3(Point3 a, Point3 b, Point3 c);
AABB create_aabb_for_aabb(const AABB *a, const AABB *b);

Point3 center_aabb(const AABB *bbox);

Interval get_axis_from_aabb(const AABB *bbox, int n);

bool hit_aabb(const Ray *ray, Interval ray_t, const AABB *bbox);
