#ifndef INTERVAL_H
#define INTERVAL_H

#include<stdbool.h>
#include<math.h>

typedef struct {
    double min, max;
} interval;

interval EMPTY = {.min = INFINITY, .max=-INFINITY};
interval UNIVERSE = {.min = -INFINITY, .max=INFINITY};

bool contains(interval *i, double x) {
    return i->min <= x && x <= i->max;
}

bool surrounds(interval *i, double x) {
    return i->min < x && x < i->max;
}

#endif
