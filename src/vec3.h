#ifndef VEC3_H
#define VEC3_H

#include<stdio.h>
#include<stdbool.h>
#include<math.h>

#include "utils.h"

typedef struct vec3 {
    double x;
    double y;
    double z;
} vec3;

typedef vec3 point3;

vec3 invert(vec3 *v) {
    vec3 ret = {
        .x = -v->x,
        .y = -v->y,
        .z = -v->z
    };
    return ret;
}

vec3 add(vec3 *u, vec3 *v) {
    vec3 ret = {
        .x = u->x + v->x,
        .y = u->y + v->y,
        .z = u->z + v->z
    };

    return ret;
}

vec3 add3(vec3 *u, vec3 *v, vec3 *w) {
    vec3 ret = {
        .x = u->x + v->x + w->x,
        .y = u->y + v->y + w->y,
        .z = u->z + v->z + w->z
    };

    return ret;
}

vec3 diff(vec3 *u, vec3 *v) {
    vec3 ret = {
        .x = u->x - v->x,
        .y = u->y - v->y,
        .z = u->z - v->z
    };

    return ret;
}

vec3 mult(vec3 *v, double t) {
    vec3 ret = {
        .x = v->x * t,
        .y = v->y * t,
        .z = v->z * t
    };

    return ret;
}

double length_squared(vec3 *v) {
    return v->x * v->x + v->y * v->y + v->z * v->z;
}

double length(vec3 *v) {
    return sqrt(length_squared(v));
}

vec3 unit_vec(vec3 *v) {
    return mult(v, 1.0 / length(v));
}

double dot(vec3 *u, vec3 *v){
    return u->x * v->x
        + u->y * v->y
        + u->z * v->z;
}

vec3 cross(vec3 *u, vec3 *v){
   vec3 ret = {
       .x = u->y * v->z - u->z * v->y,
       .y = u->z * v->x - u->x * v->z,
       .z = u->x * v->y - u->y * v->x
   };
   return ret;
}

vec3 random_vec() {
    vec3 v = {
        .x = random_double(),
        .y = random_double(),
        .z = random_double()
    };
    return v;
}

vec3 random_vec_interval(double min, double max) {
    vec3 v = {
        .x = random_double(min, max),
        .y = random_double(min, max),
        .z = random_double(min, max)
    };
    return v;
}

vec3 random_in_unit_sphere() {
    while (true) {
        vec3 p = random_vec_interval(-1, 1);
        if (length_squared(&p) < 1) {
            return p;
        }
    }
}

vec3 random_unit_vector() {
    vec3 v = random_in_unit_sphere();
    return unit_vec(&v);
}

vec3 random_on_hemisphere(vec3 *normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(&on_unit_sphere, normal) > 0.0) {
        return on_unit_sphere;
    }

    return invert(&on_unit_sphere);
}

#endif
