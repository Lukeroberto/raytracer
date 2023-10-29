#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include <math.h>

#include <SDL2/SDL.h>

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

void set_window_pixel(color pixel_color, int samples_per_pixel, int pix_i, int pix_j, SDL_Renderer *renderer) {
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
    int ri = 256 * clamp(&intensity, r);
    int gi = 256 * clamp(&intensity, g);
    int bi = 256 * clamp(&intensity, b);

    SDL_SetRenderDrawColor(renderer, ri, gi, bi, 255);
    SDL_RenderDrawPoint(renderer, pix_i, pix_j);
}

#endif
