#include "forces.h"
#include "interaction.h"

const double RADIUS_BARRIER = 30;


// Collision handling  -------------------------------------------------------------

// Adds a force creator to a scene that calls a given collision handler function each time two bodies collide.
// This generalizes create_destructive_collision() from last week, allowing different things to happen on a collision.
// It should only be called once while the bodies are still colliding.
void create_collision(scene_t *scene, body_t *body1, body_t *body2, force_creator_t handler, void *aux, free_func_t freer)
{
    list_t* bodies = list_init(2, (free_func_t) body_free);
    list_add(bodies, body1);
    list_add(bodies, body2);

    scene_add_bodies_force_creator(scene, handler, aux, bodies, freer);
}


// Adds a collision_handler_t to a scene which applies an elastic collision interaction between two bodies.
void create_physics_collision(scene_t* scene, body_t* body1, body_t* body2, double e)
{
    double *aux = malloc(sizeof(double));
    assert(aux != NULL);
    *aux = e;

    create_collision(scene, body1, body2, physics_collision, aux, free);
}


// Adds a collision_handler_t to a scene which applies a destructive collision interaction between two bodies.
void create_destructive_collision(scene_t* scene, body_t* body1, body_t* body2)
{
    // aux and free parameters are both NULL.
    create_collision(scene, body1, body2, destructive_collision, NULL, NULL);
}


// force_creator_t functions for implementing collisions --------------------------

// Implements a 'destructive collision' interaction between two bodies.
void destructive_collision(interaction_t* interaction)
{
    // Need to do this because we have to free the body afterwards.
    body_t *body1 = interaction_get_body(interaction, 0);
    body_t *body2 = interaction_get_body(interaction, 1);

    // Check if the two bodies have collided.
    if (find_collision(body1, body2).collided)
    {
        // If there is a collision, mark the two bodies for removal.
        body_remove(body1);
        body_remove(body2);
    }
}


// Implements a destructive collision interaction upon contact. The stuff to
// make sure multiple impulses aren't added is dealt with in body.c to avoid
// having to write new accessor functions.
void physics_collision(interaction_t* interaction)
{
    body_t* body1 = interaction_get_body(interaction, 0);
    body_t* body2 = interaction_get_body(interaction, 1);

    // Get the elasticicity.
    double k = *(double*) interaction_get_aux(interaction);

    collision_info_t collision = find_collision(body1, body2);

    // Sets colliding to false next time interaction is called
    if (interaction_is_colliding(interaction))
    {
        // checking if the two objects are still colliding
        if(!collision.collided)
        {
        interaction_set_colliding(interaction, false);
        }
        return;
    }
    else
    {
    // If there is no collision, return.
    if (!collision.collided)
    {
        return;
    }

    double m_reduced;
    double m1 = body_get_mass(body1);
    double m2 = body_get_mass(body2);

    // Handle case if either body has a mass of infinity.
    if (isinf(m1)) {m_reduced = m2;}
    else if (isinf(m2)) {m_reduced = m1;}
    else {m_reduced = (m1 * m2) / (m1 + m2);}

    // Find the component of each velocity vector along the collision axis.
    double v1 = vec_dot(body_get_velocity(body1), collision.axis);
    double v2 = vec_dot(body_get_velocity(body2), collision.axis);

    // Impulse on body2 by body1 (negate for the impulse on body1).
    vector_t impulse = vec_multiply(m_reduced * (1 + k) * (v1 - v2), collision.axis);


    // Add impulses to both bodies.
    body_add_impulse(body2, impulse);
    body_add_impulse(body1, vec_negate(impulse));

    interaction_set_colliding(interaction, true);
    }
}


// Force handling ------------------------------------------------------------

// Adds a force creator to a scene that applies gravity between two bodies.
// The force creator will be called each tick to compute the Newtonian gravitational force between the bodies.
void create_newtonian_gravity(scene_t *scene, double g, body_t *body1, body_t *body2)
{
    list_t *bodies = list_init(2, (free_func_t) body_free);
    list_add(bodies, body1);
    list_add(bodies, body2);

    double *aux = malloc(sizeof(double));
    *aux = g;

    scene_add_bodies_force_creator(scene, (force_creator_t) newtonian_gravity, aux, bodies, (free_func_t) free);
}


// Adds a force creator to a scene that acts like a spring between two bodies.
// The force creator will be called each tick to compute the Hooke's-Law spring force between the bodies.
void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2)
{
    list_t *bodies = list_init(2, (free_func_t) body_free);
    list_add(bodies, body1);
    list_add(bodies, body2);

    // aux now stores the constant relevant to the interaction.
    double *aux = malloc(sizeof(double));
    *aux = k;

    scene_add_bodies_force_creator(scene, (force_creator_t) spring, aux, bodies, (free_func_t) free);
}


// Adds a force creator to a scene that applies a drag force on a body.
// The force creator will be called each tick to compute the drag force on the body proportional to its velocity.
// The force points opposite the body's velocity.
void create_drag(scene_t *scene, double gamma, body_t *body)
{
    list_t *bodies = list_init(2, (free_func_t) body_free);
    list_add(bodies, body);

    double *aux = malloc(sizeof(double));
    *aux = gamma;

    scene_add_bodies_force_creator(scene, (force_creator_t) drag, aux, bodies, (free_func_t) free);
}


// force_creator_t functions which implement each force  ---------------------------------

// Adds gravitational forces to the bodies in 'interaction'.
void newtonian_gravity(interaction_t *interaction)
{
    body_t *body1 = interaction_get_body(interaction, 0);
    body_t *body2 = interaction_get_body(interaction, 1);

    if (body_get_mass(body1) == INFINITY)
    {
        body_add_force(body2, (vector_t) {0, -1e4});
        return;
    }

    if (body_get_mass(body2) == INFINITY)
    {
        body_add_force(body1, (vector_t) {0, -1e4});
        return;
    }
    // The gravitational constant.
    double G = *(double*) (interaction_get_aux(interaction));

    // The distance between the two bodies.
    vector_t ds = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
    // The distance between the two bodies squared.
    double r = pow(ds.x, 2) + pow(ds.y, 2);

    // The magnitude of the force of gravity acting between the two objects.
    // Fg = G  *(m1  *m2) / r^2
    double force;
    // if (r > RADIUS_BARRIER) {
        force = G  *(body_get_mass(body1)  *body_get_mass(body2)) / r;
    // }
    // else {
    //     force = 0;
    // }

    // Create a unit vector corresponding to the direction in which the force
    // is acting on each body, and multiply by the force magnitude.
    // The force by body2 on body1.
    vector_t f1 = vec_multiply(force, vec_normalize(ds));
    // The force by body1 on body2.
    vector_t f2 = vec_negate(f1);

    // body_add_force(body1, f1);
    body_add_force(body2, f2);

}


// Adds spring forces to the bodies in 'interaction'.
void spring(interaction_t *interaction)
{
    body_t *body1 = interaction_get_body(interaction, 0);
    body_t *body2 = interaction_get_body(interaction, 1);

    // The spring constant.
    double k = *(double*)(interaction_get_aux(interaction));

    // The distance between the two bodies.
    vector_t ds = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
    // The distance between the two bodies squared.
    double r = pow(ds.x, 2) + pow(ds.y, 2);

    // The magnitude of the spring acting between the two objects.
    // Fs = k  *x
    double force = k  *pow(r, 0.5);

    // Create a unit vector corresponding to the direction in which the force
    // is acting on each body, and multiply by the force magnitude.
    // The force by body2 on body1.
    vector_t f1 = vec_multiply(force, vec_normalize(ds));
    // The force by body1 on body2.
    vector_t f2 = vec_negate(f1);

    body_add_force(body1, f1);
    body_add_force(body2, f2);
}


// Adds a drag force to the body in interaction.
void drag(interaction_t *interaction)
{
    body_t *body = interaction_get_body(interaction, 0);

    // The constant, which I'm assuming factors in all of the
    // coefficients/constants used in the drag equation.
    double gamma = *(double*)(interaction_get_aux(interaction));

    vector_t v = body_get_velocity(body);

    // The speed of the body, squared.
    double speed = pow(v.x, 2) + pow(v.y, 2);

    // The magnitude of the drag force acting on the body.
    // Fd = gamma  * sqrt(speed)
    double force = gamma  * sqrt(speed);

    // The force acts against the direction of motion.
    body_add_force(body, vec_multiply(-1  * force, vec_normalize(v)));
}
