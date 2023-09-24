#ifndef SPHERE_H
#define SPHERE_H

#include <stdbool.h>
#include <math.h>

#include "vec3.h"
#include "hittable.h"

typedef struct {
    point3 center;
    double radius;

} sphere;

bool hit(ray *r, sphere *sphere, double ray_tmin, double ray_tmax, hit_record *rec) {
    vec3 oc = diff(&r->origin, &sphere->center);

    double a = length_squared(&r->direction);
    double half_b = dot(&oc, &r->direction);
    double c = length_squared(&oc) - sphere->radius*sphere->radius;

    double discrim = half_b*half_b - a*c;
    if (discrim < 0) return false; 
    double sqrtd = sqrt(discrim);

    // Find nearest root that lies within interval
    double root = (-half_b - sqrtd) / a;
    if (root <= ray_tmin || root >= ray_tmax) {
        return false;
    }

    rec->t = root;
    rec->p = at(r, rec->t);

    vec3 outward_normal = diff(&rec->p, &sphere->center);
    outward_normal = mult(&outward_normal, 1.0 / sphere->radius);
    set_face_normal(rec, r, &outward_normal);
    
    return true;
}

bool hit_list(ray *r, sphere spheres[2], double ray_tmin, double ray_tmax, hit_record *record) {
    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = ray_tmax;

    for (int i = 0; i < 2; i++) {
        if (hit(r, &spheres[i], ray_tmin, closest_so_far, &temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *record = temp_rec;
        }
    }

    return hit_anything;
}

#endif

