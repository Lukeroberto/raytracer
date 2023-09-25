#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#include "sphere.h"
#include "camera.h"

sphere make_sphere(double x, double y, double z, double r, material_type type, color color) {
    point3 o = {x, y, z};
    material mat = {
        .type = type,
        .albedo = color
    };
    sphere s = {
        .center = o,
        .radius = r,
        .mat = mat
    };
    fprintf(stderr, "Created sphere at (%f, %f, %f), size %f, type: %s, color (%f, %f, %f)\n", s.center.x, s.center.y, s.center.z, s.radius, s.mat.type ? "METAL": "LAMBERTIAN", s.mat.albedo.x, s.mat.albedo.y, s.mat.albedo.z);
    return s;
}

int main() {
    char buff[BUFSIZ];
    setvbuf(stderr, buff, _IOFBF, BUFSIZ);
    // World
    sphere world[4];
    world[0] = make_sphere(0.0, -100.5, -1.0, 100.0, LAMBERTIAN, (color) {0.8, 0.8, 0.0});
    world[1] = make_sphere(0.0, 0.0, -1.0, 0.5, LAMBERTIAN, (color) {0.7, 0.3, 0.3});
    world[2] = make_sphere(-1.0, 0.0, -1.0, 0.5, METAL, (color) {0.8, 0.8, 0.8});
    world[3] = make_sphere(1.0, 0.0, -1.0, 0.5, METAL, (color) {0.8, 0.6, 0.2});

    // Image
    float aspect_ratio = 16.0 / 9.0;
    int image_width = 1600;
    int samples_per_pixel = 100;

    camera camera = create_camera(image_width, aspect_ratio, samples_per_pixel);


    render(&camera, world);
}
