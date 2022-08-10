#include <stdlib.h>
#include <stdio.h>

#include "collision.h"
#include "polygon.h"
#include "list.h"
#include "math.h"
#include "vector.h"
#include "body.h"



// Calculates the overlap of two vectors along the same axis. If there is no overlap, returns -1.
double get_overlap(vector_t v1, vector_t v2)
{
    // Checks to see if there is overlap.
    if (v1.x > v2.y || v2.x > v1.y) // x is min and y is max of that component 
    {
      return -1;
    }

    // Check this later if need absolute value here or not
    double a = (v1.y < v2.y) ? v1.y : v2.y;
    double b = (v1.x > v2.x) ? v1.x : v2.x;
    
    return fabs(b - a);
}


// If objects are close then return true.
bool collision_possible(shape_t *shape1, shape_t *shape2, collision_info_t info)
{
    // Checks if they overlap on x-axis and y-axis
    // vector_t s1 = shape_centroid(shape1);
    // double r1 = shape_radius(shape1);

    // vector_t s2 = shape_centroid(shape2);
    // double r2 = shape_radius(shape2);
    extrema_t extrema1 = shape_get_extrema(shape1);
    extrema_t extrema2 = shape_get_extrema(shape2);
    // extrema_t extrema1 = {vec_add(s1, (vector_t) {-1 * r1, 0}).x, vec_add(s1, (vector_t) {r1, 0}).x, vec_add(s1, (vector_t) {0, -1 * r1}).y, vec_add(s1, (vector_t) {0, r1}).y};
    // extrema_t extrema2 = {vec_add(s2, (vector_t) {-1 * r2, 0}).x, vec_add(s2, (vector_t) {r2, 0}).x, vec_add(s2, (vector_t) {0, -1 * r2}).y, vec_add(s2, (vector_t) {0, r2}).y};
    double overlap1 = get_overlap((vector_t) {extrema1.min_x, extrema1.max_x}, (vector_t) {extrema2.min_x, extrema2.max_x});
    double overlap2 = get_overlap((vector_t) {extrema1.min_y, extrema1.max_y}, (vector_t) {extrema2.min_y, extrema2.max_y});
    if ( overlap1 > 0
            && overlap2 > 0)
    {
        // If they overlap on both axes then they are probably close.
        // IT has to be both because if they don't overlap on one than SAT thoerem
        // already states that no collision is happening
        info.overlap_v = (vector_t) {overlap1, overlap2};
        return true;
    }
    return false;
}


// Projects each vertex in the shape onto the inputted axis, and returns the coordinates along 
// the axis which form the largest "shadow" of the shape.
vector_t shape_project(shape_t *shape, vector_t axis)
{
    list_t* vertices = shape_vertices(shape);

    double min = vec_dot(axis, *(vector_t*) list_get(vertices, 0));
    double max = vec_dot(axis, *(vector_t*) list_get(vertices, 0));

    for (size_t i = 0; i < list_size(vertices); i++)
    {
        double p = vec_dot(axis, *(vector_t*) list_get(vertices, i));

        if (p < min) {min = p;}
        if (p > max) {max = p;}
    }

    return (vector_t) {min, max};
}



// Returns the axis along which the objects have collided and whether or not
// the two objects have collided (contained by collision_info_t).
collision_info_t find_collision(body_t *body1, body_t *body2)
{
    shape_t* shape1 = body_get_shape(body1);
    shape_t* shape2 = body_get_shape(body2);

    collision_info_t info;
    info.collided = true;

    // Preliminary check to see if a collision is likely.
    if (!collision_possible(shape1, shape2, info))
    {
        info.collided = false;
        shape_free(shape1);
        shape_free(shape2);
        return info;
    }

    // Initialize min_overlap.
    double overlap = INFINITY;

    list_t *normals = shape_normals(shape1);

    list_extend(normals, shape_normals(shape2));
    
    size_t size = list_size(normals);
    for (size_t i = 0; i < size; i++)
    {
        // Project each shape onto a normal vector.
        vector_t v1 = shape_project(shape1, *(vector_t*) list_get(normals, i));
        vector_t v2 = shape_project(shape2, *(vector_t*) list_get(normals, i));

        if (get_overlap(v1, v2) < 0)
        {
            // If there is one case of no overlap then the objects don't overlap
            // This is due to SAT theorem
            info.collided = false;
            break;        
        }

        double curr_overlap = get_overlap(v1, v2);
        if (curr_overlap < overlap)
        {
            overlap = curr_overlap;
            info.axis = *(vector_t*) list_get(normals, i);
            // adjusting verticies to get rid of really small numbers like -1e8 and 1e8
            info.overlap = overlap;
        }
    }
   
    list_free(normals);
    shape_free(shape1);
    shape_free(shape2);

    return info;
}
