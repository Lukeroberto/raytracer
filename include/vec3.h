#pragma once

#include "utils.h"
#include<stdio.h>
#include<stdbool.h>
#include<math.h>

typedef struct Vec3 {
    double x;
    double y;
    double z;
} Vec3;

typedef Vec3 Point3;


Vec3 invert_vec3(Vec3 v) {
    Vec3 ret = {
        .x = -v.x,
        .y = -v.y,
        .z = -v.z
    };
    return ret;
}

Vec3 add_vec3(Vec3 u, Vec3 v) {
    Vec3 ret = {
        .x = u.x + v.x,
        .y = u.y + v.y,
        .z = u.z + v.z
    };

    return ret;
}

Vec3 add3_vec3(Vec3 u, Vec3 v, Vec3 w) {
    Vec3 ret = {
        .x = u.x + v.x + w.x,
        .y = u.y + v.y + w.y,
        .z = u.z + v.z + w.z
    };

    return ret;
}

Vec3 diff_vec3(Vec3 u, Vec3 v) {
    Vec3 ret = {
        .x = u.x - v.x,
        .y = u.y - v.y,
        .z = u.z - v.z
    };

    return ret;
}

Vec3 mult_vec3(Vec3 u, Vec3 v) {
    Vec3 ret = {
        .x = u.x * v.x,
        .y = u.y * v.y,
        .z = u.z * v.z
    };

    return ret;
}

Vec3 scale_vec3(Vec3 v, double t) {
    Vec3 ret = {
        .x = v.x * t,
        .y = v.y * t,
        .z = v.z * t
    };

    return ret;
}

double length_squared(Vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

double length(Vec3 v) {
    return sqrt(length_squared(v));
}

bool near_zero(Vec3 v) {
    double tol = 1e-8;
    return (fabs(v.x) < tol) && (fabs(v.y) < tol) && (fabs(v.z) < tol);
}

Vec3 unit_vec(Vec3 v) {
    return scale_vec3(v, 1.0 / length(v));
}

double dot(Vec3 u, Vec3 v){
    return u.x * v.x
        + u.y * v.y
        + u.z * v.z;
}

Vec3 cross(Vec3 u, Vec3 v){
   Vec3 ret = {
       .x = u.y * v.z - u.z * v.y,
       .y = u.z * v.x - u.x * v.z,
       .z = u.x * v.y - u.y * v.x
   };
   return ret;
}

Vec3 random_vec() {
    Vec3 v = {
        .x = random_double(),
        .y = random_double(),
        .z = random_double()
    };
    return v;
}

Vec3 random_vec_interval(double min, double max) {
    Vec3 v = {
        .x = random_double_interval(min, max),
        .y = random_double_interval(min, max),
        .z = random_double_interval(min, max)
    };
    return v;
}

Vec3 random_in_unit_disk() {
   while (true) {
       Vec3 p = {random_double_interval(-1, 1), random_double_interval(-1, 1), 0};
       if (length_squared(p) < 1) {
           return p;
       }
   }
}

Vec3 random_in_unit_sphere() {
    while (true) {
        Vec3 p = random_vec_interval(-1, 1);
        if (length_squared(p) < 1) {
            return p;
        }
    }
}

Vec3 random_unit_vector() {
    Vec3 v = random_in_unit_sphere();
    return unit_vec(v);
}

Vec3 random_on_hemisphere(Vec3 normal) {
    Vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) {
        return on_unit_sphere;
    }

    return invert_vec3(on_unit_sphere);
}

Vec3 reflect(Vec3 v, Vec3 n) {
    double coeff = -2 * dot(v, n);
    Vec3 overlap = scale_vec3(n, coeff);

    return add_vec3(v, overlap);
}

Vec3 refract(Vec3 v, Vec3 n, double etai_over_etat) {
    Vec3 iv = invert_vec3(v);
    double cos_theta = fmin(dot(iv, n), 1.0);

    Vec3 cos_n = scale_vec3(n, cos_theta);
    Vec3 temp = add_vec3(v, cos_n);
    Vec3 r_out_perp = scale_vec3(temp, etai_over_etat);
    Vec3 r_out_parallel = scale_vec3(n, -sqrt(fabs(1.0 - length_squared(r_out_perp))));

    return add_vec3(r_out_perp, r_out_parallel);
}

