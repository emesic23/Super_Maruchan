#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "vector.h"
#include "list.h"
#include "polygon.h"
#include "color.h"

#include <math.h>
#include <stdlib.h>
#include <assert.h>

// A struct which stores four doubles representing the minimum and maximum y 
// values. {min_x, max_x, min_y, max_y}
typedef struct extrema
{
    double min_x;
    double max_x;
    double min_y;
    double max_y;

} extrema_t;


extern const int CIRCLE;
extern const int POLYGON;


// A struct which stores a CIRCLE or POLYGON shape and the corresponding 
// vertices. If the shape is of CIRCLE type, it also stores the radius.
typedef struct shape shape_t;


// Initializes a CIRCLE type shape struct.
shape_t* shape_init_circle(vector_t position, double radius);

// Initializes Rectangle
shape_t* shape_init_rectangle(vector_t v1, vector_t v2);

// Initializes a POLYGON type shape struct.
shape_t* shape_init_polygon(list_t* vertices);


// Frees the memory allocated for a shape object.
void shape_free(shape_t* shape);



// Accessors and other operations ---------------------------------------------


// Returns a list containing vectors normal to the edges of a shape. If the 
// shape is a circe, each normal vector projects radially outwards from the 
// center.
list_t* shape_normals(shape_t* shape);


// Returns an extrema_t object, which stores the minimum and maximum x and y 
// values of the shape (for use in collisions).
extrema_t shape_extrema(shape_t* shape);

// returns extrema stored in shape
extrema_t shape_get_extrema(shape_t* shape);



// Returns the area of the inputted shape.
double shape_area(shape_t* shape);


// Translates a shape by a given vector.
void shape_translate(shape_t* shape, vector_t translation);


// Rotates vertices in a shape by a given angle about a given point.
void shape_rotate(shape_t* shape, double angle, vector_t position);


// Gets the centroid of the shape.
vector_t shape_centroid(shape_t* shape);


// Gets the type of the shape.
int shape_type(shape_t* shape);


// Gets the vertices of the shape.
list_t* shape_vertices(shape_t* shape);


// Gets the radius of the shape, if the shape is a CIRCLE.
double shape_radius(shape_t* shape);

#endif
