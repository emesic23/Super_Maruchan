#include "levels.h"
#include "gameplay.h"


// Player 1 and 2 start positions
const vector_t PLAYER_START = {200, 100};
const vector_t PLAYER2_START = {250, 100};

// Level selector
scene_t* level_select(double choice, bool multiplayer)
{
    scene_t* scene;
    // World 1 levels
    if (choice == 1) {return scene = level_load((level_creator_t) world1_level1, multiplayer);}
    if (choice == 2) {return scene = level_load((level_creator_t) world1_level2, multiplayer);}
        if (choice == 1.21) {return scene = level_load((level_creator_t) world1_level2_sublevel1, multiplayer);}
    if (choice == 3) {return scene = level_load((level_creator_t) world1_level3, multiplayer);}
        if (choice == 1.31) {return scene = level_load((level_creator_t) world1_level3_sublevel1, multiplayer);}

    return NULL;
}

// Runs desired level of the game.
scene_t* level_load(level_creator_t level, bool multiplayer)
{
    scene_t* scene = scene_init();
    level(scene);
    // Add a player to the scene.
    add_player(scene, PLAYER_START, PLAYER1);
    sdl_on_key((key_handler_t) on_key); // keys for singleplayer
    // Add second player if multiplayer is on
    if (multiplayer) {add_player(scene, PLAYER2_START, PLAYER2);}
    // later will add multiplayer controls here^
    assert((sprite_t*) body_get_info(scene_get_body(scene, PLAYER, 0)) != NULL);
    // Establishes interactions between all objects in the scene.
    add_interactions(scene);
    return scene;
}

// Loads ingame sublevel caused by using a tunnel
scene_t* sublevel_load(bool multiplayer)
{
    scene_t* scene = level_select(sublevel_run(), multiplayer);
    return scene;
}

// Creates borders of levels that camera is then able to follow
// MUST BE FIRST PLATFORMS ADDED TO SCENE
void level_borders(scene_t *scene, vector_t max)
{
    // floor
    add_platform(scene, (vector_t) {max.x/2, -15}, (vector_t) {max.x, 30}, DEATH_BLOCK, NULL);
    // roof
    add_platform(scene,(vector_t) {max.x/2, max.y}, (vector_t) {max.x, 30}, REGULAR_BLOCK, NULL);
    // leftWall, rightWall
    add_platform(scene, (vector_t) {0, max.y/2}, (vector_t) {30, max.y}, REGULAR_BLOCK, NULL);
    add_platform(scene, (vector_t) {max.x, max.y/2}, (vector_t) {30, max.y}, REGULAR_BLOCK, NULL);
}

// Creates scene for each level -------------------------------------------------------
void world1_level1(scene_t* scene)
{
    add_background(scene, (vector_t) {7553, 500}, LEVEL1);
    level_borders(scene, (vector_t) {7553, 500});
    add_platform_corners(scene, (vector_t) {0, 0}, (vector_t) {7553, 30}, DEATH_BLOCK, NULL);

    //Ground Platforms
    add_platform_corners(scene, (vector_t) {0, 0}, (vector_t) {2463, 54}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2535, 0}, (vector_t) {3070, 54}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {3179, 0}, (vector_t) {5462, 54}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5535, 0}, (vector_t) {7753, 54}, REGULAR_BLOCK, NULL);

    //Obstacles
    //Start Thingy
    add_platform_corners(scene, (vector_t) {571, 160}, (vector_t) {606, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {713, 160}, (vector_t) {893, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {786, 304}, (vector_t) {820, 338}, REGULAR_BLOCK, NULL);
    //pipe thingy
    add_platform_corners(scene, (vector_t) {1000, 54}, (vector_t) {1070, 123}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1359, 54}, (vector_t) {1428, 160}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1642, 54}, (vector_t) {1715, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2034, 54}, (vector_t) {2106, 196}, REGULAR_BLOCK, NULL);
    //single powerblock
    add_platform_corners(scene, (vector_t) {2286, 196}, (vector_t) {2320, 230}, REGULAR_BLOCK, NULL);
    //blocks separated thingy
    add_platform_corners(scene, (vector_t) {2750, 160}, (vector_t) {2857, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2856, 303}, (vector_t) {3142, 339}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {3249, 303}, (vector_t) {3392, 338}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {3357, 160}, (vector_t) {3392, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {3571, 160}, (vector_t) {3641, 195}, REGULAR_BLOCK, NULL);
    //lots of power blocks
    add_platform_corners(scene, (vector_t) {3786, 160}, (vector_t) {3819, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {3892, 160}, (vector_t) {3927, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4000, 160}, (vector_t) {4033, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {3892, 304}, (vector_t) {3926, 337}, REGULAR_BLOCK, NULL);
    //Blocks after lots of powerups
    add_platform_corners(scene, (vector_t) {4213, 160}, (vector_t) {4249, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4320, 304}, (vector_t) {4427, 338}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4570, 304}, (vector_t) {4714, 339}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4606, 160}, (vector_t) {4678, 196}, REGULAR_BLOCK, NULL);
    //stairs 1
    add_platform_corners(scene, (vector_t) {4784, 54}, (vector_t) {4927, 90}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4821, 90}, (vector_t) {4927, 124}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4857, 124}, (vector_t) {4927, 160}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4891, 160}, (vector_t) {4927, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4998, 54}, (vector_t) {5142, 90}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4998, 90}, (vector_t) {5106, 124}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4998, 124}, (vector_t) {5070, 160}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4998, 160}, (vector_t) {5034, 196}, REGULAR_BLOCK, NULL);
    //stairs 2
    add_platform_corners(scene, (vector_t) {5283, 54}, (vector_t) {5462, 90}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5320, 90}, (vector_t) {5462, 124}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5356, 124}, (vector_t) {5462, 160}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5391, 160}, (vector_t) {5462, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5536, 54}, (vector_t) {5678, 90}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5536, 90}, (vector_t) {5641, 124}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5536, 124}, (vector_t) {5606, 160}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5536, 160}, (vector_t) {5571, 196}, REGULAR_BLOCK, NULL);
    //Final pipe thingy
    add_platform_corners(scene, (vector_t) {5821, 54}, (vector_t) {5891, 124}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {6000, 160}, (vector_t) {6142, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {6394, 54}, (vector_t) {6463, 124}, REGULAR_BLOCK, NULL);

    //Item Blocks
    subrole_t* powerup1 = malloc(sizeof(subrole_t));
    *powerup1 = STAR_POWERUP;
    add_platform_corners(scene, (vector_t) {572, 158}, (vector_t) {606, 198}, ITEM_BLOCK, powerup1);
    subrole_t* powerup2 = malloc(sizeof(subrole_t));
    *powerup2 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {750, 158}, (vector_t) {785, 198}, ITEM_BLOCK, powerup2);
    subrole_t* powerup3 = malloc(sizeof(subrole_t));
    *powerup3 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {822, 158}, (vector_t) {856, 198}, ITEM_BLOCK, powerup3);
    subrole_t* powerup4 = malloc(sizeof(subrole_t));
    *powerup4 = FIRE_POWERUP;
    add_platform_corners(scene, (vector_t) {786, 299}, (vector_t) {822, 340}, ITEM_BLOCK, powerup4);
    subrole_t* powerup5 = malloc(sizeof(subrole_t));
    *powerup5 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {2286, 194}, (vector_t) {2319, 234}, ITEM_BLOCK, powerup5);
    subrole_t* powerup6 = malloc(sizeof(subrole_t));
    *powerup6 = FIRE_POWERUP;
    add_platform_corners(scene, (vector_t) {2789, 158}, (vector_t) {2820, 198}, ITEM_BLOCK, powerup6);
    subrole_t* powerup7 = malloc(sizeof(subrole_t));
    *powerup7 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {3359, 300}, (vector_t) {3390, 340}, ITEM_BLOCK, powerup7);
    subrole_t* powerup8 = malloc(sizeof(subrole_t));
    *powerup8 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {1537, 411}, (vector_t) {1570, 445}, ITEM_BLOCK, powerup8);
    subrole_t* powerup9 = malloc(sizeof(subrole_t));
    *powerup9 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {1537, 411}, (vector_t) {1570, 445}, ITEM_BLOCK, powerup9);
    subrole_t* powerup10 = malloc(sizeof(subrole_t));
    *powerup10 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {1537, 411}, (vector_t) {1570, 445}, ITEM_BLOCK, powerup10);


    //Enemies
    add_enemy(scene, (vector_t) {523, 80}, KOOPA);
    add_enemy(scene, (vector_t) {650, 80}, GOOMBA);
    add_enemy(scene, (vector_t) {1208, 80}, GOOMBA);
    add_enemy(scene, (vector_t) {1535, 80}, GOOMBA);
    add_enemy(scene, (vector_t) {1875, 80}, KOOPA);
    add_enemy(scene, (vector_t) {2818, 80}, GOOMBA);
    add_enemy(scene, (vector_t) {3900, 80}, KOOPA);
    add_enemy(scene, (vector_t) {3950, 80}, KOOPA);
    add_enemy(scene, (vector_t) {4000, 80}, KOOPA);
    add_enemy(scene, (vector_t) {4050, 80}, KOOPA);
    add_enemy(scene, (vector_t) {4964, 80}, KOOPA);
    add_enemy(scene, (vector_t) {5216, 80}, GOOMBA);
    add_enemy(scene, (vector_t) {6112, 80}, KOOPA);

    //Tokens
    add_token(scene, (vector_t) {730, 230});
    add_token(scene, (vector_t) {780, 230});
    add_token(scene, (vector_t) {310, 230});
    add_token(scene, (vector_t) {340, 230});
    add_token(scene, (vector_t) {1774, 85});
    add_token(scene, (vector_t) {1825, 85});
    add_token(scene, (vector_t) {2900, 760});
    add_token(scene, (vector_t) {2950, 760});
    add_token(scene, (vector_t) {3000, 760});
    add_token(scene, (vector_t) {3050, 760});
    add_token(scene, (vector_t) {3100, 760});
    add_token(scene, (vector_t) {3802, 218});
    add_token(scene, (vector_t) {3910, 218});
    add_token(scene, (vector_t) {4018, 218});
    add_token(scene, (vector_t) {3908, 360});
    add_token(scene, (vector_t) {6122, 224});
}

void world1_level2(scene_t* scene)
{
    add_background(scene, (vector_t) {6396, 500}, LEVEL2);
    level_borders(scene, (vector_t) {6396, 500});
    add_platform_corners(scene, (vector_t) {0, 0}, (vector_t) {795, 66}, REGULAR_BLOCK, NULL);

    add_platform_corners(scene, (vector_t) {0, 0}, (vector_t) {6396, 30}, DEATH_BLOCK, NULL);

    tunnel_t* tunnel = tunnel_init(false, true, false, false, 1.21, (vector_t) {1000, 150});
    add_platform_corners(scene, (vector_t) {659, 67}, (vector_t) {736, 130}, TUNNEL_BLOCK, tunnel);
    add_platform_corners(scene, (vector_t) {728, 67}, (vector_t) {792, 199}, REGULAR_BLOCK, NULL);

    add_platform_corners(scene, (vector_t) {4996, 0}, (vector_t) {6396, 66}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5807, 67}, (vector_t) {5876, 198}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5868, 67}, (vector_t) {5941, 130}, REGULAR_BLOCK, NULL);
}

void world1_level2_sublevel1(scene_t* scene)
{
    add_background(scene, (vector_t) {6000, 500}, LEVEL2_1);
    level_borders(scene, (vector_t) {6000, 500});
    add_platform_corners(scene, (vector_t) {0, 0}, (vector_t) {6000, 30}, DEATH_BLOCK, NULL);
    // Ground Platforms
    add_platform_corners(scene, (vector_t) {0, 0}, (vector_t) {2856, 54}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2963, 0}, (vector_t) {4285, 53}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4356, 0}, (vector_t) {4429, 54}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4500, 0}, (vector_t) {4929, 54}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5178, 0}, (vector_t) {5464, 54}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5713, 0}, (vector_t) {6821, 54}, REGULAR_BLOCK, NULL);

    //Left border
    add_platform_corners(scene, (vector_t) {0, 55}, (vector_t) {36, 446}, REGULAR_BLOCK, NULL);

    //Top border
    add_platform_corners(scene, (vector_t) {213, 412}, (vector_t) {4929, 446}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5749, 412}, (vector_t) {6000, 446}, REGULAR_BLOCK, NULL);

    //power blocks
    subrole_t* powerup1 = malloc(sizeof(subrole_t));
    *powerup1 = FIRE_POWERUP;
    add_platform_corners(scene, (vector_t) {356, 160}, (vector_t) {392, 196}, ITEM_BLOCK, powerup1); //Shroom
    subrole_t* powerup2 = malloc(sizeof(subrole_t));
    *powerup2 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {392, 160}, (vector_t) {428, 196}, ITEM_BLOCK, powerup2); //Token for next 4
    subrole_t* powerup3 = malloc(sizeof(subrole_t));
    *powerup3 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {428, 160}, (vector_t) {464, 196}, ITEM_BLOCK, powerup3);
    subrole_t* powerup4 = malloc(sizeof(subrole_t));
    *powerup4 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {464, 160}, (vector_t) {500, 196}, ITEM_BLOCK, powerup4);
    subrole_t* powerup5 = malloc(sizeof(subrole_t));
    *powerup5 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {500, 160}, (vector_t) {536, 196}, ITEM_BLOCK, powerup5);
    subrole_t* powerup6 = malloc(sizeof(subrole_t));
    *powerup6 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {1037, 196}, (vector_t) {1069, 230}, ITEM_BLOCK, powerup6); //Token
    subrole_t* powerup7 = malloc(sizeof(subrole_t));
    *powerup7 = STAR_POWERUP;
    add_platform_corners(scene, (vector_t) {1643, 223}, (vector_t) {1676, 265}, ITEM_BLOCK, powerup7); //Star or shroom
    subrole_t* powerup8 = malloc(sizeof(subrole_t));
    *powerup8 = FIRE_POWERUP;
    add_platform_corners(scene, (vector_t) {2465, 196}, (vector_t) {2498, 230}, ITEM_BLOCK, powerup8); //Shroom
    subrole_t* powerup9 = malloc(sizeof(subrole_t));
    *powerup9 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {2609, 196}, (vector_t) {2641, 230}, ITEM_BLOCK, powerup9); //Coins for rest
    subrole_t* powerup10 = malloc(sizeof(subrole_t));
    *powerup10 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {3180, 412}, (vector_t) {3213, 445}, ITEM_BLOCK, powerup10);
    subrole_t* powerup11 = malloc(sizeof(subrole_t));
    *powerup11 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {5356, 196}, (vector_t) {5391, 232}, ITEM_BLOCK, powerup11);

    //Obstacles
    //Stair thingy
    add_platform_corners(scene, (vector_t) {606, 54}, (vector_t) {642, 89}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {678, 54}, (vector_t) {713, 124}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {748, 54}, (vector_t) {785, 160}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {820, 54}, (vector_t) {855, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {892, 54}, (vector_t) {928, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {963, 54}, (vector_t) {1000, 160}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1106, 54}, (vector_t) {1142, 160}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1177, 55}, (vector_t) {1214, 126}, REGULAR_BLOCK, NULL);
    //U thingy
    add_platform_corners(scene, (vector_t) {1391, 160}, (vector_t) {1427, 268}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1427, 160}, (vector_t) {1500, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1463, 197}, (vector_t) {1498, 268}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1499, 233}, (vector_t) {1607, 268}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1570, 160}, (vector_t) {1607, 233}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1642, 196}, (vector_t) {1678, 233}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1605, 162}, (vector_t) {1678, 197}, REGULAR_BLOCK, NULL);
    //thing after u thingy thingy
    add_platform_corners(scene, (vector_t) {1856, 160}, (vector_t) {1928, 340}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1928, 91}, (vector_t) {2000, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1927, 340}, (vector_t) {2000, 411}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2070, 160}, (vector_t) {2285, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2213, 196}, (vector_t) {2285, 340}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2070, 340}, (vector_t) {2285, 411}, REGULAR_BLOCK, NULL);
    //J looking thingy
    add_platform_corners(scene, (vector_t) {2356, 340}, (vector_t) {2501, 411}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2391, 160}, (vector_t) {2429, 340}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2391, 160}, (vector_t) {2500, 196}, REGULAR_BLOCK, NULL);
    //2 things after j looking thingy thingy
    add_platform_corners(scene, (vector_t) {2570, 160}, (vector_t) {2644, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2570, 196}, (vector_t) {2607, 233}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2570, 234}, (vector_t) {2644, 339}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2713, 160}, (vector_t) {2858, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2712, 340}, (vector_t) {2858, 411}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {3000, 196}, (vector_t) {3214, 268}, REGULAR_BLOCK, NULL);
    //Random block
    add_platform_corners(scene, (vector_t) {4355, 54}, (vector_t) {4428, 160}, REGULAR_BLOCK, NULL);
    //Stircase
    add_platform_corners(scene, (vector_t) {4748, 54}, (vector_t) {4929, 90}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4784, 90}, (vector_t) {4929, 126}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4820, 126}, (vector_t) {4929, 160}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4856, 160}, (vector_t) {4929, 196}, REGULAR_BLOCK, NULL);
    //Thingy thats supposed to go up
    add_platform_corners(scene, (vector_t) {4990, 129}, (vector_t) {5099, 147}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4989, 341}, (vector_t) {5099, 360}, REGULAR_BLOCK, NULL);
    //Thingy between 2 up thingys
    add_platform_corners(scene, (vector_t) {5177, 196}, (vector_t) {5357, 232}, REGULAR_BLOCK, NULL);
    //More up thingys
    add_platform_corners(scene, (vector_t) {5525, 44}, (vector_t) {5634, 64}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5525, 330}, (vector_t) {5635, 348}, REGULAR_BLOCK, NULL);

    //pipes
    add_platform_corners(scene, (vector_t) {3680, 54}, (vector_t) {3750, 160}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {3895, 54}, (vector_t) {3963, 195}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4107, 54}, (vector_t) {4178, 123}, REGULAR_BLOCK, NULL);

    //Enemies
    add_enemy(scene, (vector_t) {492, 77}, GOOMBA);
    add_enemy(scene, (vector_t) {528, 77}, GOOMBA);
    add_enemy(scene, (vector_t) {1038, 77}, GOOMBA);
    add_enemy(scene, (vector_t) {1486, 77}, KOOPA);
    add_enemy(scene, (vector_t) {1545, 77}, KOOPA);
    add_enemy(scene, (vector_t) {1884, 77}, KOOPA);
    add_enemy(scene, (vector_t) {2141, 77}, GOOMBA);
    add_enemy(scene, (vector_t) {2248, 77}, GOOMBA);
    add_enemy(scene, (vector_t) {2605, 358}, GOOMBA);
    add_enemy(scene, (vector_t) {2821, 213}, GOOMBA);
    add_enemy(scene, (vector_t) {2751, 213}, GOOMBA);
    add_enemy(scene, (vector_t) {3437, 77}, GOOMBA);
    add_enemy(scene, (vector_t) {3500, 77}, GOOMBA);
    add_enemy(scene, (vector_t) {3560, 77}, GOOMBA);
    //Supposed to be plants
    add_enemy(scene, (vector_t) {3713, 189}, GOOMBA);
    add_enemy(scene, (vector_t) {3929, 220}, GOOMBA);
    add_enemy(scene, (vector_t) {4143, 153}, GOOMBA);
    //More stuff
    add_enemy(scene, (vector_t) {4033, 77}, GOOMBA);
    add_enemy(scene, (vector_t) {4875, 214}, GOOMBA);
    add_enemy(scene, (vector_t) {4758, 111}, GOOMBA);
    add_enemy(scene, (vector_t) {5320, 77}, KOOPA);

    //Coins
    add_token(scene, (vector_t) {1443, 213});
    add_token(scene, (vector_t) {1622, 213});
    add_token(scene, (vector_t) {1480, 320});
    add_token(scene, (vector_t) {1515, 320});
    add_token(scene, (vector_t) {1550, 320});
    add_token(scene, (vector_t) {1585, 320});
    add_token(scene, (vector_t) {1622, 213});
    add_token(scene, (vector_t) {2085, 213});
    add_token(scene, (vector_t) {2120, 213});
    add_token(scene, (vector_t) {2155, 213});
    add_token(scene, (vector_t) {2190, 213});
    add_token(scene, (vector_t) {2444, 213});
    add_token(scene, (vector_t) {3015, 320});
    add_token(scene, (vector_t) {3050, 320});
    add_token(scene, (vector_t) {3085, 320});
    add_token(scene, (vector_t) {3120, 320});
    add_token(scene, (vector_t) {3155, 320});
    add_token(scene, (vector_t) {3190, 320});
}

void world1_level3(scene_t* scene)
{
    add_background(scene, (vector_t) {5696, 500}, LEVEL3);
    level_borders(scene, (vector_t) {5696, 500});
    add_platform_corners(scene, (vector_t) {0, 0}, (vector_t) {5696, 30}, DEATH_BLOCK, NULL);

    //Ground Platforms
    add_platform_corners(scene, (vector_t) {0, 0}, (vector_t) {534, 54}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {5035, 0}, (vector_t) {5696, 54}, REGULAR_BLOCK, NULL);

    //Obstacles
    add_platform_corners(scene, (vector_t) {573, 18}, (vector_t) {749, 51}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {679, 304}, (vector_t) {855, 338}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {823, 161}, (vector_t) {1071, 195}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1144, 341}, (vector_t) {1249, 373}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1287, 54}, (vector_t) {1534, 87}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1394, 304}, (vector_t) {1570, 338}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1572, 161}, (vector_t) {1677, 194}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1822, 197}, (vector_t) {1928, 230}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2070, 288}, (vector_t) {2178, 305}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2213, 330}, (vector_t) {2322, 348}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2322, 18}, (vector_t) {2498, 51}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2392, 232}, (vector_t) {2498, 266}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2500, 376}, (vector_t) {2606, 408}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2572, 160}, (vector_t) {2677, 194}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2644, 304}, (vector_t) {2820, 338}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {3001, 160}, (vector_t) {3106, 194}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {3536, 89}, (vector_t) {3641, 124}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {3750, 125}, (vector_t) {3855, 159}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4036, 160}, (vector_t) {4213, 196}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4180, 268}, (vector_t) {4284, 302}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4322, 18}, (vector_t) {4570, 52}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4643, 125}, (vector_t) {4821, 159}, REGULAR_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4855, 332}, (vector_t) {4964, 351}, REGULAR_BLOCK, NULL);

    //Pipe
    tunnel_t* tunnel = tunnel_init(false, false, true, false, 1.31, VEC_ZERO);
    add_platform_corners(scene, (vector_t) {2152, 424}, (vector_t) {2218, 499}, TUNNEL_BLOCK, tunnel);

    //Item Blocks
    subrole_t* powerup1 = malloc(sizeof(subrole_t));
    *powerup1 = FIRE_POWERUP;
    add_platform_corners(scene, (vector_t) {1537, 411}, (vector_t) {1570, 445}, ITEM_BLOCK, powerup1);
    add_platform_corners(scene, (vector_t) {1558, 412}, (vector_t) {1569, 444}, REGULAR_BLOCK, NULL);
    subrole_t* powerup2 = malloc(sizeof(subrole_t));
    *powerup2 = FIRE_POWERUP;
    add_platform_corners(scene, (vector_t) {2160, 33}, (vector_t) {2194, 66}, ITEM_BLOCK, powerup2);
    add_platform_corners(scene, (vector_t) {2161, 34}, (vector_t) {2193, 65}, REGULAR_BLOCK, NULL);
    subrole_t* powerup3 = malloc(sizeof(subrole_t));
    *powerup3 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {2160, 196}, (vector_t) {2195, 230}, ITEM_BLOCK, powerup3);
    add_platform_corners(scene, (vector_t) {2161, 197}, (vector_t) {2194, 229}, REGULAR_BLOCK, NULL);
    subrole_t* powerup4 = malloc(sizeof(subrole_t));
    *powerup3 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {3362, 266}, (vector_t) {3396, 300}, ITEM_BLOCK, powerup4);
    add_platform_corners(scene, (vector_t) {3363, 267}, (vector_t) {3395, 299}, REGULAR_BLOCK, NULL);
    subrole_t* powerup5 = malloc(sizeof(subrole_t));
    *powerup3 = TOKEN_POWERUP;
    add_platform_corners(scene, (vector_t) {4960, 464}, (vector_t) {4994, 498}, ITEM_BLOCK, powerup5);
    add_platform_corners(scene, (vector_t) {4961, 465}, (vector_t) {4993, 497}, REGULAR_BLOCK, NULL);

    //Enemies
    add_enemy(scene, (vector_t) {662, 83}, KOOPA);
    add_platform_corners(scene, (vector_t) {574, 60}, (vector_t) {575, 70}, INVISIBLE_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {740, 60}, (vector_t) {741, 70}, INVISIBLE_BLOCK, NULL);
    add_enemy(scene, (vector_t) {944, 209}, KOOPA);
    add_platform_corners(scene, (vector_t) {826, 200}, (vector_t) {827, 220}, INVISIBLE_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1062, 200}, (vector_t) {1063, 220}, INVISIBLE_BLOCK, NULL);
    add_enemy(scene, (vector_t) {1874, 245}, KOOPA);
    add_platform_corners(scene, (vector_t) {1822, 230}, (vector_t) {1823, 250}, INVISIBLE_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1926, 230}, (vector_t) {1927, 250}, INVISIBLE_BLOCK, NULL);
    add_enemy(scene, (vector_t) {2737, 376}, KOOPA);
    add_platform_corners(scene, (vector_t) {2646, 340}, (vector_t) {2647, 360}, INVISIBLE_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {2818, 340}, (vector_t) {2819, 360}, INVISIBLE_BLOCK, NULL);
    add_enemy(scene, (vector_t) {3591, 154}, KOOPA);
    add_platform_corners(scene, (vector_t) {1540, 120}, (vector_t) {1541, 140}, INVISIBLE_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {1636, 120}, (vector_t) {1637, 140}, INVISIBLE_BLOCK, NULL);
    add_enemy(scene, (vector_t) {4234, 345}, KOOPA);
    add_platform_corners(scene, (vector_t) {4182, 300}, (vector_t) {4183, 320}, INVISIBLE_BLOCK, NULL);
    add_platform_corners(scene, (vector_t) {4280, 300}, (vector_t) {4281, 320}, INVISIBLE_BLOCK, NULL);

    //Tokens
    add_token(scene, (vector_t) {698, 372});
    add_token(scene, (vector_t) {750, 372});
    add_token(scene, (vector_t) {800, 372});
    add_token(scene, (vector_t) {1195, 408});
    add_token(scene, (vector_t) {1624, 230});
    add_token(scene, (vector_t) {2268, 380});
    add_token(scene, (vector_t) {3055, 236});
    add_token(scene, (vector_t) {3805, 198});
    add_token(scene, (vector_t) {4350, 94});
    add_token(scene, (vector_t) {4400, 94});
    add_token(scene, (vector_t) {4450, 94});
    add_token(scene, (vector_t) {4500, 94});
    add_token(scene, (vector_t) {4550, 94});


}

void world1_level3_sublevel1(scene_t* scene)
{
    add_background(scene, (vector_t) {1500, 500}, LEVEL3_1);
    level_borders(scene, (vector_t) {1500, 500});

    //Ground
    add_platform_corners(scene, (vector_t) {0, 0}, (vector_t) {1320, 54}, REGULAR_BLOCK, NULL);

    //Enemies
    for(int i = 0; i < 35; i++)
    {
        add_enemy(scene, (vector_t) {50 + 30 * i , 80}, GOOMBA);
    }
}
