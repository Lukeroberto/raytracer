#pragma once

#include <stdbool.h>
#include <math.h>

#include "utils.h"
#include "interval.h"
#include "aabb.h"
#include "hittable.h"
#include "material.h"

typedef struct {
    point3 center;
    double radius;
    material mat;
} sphere;

void print_sphere(sphere *sphere) {
    printf("sphere: (center[%f, %f, %f], r[%f], material[%d])\n", sphere->center.x, sphere->center.y, sphere->center.z, sphere->radius, sphere->mat.type);
}

sphere make_sphere(point3 p, double r, material mat) {
    sphere s = {
        .center = p,
        .radius = r,
        .mat = mat
    };
    return s;
}

aabb create_aabb_for_sphere(sphere* s) {
    vec3 radius_vec = {.x = s->radius, .y = s->radius, .z = s->radius};
    point3 min_pt = diff(s->center, radius_vec);
    point3 max_pt = add(s->center, radius_vec);
    return create_aabb_for_point(min_pt, max_pt);
}

aabb create_aabb_for_array_sphere(sphere spheres[], int num_spheres) {
    aabb bbox = create_empty_aabb();
    for (int i = 0; i < num_spheres; i++) {
        aabb sphere_box = create_aabb_for_sphere(&spheres[i]);
        bbox = create_aabb_for_aabb(&bbox, &sphere_box);
    }

    return bbox;
}


bool hit(ray *r, sphere *sphere, interval *ray_t, hit_record *rec) {
    vec3 oc = diff(r->origin, sphere->center);

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

    vec3 outward_normal = diff(rec->p, sphere->center);
    outward_normal = mult(outward_normal, 1.0 / sphere->radius);
    set_face_normal(rec, r, outward_normal);
    rec->mat = sphere->mat;
    
    return true;
}

bool hit_list(ray *r, int num_spheres, sphere spheres[], interval *ray_t, hit_record *record) {
    hit_record temp_rec = {0};
    bool hit_anything = false;
    double closest_so_far = ray_t->max;

    for (int i = 0; i < num_spheres; i++) {
        interval cur_interval = {.min=ray_t->min, .max=closest_so_far};
        if (hit(r, &spheres[i], &cur_interval, &temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *record = temp_rec;
        }
    }

    record->num_tests += num_spheres;
    return hit_anything;
}
