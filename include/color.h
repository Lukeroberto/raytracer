#ifndef COLOR_H
#define COLOR_H

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <math.h>

#include <SDL2/SDL.h>

#include "vec3.h"
#include "interval.h"

typedef Vec3 color;

typedef struct color_int {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_int;

double linear_to_gamma(double linear_component) {
    return sqrt(linear_component);
}

color_int process_color(color pixel_color, int samples_per_pixel) {
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
    return (color_int) {
        .r = (Uint8) (256.0 * clamp(&intensity, r)),
        .g = (Uint8) (256.0 * clamp(&intensity, g)),
        .b = (Uint8) (256.0 * clamp(&intensity, b)),
    };
}

void write_color(color pixel_color, int samples_per_pixel) {
    color_int pixel_int = process_color(pixel_color, samples_per_pixel);
    printf("%d %d %d\n", pixel_int.r, pixel_int.g, pixel_int.b);
}

void set_window_pixel(color pixel_color, int samples_per_pixel, int pix_i, int pix_j, SDL_Renderer *renderer) {
    color_int pixel_int = process_color(pixel_color, samples_per_pixel);
    SDL_SetRenderDrawColor(renderer, pixel_int.r, pixel_int.g, pixel_int.b, 255);
    SDL_RenderDrawPoint(renderer, pix_i, pix_j);
}

#define AVERAGE(a, b)   ( ((((a) ^ (b)) & 0xfefefefeL) >> 1) + ((a) & (b)) )

void set_pixel_buffer(color pixel_color, int samples_per_pixel, size_t loc, SDL_Surface *surface) {
    color_int pixel_int = process_color(pixel_color, samples_per_pixel);
    unsigned int *pixels = surface->pixels;
    Uint32 pixel = SDL_MapRGBA(surface->format, pixel_int.r, pixel_int.g, pixel_int.b, 255);
    pixels[loc] = pixel;
    //pixels[loc] = AVERAGE(pixel, pixels[loc]);
}

#endif
