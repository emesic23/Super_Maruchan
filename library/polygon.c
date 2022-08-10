#include "polygon.h"


// Computes the area of a polygon.
//  A = 0.5 * |[sum from i = 1 to (n - 1) x_i * y_(i + 1) + ... + x_n * y_1] -
//  [sum from i = 1 to (n - 1) y_i * x_(i + 1) + ... + y_n * x_1]
double polygon_area(list_t* vertices)
{
    double area = 0;
    // n is the number of vertices/sides in the polygon.
    size_t n = list_size(vertices);

    for (size_t i = 0; i < (n - 1); i ++)
    {
        // Note that j = (i + 1).
        vector_t* v_i = list_get(vertices, i);
        vector_t* v_j = list_get(vertices, i + 1);
        area += vec_cross(*v_i, *v_j);
    }
    // Account for the edge cases in the formula.
    vector_t* v_i = list_get(vertices, n - 1);
    vector_t* v_j = list_get(vertices, 0);
    area += vec_cross(*v_i, *v_j);

    area = fabs(area) * 0.5;

    return area;
}


// Returns a list with all unit normals of a shape, for use in collision.c.
list_t* polygon_normals(list_t *vertices)
{
    list_t *normals = list_init(list_size(vertices), free);
    
    // 1. Computes all edges of polygon.
    // 2. gets the orthogonal vector, and
    // 3. normalizes that vector and adds it to list.
    size_t size = list_size(vertices);
    for (size_t i = 0; i < size; i++)
    {
        vector_t v1 = *(vector_t*) list_get(vertices, i);
        vector_t v2;

        // Edge case for first and last vector.
        if ((i + 1) == size) {v2 = *(vector_t*) list_get(vertices, 0);}
        else {v2 = *(vector_t*) list_get(vertices, i + 1);}
        
        vector_t edge = vec_subtract(v2, v1);
        
        vector_t* normal = malloc(sizeof(vector_t));
        vector_t v = vec_normalize(vec_perpendicular(edge));

        *normal = v;
        list_add(normals, normal);
    }
    return normals;
}


// Computes the centroid of a polygon.
vector_t polygon_centroid(list_t* vertices)
{
    vector_t centroid = {0, 0};
    size_t n = list_size(vertices);

    for (size_t i = 0; i < n - 1; i ++)
    {
        // Sum of (x_i + x_(i+1))(x_i * y_(i+1) - x_(i+1) * y_i).
        vector_t* v_i = list_get(vertices, i);
        vector_t* v_j = list_get(vertices, i + 1);
        centroid.x += (v_i -> x + v_j -> x) * vec_cross(*v_i, *v_j);
        centroid.y += (v_i -> y + v_j -> y) * vec_cross(*v_i, *v_j);
    }
    // Account for the edge case in formula.
    vector_t* v_i = list_get(vertices, n - 1);
    vector_t* v_j = list_get(vertices, 0);
    centroid.x += (v_i -> x + v_j -> x) * vec_cross(*v_i, *v_j);
    centroid.y += (v_i -> y + v_j -> y) * vec_cross(*v_i, *v_j);

    // Divide the cumulative sum by 6 * area.
    centroid.x *= 1 / (6 * polygon_area(vertices));
    centroid.y *= 1 / (6 * polygon_area(vertices));

    return centroid;
}


