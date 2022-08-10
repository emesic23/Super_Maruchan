#include "scene.h"

const int INITIAL_SIZE = 10;
// We can use NUM_ROLES instead of having accessor functions, because we will 
// know the size of the bodies list beforehand. NUM_ROLES is in sprite.c. 

// A collection of scene_list.
typedef struct scene
{
    // A list of force_creator_t functions, which correspond to a interaction in the
    // interactions list according to index.
    list_t *interactions;
    list_t *scene_list;

    // The max and min values of the scene.
    vector_t min;
    vector_t max;
} scene_t;


// Allocates memory for an empty scene.
scene_t *scene_init(void)
{
    scene_t *scene = malloc(sizeof(scene_t));
    assert(scene != NULL);

    scene -> scene_list = list_init(INITIAL_SIZE, (free_func_t) list_free);
    for(size_t i = 0; i < NUM_ROLES; i++)
    {
        list_add(scene -> scene_list, list_init(INITIAL_SIZE, (free_func_t) body_free));
    }


    // The force creators are just pointers to functions; no special free_func
    // needed.
    scene -> interactions = list_init(INITIAL_SIZE, (free_func_t) interaction_free);

    return scene;
}


// Releases memory allocated for a given scene and all its scene_list.
void scene_free(scene_t *scene)
{
    list_free(scene -> interactions);
    list_free(scene -> scene_list);

    free(scene);
}

// CAN WE SWITCH THIS SO THAT THE ROLE COMES BEFORE THE INDEX?
// Gets the body at a given index in a scene.
body_t *scene_get_body(scene_t *scene, role_t role, size_t index)
{
    return (body_t*) list_get(list_get(scene -> scene_list, role), index);
}


// Returns the list of scene_list of a certain type in the scene. 
list_t *scene_get_list(scene_t *scene, size_t index)
{
    return (list_t*) list_get(scene -> scene_list, index);
}


// Returns number of scene_list in scene.
size_t scene_size(scene_t *scene)
{
    size_t body_count = 0;
    for (size_t role = 0; role < NUM_ROLES; role ++)
    {
        body_count += list_size((list_t*) list_get(scene -> scene_list, role));
    }
    return body_count;
}

// Adds a body to a scene. The role acts as the index for that types list index
void scene_add_body(scene_t *scene, body_t *body, size_t index)
{
    if(list_size(scene -> scene_list) <= index)
    {
        list_add(scene -> scene_list, list_init(INITIAL_SIZE, (free_func_t) body_free));
        list_add(list_get(scene -> scene_list, index), body);
    } 
    else
    {
        list_add(list_get(scene -> scene_list, index), body); 
    }
}


// Removes and frees the body at a given index from a scene.
void scene_remove_body(scene_t *scene, size_t index, size_t role)
{
    // Note that error handling is done in the list_remove function.
    body_t *old = list_remove(list_get(scene -> scene_list, role), index);
    scene_remove_interactions(scene, old);
    body_free(old);
}


// Adds a force creator to a scene,
// The auxiliary value is passed to the force creator each time it is called.
// The force creator is registered with a list of bodies it applies to,
// so it can be removed when any one of the bodies  is removed.
void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer, void *aux, list_t *bodies, free_func_t aux_freer)
{
    // Create a interaction which corresponds to a force interaction between the inputted
    // bodies.
    interaction_t *interaction = interaction_init(bodies, aux, aux_freer, forcer);
    // Add the interaction to the scene.
    list_add(scene -> interactions, interaction);
}


// Executes a tick of a given scene over a small time interval.
// This requires executing all the force creators and then ticking each body (see body_tick()).
void scene_tick(scene_t *scene, double dt)
{
    // Adds all the forces to the relevant scene_list.
    size_t size_interactions = list_size(scene -> interactions);
    for (size_t i = 0; i < size_interactions; i ++)
    {
        interaction_t *interaction = list_get(scene -> interactions, i);
        force_creator_t forcer = interaction_get_forcer(interaction);
        assert(forcer != NULL);
        // Apply the force to the interaction.
        forcer(interaction);
    }

    // Tick each body.
    // Use a while loop to avoid issues with iterating over a list whose length
    // is changing.
    for (size_t role = 0; role < NUM_ROLES; role ++)
    {
        size_t i = 0;
        while (i < list_size(list_get(scene -> scene_list, role)))
        {
            body_t *body = (body_t*) list_get((list_t*) list_get(scene -> scene_list, role), i);
            // Check to see if the body is marked for removal; if so, free the body
            // and associated interactions.
            if (body_is_removed(body))
            {
                // Remove interactions associated with a body.
                scene_remove_interactions(scene, body);
                body_free(list_remove(list_get(scene -> scene_list, role), i));

            }
            else
            {
                body_tick(body, dt);
                i ++;
            }
        }
    }
}


// Remove all interactions (i.e. force interactions) associated with the inputted
// body from the scene.
void scene_remove_interactions(scene_t *scene, body_t *body)
{
    // Use a while loop to avoid issues with iterating over a list whose size
    // is changing.
    int i = 0;
    while (i < list_size(scene -> interactions))
    {
        interaction_t *interaction = list_get(scene -> interactions, i);
        // Check each body in the interaction for a match.
        size_t size = list_size(interaction_get_bodies(interaction));
        for (size_t j = 0; j < size; j ++)
        {
            body_t *another_body = interaction_get_body(interaction, j);
            if (body == another_body)
            {
                // Make sure to free the interaction AND remove it from the interactions
                // list.
                interaction_free(list_remove(scene -> interactions, i));
                // Move on to the next interaction.
                i--;
                break;
            }
        }
        i ++;
    }
}

