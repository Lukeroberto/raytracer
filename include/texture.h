#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.h"

typedef struct SolidTexture { 
    Color color_value;
} SolidTexture;

typedef struct CheckerTexture {
    double inv_scale;

    // TODO: eventually abstract the textures to be polymorphic or something
    Color even;
    Color odd;

} CheckerTexture;

Color value_checker(double u, double v, const Point3 *pt, const CheckerTexture *checker) {
    int x = (int) floor(checker->inv_scale * pt->x);
    int y = (int) floor(checker->inv_scale * pt->y);
    int z = (int) floor(checker->inv_scale * pt->z);

    bool isEven = (x + y + z) % 2 == 0;

    return isEven ? checker->even : checker->odd;
}

#endif // TEXTURE_H
