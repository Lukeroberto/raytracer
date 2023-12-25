#pragma once

#include "texture.h"
#include "vec3.h"
#include "color.h"

// Define some types
typedef enum MaterialType {
    LAMBERTIAN,
    LAMBERTIAN_TEXTURE,
    METAL,
    DIELECTRIC
} MaterialType;

typedef struct Material {
    MaterialType type;
    Color albedo;
    CheckerTexture texture;
    double fuzz;
    double ir;
} Material;

typedef struct HitRecord{
    Point3 p;
    Vec3 normal;
    double t;
    double u;
    double v;
    bool front_face;
    Material mat;
} HitRecord;
