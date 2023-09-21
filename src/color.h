#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>

#include "vec3.h"

typedef vec3 color;

void print_color(color pixel_color) {
    printf("%d %d %d\n", 
            (int) (255.999 * pixel_color.x),
            (int) (255.999 * pixel_color.y),
            (int) (255.999 * pixel_color.z));
}

#endif
