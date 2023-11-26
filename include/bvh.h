#pragma once

#include <stdlib.h>

#include "aabb.h"
#include "sphere.h"

typedef struct BvhNode {
    struct BvhNode *left;
    struct BvhNode *right;
    AABB bbox;
    const Sphere *sphere;
} BvhNode;

void print_bvh(const BvhNode *node, int level) {
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

int count_bvh(const BvhNode *node) {
    if (node == NULL) {
        return 0;
    }
    int l = count_bvh(node->left);        
    int r = count_bvh(node->right);

    return 1 + l + r;
}

void analyze_depth(const BvhNode *node, int currentDepth, int *maxDepth, int *totalLeaves, int *depthSum) {
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

// Function to calculate the overlap volume between two AABBs
double overlap_volume(AABB a, AABB b) {
    double dx = fmin(a.x.max, b.x.max) - fmax(a.x.min, b.x.min);
    double dy = fmin(a.y.max, b.y.max) - fmax(a.y.min, b.y.min);
    double dz = fmin(a.z.max, b.z.max) - fmax(a.z.min, b.z.min);

    if (dx > 0 && dy > 0 && dz > 0) {
        return dx * dy * dz; // Positive overlap in all dimensions
    }
    return 0; // No overlap
}

// Recursive function to calculate the total overlap volume in the BVH
double calculate_total_overlap(const BvhNode *node) {
    if (node == NULL || node->left == NULL || node->right == NULL) {
        return 0; // Base case: no overlap if node or its children are NULL
    }

    double overlap = overlap_volume(node->left->bbox, node->right->bbox);
    return overlap + calculate_total_overlap(node->left) + calculate_total_overlap(node->right);
}

// TODO: Implement a memory pool to keep number scene objects fixed
BvhNode* allocate_bvh();

void free_bvh(BvhNode *node) {
    if (node == NULL) {
        return; // Base case: node is null
    }

    // Recursively free left and right subtrees
    free_bvh(node->left);
    free_bvh(node->right);

    // Finally, free the node itself
    free(node);
}

static int current_axis = 0;

// Comparator function for sorting spheres
int compare_sphere(const void *a, const void *b) {
    Sphere *sphereA = (Sphere *)a;
    Sphere *sphereB = (Sphere *)b;
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

BvhNode* build_bvh_recursive(const Sphere spheres[], int start, int end, int depth) {
    if (start == end) {
        // Create a leaf node
        BvhNode* leaf = (BvhNode*)malloc(sizeof(BvhNode));
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
    BvhNode* node = (BvhNode*)malloc(sizeof(BvhNode));
    node->left = build_bvh_recursive(spheres, start, mid, depth + 1);
    node->right = build_bvh_recursive(spheres, mid + 1, end, depth + 1);
    node->bbox = create_aabb_for_aabb(&node->left->bbox, &node->right->bbox); 
    node->sphere = NULL;
    return node;
}

BvhNode* build_bvh(Sphere spheres[], int start, int end) {
    return build_bvh_recursive(spheres, start, end, 0);
}

bool ray_intersect_bvh(const BvhNode *node, const Ray *ray, Interval ray_t, HitRecord *record) {
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
    Interval new_int = {.min = ray_t.min, .max = hit_left ? record->t : ray_t.max};
    bool hit_right = node->right ? ray_intersect_bvh(node->right, ray, new_int, record) : false;

    return hit_left || hit_right;
}
