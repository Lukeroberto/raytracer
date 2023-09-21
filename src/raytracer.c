#include <stdio.h>

#include "vec3.h"
#include "color.h"

int main() {

    int image_width = 256;
    int image_height = 256;

    vec3 vec = {.x=1, .y=1, .z=1};

    char buff[BUFSIZ];
    setvbuf(stderr, buff, _IOFBF, BUFSIZ);

    printf("P3\n%d %d\n255\n", image_width, image_height);
    for (int j = 0; j < image_height; ++j) {
        fprintf(stderr, "\rScanlines remaining: %d", (image_height - j));
        fflush(stderr);
        for (int i = 0; i < image_width; ++i) {
            color pixel_color = {
                .x = (double) i / (image_width - 1),
                .y = 0,
                .z = (double) j / (image_height - 1),
            };
            print_color(pixel_color);
        }
    }
    fprintf(stderr, "\rDone.                    \n");
}
