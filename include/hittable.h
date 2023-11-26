#pragma once

#include <stdbool.h>

#include "types.h"
#include "vec3.h"
#include "ray.h"

void set_face_normal(HitRecord *record, Ray *r, Vec3 outward_normal) {
    // outward normal is assumed to be unit
    record->front_face = dot(r->direction, outward_normal) < 0;
    record->normal = record->front_face ? outward_normal : invert_vec3(outward_normal);
}
