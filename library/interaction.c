#include "interaction.h"


// A 'interaction' which contains information relevant for a force_creator_t.
typedef struct interaction
{
    // A list of the bodies involved in the force interaction.
    list_t *bodies;
    // Additional information associated with the interaction (e.g. a force constant k, g, etc.).
    void *aux;
    free_func_t aux_freer;
    // The force involved in the interaction interaction.
    force_creator_t forcer;

    bool colliding;

} interaction_t;


// Initializes a interaction containing the relevant bodies and force constant.
interaction_t *interaction_init(list_t *bodies, void *aux, free_func_t aux_freer, force_creator_t forcer)
{
    interaction_t *interaction = malloc(sizeof(interaction_t));
    assert(interaction != NULL);

    interaction -> bodies = bodies;
    interaction -> colliding = false;
    interaction -> aux = aux;
    interaction -> aux_freer = aux_freer;

    interaction -> forcer = forcer;

    return interaction;
}


// Frees the memory associated with a interaction.
void interaction_free(interaction_t *interaction)
{
    // If aux is not null, free it.
    if (interaction -> aux_freer != NULL)
    {
        (interaction -> aux_freer)(interaction -> aux);
    }
    void** arr = list_arr(interaction -> bodies);
    free(arr);
    free(interaction -> bodies);
    free(interaction);
}


// Gets the body at a given index in a interaction.
body_t *interaction_get_body(interaction_t *interaction, size_t index)
{
    return (body_t*) list_get(interaction -> bodies, index);
}


// Gets the force constant from the interaction.
void *interaction_get_aux(interaction_t *interaction)
{
    // Throw an error if the additional information is not a force constant or
    // is null.
    assert(interaction -> aux != NULL);
    return interaction -> aux;
}


// Gets the forcer from an interaction if it is non-null.
force_creator_t interaction_get_forcer(interaction_t* interaction)
{
    return interaction -> forcer;
}


list_t* interaction_get_bodies(interaction_t* interaction)
{
    return interaction -> bodies;
}

bool interaction_is_colliding(interaction_t* interaction)
{
    return interaction -> colliding;
}

void interaction_set_colliding(interaction_t* interaction, bool value)
{
    interaction -> colliding = value;
}

void interaction_set_aux(interaction_t* interaction, void* aux)
{
    void* old_aux = interaction -> aux;
    interaction -> aux = aux;
    free(old_aux);
}

// Prevents function to run twice during a collision
bool interaction_colliding(interaction_t* interaction)
{   
    if(interaction -> colliding)
    {
        interaction -> colliding = false;
        return true;
    }
    return false;
}
