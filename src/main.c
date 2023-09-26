#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#include "sphere.h"
#include "camera.h"

sphere make_sphere(double x, double y, double z, double r, material_type type, color color, double fuzz, double ir) {
    point3 o = {x, y, z};
    material mat = {
        .type = type,
        .albedo = color,
        .fuzz = fuzz < 1 ? fuzz : 1,
        .ir = ir
    };
    sphere s = {
        .center = o,
        .radius = r,
        .mat = mat
    };
    fprintf(stderr, "Created sphere at (%f, %f, %f), size %f, type: %d, color (%f, %f, %f), fuzz: %f, ir: %f\n", s.center.x, s.center.y, s.center.z, s.radius, s.mat.type, s.mat.albedo.x, s.mat.albedo.y, s.mat.albedo.z, s.mat.fuzz, s.mat.ir);
    return s;
}

int main() {
    char buff[BUFSIZ];
    setvbuf(stderr, buff, _IOFBF, BUFSIZ);

    // World
    sphere world[5];
    //double R = cos(pi/4);
    //world[0] = make_sphere(-R, 0, -1, R, LAMBERTIAN, (color) {0, 0,  1}, 0.0, 0.0);
    //world[1] = make_sphere( R, 0, -1, R, LAMBERTIAN, (color) {1, 0,  0}, 0.0, 0.0);
    world[0] = make_sphere( 0.0, -100.5, -1.0,100.0, LAMBERTIAN, (color) {0.8, 0.8, 0.0}, 0.0, 0.0);
    world[1] = make_sphere( 0.0,    0.0, -1.0,  0.5, LAMBERTIAN, (color) {0.1, 0.2, 0.5}, 0.0, 0.0);
    world[2] = make_sphere(-1.0,    0.0, -1.0,  0.5, DIELECTRIC, (color) {0.8, 0.8, 0.8}, 0.0, 1.5);
    world[3] = make_sphere(-1.0,    0.0, -1.0, -0.4, DIELECTRIC, (color) {0.8, 0.8, 0.8}, 0.0, 1.5);
    world[4] = make_sphere( 1.0,    0.0, -1.0,  0.5, METAL,      (color) {0.8, 0.6, 0.2}, 0.0, 0.0);

    // Image
    float aspect_ratio = 16.0 / 9.0;
    int image_width = 1600;
    int samples_per_pixel = 100;
    int max_depth = 50;
    double vfov = 30;
    point3 lookfrom = {-2, 2, 1};
    point3 lookat = {0, 0, -1};
    vec3 vup = {0, 1, 0};

    camera camera = create_camera(
            image_width, 
            aspect_ratio, 
            samples_per_pixel,
            max_depth,
            vfov,
            lookfrom,
            lookat,
            vup
    );


    render(&camera, world);
}