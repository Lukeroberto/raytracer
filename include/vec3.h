#pragma once

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

vec3 invert(vec3 v) {
    vec3 ret = {
        .x = -v.x,
        .y = -v.y,
        .z = -v.z
    };
    return ret;
}

vec3 add(vec3 u, vec3 v) {
    vec3 ret = {
        .x = u.x + v.x,
        .y = u.y + v.y,
        .z = u.z + v.z
    };

    return ret;
}

vec3 add3(vec3 u, vec3 v, vec3 w) {
    vec3 ret = {
        .x = u.x + v.x + w.x,
        .y = u.y + v.y + w.y,
        .z = u.z + v.z + w.z
    };

    return ret;
}

vec3 diff(vec3 u, vec3 v) {
    vec3 ret = {
        .x = u.x - v.x,
        .y = u.y - v.y,
        .z = u.z - v.z
    };

    return ret;
}

vec3 mult_v(vec3 u, vec3 v) {
    vec3 ret = {
        .x = u.x * v.x,
        .y = u.y * v.y,
        .z = u.z * v.z
    };

    return ret;
}

vec3 mult(vec3 v, double t) {
    vec3 ret = {
        .x = v.x * t,
        .y = v.y * t,
        .z = v.z * t
    };

    return ret;
}

double length_squared(vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

double length(vec3 v) {
    return sqrt(length_squared(v));
}

bool near_zero(vec3 v) {
    double tol = 1e-8;
    return (fabs(v.x) < tol) && (fabs(v.y) < tol) && (fabs(v.z) < tol);
}

vec3 unit_vec(vec3 v) {
    return mult(v, 1.0 / length(v));
}

double dot(vec3 u, vec3 v){
    return u.x * v.x
        + u.y * v.y
        + u.z * v.z;
}

vec3 cross(vec3 u, vec3 v){
   vec3 ret = {
       .x = u.y * v.z - u.z * v.y,
       .y = u.z * v.x - u.x * v.z,
       .z = u.x * v.y - u.y * v.x
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
        .x = random_double_interval(min, max),
        .y = random_double_interval(min, max),
        .z = random_double_interval(min, max)
    };
    return v;
}

vec3 random_in_unit_disk() {
   while (true) {
       vec3 p = {random_double_interval(-1, 1), random_double_interval(-1, 1), 0};
       if (length_squared(p) < 1) {
           return p;
       }
   }
}

vec3 random_in_unit_sphere() {
    while (true) {
        vec3 p = random_vec_interval(-1, 1);
        if (length_squared(p) < 1) {
            return p;
        }
    }
}

vec3 random_unit_vector() {
    vec3 v = random_in_unit_sphere();
    return unit_vec(v);
}

vec3 random_on_hemisphere(vec3 normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) {
        return on_unit_sphere;
    }

    return invert(on_unit_sphere);
}

vec3 reflect(vec3 v, vec3 n) {
    double coeff = -2 * dot(v, n);
    vec3 overlap = mult(n, coeff);

    return add(v, overlap);
}

vec3 refract(vec3 v, vec3 n, double etai_over_etat) {
    vec3 iv = invert(v);
    double cos_theta = fmin(dot(iv, n), 1.0);

    vec3 cos_n = mult(n, cos_theta);
    vec3 temp = add(v, cos_n);
    vec3 r_out_perp = mult(temp, etai_over_etat);
    vec3 r_out_parallel = mult(n, -sqrt(fabs(1.0 - length_squared(r_out_perp))));

    return add(r_out_perp, r_out_parallel);
}

