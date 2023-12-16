#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "aabb.h"
#include "interval.h"
#include "ray.h"
#include "sphere.h"
#include "triangle.h"

void test_ray_aabb_collisions();
void test_ray_sphere_collisions();
void test_ray_triangle_collisions();

int main() {
    printf("Testing ray/aabb collisions...\n");
    test_ray_aabb_collisions();


    printf("Testing ray/sphere collisions...\n");
    test_ray_sphere_collisions();


    printf("Testing ray/triangle collisions...\n");
    test_ray_triangle_collisions();
}

/*
 * Note: 
 * z - depth
 * y - vertical viewport direction (-v)
 * x - horizontal viewport direction (u)
 */

void test_ray_aabb_collisions() {
    Ray r = {.origin = {0, 0, 0}, .direction = {0, 0, 1}};
    Interval ray_t = {0.0001, 5};

    AABB box = create_aabb_for_point((Vec3) {-0.5, -0.5, 0.95}, (Vec3) {0.5, 0.5, 1.05});
    assert(hit_aabb(&r, ray_t, &box));
}

void test_ray_sphere_collisions() {
    Ray r = {.origin = {0, 0, 0}, .direction = {0, 0, 1}};
    Interval ray_t = {0.0001, 5};

    Sphere sphere = {.center = {0, 0, 1}, .radius = 0.25, .mat = {0}};
    HitRecord rec = {0};
    int tests = 0;
    assert(ray_intersect_sphere(&r, &sphere, &ray_t, &rec, &tests));
}

void test_ray_triangle_collisions() {
    Ray r = {.origin = {0, 0, 0}, .direction = {0, 0, 1}};
    Interval ray_t = {0.0001, 2};

    Triangle triangle = {.v1 = {-1, -1, 1}, .v2 = {0, 1, 1}, .v3 = {1, -1, 1}, .normal = {0, 0, -1}, .mat = {0}};
    HitRecord rec = {0};
    int tests = 0;
    assert(ray_intersect_triangle(&r, &triangle, &ray_t, &rec, &tests));
}
