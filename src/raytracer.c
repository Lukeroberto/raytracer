#include <stdio.h>
#include <stdlib.h>

#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "sphere.h"
#include "camera.h"

sphere make_sphere(double x, double y, double z, double r) {
    point3 o = {x, y, z};
    sphere s = {o, r};
    return s;
}

int main() {
    // Image
    float aspect_ratio = 16.0 / 9.0;
    int image_width = 1600;
    int samples_per_pixel = 100;

    camera camera = create_camera(image_width, aspect_ratio, samples_per_pixel);

    // World
    sphere world[2];
    world[0] = make_sphere(0, 0, -1, 0.5);
    world[1] = make_sphere(0, -100.5, -1, 100);

    render(&camera, world);
}
