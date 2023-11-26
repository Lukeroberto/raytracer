#pragma once

#include <stdbool.h>
#include <math.h>

#include "types.h"
#include "interval.h"
#include "aabb.h"
#include "hittable.h"

typedef struct Sphere {
    Point3 center;
    double radius;
    Material mat;
} Sphere;

void print_Sphere(const Sphere *sphere) {
    printf("sphere: (center[%f, %f, %f], r[%f], material[%d])\n", sphere->center.x, sphere->center.y, sphere->center.z, sphere->radius, sphere->mat.type);
}

Sphere make_sphere(Point3 p, double r, Material mat) {
    Sphere s = {
        .center = p,
        .radius = r,
        .mat = mat
    };
    return s;
}

AABB create_aabb_for_sphere(const Sphere* s) {
    Vec3 radius_vec = {.x = s->radius, .y = s->radius, .z = s->radius};
    Point3 min_pt = diff_vec3(s->center, radius_vec);
    Point3 max_pt = add_vec3(s->center, radius_vec);
    return create_aabb_for_point(min_pt, max_pt);
}

AABB create_aabb_for_array_sphere(const Sphere spheres[], int num_spheres) {
    AABB bbox = create_empty_aabb();
    for (int i = 0; i < num_spheres; i++) {
        AABB sphere_box = create_aabb_for_sphere(&spheres[i]);
        bbox = create_aabb_for_aabb(&bbox, &sphere_box);
    }

    return bbox;
}


bool hit(const Ray *r, const Sphere *sphere, const Interval *ray_t, HitRecord *rec) {
    Vec3 oc = diff_vec3(r->origin, sphere->center);

    double a = length_squared(r->direction);
    double half_b = dot(oc, r->direction);
    double c = length_squared(oc) - sphere->radius*sphere->radius;

    double discrim = half_b*half_b - a*c;
    if (discrim < 0) return false; 
    double sqrtd = sqrt(discrim);

    // Find nearest root that lies within interval
    double root = (-half_b - sqrtd) / a;
    if (!surrounds(ray_t, root)) {
        root = (-half_b + sqrtd) / a;
        if (!surrounds(ray_t, root)) {
            return false;
        }
    }

    rec->t = root;
    rec->p = at(r, rec->t);

    Vec3 outward_normal = diff_vec3(rec->p, sphere->center);
    outward_normal = scale_vec3(outward_normal, 1.0 / sphere->radius);
    set_face_normal(rec, r, outward_normal);
    rec->mat = sphere->mat;
    
    return true;
}

bool hit_list(const Ray *r, int num_spheres, const Sphere spheres[], const Interval *ray_t, HitRecord *record) {
    HitRecord temp_rec = {0};
    bool hit_anything = false;
    double closest_so_far = ray_t->max;

    for (int i = 0; i < num_spheres; i++) {
        Interval cur_interval = {.min=ray_t->min, .max=closest_so_far};
        if (hit(r, &spheres[i], &cur_interval, &temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *record = temp_rec;
        }
    }

    record->num_tests += num_spheres;
    return hit_anything;
}
