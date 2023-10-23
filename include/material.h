#ifndef MATERIAL_H
#define MATERIAL_H

#include <stdbool.h>

#include "utils.h"

bool scatter_lambertian(material *material, hit_record *rec, color *attenuation, ray *scattered) {
    vec3 random_unit_vec = random_unit_vector();
    vec3 scatter_direction = add(&rec->normal, &random_unit_vec);
    if (near_zero(&scatter_direction)) {
        scatter_direction = rec->normal;
    }

    ray scattered_ray = {rec->p, scatter_direction};
    *scattered = scattered_ray;
    *attenuation = material->albedo;

    return true;
}

bool scatter_metal(material *material, ray *ray_in, hit_record *rec, color *attenuation, ray *scattered) {
    vec3 unit = unit_vec(&ray_in->direction);
    vec3 reflected = reflect(&unit, &rec->normal);
    vec3 random_unit = random_unit_vector();
    vec3 fuzzed_unit = mult(&random_unit, material->fuzz);
    vec3 fuzzed_reflected = add(&reflected, &fuzzed_unit);
    ray scattered_ray = {rec->p, fuzzed_reflected};

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

bool scatter_dielectric(material *material, ray *ray_in, hit_record *rec, color *attenuation, ray *scattered) {
    *attenuation = (color) {1.0, 1.0, 1.0};
    double refraction_ratio = rec->front_face ? (1.0/material->ir): material->ir; 

    vec3 unit_direction = unit_vec(&ray_in->direction);
    vec3 iunit_direction = invert(&unit_direction);
    double cos_theta = fmin(dot(&iunit_direction, &rec->normal), 1.0); 
    double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    vec3 direction;
    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
        direction = reflect(&unit_direction, &rec->normal);
    } else {
        direction = refract(&unit_direction, &rec->normal, refraction_ratio);
    }

    *scattered = (ray) {rec->p, direction};
    return true;
}

bool scatter(material *material, ray *ray_in, hit_record *rec, color *attenuation, ray *scattered) {
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


#endif
