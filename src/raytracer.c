#include <stdio.h>

#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "sphere.h"

color ray_color(ray *r, sphere world[]) {

    hit_record rec;
    interval world_int = {.min=0, .max=INFINITY};
    if (hit_list(r, world, &world_int, &rec)) {
        color norm_color = add(&rec.normal, &((color) {1, 1, 1}));
        return mult(&norm_color, 0.5);
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

int main() {
    // Logging
    char buff[BUFSIZ];
    setvbuf(stderr, buff, _IOFBF, BUFSIZ);

    // Image
    float aspect_ratio = 16.0 / 9.0;
    int image_width = 1600;

    // Calculate image height, ensure at least 1
    int image_height = (int) image_width / aspect_ratio;
    image_height = (image_height < 1) ? 1 : image_height;

    // World
    point3 p1 = {0, 0, -1};
    point3 p2 = {0, -100.5, -1};
    sphere s1 = {.center = p1, .radius = 0.5};
    sphere s2 = {.center = p2, .radius = 100};

    sphere world[2];
    world[0] = s1;
    world[1] = s2;

    // Camera
    float focal_length = 1.0;
    float viewport_height = 2.0;
    float viewport_width = viewport_height * ((double) image_width / image_height);
    point3 camera_center = {0, 0, 0};

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
    vec3 viewport_upper_left = add(&camera_center, &viewport_focal_sum); 
    vec3 pixel_delta_sum = add(&pixel_delta_u, &pixel_delta_v);
    vec3 scaled_pixel_delta_sum = mult(&pixel_delta_sum, 0.5);

    vec3 pixel00_loc = add(&viewport_upper_left, &scaled_pixel_delta_sum);

    // Render
    printf("P3\n%d %d\n255\n", image_width, image_height);
    for (int j = 0; j < image_height; ++j) {
        fprintf(stderr, "\rScanlines remaining: %d", (image_height - j));
        fflush(stderr);
        for (int i = 0; i < image_width; ++i) {
            vec3 pixel_loc_i = mult(&pixel_delta_u, i);
            vec3 pixel_loc_j = mult(&pixel_delta_v, j);
            vec3 pixel_sum = add(&pixel_loc_i, &pixel_loc_j);

            vec3 pixel_center = add(&pixel00_loc, &pixel_sum);
            

            vec3 invert_camera_center = invert(&camera_center);
            vec3 ray_direction = add(&pixel_center, &invert_camera_center);
            ray r = {camera_center, ray_direction};

            color pixel_color = ray_color(&r, world);
            write_color(pixel_color);
        }
    }
    fprintf(stderr, "\rDone.                    \n");
}
