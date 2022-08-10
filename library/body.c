#include "body.h"


const int FORCES_SIZE = 10;
const int IMPULSES_SIZE = 10;
const vector_t LIMIT = {150, 500};

// Functions associated with gameplay  -------------------------------------------------

// Neutralizes acceraltion and velocity on axis where contact between two objects has occured, simulating a
// normal force between objects.
vector_t normal_force(body_t* body, vector_t v)
{
    if (body_get_info(body) != NULL)
    {
        contact_t contact = sprite_contact((sprite_t*) body_get_info(body));
        if (contact.left || contact.right) {v.x = 0;}
        if (contact.below || (contact.above && v.y > 0)) {v.y = 0;}
    }
    return v;
}

// Limits the velocity of the body. We might want to limit this to player
// later.
vector_t limit_velocity(vector_t v)
{
    if (v.x > LIMIT.x) {v.x = LIMIT.x;}
    else if (v.x < -1 * LIMIT.x) {v.x = -1 * LIMIT.x;}
    // Remove y case in the future, because jumps don't involve accelerating.
    if (v.y > LIMIT.y) {v.y = LIMIT.y;}
    if (v.y < -1 * LIMIT.y) {v.y = -1 * LIMIT.y;}
    return v;
}

// ----------------------------------------------------------------------------------------


typedef struct body
{
    // Intrinsic properties of the object ------------------------------

    // Any additional information associated with the body
    void *info;
    free_func_t info_freer;
    // Rotation per unit time.
    double theta;
    double mass;
    rgb_color_t color;
    shape_t* shape;

    // Fields which change over the course of the simulation -----------

    // Marks the body for removal; initially false.
    bool remove;
    vector_t a;
    vector_t v;
    double dxn;
    vector_t forces;
    vector_t impulses;
} body_t;


// Allocates memory for a body with the given parameters. The body is initially at rest.
body_t *body_init(shape_t *shape, double mass, rgb_color_t color)
{
    assert(mass > 0);

    // Allocate memory for the new object.
    body_t *body = malloc(sizeof(body_t));
    assert(body != NULL);
    assert(shape != NULL);

    // Giving the body its initial parameters.
    body -> shape = shape;
    body -> mass = mass;
    body -> color = color;

    body -> remove = false;

    // The body is initially at rest.
    body -> v = VEC_ZERO;
    body -> a = VEC_ZERO;

    // The body's orientation only changes when body_rotate is called.
    body -> dxn = 0;

    body -> forces = VEC_ZERO;
    body -> impulses = VEC_ZERO;

    body -> info_freer = NULL;
    body -> info = NULL;

    return body;
}


// Allocates memory for a body with the given parameters. The body is initially at rest.
// The info parameter contains additional information to associate with the body, e.g. its type
// if the scene has multiple types of bodies. info_freer if non-NULL, a function call on the info to free it
body_t *body_init_with_info(shape_t *shape, double mass, rgb_color_t color, void *info, free_func_t info_freer)
{
    body_t *body = body_init(shape, mass, color);
    body -> info = info;
    body -> info_freer = info_freer;

    return body;
}


// Releases the memory allocated for a body.
void body_free(body_t *body)
{
    shape_free(body -> shape);
    if (body -> info != NULL && body -> info_freer != NULL)
    {
        (body -> info_freer)(body -> info);
    }

    free(body);
}


// Returns whether a body has been marked for removal.
bool body_is_removed(body_t *body)
{
    return body -> remove;
}


// Updates the body after a given time interval has elapsed.
void body_tick(body_t *body, double dt)
{
    // Animation stuff for the sprite...
    sprite_tick((sprite_t*) body -> info, dt);

    // Handling forces...
    vector_t net_force = body -> forces;
    body -> forces = (vector_t) {0, 0};
    vector_t da = vec_multiply(1.0 / body -> mass, net_force);
    // Adjustment to simulate normal force.
    body -> a = normal_force(body, da);

    // Handling impulses...
    vector_t net_impulse = body -> impulses;
    body -> impulses = (vector_t) {0, 0};
    vector_t impulse_dv = vec_multiply(1.0 / body -> mass, net_impulse);

    // Change in velocity due to acceleration.
    vector_t dv = vec_multiply(dt, body -> a);
    dv = vec_add(dv, impulse_dv);
    // Update the body's velocity. Includes an adjustment to simulate normal
    // force.
    vector_t v0 = normal_force(body, body_get_velocity(body));

    // Limit the velocity of the body, see gameplay.c.
    body -> v = limit_velocity(vec_add(v0, dv));
    vector_t avg_v = vec_multiply(0.5, vec_add(body -> v, v0));
    vector_t ds = vec_multiply(dt, avg_v);

    shape_t* shape = body -> shape;
    shape_translate(shape, ds);
    // Rotates a body by theta to account for angular velocity.
    shape_rotate(shape, body -> theta * dt, shape_centroid(shape));
}


// Marks a body for removal; future calls to body_is_removed() will return true. Does not free the body.
// If the body is already marked for removal, does nothing.
void body_remove(body_t *body)
{
    body -> remove = true;
}


// Set functions -----------------------------------------------


void body_set_shape(body_t* body, shape_t* shape)
{
    shape_t* old = body -> shape;
    body -> shape = shape;
    shape_free(old);
}

// Translates a body to a new position.
void body_set_centroid(body_t *body, vector_t x)
{
    vector_t s = shape_centroid(body -> shape);
    // The translation vector to move the polygon to the new position.
    vector_t shift = vec_subtract(x, s);
    shape_translate(body -> shape, shift);
}


// Changes a body's velocity.
void body_set_velocity(body_t *body, vector_t new_v)
{
    body -> v = new_v;
}


// Changes a body's color.
void body_set_color(body_t *body, rgb_color_t color)
{
    body -> color = color;
}


// Changes a body's absolute orientation in the plane. O degrees is due "east."
void body_set_rotation(body_t *body, double new_dxn)
{
    // Angle to rotate by to achieve the new direction.
    shape_rotate(body -> shape, new_dxn - body -> dxn, shape_centroid(body -> shape));
    body -> dxn = new_dxn;
}


// Sets the rotations speed of body
void body_set_theta(body_t *body, double theta)
{
    body -> theta = theta;
}


// Applies a force to a body over the current tick.
// If multiple forces are applied in the same tick, they should be added.
// Should not change the body's position or velocity; see body_tick().
void body_add_force(body_t *body, vector_t force)
{
    body -> forces = vec_add(body -> forces, force);
}


// Applies an impulse to a body.
// An impulse causes an instantaneous change in velocity, which is useful for modeling collisions.
// Should not change the body's position or velocity; see body_tick().
void body_add_impulse(body_t *body, vector_t impulse)
{
    body -> impulses = vec_add(body -> impulses, impulse);
}


// Accessor functions ---------------------------------------------------------

// Gets the current shape of a body. It returns a deep copy of the shape to
// avoid issues later on with sdl_render.
shape_t* body_get_shape(body_t *body)
{
    shape_t* copy = NULL;
    assert(body -> shape != NULL);
    shape_t *shape = body -> shape;

    if (shape_type(shape) == CIRCLE)
    {
        return shape_init_circle(shape_centroid(shape), shape_radius(shape));
    }
    else // if (shape_type(shape) == POLYGON)
    {
        size_t size = list_size(shape_vertices(shape));
        list_t* vertices = list_init(size, free);
        // Just vectors are added, so only the free function needs to be passed.
        list_t* old_vertices = shape_vertices(shape);
        for (size_t i = 0; i < size; i ++)
        {
            vector_t *vertex = malloc(sizeof(vector_t));
            *vertex = *(vector_t*) list_get(old_vertices, i);
            list_add(vertices, vertex);
        }
        copy = shape_init_polygon(vertices);
    }
    // Make sure a copy was actually created.
    assert(copy != NULL);
    return copy;
}


// Gets the current center of a body.
vector_t body_get_centroid(body_t *body)
{
    return shape_centroid(body -> shape);
}


// Gets the current velocity of a body.
vector_t body_get_velocity(body_t *body)
{
    return body -> v;
}


// Gets the display color of a body.
rgb_color_t body_get_color(body_t *body)
{
    return body -> color;
}


// Gets the absolute direction in which the body is facing.
double body_get_dxn(body_t *body)
{
    return body -> dxn;
}


// Gets the mass of the body.
double body_get_mass(body_t *body)
{
    return body -> mass;
}


// Returns the body's info field.
void *body_get_info(body_t *body)
{
    return body -> info;
}


// Gets the direction in which the body is facing.
double body_get_rotation(body_t *body)
{
    return body -> dxn;
}

// Returns the forces accumulated by a body.
vector_t body_get_force(body_t *body)
{
    return body -> forces;
}


// Translate shape of body
void body_translate(body_t *body, vector_t translate)
{
    shape_translate(body -> shape, translate);
}
