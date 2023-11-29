#pragma once

#include "aabb.h"
#include "hittable.h"
#include "types.h"
#include "vec3.h"

typedef struct Triangle {
    Point3 v1, v2, v3;
    Vec3 normal;
    Material mat;
} Triangle;

void print_tri(Triangle triangle) {
    printf("triangle: (v0[%f, %f, %f], v1[%f, %f, %f], v2[%f, %f, %f]), mat: %d\n", triangle.v1.x, triangle.v1.y, triangle.v1.z, triangle.v2.x, triangle.v2.y, triangle.v2.z, triangle.v3.x, triangle.v3.y, triangle.v3.z, triangle.mat.type);
}

typedef struct TriangleMesh {
    Triangle *triangles;
    size_t size;
} TriangleMesh;

AABB create_aabb_for_triangle(const Triangle* s);

bool ray_intersect_triangle(const Ray *r, const Triangle *triangle, const Interval *ray_t, HitRecord *rec) {
    const double EPSILON = 0.0000001;
    Vec3 vertex0 = triangle->v1;
    Vec3 vertex1 = triangle->v2;  
    Vec3 vertex2 = triangle->v3;

    Vec3 edge1 = diff_vec3(vertex1, vertex0);
    Vec3 edge2 = diff_vec3(vertex2, vertex0);

    Vec3 rayVecXe2 = cross(r->direction, edge2);
    double det = dot(edge1, rayVecXe2);

    if (det > -EPSILON && det < EPSILON)
        return false;    // This ray is parallel to this triangle.

    double invDet = 1.0 / det;
    Vec3 s = diff_vec3(r->origin, vertex0);
    double u = invDet * dot(s, rayVecXe2);

    if (u < 0.0 || u > 1.0)
        return false;

    Vec3 sXe1 = cross(s, edge1);
    double v = invDet * dot(r->direction, sXe1);

    if (v < 0.0 || u + v > 1.0)
        return false;

    // At this stage we can compute t to find out where the intersection point is on the line.
    double t = invDet * dot(edge2, sXe1);

    if (t > EPSILON) // ray intersection
    {
        Point3 p = add_vec3(r->origin, scale_vec3(r->direction, t));
        rec->t = t;
        rec->p = p; 
        rec->mat = triangle->mat;
        set_face_normal(rec, r, triangle->normal);
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

bool ray_intersect_triangle_arr(const Ray *r, int num_triangles, const Triangle spheres[], const Interval *ray_t, HitRecord *record) {
    HitRecord temp_rec = {0};
    bool hit_anything = false;
    double closest_so_far = ray_t->max;

    for (int i = 0; i < num_triangles; i++) {
        Interval cur_interval = {.min=ray_t->min, .max=closest_so_far};
        if (ray_intersect_triangle(r, &spheres[i], &cur_interval, &temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *record = temp_rec;
        }
    }

    record->num_tests += num_triangles;
    return hit_anything;
}
