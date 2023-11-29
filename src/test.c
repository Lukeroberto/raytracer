#include <SDL2/SDL_events.h>
#include <stdlib.h>
#include <time.h>

#include "scene.h"
#include "camera.h"

int main() {
    TinyObjData data = {0};
    int ret = get_obj_data_from_file("assets/box.obj", &data);
    if (ret == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    printf("Number of triangles: %d\n", data.attrib.num_face_num_verts);
    printf("Number of normals: %d\n", data.attrib.num_normals);

    #define NUM_TRIANGLES 36

    Triangle triangles[NUM_TRIANGLES] = {0};
    TriangleMesh mesh = {.triangles=triangles, .size=NUM_TRIANGLES};
    Material mat = {.type=METAL, .albedo=(Color) {0.7, 0.6, 0.5}, .fuzz=0.0};

    convert_obj_data_to_mesh(&data, &mesh, &mat);


    // Image
    #define IMAGE_WIDTH 720

    double aspect_ratio = 16.0 / 9.0;
    int image_width = IMAGE_WIDTH;
    int samples_per_pixel = 3;
    int max_depth = 5;
    double vfov = 20;
    Point3 lookfrom = {13, 2, 3};
    Point3 lookat = {0, 0, 0};
    Vec3 vup = {0, 1, 0};
    double defocus_angle = 0.6;
    double focus_dist = 10.0;

    Camera camera = create_camera(
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

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, camera.image_width, camera.image_height, 0);
    SDL_Surface * surface = SDL_GetWindowSurface(window);

    // Run until user quits
    int quit = 0;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            clock_t tik = clock();
            render_triangles(&camera, NUM_TRIANGLES, triangles, surface);
            SDL_UpdateWindowSurface(window);
            clock_t tok = clock();
            printf("Drew frame in %f ms, %f fps\n", 1000.0 * ((double) (tok - tik) / CLOCKS_PER_SEC), CLOCKS_PER_SEC / (double) (tok - tik));
        }
    }

    // Cleanup 
    tinyobj_attrib_free(&data.attrib);
    tinyobj_shapes_free(data.shapes, data.num_shapes);
    tinyobj_materials_free(data.materials, data.num_materials);

    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

