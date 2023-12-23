#include <SDL2/SDL_events.h>
#include <stdlib.h>
#include <time.h>

#include "bvh.h"
#include "scene.h"
#include "camera.h"
#include "triangle.h"

int main() {
    TinyObjData data = {0};
    int ret = get_obj_data_from_file("assets/low_poly_tree/Lowpoly_tree_sample.obj", &data);
    //int ret = get_obj_data_from_file("assets/cube.obj", &data);
    if (ret == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    printf("Number of triangles: %d\n", data.attrib.num_faces / 3);
    printf("Number of vertices: %d\n", data.attrib.num_vertices);
    printf("num_face_num_verts: %d\n", data.attrib.num_face_num_verts);
    printf("Number of normals: %d\n\n", data.attrib.num_normals);

    #define NUM_TRIANGLES 500
    int n_tris = data.attrib.num_faces / 3;

    Triangle triangles[NUM_TRIANGLES] = {0};
    TriangleMesh mesh = {.triangles=triangles, .size=NUM_TRIANGLES};
    Material mat = {.type=METAL, .albedo=(Color) {0.7, 0.6, 0.5}, .fuzz=0.1};
    convert_obj_data_to_mesh(&data, &mesh, &mat);

    Point3 center = {0};

    for (int i = 0 ; i < n_tris; i++) {
        Triangle t = triangles[i];
        center.x += t.v1.x + t.v2.x + t.v3.x;
        center.y += t.v1.y + t.v2.y + t.v3.y;
        center.z += t.v1.z + t.v2.z + t.v3.z;
    }

    center.x = center.x / (double) n_tris;
    center.y = center.y / (double) n_tris;
    center.y /= 2.;
    center.z = center.z / (double) n_tris;
    printf("Center of mass: [%f, %f, %f]\n", center.x, center.y, center.z);

    // Image
    #define IMAGE_WIDTH 720

    double aspect_ratio = 16.0 / 9.0;
    int image_width = IMAGE_WIDTH;
    int samples_per_pixel = 3;
    int max_depth = 3;
    double vfov = 20;
    Point3 lookfrom = {35, 50, 3};
    Point3 lookat = center;
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

    BvhNode* bvh = build_bvh_tri(triangles, n_tris);
    double overlap = calculate_total_overlap(bvh);
    printf("num nodes in bvh: %d, overlap: %f\n", count_bvh(bvh), overlap);

    for (int i = 1; i < 5; i++) {
        clock_t tik = clock();
        int num_intersects = 0;
        render_bvh(&camera, bvh, surface, &num_intersects);
        //render_triangles(&camera, n_tris, triangles, surface, &num_intersects);
        SDL_UpdateWindowSurface(window);
        clock_t tok = clock();

        int num_rays = camera.image_height * camera.image_width * camera.samples_per_pixel;
        double int_per_ray = (double) num_intersects / num_rays;
        printf("Drew frame: [%d rays,  %.2f tests/ray, %.2f ms, %.2f fps]\n", num_rays, int_per_ray, 1000.0 * ((double) (tok - tik) / CLOCKS_PER_SEC), CLOCKS_PER_SEC / (double) (tok - tik));
    }

    // Run until user quits
    SDL_Event event;
    while(event.type != SDL_QUIT) {
        SDL_PollEvent(&event);
    }

    // Cleanup 
    tinyobj_attrib_free(&data.attrib);
    tinyobj_shapes_free(data.shapes, data.num_shapes);
    tinyobj_materials_free(data.materials, data.num_materials);

    free_bvh(bvh);

    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

