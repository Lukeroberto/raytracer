#ifndef COLOR_H
#define COLOR_H

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
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

SDL_Color read_color(SDL_Surface* pSurface, SDL_Renderer* render, int x, int y) {
    SDL_Color pixColor = {0};
    SDL_RenderReadPixels(render, NULL, SDL_PIXELFORMAT_RGB888, pSurface->pixels, pSurface->pitch);
	const Uint8 getPixel_bpp = pSurface->format->BytesPerPixel;
	Uint16* pPixel = (Uint16*)pSurface->pixels + y * pSurface->pitch + x * getPixel_bpp;
	Uint32 pixelData = {0};

	switch (getPixel_bpp) {
	case 1:
		pixelData = *pPixel;
		break;
	case 2:
		pixelData = *(Uint16*)pPixel;
		break;
	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			pixelData = pPixel[0] << 16 | pPixel[1] << 8 | pPixel[2];
		else
			pixelData = pPixel[0]| pPixel[1] << 8 | pPixel[2] << 16;
		break;
	case 4:
		pixelData = *(Uint32*)pPixel;
		break;
	}

	SDL_GetRGBA(pixelData, pSurface->format, &pixColor.r, &pixColor.g, &pixColor.b, &pixColor.a);
    return pixColor;
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

    //int w, h;
    //SDL_GetRendererOutputSize(renderer, &w, &h);
    //SDL_Surface* pSurf = SDL_CreateRGBSurface(0, w,h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    //SDL_Color color_old = read_color(pSurf, renderer, pix_i, pix_j);

    //SDL_SetRenderDrawColor(renderer, (int) (0.5*(ri + color_old.r)),(int)  (0.5*(gi + color_old.g)), (int) (0.5*(bi+color_old.b)), 255);
    SDL_SetRenderDrawColor(renderer, ri, gi, bi, 255);
    SDL_RenderDrawPoint(renderer, pix_i, pix_j);
}

#endif
