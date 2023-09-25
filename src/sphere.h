#ifndef SPHERE_H
#define SPHERE_H

#include <stdbool.h>
#include <math.h>

#include "utils.h"
#include "interval.h"

#include "hittable.h"
#include "material.h"

typedef struct {
    point3 center;
    double radius;
    material mat;
} sphere;

bool hit(ray *r, sphere *sphere, interval *ray_t, hit_record *rec) {
    vec3 oc = diff(&r->origin, &sphere->center);

    double a = length_squared(&r->direction);
    double half_b = dot(&oc, &r->direction);
    double c = length_squared(&oc) - sphere->radius*sphere->radius;

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

    vec3 outward_normal = diff(&rec->p, &sphere->center);
    outward_normal = mult(&outward_normal, 1.0 / sphere->radius);
    set_face_normal(rec, r, &outward_normal);
    rec->mat = sphere->mat;
    
    return true;
}

bool hit_list(ray *r, sphere spheres[], interval *ray_t, hit_record *record) {
    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = ray_t->max;

    for (int i = 0; i < 5; i++) {
        interval cur_interval = {.min=ray_t->min, .max=closest_so_far};
        if (hit(r, &spheres[i], &cur_interval, &temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *record = temp_rec;
        }
    }

    return hit_anything;
}

#endif

