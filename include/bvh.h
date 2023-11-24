#pragma once

#include <stdint.h>

#include "sphere.h"
#include "vec3.h"
#include "aabb.h"

typedef struct bvh_node {
    vec3 aabb_min, aabb_max;
    uint8_t left_indx, first_indx, obj_count;

} bvh_node;

bool is_leaf(bvh_node *node) {return node->obj_count > 0;}

void update_node_bounds(int node_id, bvh_node **tree, int tree_size);
void subdivide_tree(int node_id, bvh_node **tree, int tree_size);

void build_bvh(bvh_node **tree, int tree_size, sphere world[], int world_size) {
    uint8_t root_node_i = 0;
    uint8_t nodes_used = 1;

    bvh_node *root = tree[root_node_i];
    root->left_indx = 0;
    root->first_indx = 0;
    root->obj_count = world_size;
    update_node_bounds(root_node_i, tree, tree_size);
    subdivide_tree(root_node_i, tree, tree_size);
}

void ray_hit_bvh(ray *ray, interval ray_t, int node_idx, bvh_node **tree) {
    bvh_node *node = tree[node_idx];
    if (!hit_aabb(ray, ray_t, NULL)) {
        return;
    }
    if (is_leaf(node)) {
    } else {
        ray_hit_bvh(ray, ray_t, node->left_indx, tree);
        ray_hit_bvh(ray, ray_t, node->left_indx + 1, tree);
    }
}
