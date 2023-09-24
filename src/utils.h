#ifndef UTILS_H
#define UTILS_H

#include<stdlib.h>

double random_double() {
    return random() / (RAND_MAX + 1.0);
}

double random_double_interval(double min, double max) {
    return min + (max - min) * random_double();
}

#endif
