#include "forces.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "vector.h"
#include "body.h"
#include "scene.h"
#include "gameplay.h"
#include "initialize.h"

// Tests if an enemy reverses direction when it hits player and that it loses health
void test_player_enemy_rebound()
{
    double STEPS = 20000;
    double DT = 1e-6;
    scene_t *scene = scene_init();

    body_t *player = add_player();
    scene_add_body(scene, player, PLAYER);
    body_translate(player, (vector_t) {4, 0});

    body_t *enemy = add_enemy();
    body_set_velocity(enemy, (vector_t) {2000, 0});
    scene_add_body(scene, enemy, ENEMY);

    add_interactions(scene);
    sprite_t *player_sprite = (sprite_t*) body_get_info(player);

    for (int i = 0; i < STEPS; i++)
    {
        scene_tick(scene, DT);
    }
    assert(within(1e-3, body_get_velocity(enemy).x, -2000));
    assert(sprite_health(player_sprite) == 2);
    scene_free(scene);
}

void test_player_enemy_kill()
{
    double STEPS = 21000;
    double DT = 1e-6;
    scene_t *scene = scene_init();

    body_t *player = add_player();
    scene_add_body(scene, player, PLAYER);
    body_translate(player, (vector_t) {0, 4});

    body_t *enemy = add_enemy();
    body_set_velocity(enemy, (vector_t) {0, 200});
    scene_add_body(scene, enemy, ENEMY);

    add_interactions(scene);

    sprite_t *player_sprite = (sprite_t*) body_get_info(player);

    for (int i = 0; i < STEPS; i++) {
        scene_tick(scene, DT);
    }
    assert(sprite_health(player_sprite) == 3);
    assert(sprite_kills(player_sprite) == 1);
    assert(list_size(scene_get_list(scene, ENEMY)) == 0);

    body_t *enemy1 = add_enemy();
    scene_add_body(scene, enemy1, ENEMY);
    create_player_enemy_action(scene, player, enemy1);
    body_set_velocity(player, (vector_t) {0, -200});

    assert(vec_equal(body_get_velocity(player), (vector_t) {0, -200}));

    for (int i = 0; i < STEPS; i++) {
        scene_tick(scene, DT);
    }
    assert(sprite_health(player_sprite) == 3);
    assert(sprite_kills(player_sprite) == 2);
    assert(list_size(scene_get_list(scene, ENEMY)) == 0);

    body_t *enemy2 = add_enemy();
    scene_add_body(scene, enemy2, ENEMY);
    create_player_enemy_action(scene, player, enemy2);
    body_set_velocity(player, (vector_t) {0, 200});

    assert(vec_equal(body_get_velocity(player), (vector_t) {0, 200}));

    for (int i = 0; i < STEPS; i++) {
        scene_tick(scene, DT);
    }
    assert(sprite_health(player_sprite) == 2);
    assert(sprite_kills(player_sprite) == 2);
    assert(list_size(scene_get_list(scene, ENEMY)) == 1);

    scene_free(scene);
}

void test_player_enemy__complex_kill()
{
    double STEPS = 21000;
    double DT = 1e-6;
    scene_t *scene = scene_init();

    body_t *player = add_player();
    scene_add_body(scene, player, PLAYER);
    body_translate(player, (vector_t) {0.25, 4});

    body_t *enemy = add_enemy();
    body_set_velocity(enemy, (vector_t) {0, 200});
    scene_add_body(scene, enemy, ENEMY);

    add_interactions(scene);

    sprite_t *player_sprite = (sprite_t*) body_get_info(player);

    for (int i = 0; i < STEPS; i++) {
        scene_tick(scene, DT);
    }
    assert(sprite_health(player_sprite) == 3);
    assert(sprite_kills(player_sprite) == 1);
    assert(list_size(scene_get_list(scene, ENEMY)) == 0);

    body_t *enemy1 = add_enemy();
    scene_add_body(scene, enemy1, ENEMY);
    create_player_enemy_action(scene, player, enemy1);
    body_set_velocity(player, (vector_t) {0, -200});

    assert(vec_equal(body_get_velocity(player), (vector_t) {0, -200}));

    for (int i = 0; i < STEPS; i++) {
        scene_tick(scene, DT);
    }
    assert(sprite_health(player_sprite) == 3);
    assert(sprite_kills(player_sprite) == 2);
    assert(list_size(scene_get_list(scene, ENEMY)) == 0);

    scene_free(scene);
}

void test_player_enemy_platform()
{
    double STEPS = 20000;
    double DT = 1e-5;
    scene_t *scene = scene_init();

    body_t *player = add_player();
    scene_add_body(scene, player, PLAYER);
    body_translate(player, (vector_t) {0.25, 4});

    body_t *enemy = add_enemy();
    body_translate(enemy, (vector_t) {2.2, 2.2});
    scene_add_body(scene, enemy, ENEMY);

    double NUMBER_PLATFORMS = 20;

    for (size_t i = 0; i < NUMBER_PLATFORMS; i++)
    {
        body_t *platform = add_platform((vector_t) {1000, 100});
        body_set_centroid(platform, (vector_t) {2 * i, 1});
        scene_add_body(scene, platform, PLATFORM);
    }

    add_interactions(scene);

    sprite_t *player_sprite = (sprite_t*) body_get_info(player);

    for (int i = 0; i < STEPS; i++) {
        if(body_get_centroid(player).y < 3.2 && body_get_centroid(player).y > 3)
        {
            // printf(" %f, %f, %f, %f \n", body_get_force(player).x, body_get_centroid(player).y, body_get_velocity(player).x, body_get_velocity(player).y);
        }
        scene_tick(scene, DT);
    }
    vec_print(body_get_centroid(player));
    vec_print(body_get_centroid(enemy));
    // assert(vec_isclose(body_get_centroid(player), (vector_t) {0.75, 3.5}));

    scene_free(scene);
}


void test_player_token()
{
    double STEPS = 20000;
    double DT = 1e-6;
    scene_t *scene = scene_init();

    body_t *player = add_player();
    scene_add_body(scene, player, PLAYER);
    body_translate(player, (vector_t) {4, 0});

    // Need to add an enemy due to nature we are writing code
    body_t *enemy = add_enemy();
    body_set_velocity(enemy, (vector_t) {0, 0});
    body_translate(player, (vector_t) {400000, 0});
    scene_add_body(scene, enemy, ENEMY);

    add_interactions(scene);
    sprite_t *player_sprite = (sprite_t*) body_get_info(player);
    create_newtonian_gravity(scene, -   1e-4, player, enemy);

    for (int i = 0; i < STEPS; i++)
    {
        scene_tick(scene, DT);
    }
    assert(within(1e-3, body_get_velocity(enemy).x, -2000));
    assert(sprite_health(player_sprite) == 2);
    scene_free(scene);
}

int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_player_enemy_rebound)
    DO_TEST(test_player_enemy_kill)
    DO_TEST(test_player_enemy__complex_kill)
    DO_TEST(test_player_enemy_platform)

    puts("actions_test PASS");
}
