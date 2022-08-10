#include "vector.h"


// The zero vector.
const vector_t VEC_ZERO = (vector_t) {0, 0};


// Returns a vector which is the sum of v1 and v2.
vector_t vec_add(vector_t v1, vector_t v2)
{
    double new_x = v1.x + v2.x;
    double new_y = v1.y + v2.y;
    vector_t v = {new_x, new_y};

    return v;
}


// Returns a vector resulting from the subtraction of v2 from v1.
vector_t vec_subtract(vector_t v1, vector_t v2)
{
    vector_t v = vec_add(v1, vec_negate(v2));
    return v;
}


// Returns a unit vector corresponding to the inputted vector.
vector_t vec_normalize(vector_t v)
{
    double magnitude = sqrt(pow(v.x, 2) + pow(v.y, 2));
    if(magnitude == 0)
    {
        return VEC_ZERO;
    }
    return vec_multiply(1 / magnitude, v);
}


// Returns a vector which is the additive inverse (i.e. negative) of v.
vector_t vec_negate(vector_t v)
{
    return vec_multiply(-1.0, v);
}


// Returns a vector which is equal to v multiplied by the scalar input.
vector_t vec_multiply(double scalar, vector_t v)
{
    double new_x = scalar * v.x;
    double new_y = scalar * v.y;

    return (vector_t) {new_x, new_y};
}


// Returns a the dot product of v1 and v2.
double vec_dot(vector_t v1, vector_t v2)
{
    double dot_product = v1.x * v2.x + v1.y * v2.y;

    return dot_product;
}


// Returns the z-component cross product of v1 and v2;
double vec_cross(vector_t v1, vector_t v2)
{
    double cross_product = v1.x * v2.y - v1.y * v2.x;

    return cross_product;
}


// Returns a vector equivalent to the vector v rotated by the inputted angle.
// The inputted angle is in radians. The vector is rotated about the inputted 
// point.
vector_t vec_rotate(vector_t v, double angle, vector_t position)
{
    // Adjust the vector so its center is at the origin.
    vector_t v_adj = vec_subtract(v, position);
    vector_t rotated = {v_adj.x * cos(angle) - v_adj.y * sin(angle), v_adj.x * sin(angle) + v_adj.y * cos(angle)};
    // Translate the vector back to its original position.
    rotated = vec_add(rotated, position);
    
    return rotated;
}


// Returns perpendicular of a vector (x, y) -> (y, -x)
vector_t vec_perpendicular(vector_t v)
{
    return (vector_t) {v.y, -1 * v.x};
}


// Returns whether or not two vectors are equal.
bool vec_equal(vector_t v1, vector_t v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

// Returns magnitude of a vector
double vec_magnitude(vector_t v)
{
    return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

// Adjusts super small -0 value in vector
vector_t vec_adjust(vector_t v)
{   
    if (-1e-8 < v.y && v.y < 1e-8)
    {
        v.y = 0;
    }
    if (-1e-7 < v.x && v.x < 1e-8)
    {
        v.x = 0;
    }
    return (vector_t) {v.x, v.y};
}

// print vector
void vec_print(vector_t v)
{
    printf("%.50f, %.50f \n", v.x , v.y);
}