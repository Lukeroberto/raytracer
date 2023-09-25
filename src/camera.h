#ifndef CAMERA_H
#define CAMERA_H

#include "utils.h"

#include "material.h"
#include "hittable.h"

typedef struct {
    int image_width, image_height;
    double aspect_ratio;
    int samples_per_pixel;
    int max_depth;
    
    point3 center, pixel00_loc;
    vec3 pixel_delta_u, pixel_delta_v;
} camera;

camera create_camera(int image_width, double aspect_ratio, int samples_per_pixel) {
    int image_height = (int) image_width / aspect_ratio;
    image_height = (image_height < 1) ? 1 : image_height;

    // Camera
    float focal_length = 1.0;
    float viewport_height = 2.0;
    float viewport_width = viewport_height * ((double) image_width / image_height);
    point3 center = {0, 0, 0};

    // vectors across the horizontal and down the vertical viewport edges
    vec3 viewport_u = {viewport_width, 0, 0};
    vec3 viewport_v = {0, -viewport_height, 0};

    // horizontal and vertical deltas from pixel to pixel
    vec3 pixel_delta_u = mult(&viewport_u, 1.0 / image_width);
    vec3 pixel_delta_v = mult(&viewport_v, 1.0 / image_height);

    // Location of upper left pixel
    vec3 z_focal = {0, 0, -focal_length};
    vec3 half_viewport_u = mult(&viewport_u, -0.5);
    vec3 half_viewport_v = mult(&viewport_v, -0.5);

    vec3 view_port_sum = add(&half_viewport_u, &half_viewport_v);
    vec3 viewport_focal_sum = add(&z_focal, &view_port_sum);
    vec3 viewport_upper_left = add(&center, &viewport_focal_sum); 
    vec3 pixel_delta_sum = add(&pixel_delta_u, &pixel_delta_v);
    vec3 scaled_pixel_delta_sum = mult(&pixel_delta_sum, 0.5);

    vec3 pixel00_loc = add(&viewport_upper_left, &scaled_pixel_delta_sum);

    camera camera = {
        .image_width = image_width,
        .image_height = image_height,
        .aspect_ratio = aspect_ratio,
        .samples_per_pixel = samples_per_pixel,
        .max_depth = 50,
        .center = center,
        .pixel00_loc = pixel00_loc,
        .pixel_delta_u = pixel_delta_u,
        .pixel_delta_v = pixel_delta_v
    };
    return camera;
}

color ray_color(ray *r, int depth, sphere world[]) {
    hit_record rec;
    if (depth <= 0) {
        color no_light_gathered = {0, 0, 0};
        return no_light_gathered;
    }
    interval world_int = {.min=0.001, .max=INFINITY};
    if (hit_list(r, world, &world_int, &rec)) {
        ray scattered;
        color attenuation;
        if (scatter(&rec.mat, r, &rec, &attenuation, &scattered)) {
            color color = ray_color(&scattered, depth-1, world);
            return mult_v(&color, &attenuation);
        }
        color no_light_gathered = {0, 0, 0};
        return no_light_gathered;
    }

    vec3 unit = unit_vec(&r->direction);
    float interp = 0.5 * (unit.y + 1.0);
    color start = {1.0, 1.0, 1.0};
    start = mult(&start, 1.0 - interp);
    color end = {0.5, 0.7, 1.0};
    end = mult(&end, interp);
    color ret = add(&start, &end);
    return ret;
}

vec3 pixel_sample_square(camera *camera) {
    double px = -0.5 + random_double();
    double py = -0.5 + random_double();

    vec3 pixel_px = mult(&camera->pixel_delta_u, px);
    vec3 pixel_py = mult(&camera->pixel_delta_v, py);
    return add(&pixel_px, &pixel_py);
}

ray get_ray(int i, int j, camera *camera) {
    vec3 pixel_delta_i = mult(&camera->pixel_delta_u, i);
    vec3 pixel_delta_j = mult(&camera->pixel_delta_v, j);

    point3 pixel_center = add3(&camera->pixel00_loc, &pixel_delta_i, &pixel_delta_j);
    
    vec3 pixel_sample = pixel_sample_square(camera);
    point3 pixel_sample_shifted = add(&pixel_center, &pixel_sample);

    point3 ray_origin = camera->center;
    vec3 ray_dir = diff(&pixel_sample_shifted, &ray_origin);

    ray ret = {.origin = ray_origin, .direction = ray_dir};
    return ret;
}

void render(camera *camera, sphere world[]) {
    // Logging
    char buff[BUFSIZ];
    setvbuf(stderr, buff, _IOFBF, BUFSIZ);

    // Render
    printf("P3\n%d %d\n255\n", camera->image_width, camera->image_height);
    for (int j = 0; j < camera->image_height; ++j) {
        fprintf(stderr, "\rScanlines remaining: %d    ", (camera->image_height - j));
        fflush(stderr);
        for (int i = 0; i < camera->image_width; ++i) {
            color pixel_color = {0, 0, 0};
            for (int sample = 0; sample < camera->samples_per_pixel; ++sample) {
                ray r = get_ray(i, j, camera);
                color ray_c = ray_color(&r, camera->max_depth, world);
                pixel_color = add(&pixel_color, &ray_c);
            }
            write_color(pixel_color, camera->samples_per_pixel);
        }
    }
    fprintf(stderr, "\rDone.                    \n");
}

#endif
