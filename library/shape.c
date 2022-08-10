#include "shape.h"
#include <math.h>

const int CIRCLE = 0;
const int POLYGON = 1;

// The number of vertices in a circle.
const int CIRCLE_vertices = 60;


// A struct which stores a CIRCLE or POLYGON shape and the corresponding 
// vertices. If the shape is of CIRCLE type, it also stores the radius.
typedef struct shape
{
    int type;
    double radius;
    vector_t centroid;

    list_t* vertices;
    extrema_t extrema;

} shape_t;


// Initializes a CIRCLE type shape struct.
shape_t* shape_init_circle(vector_t position, double radius)
{
    shape_t* shape = malloc(sizeof(shape_t));

    list_t* vertices = list_init(CIRCLE_vertices, free);
    
    // Generates vertices for the circle (used in collision handling).
    vector_t curr_v = {position.x + radius, position.y};
    for (size_t i = 0; i < CIRCLE_vertices; i ++)
    {
        vector_t* v = malloc(sizeof(vector_t));
        *v = curr_v;
        list_add(vertices, v);
        
        // vec_rotate now rotates a vector about an inputted position.
        curr_v = vec_rotate(curr_v, 2 * M_PI / CIRCLE_vertices, position);

    }
    shape -> vertices = vertices;
    shape -> type = CIRCLE;
    shape -> radius = radius;
    shape -> centroid = position;
    shape -> extrema = shape_extrema(shape);

    return shape;
}


// Initializes a rectangle. The first input is the bottom-left vertex, and the 
// second input is the top-right vertex.
shape_t* shape_init_rectangle(vector_t v1, vector_t v2)
{
    // Make sure that the vertices make sense.
    assert(v1.x - v2.x < 0 || v1.y - v2.y < 0);

    list_t* vertices = list_init(4, free);
    vector_t* vertex1 = malloc(sizeof(vector_t));
    *vertex1 = v1;
    list_add(vertices, vertex1);
    vector_t* vertex2 = malloc(sizeof(vector_t));
    *vertex2 = (vector_t) {v2.x, v1.y};
    list_add(vertices, vertex2);
    vector_t* vertex3 = malloc(sizeof(vector_t));
    *vertex3 = v2;
    list_add(vertices, vertex3);
    vector_t* vertex4 = malloc(sizeof(vector_t));
    *vertex4 = (vector_t) {v1.x, v2.y};
    list_add(vertices, vertex4);

    shape_t *shape = shape_init_polygon(vertices);
    return shape;
}


// Initializes a POLYGON type shape struct.
shape_t* shape_init_polygon(list_t* vertices)
{
    assert(vertices != NULL);
    shape_t* shape = malloc(sizeof(shape_t));
    assert(list_size(vertices) >= 3 &&
        "Polygon must be initialized with at least three points.");

    shape -> centroid = polygon_centroid(vertices);
    double max_magnitude = 0;
    for (size_t i = 0; i < list_size(vertices); i++)
    {   
        vector_t dist_center = vec_subtract(*(vector_t*) list_get(vertices, i), shape -> centroid);
        double cur_magnitude = vec_magnitude(dist_center);
        if(cur_magnitude > max_magnitude)
        {
            max_magnitude = cur_magnitude;
        }
    }
    shape -> vertices = vertices;
    shape -> type = POLYGON;
    shape -> radius = max_magnitude;
    shape -> extrema = shape_extrema(shape);

    return shape;
}


// Frees the memory allocated for a shape object.
void shape_free(shape_t* shape)
{
    list_free(shape -> vertices);
    free(shape);
}



// Accessors and other operations ---------------------------------------------


list_t* shape_normals(shape_t* shape)
{
    assert(shape -> type == CIRCLE || shape -> type == POLYGON);
    
    list_t* normals = list_init(list_size(shape_vertices(shape)), free);
    list_t* vertices = shape -> vertices;

    if (shape_type(shape) == CIRCLE)
    {
        size_t size = list_size(vertices);
        normals = list_init(size, free);
        for (size_t i = 0; i < size; i ++)
        {
            vector_t v = *(vector_t*) list_get(vertices, i);
            vector_t* normal = malloc(sizeof(vector_t));
            
            // All normal vectors are radially outwards.
            *normal = vec_normalize(vec_subtract(v, shape_centroid(shape)));
            list_add(normals, normal);
        }
    }
    else if (shape_type(shape) == POLYGON)
    {
        normals = polygon_normals(shape -> vertices);
    }

    assert(normals != NULL);
    return normals;
}


// Returns an extrema_t object, which stores the minimum and maximum x and y 
// values of the shape (for use in collisions).
extrema_t shape_extrema(shape_t* shape)
{
    assert(shape -> type == CIRCLE || shape -> type == POLYGON);
    extrema_t extrema = {INFINITY, -1 * INFINITY, INFINITY, -1 * INFINITY};

    if (shape_type(shape) == CIRCLE)
    {
        vector_t c = shape -> centroid;
        double r = shape -> radius;
        // extrema_t stores {min_x, max_x, min_y, max_y}
        extrema = (extrema_t) {c.x - r, c.x + r, c.y - r, c.y + r};
    }
    else if (shape_type(shape) == POLYGON)
    {
        list_t* vertices = shape -> vertices;
        
        size_t size = list_size(vertices);
        for (size_t i = 0; i < size; i ++)
        {
            vector_t v = *(vector_t*) list_get(vertices, i);
            if (v.x > extrema.max_x) {extrema.max_x = v.x;}
            if (v.x < extrema.min_x) {extrema.min_x = v.x;}
            if (v.y > extrema.max_y) {extrema.max_y = v.y;}
            if (v.y < extrema.min_y) {extrema.min_y = v.y;}
        }
    }

    return extrema;
}

extrema_t shape_get_extrema(shape_t* shape)
{
    return shape -> extrema;
}

// Returns the area of the inputted shape.
double shape_area(shape_t* shape)
{
    if (shape_type(shape) == POLYGON)
    {
        return polygon_area(shape -> vertices);
    }
    else if (shape_type(shape) == CIRCLE)
    {
        return M_PI * pow(shape -> radius, 2);
    }
    // If neither circle nor polygon
    return -1;
}


// Translates a shape by a given vector.
void shape_translate(shape_t* shape, vector_t translation)
{
    // Update the centroid.
    shape -> centroid = vec_add(shape -> centroid, translation);
    shape -> extrema = (extrema_t) {shape -> extrema.min_x + translation.x, shape -> extrema.max_x + translation.x, 
    shape -> extrema.min_y + translation.y, shape -> extrema.max_y + translation.y};

    size_t size = list_size(shape -> vertices);
    for (size_t i = 0; i < size; i ++)
    {
        vector_t* v = list_get(shape -> vertices, i);
        *v = vec_add(*v, translation);
    }
}


// Rotates vertices in a shape by a given angle about a given point.
void shape_rotate(shape_t* shape, double angle, vector_t position)
{
    // Update the centroid.
    shape -> centroid = vec_rotate(shape -> centroid, angle, position);

    size_t size = list_size(shape -> vertices);
    for (size_t i = 0; i < size; i ++)
    {
        vector_t* v = list_get(shape -> vertices, i);
        *v = vec_rotate(*v, angle, position);
    }
}


// Gets the centroid of the shape.
vector_t shape_centroid(shape_t* shape)
{
    return shape -> centroid;
}


// Gets the type of the shape.
int shape_type(shape_t* shape)
{
    return shape -> type;
}

list_t* shape_vertices(shape_t* shape)
{
    return shape -> vertices;
}

// Gets the radius of the shape, if the shape is a CIRCLE.
double shape_radius(shape_t* shape)
{
    return shape -> radius;
}
