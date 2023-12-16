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
    printf("triangle: \n\t(v0[%f, %f, %f], v1[%f, %f, %f], v2[%f, %f, %f]), \n\tn[%f, %f, %f], \n\tmat: %d\n", triangle.v1.x, triangle.v1.y, triangle.v1.z, triangle.v2.x, triangle.v2.y, triangle.v2.z, triangle.v3.x, triangle.v3.y, triangle.v3.z, triangle.normal.x, triangle.normal.y, triangle.normal.z, triangle.mat.type);
}

typedef struct TriangleMesh {
    Triangle *triangles;
    size_t size;
} TriangleMesh;

AABB create_aabb_for_triangle(const Triangle* s);

bool ray_intersect_triangle(const Ray *r, const Triangle *triangle, const Interval *ray_t, HitRecord *rec, int *num_intersects) {
    (*num_intersects)++;
    const double EPSILON = 0.0000001;

    Vec3 dir = unit_vec(r->direction);
    Vec3 edge1 = diff_vec3 (triangle->v2, triangle->v1);
    Vec3 edge2 = diff_vec3(triangle->v3, triangle->v1);

    Vec3 rayVecXe2 = cross(dir, edge2);
    double det = dot(edge1, rayVecXe2);

    // Full compare
    // This ray is parallel to this triangle.
    //if (det > -EPSILON && det < EPSILON) return false;
    // "Back facing compare
    if (det < EPSILON) return false;

    double invDet = 1.0 / det;
    Vec3 s = diff_vec3(r->origin, triangle->v1);
    double u = invDet * dot(s, rayVecXe2);

    if (u < 0.0 || u > 1.0) 
        return false;

    Vec3 sXe1 = cross(s, edge1);
    double v = invDet * dot(dir, sXe1);

    if (v < 0.0 || u + v > 1.0) 
        return false;

    // At this stage we can compute t to find out where the intersection point is on the line.
    double t = invDet * dot(edge2, sXe1);
    if (t > EPSILON) {
        Point3 p = at(r, t);
        rec->t = t;
        rec->p = p; 
        rec->mat = triangle->mat;
        set_face_normal(rec, r, triangle->normal);
        return true;
    }
    return false;
}

bool ray_intersect_triangle_arr(const Ray *r, int num_triangles, const Triangle triangles[], const Interval *ray_t, HitRecord *record, int *num_intersections) {
    HitRecord temp_rec;
    bool hit_anything = false;
    double closest_so_far = ray_t->max;

    for (int i = 0; i < num_triangles; i++) {
        Interval cur_interval = {.min=ray_t->min, .max=closest_so_far};
        if (ray_intersect_triangle(r, &triangles[i], &cur_interval, &temp_rec, num_intersections)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *record = temp_rec;
        }
    }

    return hit_anything;
}
