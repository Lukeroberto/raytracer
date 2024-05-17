#ifndef QUAD_H
#define QUAD_H
#include "aabb.h"
#include "hittable.h"
#include "types.h"
#include "vec3.h"

// Quad is defined by a point and 2 directions
typedef struct Quad {
    Point3 corner;
    Vec3 dir1;
    Vec3 dir2;
    Material mat;

    // Computed
    Vec3 normal;
    double plane_d;
    Vec3 scaled_normal;

} Quad;

Quad create_quad(Point3 corner, Vec3 dir1, Vec3 dir2, Material mat) {
    Vec3 normal = cross(dir1, dir2);
    normal = unit_vec(normal);

    double plane_d = dot(normal, corner);
    return (Quad) {
        .corner = corner,
        .dir1 = dir1,
        .dir2 = dir2,
        .mat = mat,
        .normal = normal,
        .plane_d = plane_d,
        .scaled_normal = scale_vec3(normal, dot(normal, normal))
    };
}

Point3 get_opposite_corner(Quad q) {
    return add_vec3(q.corner, add_vec3(q.dir1, q.dir2));
}

AABB create_aabb_for_quad(Quad *q) {
    return create_aabb_for_point(q->corner, get_opposite_corner(*q));
}

bool is_interior(double alpha, double beta, HitRecord* rec) {
    // Given the hit point in plane coordinates, return false if it is outside the
    // primitive, otherwise set the hit record UV coordinates and return true.

    if ((alpha < 0) || (1 < alpha) || (beta < 0) || (1 < beta))
        return false;

    rec->u = alpha;
    rec->v = beta;
    return true;
}

bool ray_intersect_quad(const Ray *r, const Quad *quad, const Interval *ray_t, HitRecord *rec, int *num_intersects) {
    double denom = dot(quad->normal, r->direction);
    
    if (fabs(denom) < 1e-8) return false;

    double t = (quad->plane_d - dot(quad->normal, r->origin)) / denom;

    Point3 intersection = at(r, t);
    Vec3 planar_hitpt_vector = diff_vec3(intersection, quad->corner);
    double alpha = dot(quad->scaled_normal, cross(planar_hitpt_vector, quad->dir1));
    double beta = dot(quad->scaled_normal, cross(quad->dir2, planar_hitpt_vector));

    if (!is_interior(alpha, beta, rec))
        return false;

    // Ray hits the 2D shape; set the rest of the hit record and return true.
    rec->t = t;
    rec->p = intersection;
    rec->mat = quad->mat;
    set_face_normal(rec, r, quad->normal);

    return true;
}

bool ray_intersect_quad_arr(const Ray *r, int num_quads, const Quad quads[], const Interval *ray_t, HitRecord *record, int *num_intersections) {
    HitRecord temp_rec;
    bool hit_anything = false;
    double closest_so_far = ray_t->max;

    for (int i = 0; i < num_quads; i++) {
        Interval cur_interval = {.min=ray_t->min, .max=closest_so_far};
        if (ray_intersect_quad(r, &quads[i], &cur_interval, &temp_rec, num_intersections)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *record = temp_rec;
        }
    }

    return hit_anything;
}

#endif // QUAD_H
