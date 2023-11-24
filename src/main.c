#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#include "sphere.h"
#include "camera.h"

#include <SDL2/SDL.h>
#include <time.h>



int random_spheres() {
    // World
    sphere world[500];

    int num_spheres = 0;
    material ground_material = {.type=LAMBERTIAN, .albedo=(color) {0.5, 0.5, 0.5}};
    world[0] = make_sphere((point3) {0, -1000, 0}, 1000, ground_material);
    num_spheres++;

    material mat1 = {.type=DIELECTRIC, .ir=1.5};
    world[num_spheres] = make_sphere((point3) {0, 1, 0}, 1.0, mat1);
    num_spheres++;

    material mat2 = {.type=LAMBERTIAN, .albedo=(color) {0.4, 0.2, 0.1}};
    world[num_spheres] = make_sphere((point3) {-4, 1, 0}, 1.0, mat2);
    num_spheres++;

    material mat3 = {.type=METAL, .albedo=(color) {0.7, 0.6, 0.5}, .fuzz=0.0};
    world[num_spheres] = make_sphere((point3) {4, 1, 0}, 1.0, mat3);
    num_spheres++;

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            double choose_mat = random_double();
            point3 center = {a+0.9*random_double(), 0.2, b + 0.9*random_double()};

            vec3 vec = diff(center, (point3) {4, 0.2, 0});
            if (length(vec) > 0.9) {
                if (choose_mat < 0.8) {
                    // Diffuse
                    color albedo = random_vec();
                    material diffuse_mat = {.type=LAMBERTIAN, .albedo=albedo};
                    world[num_spheres] = make_sphere(center, 0.2, diffuse_mat);
                    num_spheres++;
                } else if (choose_mat < 0.90) {
                    // Metal
                    color albedo = random_vec_interval(0.5, 1);
                    double fuzz = random_double_interval(0, 0.5);
                    material metal_mat = {.type=METAL, .albedo=albedo, .fuzz=fuzz};
                    world[num_spheres] = make_sphere(center, 0.2, metal_mat);
                    num_spheres++;
                } else {
                    // Glass
                    material glass_mat = {.type=DIELECTRIC, .ir=1.5};
                    world[num_spheres] = make_sphere(center, 0.2, glass_mat);
                    num_spheres++;
                }
            }
        }
    }

    aabb bbox = create_aabb_for_array_sphere(world, num_spheres);
    printf("World: %d objects, ", num_spheres); print_aabb(&bbox); 

    // Image
    float aspect_ratio = 16.0 / 9.0;
    int image_width = 720;
    int samples_per_pixel = 1;
    int max_depth = 5;
    double vfov = 20;
    point3 lookfrom = {13, 2, 3};
    point3 lookat = {0, 0, 0};
    vec3 vup = {0, 1, 0};
    double defocus_angle = 0.6;
    double focus_dist = 10.0;

    camera camera = create_camera(
            image_width, 
            aspect_ratio, 
            samples_per_pixel,
            max_depth,
            vfov,
            lookfrom,
            lookat,
            vup,
            defocus_angle,
            focus_dist
    );

    SDL_Renderer *renderer;
    SDL_Window *window; 
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(camera.image_width, camera.image_height, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    for (int i = 0; i < 3; i++) {
        clock_t tik = clock();
        render(&camera, num_spheres, world, renderer);
        clock_t tok = clock();

        SDL_RenderPresent(renderer);

        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
        camera.center = diff(camera.center, (vec3) {0.05, -0.001, -0.05});
        if (tok - tik < CLOCKS_PER_SEC) {
            printf("%d fps\n",(int) ( CLOCKS_PER_SEC / (float) (tok - tik)));
        } else {
            printf("%d seconds/frame\n",(int) ( (float) (tok - tik) / CLOCKS_PER_SEC));
        }
    }
    while (true) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

int three_spheres() {
    // World
    sphere world[5];
    material ground = {.type=LAMBERTIAN, .albedo=(color) {0.8, 0.8, 0.0}};
    material mat_center = {.type=LAMBERTIAN, .albedo=(color) {0.1, 0.2, 0.5}};
    material mat_left = {.type=DIELECTRIC, .ir=1.5};
    material mat_right = {.type=METAL, .albedo=(color) {0.8, 0.6, 0.2}, .fuzz=0.0};

    world[0] = make_sphere((point3) { 0.0, -100.5, -1.0}, 100.0, ground);
    world[1] = make_sphere((point3) { 0.0,    0.0, -1.0},   0.5, mat_center);
    world[2] = make_sphere((point3) {-1.0,    0.0, -1.0},   0.5, mat_left);
    world[3] = make_sphere((point3) {-1.0,    0.0, -1.0},  -0.4, mat_left);
    world[4] = make_sphere((point3) { 1.0,    0.0, -1.0},   0.5, mat_right);

    // Image
    float aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int samples_per_pixel = 1;
    int max_depth = 5;
    double vfov = 20;
    point3 lookfrom = {-2, 2, 1};
    point3 lookat = {0, 0, -1};
    vec3 vup = {0, 1, 0};
    double defocus_angle = 10.0;
    double focus_dist = 3.4;

    camera camera = create_camera(
            image_width, 
            aspect_ratio, 
            samples_per_pixel,
            max_depth,
            vfov,
            lookfrom,
            lookat,
            vup,
            defocus_angle,
            focus_dist
    );

    return render(&camera, 5, world, NULL);
}

int main() {
    switch (0) {
        case 1: three_spheres(); break;
        case 2: random_spheres(); break;
        default: random_spheres(); break;
    }
}
