#pragma once

#include "aabb.h"
#include "sphere.h"
#include "triangle.h"
#include "quad.h"

typedef enum {
    SPHERE, TRIANGLE, QUAD
} GeomType;

typedef union GeometryData {
    Sphere *sphere;
    Triangle *triangle;
    Quad *quad;
} GeometryData;

typedef struct Geometry {
    GeomType type; 
    Material mat;
    GeometryData data;
} Geometry;

Geometry make_from_sphere(Sphere *sphere) {
    return (Geometry) {
        .type = SPHERE,
        .mat = sphere->mat, 
        .data = {
            .sphere = sphere
        }
    };
}

Geometry make_from_triangle(Triangle *triangle) {
    return (Geometry) {
        .type = TRIANGLE,
        .mat = triangle->mat, 
        .data = {
            .triangle = triangle
        }
    };
}

Geometry make_from_quad(Quad *quad) {
    return (Geometry) {
        .type = QUAD,
        .mat = quad->mat, 
        .data = {
            .quad = quad
        }
    };
}

bool ray_intersect_geom(const Ray *r, const Geometry *geom, const Interval *ray_t, HitRecord *rec, int *num_intersects) {
    switch (geom->type) {
        case SPHERE:
            return ray_intersect_sphere(r, geom->data.sphere, ray_t, rec, num_intersects);
        case TRIANGLE:
            return ray_intersect_triangle(r, geom->data.triangle, ray_t, rec, num_intersects);
        case QUAD:
            return ray_intersect_quad(r, geom->data.quad, ray_t, rec, num_intersects);
    }

    return false;
}

bool ray_intersect_geom_arr(const Ray *r, int num_geoms, const Geometry geoms[], const Interval *ray_t, HitRecord *record, int *num_intersects) {
    HitRecord temp_rec = {0};
    bool hit_anything = false;
    double closest_so_far = ray_t->max;

    for (int i = 0; i < num_geoms; i++) {
        Interval cur_interval = {.min=ray_t->min, .max=closest_so_far};
        if (ray_intersect_geom(r, &geoms[i], &cur_interval, &temp_rec, num_intersects)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *record = temp_rec;
        }
    }

    return hit_anything;
}

AABB create_aabb_for_geom(const Geometry* geom) {
    switch (geom->type) {
        case SPHERE:
            return create_aabb_for_sphere(geom->data.sphere);
        case TRIANGLE:
            return create_aabb_for_triangle(geom->data.triangle);
        case QUAD:
            return create_aabb_for_quad(geom->data.quad);
    }

    return create_empty_aabb();
}
