#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#include "sphere.h"
#include "camera.h"

sphere make_sphere(point3 p, double r, material mat) {
    sphere s = {
        .center = p,
        .radius = r,
        .mat = mat
    };
    //fprintf(stderr, "Created sphere at (%f, %f, %f), size %f, type: %d, color (%f, %f, %f), fuzz: %f, ir: %f\n", s.center.x, s.center.y, s.center.z, s.radius, s.mat.type, s.mat.albedo.x, s.mat.albedo.y, s.mat.albedo.z, s.mat.fuzz, s.mat.ir);
    return s;
}

int main() {
    char buff[BUFSIZ];
    setvbuf(stderr, buff, _IOFBF, BUFSIZ);

    // World
    sphere world[500];

    material ground_material = {.type=LAMBERTIAN, .albedo=(color) {0.5, 0.5, 0.5}};
    world[0] = make_sphere((point3) {0, -1000, 0}, 1000, ground_material);

    int indx = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            double choose_mat = random_double();
            point3 center = {a+0.9*random_double(), 0.2, b + 0.9*random_double()};

            vec3 vec = diff(&center, &(point3) {4, 0.2, 0});
            if (length(&vec) > 0.9) {
                if (choose_mat < 0.8) {
                    // Diffuse
                    color albedo = random_vec();
                    material diffuse_mat = {.type=LAMBERTIAN, .albedo=albedo};
                    world[indx] = make_sphere(center, 0.2, diffuse_mat);
                    indx++;
                } else if (choose_mat < 0.90) {
                    // Metal
                    color albedo = random_vec_interval(0.5, 1);
                    double fuzz = random_double_interval(0, 0.5);
                    material metal_mat = {.type=METAL, .albedo=albedo, .fuzz=fuzz};
                    world[indx] = make_sphere(center, 0.2, metal_mat);
                    indx++;
                } else {
                    // Glass
                    material glass_mat = {.type=DIELECTRIC, .ir=1.5};
                    world[indx] = make_sphere(center, 0.2, glass_mat);
                    indx++;
                }
            }
        }
    }

    material mat1 = {.type=DIELECTRIC, .ir=1.5};
    world[indx] = make_sphere((point3) {0, 1, 0}, 1.0, mat1);
    indx++;

    material mat2 = {.type=LAMBERTIAN, .albedo=(color) {0.4, 0.2, 0.1}};
    world[indx] = make_sphere((point3) {-4, 1, 0}, 1.0, mat2);
    indx++;

    material mat3 = {.type=METAL, .albedo=(color) {0.7, 0.6, 0.5}, .fuzz=0.0};
    world[indx] = make_sphere((point3) {4, 1, 0}, 1.0, mat3);

    // Image
    float aspect_ratio = 16.0 / 9.0;
    int image_width = 1600;
    int samples_per_pixel = 500;
    int max_depth = 50;
    double vfov = 20;
    point3 lookfrom = {13, 2, 3};
    point3 lookat = {0, 0, 0};
    vec3 vup = {0, 1, 0};
    double defocus_angle = 0.6;
    double focus_dist = 10.0;

    camera camera = create_camera(
            image_width, 
            aspect_ratio, 
            samples_per_pixel,
            max_depth,
            vfov,
            lookfrom,
            lookat,
            vup,
            defocus_angle,
            focus_dist
    );

    render(&camera, world);
}
