#include "stdio.h"
#include "list.h"
#include "body.h"

#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>


// A 'interaction' which contains information relevant for a force_creator_t.
typedef struct interaction interaction_t;

// A function which adds some forces or impulses to bodies, e.g. from collisions, gravity, or spring forces.
typedef void (*force_creator_t)(interaction_t *interaction);

// Initializes a interaction containing the relevant bodies and force constant.
interaction_t *interaction_init(list_t *bodies, void *aux, free_func_t aux_freer, force_creator_t forcer);


// Frees the memory associated with a interaction.
void interaction_free(interaction_t *interaction);


// Gets the body at a given index in a interaction.
body_t *interaction_get_body(interaction_t *interaction, size_t index);


// Gets the force constant from the interaction.
void *interaction_get_aux(interaction_t *interaction);


force_creator_t interaction_get_forcer(interaction_t* interaction);

list_t* interaction_get_bodies(interaction_t* interaction);

bool interaction_is_colliding(interaction_t* interaction);

void interaction_set_colliding(interaction_t* interaction, bool value);

void interaction_set_aux(interaction_t* interaction, void* aux);

// Returns if two ticks collided during last tick
bool interaction_colliding(interaction_t* interaction);
