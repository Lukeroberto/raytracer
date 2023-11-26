#pragma once

#include <stdbool.h>

#include "utils.h"
#include "types.h"
#include "ray.h"
#include "color.h"

bool scatter_lambertian(const Material *material, const HitRecord *rec, Color *attenuation, Ray *scattered) {
    Vec3 random_unit_vec = random_unit_vector();
    Vec3 scatter_direction = add_vec3(rec->normal, random_unit_vec);
    if (near_zero(scatter_direction)) {
        scatter_direction = rec->normal;
    }

    Ray scattered_ray = {rec->p, scatter_direction};
    *scattered = scattered_ray;
    *attenuation = material->albedo;

    return true;
}

bool scatter_metal(const Material *material, const Ray *ray_in, const HitRecord *rec, Color *attenuation, Ray *scattered) {
    Vec3 unit = unit_vec(ray_in->direction);
    Vec3 reflected = reflect(unit, rec->normal);
    Vec3 random_unit = random_unit_vector();
    Vec3 fuzzed_unit = scale_vec3(random_unit, material->fuzz);
    Vec3 fuzzed_reflected = add_vec3(reflected, fuzzed_unit);
    Ray scattered_ray = {rec->p, fuzzed_reflected};

    *scattered = scattered_ray;
    *attenuation = material->albedo;

    return true;
}

double reflectance(double cosine, double ref_indx) {
    // Schlick's approximation
    double r0 = (1-ref_indx) / (1+ref_indx);
    r0 = r0*r0;

    return r0 + (1-r0)*pow((1-cosine), 5);
}

bool scatter_dielectric(const Material *material, const Ray *ray_in, const HitRecord *rec, Color *attenuation, Ray *scattered) {
    *attenuation = (Color) {1.0, 1.0, 1.0};
    double refraction_ratio = rec->front_face ? (1.0/material->ir): material->ir; 

    Vec3 unit_direction = unit_vec(ray_in->direction);
    Vec3 iunit_direction = invert_vec3(unit_direction);
    double cos_theta = fmin(dot(iunit_direction, rec->normal), 1.0); 
    double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    Vec3 direction;
    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
        direction = reflect(unit_direction, rec->normal);
    } else {
        direction = refract(unit_direction, rec->normal, refraction_ratio);
    }

    *scattered = (Ray) {rec->p, direction};
    return true;
}

bool scatter(const Material *material, const Ray *ray_in, const HitRecord *rec, Color *attenuation, Ray *scattered) {
    switch (material->type) {
        bool ret;
        case LAMBERTIAN:
            ret = scatter_lambertian(material, rec, attenuation, scattered);
            return ret;
        case METAL:
            ret = scatter_metal(material, ray_in, rec, attenuation, scattered);
            return ret;
        case DIELECTRIC:
            ret = scatter_dielectric(material, ray_in, rec, attenuation, scattered);
            return ret;
    }

    return false;
}
