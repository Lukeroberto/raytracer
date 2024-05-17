#pragma once

#include <stdlib.h>

#include "aabb.h"
#include "geometry.h"

typedef struct BvhNode {
    struct BvhNode *left;
    struct BvhNode *right;
    AABB bbox;
    Geometry *geom;
    int geom_count;
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

    
    free((Geometry*) node->geom);

    // Finally, free the node itself
    free(node);
}

int sortAxis;

int compareCentroids(const void* a, const void* b) {
    Vec3* centroidA = (Vec3*)a;
    Vec3* centroidB = (Vec3*)b;

    if (sortAxis == 0) { // Sorting along the x-axis
        return (centroidA->x > centroidB->x) - (centroidA->x < centroidB->x);
    } else if (sortAxis == 1) { // Sorting along the y-axis
        return (centroidA->y > centroidB->y) - (centroidA->y < centroidB->y);
    } else { // Sorting along the z-axis
        return (centroidA->z > centroidB->z) - (centroidA->z < centroidB->z);
    }
}

// Function to build the BVH
BvhNode* build_bvh_geoms(Geometry geoms[], int length, int depth) {
    BvhNode* node = (BvhNode*)malloc(sizeof(BvhNode));
    node->left = node->right = NULL;
    node->geom = NULL;
    node->geom_count = 0;


    if (length <= 0) {
        return node;
    }

    // Compute overall bounding box for this node
    node->bbox = create_aabb_for_geom(&geoms[0]);
    // Calculate centroids to determine splitting point
    Vec3* centroids = (Vec3*)malloc(sizeof(Vec3) * length);
    for (int i = 1; i < length; ++i) {
        AABB bounds = create_aabb_for_geom(&geoms[i]);
        centroids[i] = center_aabb(&bounds);
        node->bbox = create_aabb_for_aabb(&node->bbox, &bounds);
    }

    // Base case: if there's only one geom, this is a leaf node
    if (length == 1) {
        node->geom = geoms;
        node->geom_count = 1;
        return node;
    }


    // Choose the axis to split based on the depth
    Vec3 extent = diff_vec3((Vec3) {node->bbox.x.max, node->bbox.y.max, node->bbox.z.max}, (Vec3) {node->bbox.x.min, node->bbox.y.min, node->bbox.z.min});
    if (extent.x > extent.y && extent.x > extent.z) sortAxis = 0;
    if (extent.y > extent.x && extent.y > extent.z) sortAxis = 1;
    if (extent.z > extent.x && extent.z > extent.y) sortAxis = 2;

    // Sort centroids
    qsort(centroids, length, sizeof(Vec3), compareCentroids);

    // Split triangles into two groups at the median
    int median = length / 2;

    Geometry* left_geom = (Geometry*)malloc(sizeof(Geometry) * median);
    Geometry* right_geom = (Geometry*)malloc(sizeof(Geometry) * (length - median));
    int leftCount = 0, rightCount = 0;

    for (int i = 0; i < length; ++i) {
        if (i < median) {
            left_geom[leftCount++] = geoms[i];
        } else {
            right_geom[rightCount++] = geoms[i];
        }
    }

    // Recursively build left and right children
    node->left = build_bvh_geoms(left_geom, leftCount, depth + 1);
    node->right = build_bvh_geoms(right_geom, rightCount, depth + 1);

    free(centroids);

    return node;
}


BvhNode* build_bvh(Geometry geoms[], int length) {
    return build_bvh_geoms(geoms, length, 0);
}

bool ray_intersect_bvh(const BvhNode *node, const Ray *ray, Interval ray_t, HitRecord *record, int *num_intersects, int depth) {
    if (node == NULL) {
        return false;
    }

    // Check for ray intersection with the node's AABB
    (*num_intersects)++;
    if (!hit_aabb(ray, ray_t, &node->bbox)) {
        return false; // Ray does not intersect the bounding box
    }

    // Check if this is a leaf node
    if (node->left == NULL && node->right == NULL && (node->geom != NULL)) {
        // Test intersection with the sphere at this leaf node
        return ray_intersect_geom(ray, node->geom, &ray_t, record, num_intersects);
    }

    // If not a leaf node, recursively check children
    bool hit_left = node->left ? ray_intersect_bvh(node->left, ray, ray_t, record, num_intersects, depth + 1) : false;
    Interval new_int = {.min = ray_t.min, .max = hit_left ? record->t : ray_t.max};
    bool hit_right = node->right ? ray_intersect_bvh(node->right, ray, new_int, record, num_intersects, depth + 1) : false;

    return hit_left || hit_right;
}
