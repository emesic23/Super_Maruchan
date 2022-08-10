#include "vector.h"
#include "list.h"
#include "vector.h"

#include <math.h>
#include <assert.h>
#include <stdlib.h>


// Computes the area of a polygon.
//  A = 0.5 * |[sum from i = 1 to (n - 1) x_i * y_(i + 1) + ... + x_n * y_1] -
//  [sum from i = 1 to (n - 1) y_i * x_(i + 1) + ... + y_n * x_1]|
double polygon_area(list_t* verticies);


// Returns a list with all unit normals of a shape, for use in collision.c.
list_t* polygon_normals(list_t* verticies);


// Computes the centroid of a polygon.
vector_t polygon_centroid(list_t* verticies);

