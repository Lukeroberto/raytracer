#ifndef CAMERA_H
#define CAMERA_H

#include "bvh.h"
#include "color.h"
#include "utils.h"

#include "material.h"
#include "hittable.h"
#include "sphere.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

typedef struct {
    // Passed in
    int image_width;
    double aspect_ratio;
    int samples_per_pixel;
    int max_depth;
    double vfov;
    vec3 vup; // camera-relative "up" direction
    point3 lookfrom;
    point3 lookat;
    double defocus_angle;
    double focus_dist;

    // Computed
    int image_height;
    vec3 u, v, w; // camera frame basis vectors
    point3 center, pixel00_loc;
    vec3 pixel_delta_u, pixel_delta_v;
    vec3 defocus_disk_u, defocus_disk_v;
} camera;

camera create_camera(int image_width, double aspect_ratio, int samples_per_pixel, int max_depth, double vfov, point3 lookfrom, point3 lookat, vec3 vup, double defocus_angle, double focus_dist) {
    camera camera;

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
    vec3 looktowards = diff(lookfrom, lookat);
    double theta = degrees_to_radians(vfov);
    double h = tan(theta/2);
    double viewport_height = 2.0 * h * focus_dist;
    double viewport_width = viewport_height * ((double) image_width / image_height);

    camera.center = lookfrom;
    
    vec3 w = unit_vec(looktowards);
    camera.w = w;

    vec3 vup_cross_w = cross(vup, w);
    vec3 u = unit_vec(vup_cross_w);
    camera.u = u;

    vec3 v = cross(w, u);
    camera.v = v;

    // vectors across the horizontal and down the vertical viewport edges
    vec3 viewport_u = mult(u, viewport_width);
    vec3 viewport_v = mult(v, -viewport_height);

    // horizontal and vertical deltas from pixel to pixel
    camera.pixel_delta_u = mult(viewport_u, 1.0 / image_width);
    camera.pixel_delta_v = mult(viewport_v, 1.0 / image_height);

    vec3 pixel_delta_sum = add(camera.pixel_delta_u, camera.pixel_delta_v);
    vec3 scaled_pixel_delta_sum = mult(pixel_delta_sum, 0.5);

    // Location of upper left pixel
    vec3 scaled_vp_u = mult(viewport_u, -0.5);
    vec3 scaled_vp_v = mult(viewport_v, -0.5);
    vec3 scaled_vp_sum = add(scaled_vp_u, scaled_vp_v);

    vec3 scaled_focal_length = mult(w, -focus_dist);
    vec3 viewport_upper_left = add3(camera.center, scaled_focal_length, scaled_vp_sum);

    camera.pixel00_loc = add(viewport_upper_left, scaled_pixel_delta_sum);

    double defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle/2));
    camera.defocus_disk_u = mult(u, defocus_radius);
    camera.defocus_disk_v = mult(v, defocus_radius);

    return camera;
}

color ray_color(ray *r, int depth, int num_spheres, sphere world[]) {
    hit_record rec = {0};
    if (depth <= 0) {
        color no_light_gathered = {0, 0, 0};
        return no_light_gathered;
    }
    interval world_int = {.min=0.001, .max=INFINITY};
    if (hit_list(r, num_spheres, world, &world_int, &rec)) {
        ray scattered;
        color attenuation;
        if (scatter(&rec.mat, r, &rec, &attenuation, &scattered)) {
            color color = ray_color(&scattered, depth-1, num_spheres, world);
            return mult_v(color, attenuation);
        }
        color no_light_gathered = {0, 0, 0};
        return no_light_gathered;
    }

    vec3 unit = unit_vec(r->direction);
    double interp = 0.5 * (unit.y + 1.0);
    color start = {1.0, 1.0, 1.0};
    start = mult(start, 1.0 - interp);
    color end = {0.5, 0.7, 1.0};
    end = mult(end, interp);
    color ret = add(start, end);
    return ret;
}

color ray_color_bvh(ray *r, int depth, bvh_node *bvh) {
    hit_record rec = {0};
    if (depth <= 0) {
        color no_light_gathered = {0, 0, 0};
        return no_light_gathered;
    }

    interval world_int = {.min=0.001, .max=INFINITY};
    if (ray_intersect_bvh(bvh, r, world_int, &rec)) {
        ray scattered;
        color attenuation;
        if (scatter(&rec.mat, r, &rec, &attenuation, &scattered)) {
            color color = ray_color_bvh(&scattered, depth-1, bvh);
            //printf("ray checks in bvh: %d", rec.num_tests);
            return mult_v(color, attenuation);
        }
        color no_light_gathered = {0, 0, 0};
        return no_light_gathered;
    }

    vec3 unit = unit_vec(r->direction);
    double interp = 0.5 * (unit.y + 1.0);
    color start = {1.0, 1.0, 1.0};
    start = mult(start, 1.0 - interp);
    color end = {0.5, 0.7, 1.0};
    end = mult(end, interp);
    color ret = add(start, end);
    return ret;
}

vec3 pixel_sample_square(camera *camera) {
    double px = -0.5 + random_double();
    double py = -0.5 + random_double();

    vec3 pixel_px = mult(camera->pixel_delta_u, px);
    vec3 pixel_py = mult(camera->pixel_delta_v, py);
    return add(pixel_px, pixel_py);
}

point3 defocus_disk_sample(camera *camera) {
    point3 p = random_in_unit_disk();

    vec3 defocus_p_u = mult(camera->defocus_disk_u, p.x);
    vec3 defocus_p_v = mult(camera->defocus_disk_v, p.y);
    return add3(camera->center, defocus_p_u, defocus_p_v);
}

ray get_ray(int i, int j, camera *camera) {
    vec3 pixel_delta_i = mult(camera->pixel_delta_u, i);
    vec3 pixel_delta_j = mult(camera->pixel_delta_v, j);

    point3 pixel_center = add3(camera->pixel00_loc, pixel_delta_i, pixel_delta_j);
    
    vec3 pixel_sample = pixel_sample_square(camera);
    point3 pixel_sample_shifted = add(pixel_center, pixel_sample);

    //point3 ray_origin = (camera->defocus_angle <= 0) ? camera->center : defocus_disk_sample(camera);
    point3 ray_origin = camera->center;
    vec3 ray_dir = diff(pixel_sample_shifted, ray_origin);

    ray ret = {.origin = ray_origin, .direction = ray_dir};
    return ret;
}

int render(camera *camera, int num_spheres, sphere world[], SDL_Renderer *renderer) {
    // Render
    for (int j = 0; j < camera->image_height; ++j) {
        for (int i = 0; i < camera->image_width; ++i) {
            color pixel_color = {0, 0, 0};
            for (int sample = 0; sample < camera->samples_per_pixel; ++sample) {
                ray r = get_ray(i, j, camera);
                color ray_c = ray_color(&r, camera->max_depth, num_spheres, world);
                pixel_color = add(pixel_color, ray_c);
            }
            //write_color(pixel_color, camera->samples_per_pixel);
            set_window_pixel(pixel_color, camera->samples_per_pixel, i, j, renderer);
        }
    }

    return EXIT_SUCCESS;
}

int render_bvh(camera *camera, bvh_node *bvh, SDL_Renderer *renderer) {
    // Render
    for (int j = 0; j < camera->image_height; ++j) {
        for (int i = 0; i < camera->image_width; ++i) {
            color pixel_color = {0, 0, 0};
            for (int sample = 0; sample < camera->samples_per_pixel; ++sample) {
                ray r = get_ray(i, j, camera);
                color ray_c = ray_color_bvh(&r, camera->max_depth, bvh);
                pixel_color = add(pixel_color, ray_c);
            }
            //write_color(pixel_color, camera->samples_per_pixel);
            set_window_pixel(pixel_color, camera->samples_per_pixel, i, j, renderer);
        }
    }

    return EXIT_SUCCESS;
}

#endif
