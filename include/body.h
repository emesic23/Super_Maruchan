#ifndef __BODY_H__
#define __BODY_H__

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "color.h"
#include "list.h"
#include "shape.h"
#include "vector.h"
#include "sprite.h"





// A rigid body constrained to the plane.
// Bodies can accumulate forces and impulses during each tick.
typedef struct body body_t;


// Initializes a body without any info. Acts like body_init_with_info() where info and info_freer are NULL.
body_t *body_init(shape_t *shape, double mass, rgb_color_t color);


// Allocates memory for a body with the given parameters. The body is initially at rest.
// The info parameter contains additional information to associate with the body, e.g.
// you could add that it is an enemy or ally, things like that
// if the scene has multiple types of bodies. info_freer if non-NULL, a function call on the info to free it
body_t *body_init_with_info(shape_t *shape, double mass, rgb_color_t color, void *info, free_func_t info_freer);


// Sets a new shape, freeing the old shape.
void body_set_shape(body_t* body, shape_t* shape);


// Releases the memory allocated for a body.
void body_free(body_t *body);


// Gets the current shape of a body.
shape_t *body_get_shape(body_t *body);


// Gets the current center of mass of a body.
vector_t body_get_centroid(body_t *body);


// Gets the current velocity of a body.
vector_t body_get_velocity(body_t *body);


// Gets the mass of a body.
double body_get_mass(body_t *body);


// Gets the display color of a body.
rgb_color_t body_get_color(body_t *body);


// Gets the information associated with a body.
void *body_get_info(body_t *body);


// Gets the direciton body is facing
double body_get_rotation(body_t *body);


vector_t body_get_force(body_t *body);


// Translates a body to a new position.
void body_set_centroid(body_t *body, vector_t x);


// Changes a body's velocity (the time-derivative of its position).
void body_set_velocity(body_t *body, vector_t v);


// Changes a body's orientation in the plane.
// The body is rotated about its center of mass. Note that the angle is *absolute*, not relative to the current orientation.
void body_set_rotation(body_t *body, double angle);


// Sets radius of body.
void body_set_radius(body_t *body, double new_r);


// Sets the rotations speed of body.
void body_set_theta(body_t *body, double theta);


// Sets the color of a body.
void body_set_color(body_t *body, rgb_color_t color);


// Applies a force to a body over the current tick.
void body_add_force(body_t *body, vector_t force);


// Applies an impulse to a body.
// An impulse causes an instantaneous change in velocity, which is useful for modeling collisions.
void body_add_impulse(body_t *body, vector_t impulse);


// Updates the body after a given time interval has elapsed.
// The body should be translated at the *average *of the velocities before and after the tick.
// Resets the forces and impulses accumulated on the body.
void body_tick(body_t *body, double dt);


// Marks a body for removal--future calls to body_is_removed() will return true. Does not free the body.
// If the body is already marked for removal, does nothing.
void body_remove(body_t *body);


// Returns whether a body has been marked for removal.
bool body_is_removed(body_t *body);


// Translates the shape of the body.
void body_translate(body_t *body, vector_t translate);

// limits max velocity of player so it deosn't go too fast
void body_velocity_limit(body_t *(body));

// Functions associated with gameplay -------------------------


// Limits the velocity of the body.
vector_t limit_velocity(vector_t v);

// return vector_t
vector_t normal_force(body_t *body, vector_t v);
// -------------------------------------------------------------

#endif // #ifndef __BODY_H__
