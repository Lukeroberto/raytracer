#include <stdio.h>

#include "vec3.h"
#include "ray.h"
#include "color.h"

double hit_sphere(point3 *center, double radius, ray *r) {
    vec3 inv_center = invert(center);
    vec3 oc = add(&r->origin, &inv_center);

    double a = dot(&r->direction, &r->direction);
    double b = 2.0 * dot(&oc, &r->direction);
    double c = dot(&oc, &oc) - radius*radius;

    double discrim = b*b - 4 *a*c;
    if (discrim < 0) {
        return -1.0;
    } else {
        return (-b - sqrt(discrim)) / (2.0*a);
    }
}

color ray_color(ray *r) {

    point3 center = {0, 0, -1};
    double t = hit_sphere(&center, 0.5, r); 
    if (t > 0.0) {
        point3 rat = at(r, t);
        point3 down = {0, 0, -1};
        point3 sum = add(&rat, &down);
        vec3 n = unit_vec(&sum);
        color normal_color = {n.x + 1.0, n.y + 1.0, n.z + 1.0};
        normal_color = mult(&normal_color, 0.5);
        return normal_color;
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
    int image_width = 400;

    // Calculate image height, ensure at least 1
    int image_height = (int) image_width / aspect_ratio;
    image_height = (image_height < 1) ? 1 : image_height;

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

            color pixel_color = ray_color(&r);
            write_color(pixel_color);
        }
    }
    fprintf(stderr, "\rDone.                    \n");
}
