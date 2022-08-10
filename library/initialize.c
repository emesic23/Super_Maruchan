#include "initialize.h"
#include "sprite.h"

const double PLAYER_MASS = 10;
const vector_t PLAYER_SIZE = {25, 28};
const size_t PLAYER_HEALTH = 3;

const double ENEMY_MASS = 10;
const double KOOPA_SIZE = 15;
const double ENEMY_SIZE = 10;
const vector_t ENEMY_SPEED = {20, 0};

const double FIREBALL_MASS = 10;
const double FIREBALL_SIZE = 5;
const vector_t FIREBALL_RIGHT = {40, 0};
const vector_t FIREBALL_LEFT = {-40, 0};

const double TOKEN_MASS = 10;
const double TOKEN_SIZE = 10;

const double PLATFORM_MASS = INFINITY;

// Just in case we want powerups to bounce or something later.
const double POWERUP_MASS = 10;
const vector_t POWERUP_SIZE = {10, 15};
const vector_t POWERUP_SPEED = {20, 0};

// The uniform gravitional field that is applied
const double G = -1050;
// Applying a drag
const double DRAG = 0.8;

const vector_t CENTER = {250, 250};

// Colors ------------------------------------------------
// We can get rid of this later if we want.
const rgb_color_t BLUE = (rgb_color_t){0.52, 0.81, 0.98};
const rgb_color_t ORANGE = (rgb_color_t){1, 0.52, 0};
// -------------------------------------------------------

// All shapes will be initialized in the center of the scene.
void add_player(scene_t *scene, vector_t position, subrole_t subrole)
{
    // Initialize a rectangle to back the player sprite.
    shape_t *shape = shape_init_rectangle(VEC_ZERO, PLAYER_SIZE);
    sprite_t *sprite = sprite_init(PLAYER, subrole, PLAYER_HEALTH, true);
    body_t *body = body_init_with_info(shape, PLAYER_MASS, BLUE, sprite, (free_func_t)sprite_free);

    body_set_centroid(body, position);
    // Add the body to the PLAYER sublist.
    scene_add_body(scene, body, PLAYER);
}

// Used during firemode.....releases a fireball that kills enemies
void add_fireball(scene_t *scene, vector_t position, vector_t velocity)
{
    // Initialize a rectangle to back the player sprite.
    shape_t *shape = shape_init_circle(VEC_ZERO, FIREBALL_SIZE);
    sprite_t *sprite = sprite_init(POWERUP, FIREBALL, 1, true);
    body_t *body = body_init_with_info(shape, FIREBALL_MASS, BLUE, sprite, (free_func_t)sprite_free);

    body_set_centroid(body, position);
    // Sets direction the firball is launched
    body_set_velocity(body, velocity);
    // Since firball is added mid game all its interactions need to be set when its added
    initialize_fireball(scene, body);
    // Add the body to the ENEMY sublist.
    scene_add_body(scene, body, ENEMY);
}

void add_enemy(scene_t *scene, vector_t position, subrole_t subrole)
{
    // Initialize a circle to back the player sprite.
    shape_t *shape;
    if (subrole == KOOPA) {shape = shape_init_circle(CENTER, KOOPA_SIZE);}
    else {shape = shape_init_circle(CENTER, ENEMY_SIZE);}
    sprite_t *sprite = sprite_init(ENEMY, subrole, 1, true);
    body_t *body = body_init_with_info(shape, ENEMY_MASS, BLUE, sprite, (free_func_t)sprite_free);

    body_set_centroid(body, position);
    // Add the body to the ENEMY sublist.
    scene_add_body(scene, body, ENEMY);
}
// Will subrole is pointless, this helps keep it consistent and less confusing
// if only one add_.... doesn't have subrole
void add_token(scene_t *scene, vector_t position)
{
    shape_t *shape = shape_init_circle(CENTER, TOKEN_SIZE);
    sprite_t *sprite = sprite_init(TOKEN, NO_SUBROLE, 1, true);
    body_t *body = body_init_with_info(shape, TOKEN_MASS, BLUE, sprite, (free_func_t)sprite_free);
    body_set_centroid(body, position);
    // Add the body to the TOKEN role.
    scene_add_body(scene, body, TOKEN);
}

void add_powerup(scene_t *scene, vector_t position, subrole_t subrole)
{
    // Initialize a circle to back the player sprite.
    shape_t *shape = shape_init_rectangle(VEC_ZERO, POWERUP_SIZE);
    sprite_t *sprite = sprite_init(POWERUP, subrole, 0, false);
    body_t *body = body_init_with_info(shape, POWERUP_MASS, BLUE, sprite, (free_func_t)sprite_free);
    body_set_centroid(body, position);

    // Add the body to the PLAYER sublist.
    scene_add_body(scene, body, POWERUP);
}

void add_platform(scene_t *scene, vector_t position, vector_t size, subrole_t subrole, void *info)
{
    // Initialize a circle to back the player sprite.
    shape_t *shape = shape_init_rectangle((vector_t){0, 0}, size);
    sprite_t *sprite;
    if (info != NULL)
    {
        sprite = sprite_init_with_info(PLATFORM, subrole, 0, false, info, free);
    }
    else
    {
        sprite = sprite_init(PLATFORM, subrole, 0, false);
    }

    body_t *body = body_init_with_info(shape, PLATFORM_MASS, ORANGE, sprite, (free_func_t)sprite_free);
    body_set_centroid(body, position);
    // Add the body to the PLAYER sublist.
    scene_add_body(scene, body, PLATFORM);

    if (subrole == ITEM_BLOCK || subrole == TUNNEL_BLOCK)
    {
        shape_t *shape = shape_init_rectangle((vector_t){0, 0}, size);
        sprite_t *sprite = sprite_init(PLATFORM, REGULAR_BLOCK, 0, false);
        body_t *body = body_init_with_info(shape, PLATFORM_MASS, ORANGE, sprite, (free_func_t)sprite_free);
        body_set_centroid(body, position);
        // Add the body to the PLAYER sublist.
        scene_add_body(scene, body, PLATFORM);
    }
}

void add_platform_corners(scene_t *scene, vector_t p1, vector_t p2, subrole_t subrole, void *info)
{
    // Initialize a circle to back the player sprite.
    shape_t *shape = shape_init_rectangle(p1, p2);
    sprite_t *sprite;
    if (info != NULL)
    {
        sprite = sprite_init_with_info(PLATFORM, subrole, 0, false, info, free);
    }
    else
    {
        sprite = sprite_init(PLATFORM, subrole, 0, false);
    }

    body_t *body = body_init_with_info(shape, PLATFORM_MASS, ORANGE, sprite, (free_func_t)sprite_free);
    // Add the body to the PLAYER sublist.
    scene_add_body(scene, body, PLATFORM);

    if (subrole == ITEM_BLOCK || subrole == TUNNEL_BLOCK)
    {
        shape_t *shape = shape_init_rectangle((vector_t) {p1.x + 1, p1.y + 1}, (vector_t) {p2.x - 1, p2.y - 1});
        sprite_t *sprite = sprite_init(PLATFORM, REGULAR_BLOCK, 0, false);
        body_t *body = body_init_with_info(shape, PLATFORM_MASS, ORANGE, sprite, (free_func_t)sprite_free);
        // Add the body to the PLAYER sublist.
        scene_add_body(scene, body, PLATFORM);
    }
}

void add_background(scene_t *scene, vector_t size, subrole_t subrole)
{
    // Initialize a circle to back the player sprite.
    shape_t *shape = shape_init_rectangle((vector_t){0, 0}, size);
    sprite_t *sprite = sprite_init(BACKGROUND, subrole, 0, false);

    body_t *body = body_init_with_info(shape, PLATFORM_MASS, BLUE, sprite, (free_func_t)sprite_free);
    scene_add_body(scene, body, BACKGROUND);
}

// Since firball is added mid game all its interactions need to be set when its added
void initialize_fireball(scene_t *scene, body_t *fireball)
{
    
    // Init fireball gameplay
    list_t *enemy_list = scene_get_list(scene, ENEMY);
    size_t size_enemy = list_size(enemy_list);
    for (size_t q = 0; q < size_enemy; q++) // creates player/enemy actions
    {
        body_t *enemy = list_get(enemy_list, q);
        double *bounces = malloc(sizeof(double));
        *bounces = 0;
        gameplay_create(scene, enemy, fireball, gameplay_fireball, bounces, free);
    }
    platform_roles_init_actions(scene, fireball);
}

// Initializes all necessary actions for everything in scene for the entire game
void add_interactions(scene_t *scene)
{
    list_t *player_list = scene_get_list(scene, PLAYER); // starts by init actions for players
    for (size_t z = 0; z < list_size(player_list); z++)  // loops through all the players
    {
        body_t *player = list_get(player_list, z);
        gameplay_create(scene, player, NULL, gameplay_player, NULL, NULL);
        double *time_invincibe = malloc(sizeof(double));
        *time_invincibe = 0;
        gameplay_create(scene, player, NULL, gameplay_star_powerup, time_invincibe, free);
        // Init enemy gameplay
        list_t *enemy_list = scene_get_list(scene, ENEMY);
        size_t size_enemy = list_size(enemy_list);
        for (size_t q = 0; q < size_enemy; q++) // creates player/enemy actions
        {
            body_t *enemy = list_get(enemy_list, q);
            double *time_squashed = malloc(sizeof(double));
            *time_squashed = 0;
            if (sprite_subrole((sprite_t *)body_get_info(enemy)) == GOOMBA)
            {
                gameplay_create(scene, player, enemy, (force_creator_t)gameplay_goomba, time_squashed, free);
            }
            if (sprite_subrole((sprite_t *)body_get_info(enemy)) == KOOPA)
            {
                gameplay_create(scene, player, enemy, (force_creator_t)gameplay_koopa, time_squashed, free);
            }

            platform_roles_init_actions(scene, enemy); //Inits platform/enemy actions
            // Sets gameplay mechanics for enemy only once
            if (z == 0 && sprite_subrole((sprite_t*) body_get_info(enemy)) != KOOPA)
            {
                create_bot_mechanics(scene, enemy, ENEMY_SPEED);
            } else {body_set_velocity(enemy, ENEMY_SPEED);}
            if (z == 0)
            {
                enemy_enemy_init_actions(scene, enemy_list, enemy, size_enemy, q);
            }
        }
        // Init tokens gameplay
        list_t *token_list = scene_get_list(scene, TOKEN);
        size_t size_token = list_size(token_list);
        for (size_t q = 0; q < size_token; q++) // creates player/token actions
        {
            gameplay_create(scene, player, list_get(token_list, q), (force_creator_t)gameplay_token, NULL, NULL);
        }
        // Init powerup gameplay ....
        list_t *powerup_list = scene_get_list(scene, POWERUP);
        size_t size_powerup = list_size(powerup_list);
        for (size_t q = 0; q < size_powerup; q++) // creates player/powerup actions
        {
            body_t *powerup = list_get(powerup_list, q);
            // gameplay_create(scene, player, powerup, (force_creator_t) , NULL, NULL);
            platform_roles_init_actions(scene, powerup); //Inits platform/powerup actions
            if (z == 0)
            {
                create_bot_mechanics(scene, powerup, POWERUP_SPEED);
            }
        }

        // create_drag(scene, DRAG, player);
        platform_roles_init_actions(scene, player);
    }
}

// Initializes actions between the platform and a role
void platform_roles_init_actions(scene_t *scene, body_t *body)
{
    if (scene_size(scene) > PLATFORM)
    {
        list_t *platform_list = scene_get_list(scene, PLATFORM);
        size_t size_platform = list_size(platform_list);
        // Sets gravity to uniform -1e4.
        double *aux = malloc(sizeof(double));
        *aux = G;
        gameplay_create(scene, body, NULL, gameplay_gravity, aux, free);
        for (size_t q = 0; q < size_platform; q++) // creates player/platform
        {
            body_t *platform = list_get(platform_list, q);
            if (sprite_subrole((sprite_t *)body_get_info(platform)) == REGULAR_BLOCK)
            {
                gameplay_create(scene, body, platform, gameplay_regular_block, NULL, NULL);
            }
            if (sprite_subrole((sprite_t *)body_get_info(platform)) == DEATH_BLOCK)
            {
                gameplay_create(scene, body, platform, gameplay_death_block, NULL, NULL);
            }
            if (sprite_subrole((sprite_t *)body_get_info(platform)) == TUNNEL_BLOCK && sprite_role((sprite_t *)body_get_info(body)) == PLAYER)
            {
                gameplay_create(scene, body, platform, gameplay_tunnel_block, NULL, NULL);
            }
            if (sprite_subrole((sprite_t *)body_get_info(platform)) == ITEM_BLOCK && sprite_role((sprite_t *)body_get_info(body)) == PLAYER)
            {
                gameplay_create(scene, body, platform, gameplay_item_block, scene, NULL);
            }
            if (sprite_subrole((sprite_t *)body_get_info(platform)) == INVISIBLE_BLOCK && sprite_role((sprite_t *)body_get_info(body)) == ENEMY)
            {
                gameplay_create(scene, body, platform, gameplay_invisible_block, scene, NULL);
            }
        }
    }
}

void enemy_enemy_init_actions(scene_t *scene, list_t *list_bodies, body_t *body, size_t size, size_t curr_index)
{
    if (sprite_subrole((sprite_t *)body_get_info(body)) == KOOPA)
    {
        for (size_t q = 0; q < size; q++) // creates enemy, koopa
        {
            double *time = malloc(sizeof(double));
            *time = 0;
            if (q != curr_index)
            {
                gameplay_create(scene, list_get(list_bodies, q), body, gameplay_koopa_enemy, time, free);
            }
        }
    }
}