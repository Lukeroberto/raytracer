#ifndef INTERVAL_H
#define INTERVAL_H

#include<stdbool.h>
#include<math.h>

typedef struct {
    double min, max;
} interval;

interval EMPTY = {.min = INFINITY, .max=-INFINITY};
interval UNIVERSE = {.min = -INFINITY, .max=INFINITY};

interval create_from_interval(interval a, interval b) {
    return (interval) {.min = fmin(a.min, b.min), .max = fmax(a.max, b.max)};
}

bool contains(interval *i, double x) {
    return i->min <= x && x <= i->max;
}

bool surrounds(interval *i, double x) {
    return i->min < x && x < i->max;
}

double clamp(interval *i, double x) {
    if (x < i->min) return i->min;
    if (x > i->max) return i->max;

    return x;
}

#endif
