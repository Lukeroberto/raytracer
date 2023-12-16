#pragma once

#include "bvh.h"
#include "color.h"
#include "triangle.h"
#include "utils.h"

#include "material.h"
#include "sphere.h"
#include "vec3.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

typedef struct {
    // Passed in
    int image_width;
    double aspect_ratio;
    int samples_per_pixel;
    int max_depth;
    double vfov;
    Vec3 vup; // camera-relative "up" direction
    Point3 lookfrom;
    Point3 lookat;
    double defocus_angle;
    double focus_dist;

    // Computed
    int image_height;
    Vec3 u, v, w; // camera frame basis vectors
    Point3 center, pixel00_loc;
    Vec3 pixel_delta_u, pixel_delta_v;
    Vec3 defocus_disk_u, defocus_disk_v;
} Camera;

Camera create_camera(int image_width, double aspect_ratio, int samples_per_pixel, int max_depth, double vfov, Point3 lookfrom, Point3 lookat, Vec3 vup, double defocus_angle, double focus_dist) {
    Camera camera;

    // Config
    camera.image_width = image_width;
    camera.aspect_ratio = aspect_ratio;
    camera.samples_per_pixel = samples_per_pixel;
    camera.max_depth = max_depth;
    camera.vfov = vfov;
    camera.vup = vup;
    camera.lookfrom = lookfrom;
    camera.lookat = lookat;
    camera.defocus_angle = defocus_angle;
    camera.focus_dist = focus_dist;

    // Compute height
    int image_height = (int) ((double) image_width / aspect_ratio);
    camera.image_height = (image_height < 1) ? 1 : image_height;

    // Compute viewport, center, and basis vectors
    Vec3 looktowards = diff_vec3(lookfrom, lookat);
    double theta = degrees_to_radians(vfov);
    double h = tan(theta/2);
    double viewport_height = 2.0 * h * focus_dist;
    double viewport_width = viewport_height * ((double) image_width / image_height);

    camera.center = lookfrom;
    
    Vec3 w = unit_vec(looktowards);
    camera.w = w;

    Vec3 vup_cross_w = cross(vup, w);
    Vec3 u = unit_vec(vup_cross_w);
    camera.u = u;

    Vec3 v = cross(w, u);
    camera.v = v;

    // vectors across the horizontal and down the vertical viewport edges
    Vec3 viewport_u = scale_vec3(u, viewport_width);
    Vec3 viewport_v = scale_vec3(v, -viewport_height);

    // horizontal and vertical deltas from pixel to pixel
    camera.pixel_delta_u = scale_vec3(viewport_u, 1.0 / image_width);
    camera.pixel_delta_v = scale_vec3(viewport_v, 1.0 / image_height);

    Vec3 pixel_delta_sum = mult_vec3(camera.pixel_delta_u, camera.pixel_delta_v);
    Vec3 scaled_pixel_delta_sum = scale_vec3(pixel_delta_sum, 0.5);

    // Location of upper left pixel
    Vec3 scaled_vp_u = scale_vec3(viewport_u, -0.5);
    Vec3 scaled_vp_v = scale_vec3(viewport_v, -0.5);
    Vec3 scaled_vp_sum = add_vec3(scaled_vp_u, scaled_vp_v);

    Vec3 scaled_focal_length = scale_vec3(w, -focus_dist);
    Vec3 viewport_upper_left = add3_vec3(camera.center, scaled_focal_length, scaled_vp_sum);

    camera.pixel00_loc = add_vec3(viewport_upper_left, scaled_pixel_delta_sum);

    double defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle/2));
    camera.defocus_disk_u = scale_vec3(u, defocus_radius);
    camera.defocus_disk_v = scale_vec3(v, defocus_radius);

    return camera;
}

Color sky(Vec3 unit_dir) {
    double interp = 0.5 * (unit_dir.y + 1.0);
    Color start = {1.0, 1.0, 1.0};
    start = scale_vec3(start, 1.0 - interp);
    Color end = {0.5, 0.7, 1.0};
    end = scale_vec3(end, interp);
    return add_vec3(start, end);
}

Color ray_color(const Ray *r, int depth, int num_spheres, Sphere world[], int *num_intersects) {
    HitRecord rec = {0};
    if (depth <= 0) {
        Color no_light_gathered = {0, 0, 0};
        return no_light_gathered;
    }
    Interval world_int = {.min=0.001, .max=INFINITY};
    if (ray_intersect_sphere_arr(r, num_spheres, world, &world_int, &rec, num_intersects)) {
        Ray scattered;
        Color attenuation;
        if (scatter(&rec.mat, r, &rec, &attenuation, &scattered)) {
            Color color = ray_color(&scattered, depth-1, num_spheres, world, num_intersects);
            return mult_vec3(color, attenuation);
        }
        Color no_light_gathered = {0, 0, 0};
        return no_light_gathered;
    }

    return sky(unit_vec(r->direction));
}

Color ray_color_triangle(const Ray *r, int depth, int num_triangles, Triangle mesh[], int *num_intersects) {
    HitRecord rec = {0};
    if (depth <= 0) {
        Color no_light_gathered = {0, 0, 0};
        return no_light_gathered;
    }
    Interval world_int = {.min=0.001, .max=INFINITY};
    if (ray_intersect_triangle_arr(r, num_triangles, mesh, &world_int, &rec, num_intersects)) {
        Ray scattered;
        Color attenuation;
        if (scatter(&rec.mat, r, &rec, &attenuation, &scattered)) {
            Color color = ray_color_triangle(&scattered, depth-1, num_triangles, mesh, num_intersects);
            return mult_vec3(color, attenuation);
        }

        // Should never happen, scattering always returns true
        printf("Something bad happened, no scattering for mat %d\n", rec.mat.type);
        Color no_light_gathered = {0, 0, 0};
        return no_light_gathered;
    }

    return sky(unit_vec(r->direction));
}

Color ray_color_bvh(Ray *r, int depth, BvhNode *bvh, int *num_intersects) {
    HitRecord rec = {0};
    if (depth <= 0) {
        Color no_light_gathered = {0, 0, 0};
        return no_light_gathered;
    }
    Interval world_int = {.min=0.001, .max=INFINITY};
    if (ray_intersect_bvh(bvh, r, world_int, &rec, num_intersects, 0)) {
        Ray scattered;
        Color attenuation;
        if (scatter(&rec.mat, r, &rec, &attenuation, &scattered)) {
            Color color = ray_color_bvh(&scattered, depth-1, bvh, num_intersects);
            return mult_vec3(color, attenuation);
        }

        // Should never happen, scattering always returns true
        printf("Something bad happened, no scattering for mat %d\n", rec.mat.type);
        Color no_light_gathered = {0, 0, 0};
        return no_light_gathered;
    }

    return sky(unit_vec(r->direction));
}

Vec3 pixel_sample_square(Camera *camera) {
    double px = -0.5 + random_double();
    double py = -0.5 + random_double();

    Vec3 pixel_px = scale_vec3(camera->pixel_delta_u, px);
    Vec3 pixel_py = scale_vec3(camera->pixel_delta_v, py);
    return add_vec3(pixel_px, pixel_py);
}

Point3 defocus_disk_sample(Camera *camera) {
    Point3 p = random_in_unit_disk();

    Vec3 defocus_p_u = scale_vec3(camera->defocus_disk_u, p.x);
    Vec3 defocus_p_v = scale_vec3(camera->defocus_disk_v, p.y);
    return add3_vec3(camera->center, defocus_p_u, defocus_p_v);
}

Ray get_ray(int i, int j, Camera *camera) {
    Vec3 pixel_delta_i = scale_vec3(camera->pixel_delta_u, i);
    Vec3 pixel_delta_j = scale_vec3(camera->pixel_delta_v, j);

    Point3 pixel_center = add3_vec3(camera->pixel00_loc, pixel_delta_i, pixel_delta_j);
    
    Vec3 pixel_sample = pixel_sample_square(camera);
    Point3 pixel_sample_shifted = add_vec3(pixel_center, pixel_sample);

    //Point3 ray_origin = (camera->defocus_angle <= 0) ? camera->center : defocus_disk_sample(camera);
    Point3 ray_origin = camera->center;
    Vec3 ray_dir = diff_vec3(pixel_sample_shifted, ray_origin);

    Ray ret = {.origin = ray_origin, .direction = ray_dir};
    return ret;
}

int render_spheres(Camera *camera, int num_spheres, Sphere world[], SDL_Surface *surface, int *num_intersects) {
    for (int j = 0; j < camera->image_height; ++j) {
        for (int i = 0; i < camera->image_width; ++i) {
            Color pixel_color = {0, 0, 0};
            for (int sample = 0; sample < camera->samples_per_pixel; ++sample) {
                Ray r = get_ray(i, j, camera);
                Color ray_c = ray_color(&r, camera->max_depth, num_spheres, world, num_intersects);
                pixel_color = add_vec3(pixel_color, ray_c);
            }
            set_pixel_buffer(pixel_color, camera->samples_per_pixel, i + j * surface->w, surface);
        }
    }

    return EXIT_SUCCESS;
}

int render_triangles(Camera *camera, int num_triangles, Triangle mesh[], SDL_Surface *surface, int *num_intersects) {
    for (int j = 0; j < camera->image_height; ++j) {
        for (int i = 0; i < camera->image_width; ++i) {
            Color pixel_color = {0, 0, 0};
            for (int sample = 0; sample < camera->samples_per_pixel; ++sample) {
                Ray r = get_ray(i, j, camera);
                Color ray_c = ray_color_triangle(&r, camera->max_depth, num_triangles, mesh, num_intersects);
                pixel_color = add_vec3(pixel_color, ray_c);
            }
            set_pixel_buffer(pixel_color, camera->samples_per_pixel, i + j * surface->w, surface);
        }
    }

    return EXIT_SUCCESS;
}

int render_bvh(Camera *camera, BvhNode *bvh, SDL_Surface *surface, int *num_intersects ) {
    for (int j = 0; j < camera->image_height; ++j) {
        for (int i = 0; i < camera->image_width; ++i) {
            Color pixel_color = {0, 0, 0};
            //int t = *num_intersects;
            for (int sample = 0; sample < camera->samples_per_pixel; ++sample) {
                Ray r = get_ray(i, j, camera);
                Color ray_c = ray_color_bvh(&r, camera->max_depth, bvh, num_intersects);
                pixel_color = add_vec3(pixel_color, ray_c);
            }
            //printf("tests on ray: %d\n", *num_intersects - t);
            set_pixel_buffer(pixel_color, camera->samples_per_pixel, i + j * surface->w, surface);
        }
    }

    return EXIT_SUCCESS;
}

