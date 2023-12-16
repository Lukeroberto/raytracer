#pragma once

#include "vec3.h"
#include "color.h"

// Define some types
typedef enum MaterialType {
    LAMBERTIAN,
    METAL,
    DIELECTRIC
} MaterialType;

typedef struct Material {
    MaterialType type;
    Color albedo;
    double fuzz;
    double ir;
} Material;

typedef struct HitRecord{
    Point3 p;
    Vec3 normal;
    double t;
    bool front_face;
    Material mat;
} HitRecord;
