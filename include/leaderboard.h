#include <stdlib.h>
#include <stdio.h>

#include "controls.h"
#include "sdl_wrapper.h"
#include "list.h"
#include "scene.h"
#include "vector.h"
#include "color.h"

// Struct for one entry of a leaderboard. This should not need to be used 
// outside of this file.
typedef struct entry entry_t;

list_t* leaderboard_init();

// Saves the leaderboard struct to leaderboard.txt. Returns 1 upon failure and 
// 0 upon success. 
int leaderboard_save(list_t* leaderboard);

// Displays the leaderboard.
bool leaderboard_show();
// Sorts the leaderboard according to collected tokens.
list_t* leaderboard_sort(list_t* leaderboard);

// Updates leaderboard with a new finisher.
void leaderboard_update(entry_t* entry);

// Entry struct accessors
entry_t* entry_init(char* name);
size_t entry_tokens(entry_t* entry);
size_t entry_kills(entry_t* entry);
char* entry_name(entry_t* entry);

void entry_set_kills(entry_t* entry, size_t kills);
void entry_set_tokens(entry_t* entry, size_t tokens);
void entry_set_name(entry_t* entry, char* name);