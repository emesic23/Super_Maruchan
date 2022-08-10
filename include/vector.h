#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>


// The vector struct, containing x and y coordinates.
typedef struct vector 
{
    double x;
    double y;
} vector_t;


// The zero vector.
extern const vector_t VEC_ZERO;


// Returns a vector which is the sum of v1 and v2.
vector_t vec_add(vector_t v1, vector_t v2);


// Returns a vector resulting from the subtraction of v2 from v1.
vector_t vec_subtract(vector_t v1, vector_t v2);


// Returns a unit vector corresponding to the inputted vector.
vector_t vec_normalize(vector_t v);


// Returns a vector which is the additive inverse (i.e. negative) of v.
vector_t vec_negate(vector_t v);


// Returns a vector which is equal to v multiplied by the scalar input.
vector_t vec_multiply(double scalar, vector_t v);


// Returns a the dot product of v1 and v2.
double vec_dot(vector_t v1, vector_t v2);


// Returns the z-component cross product of v1 and v2;
double vec_cross(vector_t v1, vector_t v2);


// Returns a vector equivalent to the vector v rotated by the inputted angle.
// The inputted angle is in radians. The vector is rotated about the inputted 
// point.
vector_t vec_rotate(vector_t v, double angle, vector_t position);


// Returns perpendicular of a vector (x, y) -> (y, -x)
vector_t vec_perpendicular(vector_t v);


// Returns whether or not two vectors are equal.
bool vec_equal(vector_t v1, vector_t v2);

// Returns magnitude of a vector
double vec_magnitude(vector_t v);

// Adjusts super small -0 value in vector
vector_t vec_adjust(vector_t v);

// print vector
void vec_print(vector_t v);


#endif
