#include <assert.h>
#include <stdio.h>

#include "aabb.h"
#include "interval.h"
#include "ray.h"
#include "sphere.h"
#include "triangle.h"

void test_ray_aabb_collisions();
void test_ray_sphere_collisions();
void test_ray_triangle_collisions();
void testCubeIntersection();

int main() {
    printf("Testing ray/aabb collisions...");
    test_ray_aabb_collisions();


    printf("Testing ray/sphere collisions...");
    test_ray_sphere_collisions();


    printf("Testing ray/triangle collisions...");
    test_ray_triangle_collisions();

    printf("Testing ray/cube collisions...");
    testCubeIntersection();
}

/*
 * Note: 
 * z - depth
 * y - vertical viewport direction (-v)
 * x - horizontal viewport direction (u)
 */

void test_ray_aabb_collisions() {
    Ray r = {.origin = {0, 0, 0}, .direction = {0, 0, 1}};
    Interval ray_t = {0.0, 5};

    AABB box = create_aabb_for_point((Vec3) {-0.5, -0.5, 0.95}, (Vec3) {0.5, 0.5, 1.05});
    assert(hit_aabb(&r, ray_t, &box));
    printf("PASSED.\n");
}

void test_ray_sphere_collisions() {
    Ray r = {.origin = {0, 0, 0}, .direction = {0, 0, 1}};
    Interval ray_t = {0.0, 5};

    Sphere sphere = {.center = {0, 0, 1}, .radius = 0.25, .mat = {0}};
    HitRecord rec = {0};
    int tests = 0;
    assert(ray_intersect_sphere(&r, &sphere, &ray_t, &rec, &tests));
    printf("PASSED.\n");
}

void test_ray_triangle_collisions() {
    Ray r = {.origin = {0, 0, 0}, .direction = {0, 0, 1}};
    Interval ray_t = {0.0, 2};

    Triangle triangle = {.v1 = {-1, -1, 1}, .v2 = {0, 1, 1}, .v3 = {1, -1, 1}, .normal = {0, 0, -1}, .mat = {0}};
    HitRecord rec = {0};
    int tests = 0;
    assert(ray_intersect_triangle(&r, &triangle, &ray_t, &rec, &tests));

    // NO intersection
    r = (Ray) {.origin = {0, 0, 0}, .direction = {1, 0, 0}};
    ray_t = (Interval) {0.0, 2};

    triangle = (Triangle) {.v1 = {0, 1, 0}, .v2 = {1, 1, 0}, .v3 = {0, 1, 1}, .normal = {0}, .mat = {0}};
    rec = (HitRecord) {0};
    tests = 0;
    assert(!ray_intersect_triangle(&r, &triangle, &ray_t, &rec, &tests));

    // Triangle in y=1 plane, intersction
    r = (Ray) {.origin = {0, 0, 0}, .direction = {0, 1, 0}};
    ray_t = (Interval) {0.0, 2};

    triangle = (Triangle) {.v1 = {-1, 1, -1}, .v2 = {1, 1, -1}, .v3 = {0, 1, 1}, .normal = {0}, .mat = {0}};
    rec = (HitRecord) {0};
    tests = 0;
    assert(ray_intersect_triangle(&r, &triangle, &ray_t, &rec, &tests));
    //assert(intersectionPoint == Vec3(0, 1, 0)); // Expected intersection at (0, 1, 0)

    // Edge intersection, triangle in z=1 plane
    //r = (Ray) {.origin = {0, 0, 0}, .direction = {0, 0, 1}};
    //ray_t = (Interval) {0.0, 2};

    //triangle = (Triangle) {.v1 = {-1, -1, 1}, .v2 = {1, -1, 1}, .v3 = {0, 1, 1}, .normal = {0}, .mat = {0}};
    //rec = (HitRecord) {0};
    //tests = 0;
    //assert(ray_intersect_triangle(&r, &triangle, &ray_t, &rec, &tests));
    //assert(intersectionPoint == Vec3(0, 0, 1)); // Expected intersection at the edge

    // Ray parallel to triangle, no intersection
    r = (Ray) {.origin = {0, 0, 0}, .direction = {0, 0, 1}};
    ray_t = (Interval) {0.0, 2};

    triangle = (Triangle) {.v1 = {-1, -1, 0}, .v2 = {1, -1, 0}, .v3 = {0, 1, 0}, .normal = {0}, .mat = {0}};
    rec = (HitRecord) {0};
    tests = 0;
    assert(!ray_intersect_triangle(&r, &triangle, &ray_t, &rec, &tests));
    printf("PASSED.\n");
}

void buildCubeTriangles(Triangle *cubeTriangles) {
    // Front face
    cubeTriangles[0] = (Triangle){{-0.5, -0.5,  0.5}, {-0.5,  0.5,  0.5}, { 0.5,  0.5,  0.5}};
    cubeTriangles[1] = (Triangle){{ 0.5,  0.5,  0.5}, { 0.5, -0.5,  0.5}, {-0.5, -0.5,  0.5}};

    // Back face
    cubeTriangles[2] = (Triangle){{-0.5, -0.5, -0.5}, { 0.5, -0.5, -0.5}, { 0.5,  0.5, -0.5}};
    cubeTriangles[3] = (Triangle){{ 0.5,  0.5, -0.5}, {-0.5,  0.5, -0.5}, {-0.5, -0.5, -0.5}};

    // Left face
    cubeTriangles[4] = (Triangle){{-0.5, -0.5, -0.5}, {-0.5, -0.5,  0.5}, {-0.5,  0.5,  0.5}};
    cubeTriangles[5] = (Triangle){{-0.5,  0.5,  0.5}, {-0.5,  0.5, -0.5}, {-0.5, -0.5, -0.5}};

    // Right face
    cubeTriangles[6] = (Triangle){{ 0.5, -0.5, -0.5}, { 0.5,  0.5, -0.5}, { 0.5,  0.5,  0.5}};
    cubeTriangles[7] = (Triangle){{ 0.5,  0.5,  0.5}, { 0.5, -0.5,  0.5}, { 0.5, -0.5, -0.5}};

    // Top face
    cubeTriangles[8] = (Triangle){{-0.5,  0.5, -0.5}, {-0.5,  0.5,  0.5}, { 0.5,  0.5,  0.5}};
    cubeTriangles[9] = (Triangle){{ 0.5,  0.5,  0.5}, { 0.5,  0.5, -0.5}, {-0.5,  0.5, -0.5}};

    // Bottom face
    cubeTriangles[10] = (Triangle){{-0.5, -0.5, -0.5}, { 0.5, -0.5, -0.5}, { 0.5, -0.5,  0.5}};
    cubeTriangles[11] = (Triangle){{ 0.5, -0.5,  0.5}, {-0.5, -0.5,  0.5}, {-0.5, -0.5, -0.5}};
}

// Test function
void testCubeIntersection() {
    // Define the unit cube by its triangles
    Triangle cubeTriangles[12];
    buildCubeTriangles(cubeTriangles);

    // Define rays
    Vec3 rayOrigin = {0, 0, -1};
    Ray r1 = {.origin = rayOrigin, .direction = {0, 0, 1}}; // Should intersect
    Ray r2 = {.origin = rayOrigin, .direction = {1, 0, 0}}; // Should not intersect
    Interval ray_t = {0.0, 2};
    HitRecord rec = {0};
    int tests = 0;

    Vec3 intersectionPoint;
    int intersected1 = 0, intersected2 = 0;

    for (int i = 0; i < 12; i++) {
        if (ray_intersect_triangle(&r1, &cubeTriangles[i], &ray_t, &rec, &tests)) {
            intersected1 = 1;
        }
        if (ray_intersect_triangle(&r2, &cubeTriangles[i], &ray_t, &rec, &tests)) {
            intersected2 = 1;
        }
    }


    assert(intersected1);
    assert(!intersected2);

    printf("PASSED.\n");
}


