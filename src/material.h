#ifndef MATERIAL_H
#define MATERIAL_H

#include <stdbool.h>

#include "utils.h"

bool scatter_lambertian(material *material, ray *ray_in, hit_record *rec, color *attenuation, ray *scattered) {
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
    ray scattered_ray = {rec->p, reflected};

    *scattered = scattered_ray;
    *attenuation = material->albedo;

    return true;
}

bool scatter(material *material, ray *ray_in, hit_record *rec, color *attenuation, ray *scattered) {
    switch (material->type) {
        bool ret;
        case LAMBERTIAN:
            ret = scatter_lambertian(material, ray_in, rec, attenuation, scattered);
            return ret;
        case METAL:
            ret = scatter_metal(material, ray_in, rec, attenuation, scattered);
            return ret;
        case DIELECTRIC:
            break;
    }
}


#endif
