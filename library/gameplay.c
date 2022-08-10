#include "gameplay.h"
#include "body.h"
#include "initialize.h"

// WE SHOULD PROBABLY INCLUDE ASSERT STATEMENTS.

double sublevel_choice = -1;
// Sets the time that enemy switches patrols
// So the enemy changes velocity every 500 ticks
const double PATROL_SWITCH = 500;

// This is the min overlap we want between objects
// This always us to simulate solid ground
const double MIN_OVERLAP = 0.1;


void gameplay_create(scene_t *scene, body_t *body1, body_t *body2, force_creator_t handler, void *aux, free_func_t freer)
{
    sublevel_choice = -1;
    list_t *bodies = list_init(2, (free_func_t) body_free);
    list_add(bodies, body1);
    if (body2 != NULL) {list_add(bodies, body2);}

    scene_add_bodies_force_creator(scene, handler, aux, bodies, freer); 
}


// Gameplay operations ------------------------------------------------------------------------------------------------
// ORDER: GENERAL -> TOKEN -> ENEMY -> PLATFORM -> PLAYER

// GENERAL gameplay functions used across differnt roles ---------------------------------------------------------------------------------

// Applies elastic collisions effect
void elastic_collisions(body_t *body1, body_t *body2, collision_info_t collision, double k, bool apply1, bool apply2)
{
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
    if (impulse.y < 0) { impulse = (vector_t) {impulse.x, 0};}
    // Add impulses to only the first body.
    if (apply1) {body_add_impulse(body1, (vec_negate(impulse)));}
    if (apply2) {body_add_impulse(body2, impulse);}
}

// Applies bot mechnics to enemies and powerups
void create_bot_mechanics(scene_t* scene, body_t* bot, vector_t speed)
{
    body_set_velocity(bot, vec_negate(speed));
    // need to alloc memory for vector to pass in as *aux
    double* time_travelled = malloc(sizeof(vector_t));
    *time_travelled = 0;
    gameplay_create(scene, bot, NULL, gameplay_patrol, time_travelled, free);
}


// Player mechanics such as gravity are handled here 
void gameplay_gravity(interaction_t *interaction)
{
    body_t *body = interaction_get_body(interaction, 0);
    
    // Only applies gravity if player isn't touching the ground
    // Helps avoid collision issues
    if (!(sprite_t*) sprite_contact(body_get_info(body)).below)
    {
        // The gravitational field that is applied constantly.
        double G = *(double*) (interaction_get_aux(interaction));
        body_add_force(body, (vector_t) {0, G});
    }
}


// Applies basic bot patrol where power/enemy move back and forth certain radius from starting point
void gameplay_patrol(interaction_t *interaction)
{
    body_t *body = interaction_get_body(interaction, 0);
    double time_travelled = *(double*) (interaction_get_aux(interaction));
    time_travelled = time_travelled + 1;
    
    if (time_travelled > PATROL_SWITCH)
    {
        vector_t v = body_get_velocity(body);
        body_set_velocity(body, (vector_t) {-1 * v.x, 0});
        time_travelled = 0;
    }
    double* new_time = malloc(sizeof(double));
    *new_time = time_travelled;
    interaction_set_aux(interaction, new_time);
}

// TOKEN GAMEPLAY ---------------------------------------------------------------------------------------

// interaction with player and token
void gameplay_token(interaction_t* interaction)
{
    if(!interaction_colliding(interaction))
    {
        body_t* body_player = interaction_get_body(interaction, 0);
        body_t* token = interaction_get_body(interaction, 1);
        // Check if the two bodies have collided.
        if (find_collision(body_player, token).collided)
        {
            sprite_t* player = (sprite_t*) body_get_info(body_player);
            // Remove token and increase token count by 1
            sprite_add_token(player);
            body_remove(token);
        }
    }
}

// ENEMY gameplay --------------------------------------------------------------------------------------------------------

// Goomba gameplay interactions
// Even though we could get everything from interaction, we pass other things in because we want to avoid repetitive accessing
void gameplay_goomba(interaction_t* interaction)
{
    body_t* body_player = interaction_get_body(interaction, 0);
    body_t* body_goomba = interaction_get_body(interaction, 1);
    sprite_t* goomba = (sprite_t*) body_get_info(body_goomba);
    // Each enemy uses counter differently
    double time_squashed = *(double*) (interaction_get_aux(interaction));
    // Time gets added after first kill collision

    if(time_squashed >= 1)
    {
        time_squashed ++;
        body_set_velocity(body_goomba, VEC_ZERO);
        sprite_set_state(goomba, GOOMBA_SQUASHED);
        // Remove body after 200 ticks
        if (time_squashed >= 200)
        {
            body_remove(body_goomba);
        }
    }
    else 
    {
        // only process if during last tick a collision between the two didn't occur
        // Sets colliding to false next time interaction is called
        if (interaction_is_colliding(interaction))
        {
            collision_info_t collision = find_collision(body_player, body_goomba); 
            // checking if the two objects are still colliding
            if(!collision.collided)
            {
                interaction_set_colliding(interaction, false);
            }
        }
        else
        {
        // only process if during last tick a collision between the two didn't occur
        // Sets colliding to false next time interaction is called
            collision_info_t collision = find_collision(body_player, body_goomba); 
            // Checks if the player jumped on top of enemy
            if (collision.collided)
            {
                sprite_t* player = (sprite_t*) body_get_info(body_player);
                vector_t axis = collision.axis;
                if (sprite_current_frame(player).x == STAR_MODE || sprite_current_frame(player).x == STAR_MODE + 6)
                {body_remove(body_goomba); return;} // Player insta kills when he is in star mode
                // Checks if player is indeed above the enemy
                if (fabs(axis.y) > 0.1 && (body_get_centroid(body_player).y > body_get_centroid(body_goomba).y))
                {
                    time_squashed ++; // Starts squash timer
                    // If the player jumps on top of enemy then kill enemy
                    sprite_add_kills(player);
                    interaction_set_colliding(interaction, true); // Lets code know that objects are colliding
                } 
                else if (fabs(axis.x) > 0.1)// else if player hits enemy on sides, then player loses one health
                { 
                    sprite_lower_health(player);
                    interaction_set_colliding(interaction, true); // lets code know that bodies are colliding
                    elastic_collisions(body_player, body_goomba, collision, 1, true, false);
                    body_set_velocity(body_goomba, (vector_t) {-1 * body_get_velocity(body_goomba).x, 0});
                    body_translate(body_player, vec_negate((vector_t) {collision.overlap_v.x, 0}));
                    sprite_immobilized_tick(player);
                }
            }
        }
    }
    // Set new squash time
    double* new_counter = malloc(sizeof(double));
    *new_counter = time_squashed;
    interaction_set_aux(interaction, new_counter);
}

// koopa gameplay interactions
// Still needs to be implemented
// need to make it so it impluses you back into sky when you jump on koopa
void gameplay_koopa(interaction_t* interaction)
{
    body_t* body_player = interaction_get_body(interaction, 0);
    body_t* body_koopa = interaction_get_body(interaction, 1);
    sprite_t* koopa = (sprite_t*) body_get_info(body_koopa);
    // Each enemy uses counter differently
    double hits = *(double*) (interaction_get_aux(interaction));
    vector_t v = body_get_velocity(body_koopa);
    if (!sprite_state_equal(koopa, KOOPA_SHELL) && v.x > 0 && !sprite_state_equal(koopa, KOOPA_RIGHT)) {sprite_set_state(koopa, KOOPA_RIGHT);}
    else if (!sprite_state_equal(koopa, KOOPA_SHELL) && v.x < 0 && !sprite_state_equal(koopa, KOOPA_LEFT)) {sprite_set_state(koopa, KOOPA_LEFT);} 

    // only process if during last tick a collision between the two didn't occur
    // Sets colliding to false next time interaction is called
    if (interaction_is_colliding(interaction))
    {
        collision_info_t collision = find_collision(body_player, body_koopa); 
        // checking if the two objects are still colliding
        if(!collision.collided)
        {
            interaction_set_colliding(interaction, false);
        }
    }
    else
    {
        collision_info_t collision = find_collision(body_player, body_koopa); 
        // Checks if the player jumped on top of enemy
        if (collision.collided)
        {
            sprite_t* player = (sprite_t*) body_get_info(body_player);
            if (sprite_current_frame(player).x == STAR_MODE || sprite_current_frame(player).x == STAR_MODE + 6)
            {body_remove(body_koopa); return;} // Player insta kills when he is in star mode
            vector_t axis = collision.axis;
            // Checks if player is indeed above the enemy
            if (fabs(axis.y) > 0.1 && (body_get_centroid(body_player).y > body_get_centroid(body_koopa).y))
            {
                hits ++; // koopa goes into shell after 1st hit and stops
                vector_t koopa_velocity;
                if ((int) hits % 2 == 1) {sprite_set_state(koopa, KOOPA_SHELL); body_set_velocity(body_koopa, VEC_ZERO);} 
                if(body_get_centroid(body_player).x > body_get_centroid(body_koopa).x) {koopa_velocity = (vector_t) {-100, 0};}
                else {koopa_velocity = (vector_t) {100, 0};}
                if ((int) hits % 2 == 0) {body_set_velocity(body_koopa, koopa_velocity);} // kopa moves
                
                elastic_collisions(body_player, body_koopa, collision, 1, true, false);
                interaction_set_colliding(interaction, true); // Lets code know that objects are colliding
            } 
            else if (fabs(axis.x) > 0.1)// else if player hits enemy on sides, then player loses one health
            { 
                interaction_set_colliding(interaction, true); // lets code know that bodies are colliding
                sprite_lower_health(player);
                elastic_collisions(body_player, body_koopa, collision, 1, true, false);
                vector_t v = body_get_velocity(body_koopa);
                body_set_velocity(body_koopa, (vector_t) {-1 * v.x, v.y});
                sprite_immobilized_tick(player);
            }
        }
    }
    // Set new squash time
    double* new_counter = malloc(sizeof(double));
    *new_counter = hits;
    interaction_set_aux(interaction, new_counter);
}
// Koopa and enemy interactions... Mainly has to do with how koopa in sheel mode kills enemies
void gameplay_koopa_enemy(interaction_t* interaction)
{
    body_t* body_enemy = interaction_get_body(interaction, 0);
    body_t* body_koopa = interaction_get_body(interaction, 1);
    sprite_t* koopa = (sprite_t*) body_get_info(body_koopa);
    if (interaction_is_colliding(interaction))
    {
        collision_info_t collision = find_collision(body_enemy, body_koopa); 
        // checking if the two objects are still colliding
        if(!collision.collided) {interaction_set_colliding(interaction, false);}
    }
    else
    {
        collision_info_t collision = find_collision(body_enemy, body_koopa); 
        // Only enter if colliding and if koopa is in shell mode
        if (collision.collided && sprite_state_equal(koopa, KOOPA_SHELL))
        {
            vector_t axis = collision.axis;
            // Checks if player is indeed above the enemy
            if (fabs(axis.y) > 0.1 && (body_get_centroid(body_enemy).y < body_get_centroid(body_koopa).y))
            {
                body_remove(body_enemy); 
                interaction_set_colliding(interaction, true); // Lets code know that objects are colliding
            } 
            else if (fabs(axis.x) > 0.1)// else if koopa hits enemy on sides, then player loses one health
            { 
                body_remove(body_enemy);
                interaction_set_colliding(interaction, true); // lets code know that bodies are colliding
            }
        }
    }
}
// Gameplay for platforms --------------------------------------------------------------------------------------------------------------

// Regular solid block
void gameplay_regular_block(interaction_t* interaction)
{
    body_t* body1 = interaction_get_body(interaction, 0);
    body_t* body_platform = interaction_get_body(interaction, 1);
    collision_info_t collision = find_collision(body1, body_platform); 

    // If axis is with reference of player than .y < 0 and if reference enemy .y > 1
    if (collision.collided) 
    {
        sprite_t* sprite1 = (sprite_t*) body_get_info(body1);
        contact_t contact = sprite_contact(sprite1);
       
        // If statements to create solid effect 0.1 is used to avoid small variations.
        if (fabs(collision.axis.y) > 0.1)
        {   
            // Fireball gets an elastic collision
            if (sprite_subrole(sprite1) == FIREBALL && !interaction_is_colliding(interaction)) 
            {
                elastic_collisions(body1, body_platform, collision, 1, true, false);
                body_translate(body1, (vector_t) {0, (collision.overlap + MIN_OVERLAP)});
                interaction_set_colliding(interaction, true);
                return;
            }
            if(body_get_centroid(body1).y > body_get_centroid(body_platform).y) // player is above platform
            {
                sprite_set_contact(sprite1, (contact_t) {contact.left, contact.right, false, true});  
                body_translate(body1, (vector_t) {0, (collision.overlap - MIN_OVERLAP)});
            }
            else // player is below the platform
            {
                sprite_set_contact(sprite1, (contact_t) {contact.left, contact.right, true, false});
                body_translate(body1, (vector_t) {0, -1 * (collision.overlap - MIN_OVERLAP)});
            }
        }
        else if (fabs(collision.axis.x) > 0.1)
        {
            bool collided = interaction_is_colliding(interaction);
            // Fireball gets removed if it hits a wall
            if (sprite_subrole(sprite1) == FIREBALL) {body_remove(body1); return;}
            // Player is in contact with the platform on left
            if(body_get_centroid(body1).x > body_get_centroid(body_platform).x) 
            {   // Rebounds enemy off wall
                if (sprite_role(sprite1) == ENEMY && !collided) 
                {
                    body_translate(body1, (vector_t) {(collision.overlap + MIN_OVERLAP), 0});
                    body_set_velocity(body1, (vector_t) {-1 * body_get_velocity(body1).x, 0});
                    interaction_set_colliding(interaction, true);
                    return;
                }
                sprite_set_contact(sprite1, (contact_t) {true, false, contact.above, contact.below});
                body_translate(body1, (vector_t) {(collision.overlap - MIN_OVERLAP), 0});
            }
            else // Player is in contact with platform on right
            {   // Rebounds enemy off wall
                if (sprite_role(sprite1) == ENEMY && !collided) 
                {
                    body_translate(body1, (vector_t) {-1 * (collision.overlap + MIN_OVERLAP), 0});
                    body_set_velocity(body1, (vector_t) {-1 * body_get_velocity(body1).x, 0});
                    interaction_set_colliding(interaction, true);
                    return;
                }
                sprite_set_contact(sprite1, (contact_t) {false, true, contact.above, contact.below});
                body_translate(body1, (vector_t) {-1 * (collision.overlap - MIN_OVERLAP), 0}); 
            }
        }
        interaction_set_colliding(interaction, true); // Sets the objects are under collision
    }
    else 
    {
        // This will set all contacts to false once the objects STOP colliding
        // If not other blocks will turn off contacts and then player will fall through a block
        if (interaction_is_colliding(interaction)) // Need if to ensure that they were colliding
        {   
            sprite_t* sprite1 = (sprite_t*) body_get_info(body1);
            interaction_set_colliding(interaction, false); // lets code know that bodies are no longer colliding
            sprite_set_contact(sprite1, (contact_t) {false, false, false, false});
            return;
        }
    }  
}

void gameplay_invisible_block(interaction_t* interaction)
{
    body_t* body1 = interaction_get_body(interaction, 0);
    body_t* body_platform = interaction_get_body(interaction, 1);
    collision_info_t collision = find_collision(body1, body_platform); 

    // If axis is with reference of player than .y < 0 and if reference enemy .y > 1
    if (collision.collided) 
    {
        sprite_t* sprite1 = (sprite_t*) body_get_info(body1);
        // Only bounces on x axis
        if (fabs(collision.axis.x) > 0.1)
        {
            bool collided = interaction_is_colliding(interaction);
            // Fireball gets removed if it hits a wall
            if (sprite_subrole(sprite1) == FIREBALL) {body_remove(body1); return;}
            // Player is in contact with the platform on left
            if(body_get_centroid(body1).x > body_get_centroid(body_platform).x) 
            {   // Rebounds enemy off wall
                if (sprite_role(sprite1) == ENEMY && !collided) 
                {
                    body_translate(body1, (vector_t) {(collision.overlap + MIN_OVERLAP), 0});
                    body_set_velocity(body1, (vector_t) {-1 * body_get_velocity(body1).x, 0});
                    interaction_set_colliding(interaction, true);
                    return;
                }
            }
            else // Player is in contact with platform on right
            {   // Rebounds enemy off wall
                if (sprite_role(sprite1) == ENEMY && !collided) 
                {
                    body_translate(body1, (vector_t) {-1 * (collision.overlap + MIN_OVERLAP), 0});
                    body_set_velocity(body1, (vector_t) {-1 * body_get_velocity(body1).x, 0});
                    interaction_set_colliding(interaction, true);
                    return;
                }
             }
        }
        interaction_set_colliding(interaction, true); // Sets the objects are under collision
    }
    else 
    {
        // This will set all contacts to false once the objects STOP colliding
        // If not other blocks will turn off contacts and then player will fall through a block
        if (interaction_is_colliding(interaction)) // Need if to ensure that they were colliding
        {   
            sprite_t* sprite1 = (sprite_t*) body_get_info(body1);
            interaction_set_colliding(interaction, false); // lets code know that bodies are no longer colliding
            sprite_set_contact(sprite1, (contact_t) {false, false, false, false});
            return;
        }
    }  
}

// Player dies when they touch this block
void gameplay_death_block(interaction_t* interaction)
{
    body_t* body1 = interaction_get_body(interaction, 0);
    body_t* body_platform = interaction_get_body(interaction, 1);
    
    collision_info_t collision = find_collision(body1, body_platform); 
    // As soon as player or any enemy collides they lose all health
    if (collision.collided) 
    {
        sprite_t* sprite1 = (sprite_t*) body_get_info(body1);
        for(size_t i = 0; i < sprite_health(sprite1); i++) {sprite_lower_health(sprite1);}
        if (sprite_role(sprite1) != PLAYER) {body_remove(body1);}
    }  
}
// top, left, right, bottom determine entrance to the tunnel for player
typedef struct tunnel
{
    bool top;
    bool left;
    bool right;
    bool bottom;
    double sublevel;
    vector_t exit;
} tunnel_t;

// Returns tunnel struct, for exit vector specify where you want play body to exit, do VEC_ZERO if you want same place as entered
tunnel_t* tunnel_init(bool top, bool left, bool bottom, bool right, double sublevel, vector_t exit)
{
    tunnel_t* tunnel = malloc(sizeof(tunnel_t));
    tunnel -> top = top;
    tunnel -> left = left;
    tunnel -> right = right;
    tunnel -> bottom = bottom;
    tunnel -> sublevel = sublevel;
    tunnel -> exit = exit;
    return  tunnel;
}

// Used in gameplay so sublevel can be selected 
void sublevel_select(double sublevel) {sublevel_choice = sublevel;}
double sublevel_run() {return sublevel_choice;}
// Trasnports player to a sublevel
// Considering making a struct to pass into interactions for this
void gameplay_tunnel_block(interaction_t* interaction)
{
    body_t* body_player = interaction_get_body(interaction, 0);
    body_t* body_platform = interaction_get_body(interaction, 1);
    collision_info_t collision = find_collision(body_player, body_platform);
    // If axis is with reference of player than .y < 0 and if reference enemy .y > 1
    if (collision.collided) 
    {
        sprite_t* player = (sprite_t*) body_get_info(body_player);
        sprite_t* platform = (sprite_t*) body_get_info(body_platform);
        tunnel_t* tunnel = (tunnel_t*) sprite_get_info(platform);
        double level = tunnel -> sublevel;
       
        // If statements to create solid effect 0.1 is used to avoid small variations.
        if (fabs(collision.axis.y) > 0.1)
        {        
            if (body_get_centroid(body_player).y > body_get_centroid(body_platform).y && tunnel -> top) // player is above platform
            {
                if (sprite_state_equal(player, PLAYER_GROUNDPOUND_LEFT) || sprite_state_equal(player, PLAYER_GROUNDPOUND_RIGHT))
                // Sets exit and selects sublevel
                {vector_t* exit = malloc(sizeof(vector_t)); *exit = tunnel -> exit; sprite_set_info(player, exit); sublevel_select(level);}
            }
            else if (tunnel -> bottom)// player is below the platform
            {
                // Sets exit and selects sublevel
                vector_t* exit = malloc(sizeof(vector_t)); *exit = tunnel -> exit; sprite_set_info(player, exit); sublevel_select(level);
            }
        }
        else if (fabs(collision.axis.x) > 0.1)
        {
            // Player is in contact with the platform on left
            if(body_get_centroid(body_player).x > body_get_centroid(body_platform).x) 
            {
                // Sets exit and selects sublevel
                if(tunnel -> right) {vector_t* exit = malloc(sizeof(vector_t)); *exit = tunnel -> exit; sprite_set_info(player, exit); sublevel_select(level);}
            }
            else // Player is in contact with platform on right
            {
                if(tunnel -> left) {vector_t* exit = malloc(sizeof(vector_t)); *exit = tunnel -> exit; sprite_set_info(player, exit); sublevel_select(level);}
            }
        }
        interaction_set_colliding(interaction, true); // Sets the objects are under collision
    }
    else 
    {
        // This will set all contacts to false once the objects STOP colliding
        // If not other blocks will turn off contacts and then player will fall through a block
        if (interaction_is_colliding(interaction)) // Need if to ensure that they were colliding
        {   
            sprite_t* player = (sprite_t*) body_get_info(body_player);
            interaction_set_colliding(interaction, false); // lets code know that bodies are no longer colliding
            sprite_set_contact(player, (contact_t) {false, false, false, false});
            return;
        }
    }  
}

// Causes a powerup item to pop up
void gameplay_item_block(interaction_t* interaction)
{
    body_t* body1 = interaction_get_body(interaction, 0);
    body_t* body_platform = interaction_get_body(interaction, 1);
    sprite_t* platform = (sprite_t*) body_get_info(body_platform);
    subrole_t subrole = *(subrole_t*) sprite_get_info(platform);
    collision_info_t collision = find_collision(body1, body_platform); 
    if(interaction_is_colliding(interaction)) {return;} // interaction only happens once entire game
    // So this never gets set back to false
    if (collision.collided) 
    {
        sprite_t* sprite1 = (sprite_t*) body_get_info(body1);
       
        // If player grounds bounds and has some y velocity then continue
        if (fabs(collision.axis.y) > 0.1 && (sprite_state_equal(sprite1, PLAYER_GROUNDPOUND_LEFT) || sprite_state_equal(sprite1, PLAYER_GROUNDPOUND_LEFT)))
        {     
            if(body_get_velocity(body1).y < -10) 
            {
                // Applies the powerup and then changes state
                sprite_activate_powerup(sprite1, subrole);
                if (subrole == HEALTH_POWERUP) {sprite_set_state(platform, BLOCK_HEALTH);}
                if (subrole == STAR_POWERUP) {sprite_set_state(platform, BLOCK_STAR);}
                if (subrole == FIRE_POWERUP) {sprite_set_state(platform, BLOCK_FIRE);}
                if (subrole == TOKEN_POWERUP) {sprite_set_state(platform, BLOCK_TOKEN);}
                interaction_set_colliding(interaction, true);
            }   
        }
        // Player is below block and jumps up to hit it
        else if (fabs(collision.axis.y) > 0.1 && (body_get_centroid(body1).y < body_get_centroid(body_platform).y))
        {
            // Applies the powerup and then changes state
            sprite_activate_powerup(sprite1, subrole);
            if (subrole == HEALTH_POWERUP) {sprite_set_state(platform, BLOCK_HEALTH);}
            if (subrole == STAR_POWERUP) {sprite_set_state(platform, BLOCK_STAR);}
            if (subrole == FIRE_POWERUP) {sprite_set_state(platform, BLOCK_FIRE);}
            if (subrole == TOKEN_POWERUP) {sprite_set_state(platform, BLOCK_TOKEN);}
            interaction_set_colliding(interaction, true);
        }
         // Sets the objects are under collision
    }
}

// Powerup gameplay ------------------------------------------------------------------------------------------------------------------------

// Will have to use time tracking ability through interaction
void gameplay_star_powerup(interaction_t* interaction)
{
    body_t* body_player = interaction_get_body(interaction, 0);
    sprite_t* player = (sprite_t*) body_get_info(body_player);
    // +6 is for player 2
    if (sprite_current_frame(player).x == STAR_MODE || sprite_current_frame(player).x == STAR_MODE + 6)
    {
        double time_invincibe = *(double*) interaction_get_aux(interaction);
        time_invincibe ++;
    
        if (time_invincibe >= 2000)
        {
            if (sprite_subrole(player) == PLAYER1) {sprite_set_current_frame(player, 0);}
            else {sprite_set_current_frame(player, 6);}
            time_invincibe = 0;
        }
        double* new_time = malloc(sizeof(double));
        *new_time = time_invincibe;
        interaction_set_aux(interaction, new_time);
    }
}

// Fireball interaction
void gameplay_fireball(interaction_t* interaction)
{
    body_t* enemy = interaction_get_body(interaction, 0);
    body_t* fireball = interaction_get_body(interaction, 1);
    double bounces = *(double*) interaction_get_aux(interaction);

    
    collision_info_t collision = find_collision(enemy, fireball); 
    // As soon as player or any enemy collides they lose all health
    if (collision.collided) 
    {
        sprite_t* sprite1 = (sprite_t*) body_get_info(enemy);
        for(size_t i = 0; i < sprite_health(sprite1); i++) {sprite_lower_health(sprite1);}
        if (sprite_role(sprite1) != PLAYER) {body_remove(enemy);}
    }  
    // After three bounces remove fireball
    if (bounces == 500) {body_remove(fireball);}
    bounces ++;
    double* new_bounces = malloc(sizeof(double));
    *new_bounces = bounces;
    interaction_set_aux(interaction, new_bounces);
}

// PLAYER gameplay --------------------------------------------------------------------------------------------------

// Handles player animations
void gameplay_player(interaction_t *interaction)
{
    body_t *body = interaction_get_body(interaction, 0);
    sprite_t *player = (sprite_t*) body_get_info(body);
    contact_t contact = sprite_contact(player);
    vector_t v = body_get_velocity(body);

    if (v.x == 0) 
    {
        if (sprite_state_equal(player, PLAYER_RUNNING_RIGHT)) {sprite_set_state(player, PLAYER_IDLE_RIGHT);}
        if (sprite_state_equal(player, PLAYER_RUNNING_LEFT)) {sprite_set_state(player, PLAYER_IDLE_LEFT);}
        if (sprite_state_equal(player, PLAYER_JUMPING_RIGHT)) {sprite_set_state(player, PLAYER_IDLE_RIGHT);}
        if (sprite_state_equal(player, PLAYER_JUMPING_LEFT)) {sprite_set_state(player, PLAYER_IDLE_LEFT);}
        return;
    }
    else if (!contact.below)
    {
        if (v.x > 0 && !sprite_state_equal(player, PLAYER_JUMPING_RIGHT)) {sprite_set_state(player, PLAYER_JUMPING_RIGHT);}
        else if (v.x < 0 && !sprite_state_equal(player, PLAYER_JUMPING_LEFT)) {sprite_set_state(player, PLAYER_JUMPING_LEFT);}
    
    } else
    {
        if (v.x > 0 && !sprite_state_equal(player, PLAYER_RUNNING_RIGHT)) {sprite_set_state(player, PLAYER_RUNNING_RIGHT);}
        else if (v.x < 0 && !sprite_state_equal(player, PLAYER_RUNNING_LEFT)) {sprite_set_state(player, PLAYER_RUNNING_LEFT);}
    }
}



