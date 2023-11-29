#include <stdlib.h>

#include "triangle.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

void load_obj_file(void *ctx, const char * filename, const int is_mtl, const char *obj_filename, char ** buffer, size_t * len)
{
    long string_size = 0, read_size = 0;
    FILE * handler = fopen(filename, "r");

    if (handler) {
        fseek(handler, 0, SEEK_END);
        string_size = ftell(handler);
        rewind(handler);
        *buffer = (char *) malloc(sizeof(char) * (string_size + 1));
        read_size = fread(*buffer, sizeof(char), (size_t) string_size, handler);
        (*buffer)[string_size] = '\0';
        if (string_size != read_size) {
            free(buffer);
            *buffer = NULL;
        }
        fclose(handler);
    }

    *len = read_size;
}

typedef struct TinyObjData {
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes;
    size_t num_shapes;
    tinyobj_material_t* materials;
    size_t num_materials;
} TinyObjData;

int get_obj_data_from_file(const char* filename, TinyObjData* data) {
    unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
    int ret = tinyobj_parse_obj(&data->attrib, &data->shapes, &data->num_shapes, &data->materials,
                                &data->num_materials, filename, load_obj_file, NULL, flags);

    if (ret != 0) {
        printf("Failed to parse %s for some reason :(\n", filename);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void convert_obj_data_to_mesh(TinyObjData* data, TriangleMesh* mesh, Material* mat) {
    size_t face_offset = 0;
    for (uint32_t i = 0; i < data->attrib.num_face_num_verts; i++) {
        for (uint32_t face_id = 0; face_id < (size_t) data->attrib.face_num_verts[i] / 3; face_id++) {
            tinyobj_vertex_index_t idx0 = data->attrib.faces[face_offset + 3 * face_id + 0];
            tinyobj_vertex_index_t idx1 = data->attrib.faces[face_offset + 3 * face_id + 1];
            tinyobj_vertex_index_t idx2 = data->attrib.faces[face_offset + 3 * face_id + 2];

            int f0 = idx0.v_idx;
            int f1 = idx1.v_idx;
            int f2 = idx2.v_idx;
            assert(f0 >= 0);
            assert(f1 >= 0);
            assert(f2 >= 0);

            float v00 = data->attrib.vertices[3 * (size_t)f0 + 0];
            float v01 = data->attrib.vertices[3 * (size_t)f0 + 1];
            float v02 = data->attrib.vertices[3 * (size_t)f0 + 2];
            Point3 v0 = {(double) v00, (double) v01, (double) v02};

            float v10 = data->attrib.vertices[3 * (size_t)f1 + 0];
            float v11 = data->attrib.vertices[3 * (size_t)f1 + 1];
            float v12 = data->attrib.vertices[3 * (size_t)f1 + 2];
            Point3 v1 = {(double) v10, (double) v11, (double) v12};

            float v20 = data->attrib.vertices[3 * (size_t)f2 + 0];
            float v21 = data->attrib.vertices[3 * (size_t)f2 + 1];
            float v22 = data->attrib.vertices[3 * (size_t)f2 + 2];
            Point3 v2 = {(double) v20, (double) v21, (double) v22};

            // Assume all normals are the same?
            Point3 normal;
            if (data->attrib.num_normals > 0) {
                int n0 = idx0.vn_idx;
                float n00 = data->attrib.normals[3 * (size_t)n0 + 0];
                float n01 = data->attrib.normals[3 * (size_t)n0 + 1];
                float n02 = data->attrib.normals[3 * (size_t)n0 + 2];
                //int n1 = idx1.vn_idx;
                //int n2 = idx2.vn_idx;
                normal = (Point3) {(double) n00, (double) n01, (double) n02};
            } else {
                Vec3 e1 = diff_vec3(v1, v0);
                Vec3 e2 = diff_vec3(v2, v0);
                normal = cross(e1, e2);
            }

            Triangle tri = {.v1=v0, .v2=v1, .v3=v2, .normal=normal, .mat=*mat};
            mesh->triangles[i] = tri;

        }
        face_offset += (size_t) data->attrib.face_num_verts[i];
    }
}
