#pragma once

#include "aabb.h"
#include "sphere.h"
#include <stdlib.h>

typedef struct bvh_node {
    struct bvh_node *left;
    struct bvh_node *right;
    aabb bbox;
    sphere *sphere;
} bvh_node;

void print_bvh(bvh_node *node, int level) {
    for (int i = 0; i < level; i++) {
        printf("\t");
    }
    printf("Node level %d: ", level); print_aabb(&node->bbox);
    if (node->left != NULL) {
        print_bvh(node->left, level + 1);
    } 
    if (node->right != NULL) {
        print_bvh(node->right, level + 1);
    }
}

// TODO: Implement a memory pool to keep number scene objects fixed
bvh_node* allocate_bvh();

void free_bvh(bvh_node *node) {
    if (node == NULL) {
        return; // Base case: node is null
    }

    // Recursively free left and right subtrees
    free_bvh(node->left);
    free_bvh(node->right);

    // If your BVH nodes contain dynamically allocated memory, free it here
    // For example, if you have dynamically allocated AABBs:
    // free(node->bbox);

    // Finally, free the node itself
    free(node);
}

bvh_node* build_bvh(sphere spheres[], int start, int end) {
    if (start == end) {
        // Create a leaf node
        bvh_node* leaf = (bvh_node*)malloc(sizeof(bvh_node));
        leaf->sphere = &spheres[start];
        leaf->bbox = create_aabb_for_sphere(leaf->sphere); 
        leaf->left = leaf->right = NULL;
        return leaf;
    } else {
        // Split spheres and create internal node
        int mid = (start + end) / 2;
        bvh_node* node = (bvh_node*)malloc(sizeof(bvh_node));
        node->left = build_bvh(spheres, start, mid);
        node->right = build_bvh(spheres, mid + 1, end);
        node->bbox = create_aabb_for_aabb(&node->left->bbox, &node->right->bbox); 
        node->sphere = NULL;
        return node;
    }
}

bool ray_intersect_bvh(bvh_node *node, ray *ray, interval ray_t, hit_record *record) {
    if (node == NULL) {
        return false;
    }

    // Check for ray intersection with the node's AABB
    if (!hit_aabb(ray, ray_t, &node->bbox)) {
        return false; // Ray does not intersect the bounding box
    }

    // Check if this is a leaf node
    if (node->left == NULL && node->right == NULL && node->sphere != NULL) {
        // Test intersection with the sphere at this leaf node
        return hit(ray, node->sphere, &ray_t, record);
    }

    // If not a leaf node, recursively check children
    bool hitLeft = node->left ? ray_intersect_bvh(node->left, ray, ray_t, record) : false;
    bool hitRight = node->right ? ray_intersect_bvh(node->right, ray, ray_t, record) : false;

    return hitLeft || hitRight;
}

