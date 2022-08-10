#ifndef __COLLISION_H__
#define __COLLISION_H__

#include "list.h"
#include "vector.h"
#include "body.h"
#include "shape.h"

#include <math.h>
#include <stdlib.h>

/* OVERVIEW:
*
* Collision works by having two phases: broad and narrow
*
* Broad phase:
*     First the broad phase is excuted. A less computational intensive
*     function is run to determine if there exists a possibility of collision.
*         - This is done by comparing the objext extrema along x and y axis
*         - According to SAT theorem if they don't overlap on both of those axis
*           then they are defninitly not colliding
*
* Narrow phase:
*     Assuming that the broad phase is passed. Now a more accurate function is run.
*     First, the normals for all the edges (b/w two points) of boths shapes are computed.
*     Next, both of the shapes are projected onto all of the normals.
*     If any of the projection of the shapes do not overlap on each normal (or axis)
*     Then according to SAT theorem, they do not collide.
*     In addition, the overlap between two objects is computed.
*
*
* If you have more questions: ask Julen or look up SAT theorem and collisions.
*/

// Represents the status of a collision between two shapes.
// The shapes are either not colliding, or they are colliding along some axis.
typedef struct collision_info
{
    bool collided;
    // If the shapes are colliding, the axis they are colliding on.
    // This is a unit vector pointing from the first shape towards the second.
    // Normal impulses are applied along this axis.
    // If collided is false, this value is undefined.
    vector_t axis;
    double overlap;
    vector_t overlap_v;
    
} collision_info_t;


// If objects are close then return true.
bool collision_possible(shape_t *shape1, shape_t *shape2, collision_info_t info);


// Projects vertex in the shape onto the inputted axis, and returns projected vector of greatest
// magnitude.
vector_t shape_project(shape_t *shape, vector_t axis);


// Calculates the overlap. If there is no overlap, returns -1.
double get_overlap(vector_t v1, vector_t v2);


// Returns the axis along which the objects have collided; if the axis is {0,
// 0}, no collision occurred.
collision_info_t find_collision(body_t *body1, body_t *body2);




#endif // #ifndef __COLLISION_H__
