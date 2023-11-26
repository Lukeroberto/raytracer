#ifndef INTERVAL_H
#define INTERVAL_H

#include<stdbool.h>
#include<math.h>

typedef struct Interval{
    double min, max;
} Interval;

Interval EMPTY = {.min = INFINITY, .max=-INFINITY};
Interval UNIVERSE = {.min = -INFINITY, .max=INFINITY};

Interval create_from_interval(Interval a, Interval b) {
    return (Interval) {.min = fmin(a.min, b.min), .max = fmax(a.max, b.max)};
}

bool contains(Interval *i, double x) {
    return i->min <= x && x <= i->max;
}

bool surrounds(Interval *i, double x) {
    return i->min < x && x < i->max;
}

double clamp(Interval *i, double x) {
    if (x < i->min) return i->min;
    if (x > i->max) return i->max;

    return x;
}

#endif
