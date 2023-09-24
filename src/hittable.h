#ifndef HITTABLE_H
#define HITTABLE_H

#include <stdbool.h>

#include "ray.h"

typedef struct {
    point3 p;
    vec3 normal;
    double t;
    bool front_face;
} hit_record;

void set_face_normal(hit_record *record, ray *r, vec3 *outward_normal) {
    // outward normal is assumed to be unit
    record->front_face = dot(&r->direction, outward_normal) < 0;
    record->normal = record->front_face ? *outward_normal : invert(outward_normal);
}

#endif