#ifndef VEC3_H
#define VEC3_H

#include<stdio.h>
#include<math.h>

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

void print_vec(vec3 *v) {
    printf("vec3: {%f, %f, %f}", v->x, v->y, v->z);
}

#endif
