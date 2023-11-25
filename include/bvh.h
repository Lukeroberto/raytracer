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

int count_bvh(bvh_node *node) {
    if (node == NULL) {
        return 0;
    }
    int l = count_bvh(node->left);        
    int r = count_bvh(node->right);

    return 1 + l + r;
}

void analyze_depth(bvh_node *node, int currentDepth, int *maxDepth, int *totalLeaves, int *depthSum) {
    if (node == NULL) return;

    if (node->left == NULL && node->right == NULL) {
        // Leaf node
        *totalLeaves += 1;
        *depthSum += currentDepth;
        if (currentDepth > *maxDepth) *maxDepth = currentDepth;
        return;
    }

    analyze_depth(node->left, currentDepth + 1, maxDepth, totalLeaves, depthSum);
    analyze_depth(node->right, currentDepth + 1, maxDepth, totalLeaves, depthSum);
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

static int current_axis = 0;

// Comparator function for sorting spheres
int compare_sphere(const void *a, const void *b) {
    sphere *sphereA = (sphere *)a;
    sphere *sphereB = (sphere *)b;
    double valA, valB;

    switch (current_axis) {
        case 0: // X-axis
            valA = sphereA->center.x;
            valB = sphereB->center.x;
            break;
        case 1: // Y-axis
            valA = sphereA->center.y;
            valB = sphereB->center.y;
            break;
        case 2: // Z-axis
            valA = sphereA->center.z;
            valB = sphereB->center.z;
            break;
        default:
            // Default to X-axis if something goes wrong
            valA = sphereA->center.x;
            valB = sphereB->center.x;
            break;
    }

    if (valA < valB) return -1;
    else if (valA > valB) return 1;
    else return 0;
}

bvh_node* build_bvh_recursive(sphere spheres[], int start, int end, int depth) {
    if (start == end) {
        // Create a leaf node
        bvh_node* leaf = (bvh_node*)malloc(sizeof(bvh_node));
        leaf->sphere = &spheres[start];
        leaf->bbox = create_aabb_for_sphere(leaf->sphere); 
        leaf->left = leaf->right = NULL;
        return leaf;
    }
    // Sort spheres based on chosen axis
    //current_axis = depth % 3;
    //qsort(spheres + start, end - start, sizeof(sphere), compare_sphere);

    // Split spheres and create internal node
    int mid = (end + start) / 2;
    bvh_node* node = (bvh_node*)malloc(sizeof(bvh_node));
    node->left = build_bvh_recursive(spheres, start, mid, depth + 1);
    node->right = build_bvh_recursive(spheres, mid + 1, end, depth + 1);
    node->bbox = create_aabb_for_aabb(&node->left->bbox, &node->right->bbox); 
    node->sphere = NULL;
    return node;
}

bvh_node* build_bvh(sphere spheres[], int start, int end) {
    return build_bvh_recursive(spheres, start, end, 0);
}

bool ray_intersect_bvh(bvh_node *node, ray *ray, interval ray_t, hit_record *record) {
    if (node == NULL) {
        return false;
    }

    // Check for ray intersection with the node's AABB
    record->num_tests++;
    if (!hit_aabb(ray, ray_t, &node->bbox)) {
        return false; // Ray does not intersect the bounding box
    }

    // Check if this is a leaf node
    if (node->left == NULL && node->right == NULL && node->sphere != NULL) {
        // Test intersection with the sphere at this leaf node
        record->num_tests++;
        return hit(ray, node->sphere, &ray_t, record);
    }

    // If not a leaf node, recursively check children
    bool hit_left = node->left ? ray_intersect_bvh(node->left, ray, ray_t, record) : false;
    interval new_int = {.min = ray_t.min, .max = hit_left ? record->t : ray_t.max};
    bool hit_right = node->right ? ray_intersect_bvh(node->right, ray, new_int, record) : false;

    return hit_left || hit_right;
}
