#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

#include "bvh.h"
#include "utils.h"
#include "types.h"

#include "sphere.h"
#include "camera.h"

#include <SDL2/SDL.h>
#include <time.h>

#define IMAGE_WIDTH 720

BvhNode* create_random_spheres(int max_spheres);
void create_random_spheres_arr(Sphere *spheres);
void update_camera(Vec3 delta, Camera *camera);


int main() {
    BvhNode* world = create_random_spheres(3);
    Camera camera;
    update_camera((Vec3) {0.0, 0.0, 0.0}, &camera);

    // Setup SDL objects
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, camera.image_width, camera.image_height, 0);
    SDL_Surface * surface = SDL_GetWindowSurface(window);

    double overlap = calculate_total_overlap(world);
    printf("num nodes in bvh: %d, overlap: %f\n", count_bvh(world), overlap);

    // Run until user quits
    int quit = 0;
    int num_intersects = 0;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            clock_t tik = clock();
            num_intersects = 0;
            switch( event.type ){
                case SDL_KEYDOWN:
                    switch( event.key.keysym.sym ){
                        case SDLK_LEFT:
                            update_camera((Vec3) {-0.3, 0.0, 0.0}, &camera);
                            render_bvh(&camera, world, surface, &num_intersects);
                            SDL_UpdateWindowSurface(window);
                            break;
                        case SDLK_RIGHT:
                            update_camera((Vec3) {0.3, 0.0, 0.0}, &camera);
                            render_bvh(&camera, world, surface, &num_intersects);
                            SDL_UpdateWindowSurface(window);
                            break;
                        case SDLK_UP:
                            update_camera((Vec3) {0.0, 0.3, 0.0}, &camera);
                            render_bvh(&camera, world, surface, &num_intersects);
                            SDL_UpdateWindowSurface(window);
                            break;
                        case SDLK_DOWN:
                            update_camera((Vec3) {0.0, -0.3, 0.0}, &camera);
                            render_bvh(&camera, world, surface, &num_intersects);
                            SDL_UpdateWindowSurface(window);
                            break;
                        default:
                            break;
                    }
                    clock_t tok = clock();

                    int num_rays = camera.image_height * camera.image_width * camera.samples_per_pixel;
                    double int_per_ray = (double) num_intersects / num_rays;
                    printf("Drew frame: [%d rays,  %.2f tests/ray, %.2f ms, %.2f fps]\n", num_rays, int_per_ray, 1000.0 * ((double) (tok - tik) / CLOCKS_PER_SEC), CLOCKS_PER_SEC / (double) (tok - tik));
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
    //free_bvh(world);
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

void create_random_spheres_arr(Sphere *sphere_list) {
    int num_spheres = 0;
    Material ground_material = {.type=LAMBERTIAN, .albedo=(Color) {0.5, 0.5, 0.5}};
    sphere_list[0] = make_sphere((Point3) {0, -1000, 0}, 1000, ground_material);
    num_spheres++;

    Material mat1 = {.type=DIELECTRIC, .ir=1.5};
    sphere_list[num_spheres] = make_sphere((Point3) {0, 1, 0}, 1.0, mat1);
    num_spheres++;

    Material mat2 = {.type=LAMBERTIAN, .albedo=(Color) {0.4, 0.2, 0.1}};
    sphere_list[num_spheres] = make_sphere((Point3) {-4, 1, 0}, 1.0, mat2);
    num_spheres++;

    Material mat3 = {.type=METAL, .albedo=(Color) {0.7, 0.6, 0.5}, .fuzz=0.0};
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
                    Color albedo = random_vec();
                    Material diffuse_mat = {.type=LAMBERTIAN, .albedo=albedo};
                    sphere_list[num_spheres] = make_sphere(center, 0.2, diffuse_mat);
                    num_spheres++;
                } else if (choose_mat < 0.90) {
                    // Metal
                    Color albedo = random_vec_interval(0.5, 1);
                    double fuzz = random_double_interval(0, 0.5);
                    Material metal_mat = {.type=METAL, .albedo=albedo, .fuzz=fuzz};
                    sphere_list[num_spheres] = make_sphere(center, 0.2, metal_mat);
                    num_spheres++;
                } else {
                    // Glass
                    Material glass_mat = {.type=DIELECTRIC, .ir=1.5};
                    sphere_list[num_spheres] = make_sphere(center, 0.2, glass_mat);
                    num_spheres++;
                }
            }
        }
    }
}

BvhNode* create_random_spheres(int max_spheres) {
    // World
    Sphere sphere_list[500];

    int num_spheres = 0;
    Material ground_material = {.type=LAMBERTIAN, .albedo=(Color) {0.5, 0.5, 0.5}};
    sphere_list[0] = make_sphere((Point3) {0, -1000, 0}, 1000, ground_material);
    num_spheres++;

    Material mat1 = {.type=DIELECTRIC, .ir=1.5};
    sphere_list[num_spheres] = make_sphere((Point3) {0, 1, 0}, 1.0, mat1);
    num_spheres++;

    Material mat2 = {.type=LAMBERTIAN, .albedo=(Color) {0.4, 0.2, 0.1}};
    sphere_list[num_spheres] = make_sphere((Point3) {-4, 1, 0}, 1.0, mat2);
    num_spheres++;

    Material mat3 = {.type=METAL, .albedo=(Color) {0.7, 0.6, 0.5}, .fuzz=0.0};
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
                    Color albedo = random_vec();
                    Material diffuse_mat = {.type=LAMBERTIAN, .albedo=albedo};
                    sphere_list[num_spheres] = make_sphere(center, 0.2, diffuse_mat);
                    num_spheres++;
                } else if (choose_mat < 0.90) {
                    // Metal
                    Color albedo = random_vec_interval(0.5, 1);
                    double fuzz = random_double_interval(0, 0.5);
                    Material metal_mat = {.type=METAL, .albedo=albedo, .fuzz=fuzz};
                    sphere_list[num_spheres] = make_sphere(center, 0.2, metal_mat);
                    num_spheres++;
                } else {
                    // Glass
                    Material glass_mat = {.type=DIELECTRIC, .ir=1.5};
                    sphere_list[num_spheres] = make_sphere(center, 0.2, glass_mat);
                    num_spheres++;
                }
            }
        }
    }
    return build_bvh(sphere_list, 0, max_spheres);
}

void update_camera(Vec3 lookat_delta, Camera *camera) {
    // Image
    double aspect_ratio = 16.0 / 9.0;
    int image_width = IMAGE_WIDTH;
    int samples_per_pixel = 3;
    int max_depth = 15;
    double vfov = 20;
    Point3 lookfrom = {13, 2, 3};
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
            add_vec3(camera->lookat,lookat_delta),
            vup,
            defocus_angle,
            focus_dist
    );
}

Sphere* create_three_spheres_world_arr(Sphere sphere_list[]) {
    // World
    Material ground = {.type=LAMBERTIAN, .albedo=(Color) {0.8, 0.8, 0.0}};
    Material mat_center = {.type=LAMBERTIAN, .albedo=(Color) {0.1, 0.2, 0.5}};
    Material mat_left = {.type=DIELECTRIC, .ir=1.5};
    Material mat_right = {.type=METAL, .albedo=(Color) {0.8, 0.6, 0.2}, .fuzz=0.0};

    sphere_list[0] = make_sphere((Point3) { 0.0, -100.5, -1.0}, 100.0, ground);
    sphere_list[1] = make_sphere((Point3) { 0.0,    0.0, -1.0},   0.5, mat_center);
    sphere_list[2] = make_sphere((Point3) {-1.0,    0.0, -1.0},   0.5, mat_left);
    sphere_list[3] = make_sphere((Point3) {-1.0,    0.0, -1.0},  -0.4, mat_left);
    sphere_list[4] = make_sphere((Point3) { 1.0,    0.0, -1.0},   0.5, mat_right);

    return sphere_list;
}

Camera create_three_spheres_camera() {

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

