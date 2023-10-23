#include <stdio.h>
#include <assert.h>

#include "vec3.h"
#include "ray.h"
#include "color.h"


void testRay_at(void);
void assertEqual(vec3 actual, vec3 expected);
void testVec_invert(void);
void testVec_add(void);
void testVec_mult(void);
void testVec_dot(void);
void testVec_unit(void);

int main() {
    testRay_at();
    testVec_invert();
    testVec_add();
    testVec_mult();
    testVec_dot();
    testVec_unit();
}

void testVec_invert() {
    vec3 v = {1, 0, -1};
    vec3 iv_pass = {-1, 0, 1};

    vec3 iv = invert(&v);
    assertEqual(iv, iv_pass);
    printf("testVec_invert... PASS.\n");
}

void testVec_add() {
    vec3 v = {1, 0, 1};
    vec3 u = {0, 1, 0};

    vec3 sum = add(&v, &u);
    vec3 sum_pass = {1, 1, 1};
    assertEqual(sum, sum_pass);
    printf("testVec_add... PASS.\n");
}

void testVec_mult() {
    vec3 v = {1, 1, 1};
    
    vec3 doubled = mult(&v, 2.0);
    vec3 doubled_pass = {2, 2, 2};
    assertEqual(doubled, doubled_pass);

    vec3 halved = mult(&v, 0.5);
    vec3 halved_pass = {0.5, 0.5, 0.5};
    assertEqual(halved, halved_pass);

    vec3 inverted = mult(&v, -1.0);
    vec3 inverted_pass = {-1, -1, -1};
    assertEqual(inverted, inverted_pass);

    printf("testVec_mult... PASS.\n");
}

void testVec_dot() {
    vec3 v = {1, 1, 1};

    vec3 x = {2, 0, 0};
    vec3 y = {0, 5, 0};
    vec3 z = {0, 0, -11};

    double dot_x = dot(&v, &x);
    double dot_x_pass = 2.0;
    assert(dot_x == dot_x_pass);

    double dot_y = dot(&v, &y);
    double dot_y_pass = 5.0;
    assert(dot_y == dot_y_pass);

    double dot_z = dot(&v, &z);
    double dot_z_pass = -11.0;
    assert(dot_z == dot_z_pass);

    double dot_v = dot(&v, &v);
    double dot_v_pass = 3.0;
    assert(dot_v == dot_v_pass);

    printf("testVec_dot... PASS.\n");
}

void testVec_unit() {
    vec3 v = {2, 2, 2};
    vec3 u = {10, 10, 0};
    vec3 w = {7, 0, 0};

    vec3 unit_v = unit_vec(&v);
    vec3 unit_v_pass = {1.0 / sqrt(3), 1.0 / sqrt(3), 1.0 / sqrt(3)};
    assertEqual(unit_v, unit_v_pass);

    vec3 unit_u = unit_vec(&u);
    vec3 unit_u_pass = {1.0 / sqrt(2), 1.0 / sqrt(2), 0};
    assertEqual(unit_u, unit_u_pass);

    vec3 unit_w = unit_vec(&w);
    vec3 unit_w_pass = {1, 0, 0};
    assertEqual(unit_w, unit_w_pass);

    printf("testVec_unit... PASS.\n");
}

void testRay_at() {
    vec3 direction = {1, 1, 1};
    point3 origin = {0, 0, 0};
    ray r = {.origin=origin, .direction=direction};

    point3 p0_pass = {0, 0, 0};
    point3 p0_at = at(&r, 0);
    assertEqual(p0_at, p0_pass);

    point3 p1_pass = {1, 1, 1};
    point3 p1_at = at(&r, 1);
    assertEqual(p1_at, p1_pass);

    point3 pm1_pass = {-1, -1, -1};
    point3 pm1_at = at(&r, -1);
    assertEqual(pm1_at, pm1_pass);

    printf("testRay_at... PASS.\n");
}

void assertEqual(vec3 actual, vec3 expected) {
    assert(actual.x == expected.x);
    assert(actual.y == expected.y);
    assert(actual.z == expected.z);
}
