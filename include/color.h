#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include <math.h>

#include "vec3.h"
#include "interval.h"

typedef vec3 color;

double linear_to_gamma(double linear_component) {
    return sqrt(linear_component);
}

void write_color(color pixel_color, int samples_per_pixel) {
    double r = pixel_color.x;
    double g = pixel_color.y;
    double b = pixel_color.z;

    double scale = 1.0 / samples_per_pixel;
    r *= scale;
    b *= scale;
    g *= scale;

    // Apply linear -> gamma transform
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    interval intensity = {.min=0.000, .max=0.999};
    printf("%d %d %d\n", 
            (int) (256 * clamp(&intensity, r)),
            (int) (256 * clamp(&intensity, g)),
            (int) (256 * clamp(&intensity, b)));
}

#endif
