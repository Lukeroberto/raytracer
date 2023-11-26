#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

#include "bvh.h"
#include "utils.h"

#include "sphere.h"
#include "camera.h"

#include <SDL2/SDL.h>
#include <time.h>

#define IMAGE_WIDTH 720

bvh_node* create_random_spheres(int max_spheres) {
    // World
    sphere sphere_list[500];

    int num_spheres = 0;
    material ground_material = {.type=LAMBERTIAN, .albedo=(color) {0.5, 0.5, 0.5}};
    sphere_list[0] = make_sphere((Point3) {0, -1000, 0}, 1000, ground_material);
    num_spheres++;

    material mat1 = {.type=DIELECTRIC, .ir=1.5};
    sphere_list[num_spheres] = make_sphere((Point3) {0, 1, 0}, 1.0, mat1);
    num_spheres++;

    material mat2 = {.type=LAMBERTIAN, .albedo=(color) {0.4, 0.2, 0.1}};
    sphere_list[num_spheres] = make_sphere((Point3) {-4, 1, 0}, 1.0, mat2);
    num_spheres++;

    material mat3 = {.type=METAL, .albedo=(color) {0.7, 0.6, 0.5}, .fuzz=0.0};
    sphere_list[num_spheres] = make_sphere((Point3) {4, 1, 0}, 1.0, mat3);
    num_spheres++;

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            double choose_mat = random_double();
            Point3 center = {a+ 0.9*random_double(), 0.2, b + 0.9*random_double()};

            Vec3 vec = diff_vec3(center, (Point3) {4, 0.2, 0});
            if (length(vec) > 0.9) {
                if (choose_mat < 0.8) {
                    // Diffuse
                    color albedo = random_vec();
                    material diffuse_mat = {.type=LAMBERTIAN, .albedo=albedo};
                    sphere_list[num_spheres] = make_sphere(center, 0.2, diffuse_mat);
                    num_spheres++;
                } else if (choose_mat < 0.90) {
                    // Metal
                    color albedo = random_vec_interval(0.5, 1);
                    double fuzz = random_double_interval(0, 0.5);
                    material metal_mat = {.type=METAL, .albedo=albedo, .fuzz=fuzz};
                    sphere_list[num_spheres] = make_sphere(center, 0.2, metal_mat);
                    num_spheres++;
                } else {
                    // Glass
                    material glass_mat = {.type=DIELECTRIC, .ir=1.5};
                    sphere_list[num_spheres] = make_sphere(center, 0.2, glass_mat);
                    num_spheres++;
                }
            }
        }
    }
    return build_bvh(sphere_list, 0, max_spheres);
}

void update_camera(Vec3 lookfrom_delta, camera *camera) {
    // Image
    double aspect_ratio = 16.0 / 9.0;
    int image_width = IMAGE_WIDTH;
    int samples_per_pixel = 3;
    int max_depth = 5;
    double vfov = 20;
    Point3 lookfrom = add_vec3((Vec3) {13, 2, 3}, lookfrom_delta);
    Point3 lookat = {0, 0, 0};
    Vec3 vup = {0, 1, 0};
    double defocus_angle = 0.6;
    double focus_dist = 10.0;

    *camera = create_camera(
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
}

sphere* create_three_spheres_world_arr(sphere sphere_list[]) {
    // World
    material ground = {.type=LAMBERTIAN, .albedo=(color) {0.8, 0.8, 0.0}};
    material mat_center = {.type=LAMBERTIAN, .albedo=(color) {0.1, 0.2, 0.5}};
    material mat_left = {.type=DIELECTRIC, .ir=1.5};
    material mat_right = {.type=METAL, .albedo=(color) {0.8, 0.6, 0.2}, .fuzz=0.0};

    sphere_list[0] = make_sphere((Point3) { 0.0, -100.5, -1.0}, 100.0, ground);
    sphere_list[1] = make_sphere((Point3) { 0.0,    0.0, -1.0},   0.5, mat_center);
    sphere_list[2] = make_sphere((Point3) {-1.0,    0.0, -1.0},   0.5, mat_left);
    sphere_list[3] = make_sphere((Point3) {-1.0,    0.0, -1.0},  -0.4, mat_left);
    sphere_list[4] = make_sphere((Point3) { 1.0,    0.0, -1.0},   0.5, mat_right);

    return sphere_list;
}

camera create_three_spheres_camera() {

    // Image
    double aspect_ratio = 16.0 / 9.0;
    int image_width = IMAGE_WIDTH;
    int samples_per_pixel = 1;
    int max_depth = 5;
    double vfov = 20;
    Point3 lookfrom = {-2, 2, 1};
    Point3 lookat = {0, 0, -1};
    Vec3 vup = {0, 1, 0};
    double defocus_angle = 10.0;
    double focus_dist = 3.4;

    return create_camera(
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
}

int main() {
    bvh_node* world = create_random_spheres(3);
    camera camera;
    update_camera((Vec3) {0.0, 0.0, 0.0}, &camera);

    // Setup SDL objects
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, camera.image_width, camera.image_height, 0);
    SDL_Surface * surface = SDL_GetWindowSurface(window);

    double overlap = calculate_total_overlap(world);
    printf("num nodes in bvh: %d, overlap: %f\n", count_bvh(world), overlap);

    // Run until user quits
    int quit = 0;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            clock_t tik = clock();
            switch( event.type ){
                case SDL_KEYDOWN:
                    switch( event.key.keysym.sym ){
                        case SDLK_LEFT:
                            update_camera((Vec3) {-0.1, 0.0, 0.0}, &camera);
                            render_bvh(&camera, world, surface);
                            SDL_UpdateWindowSurface(window);
                            break;
                        case SDLK_RIGHT:
                            update_camera((Vec3) {0.1, 0.0, 0.0}, &camera);
                            render_bvh(&camera, world, surface);
                            SDL_UpdateWindowSurface(window);
                            break;
                        case SDLK_UP:
                            update_camera((Vec3) {0.0, 0.0, 0.1}, &camera);
                            render_bvh(&camera, world, surface);
                            SDL_UpdateWindowSurface(window);
                            break;
                        case SDLK_DOWN:
                            update_camera((Vec3) {0.0, 0.0, -0.1}, &camera);
                            render_bvh(&camera, world, surface);
                            SDL_UpdateWindowSurface(window);
                            break;
                        default:
                            break;
                    }
                    clock_t tok = clock();
                    printf("Drew frame in %f ms, %f fps\n", 1000.0 * ((double) (tok - tik) / CLOCKS_PER_SEC), CLOCKS_PER_SEC / (double) (tok - tik));
                    break;

                case SDL_QUIT:
                    quit = 1;
                    break;

                default:
                    break;
            }
        }
    }


    // Cleanup 
    free_bvh(world);
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
