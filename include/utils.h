#pragma once

#include<stdlib.h>

const double pi = 3.1415926535897932385;

double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

static unsigned int g_seed = 11;

// Used to seed the generator.           
//inline void fast_srand(int seed) {
//    g_seed = seed;
//}

// Compute a pseudorandom integer.
// Output value in range [0, 32767]
static inline int fast_rand(void) {
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16)&0x7FFF;
}

double random_double() {
    return (double) fast_rand() / (32767.0);
}

double random_double_interval(double min, double max) {
    return min + (max - min) * random_double();
}

#include "ray.h"
#include "vec3.h"
#include "color.h"

// Define some types
typedef enum {
    LAMBERTIAN,
    METAL,
    DIELECTRIC
} material_type;

typedef struct {
    material_type type;
    color albedo;
    double fuzz;
    double ir;
} material;

typedef struct {
    point3 p;
    vec3 normal;
    double t;
    bool front_face;
    material mat;
} hit_record;

