#include "leaderboard.h"
#include "menu.h"
#include "sdl_wrapper.h"

// Only top 10 displayed on leaderboard
const size_t DISPLAY_SIZE = 10;
const size_t MAX_SIZE = 100;

extern const rgb_color_t ORANGE;
extern const rgb_color_t BLUE;

// Vector positions that each line is rendered at
extern const vector_t LINE1;
extern const vector_t LINE2;
// Increaments position of each line by 25;
extern const double LINE_INC;
// Max character count for name;
double MAX_NAME = 25;
// Max character count for a string
double MAX_LENGTH = 100;

typedef struct entry
{
    char* name;
    size_t tokens;
    size_t kills;

} entry_t;

// Creates list with leaderboard information
list_t* leaderboard_init()
{
    FILE* file = fopen("resources/leaderboard.txt", "r");

    if (file == NULL)
    {
        printf("leaderboard.txt file not found.");
        exit(1);
    }

    list_t* leaderboard = list_init(DISPLAY_SIZE, free);

    // Reads leaderboard line by line until reaching end of function.
    
    size_t* tokens = malloc(sizeof(size_t));
    size_t* kills = malloc(sizeof(size_t));
    
    // CHECK ARGUMENTS; not sure if we shouls be passing by reference.
    while(true)
    {
        char* name = malloc(sizeof(char*) * 100);
        if(fscanf(file, "%s %zu %zu", name, tokens, kills) == EOF) {break;}
        // Adds all info of each row to leaderboard list
        entry_t* entry = malloc(sizeof(entry_t));
        *entry = (entry_t) {name, *tokens, *kills};  
        list_add(leaderboard, entry);
    }
    fclose(file);
    
    return leaderboard;
}


// Saves the leaderboard struct to leaderboard.txt. Returns 1 upon failure and 
// 0 upon success. 
int leaderboard_save(list_t* leaderboard)
{
    FILE *file = fopen("resources/leaderboard.txt", "w");
    if (file == NULL)
    {
        printf("Failure to open leaderboard.txt\n");
        return 1;
    }

    // Writes everything into the leaderboard.txt.
    // Is this adding lines or rewriting the whole thing?
    for (size_t i = 0; i < list_size(leaderboard); i++)
    {
        // Gets each row from list from first to last
        entry_t* entry = (entry_t*) list_get(leaderboard, i);

        // Does this automatically end EOL characters? 
        fprintf(file, "%s %zu %zu\n", entry -> name, entry -> tokens, entry -> kills);
    }
    
    fclose(file);
    return 0;
}


// Sequence to display the leaderboard.
bool leaderboard_show()
{
    list_t* leaderboard = leaderboard_init();
    // Prints the header.
    sdl_write_title("    NAME     TOKENS     KILLS", ORANGE, (vector_t) LINE1);

    for (size_t i = 0; i < list_size(leaderboard); i ++)
    {
        // Prints leaderboard in command line.
        entry_t entry = *(entry_t*) list_get(leaderboard, i);
        // First field is the place, leaderboard is stored in order.
        char* str = malloc(sizeof(char) * 30);
        str[0] = '\0';
        sprintf(str, "%lu    %s    %zu    %zu", (i + 1), entry.name, entry.tokens, entry.kills);
        sdl_write_title(str, ORANGE, (vector_t) {LINE2.x, LINE2.y + i * LINE_INC});
    }
    sdl_write_title("[enter or esc to return to main menu]", ORANGE, (vector_t) {LINE1.x, 50});
    list_free(leaderboard);
    return true;
}


// Sorts the leaderboard according to collected tokens.
list_t* leaderboard_sort(list_t* leaderboard)
{
    void** arr = list_arr(leaderboard);
    size_t select; 

    size_t size = list_size(leaderboard);
    for (size_t i = 0; i < size; i ++)
    {
        // The index of the selected element.
        select = i;
        // Find the smallest (or largest) element in the remaining array.
        for (size_t j = i + 1; j < size; j ++)
        {
            // The index of the "selected" element.
            int select = i;
            entry_t *entry1 = (entry_t*) arr[i];
            entry_t *entry2 = (entry_t*) arr[j];

            if ((int) entry2 -> tokens - (int) entry1 -> tokens > 0)
            {
                select = j;
            }
            // Now that we have the location of the largest element, swap the
            // values at i and the selected location.
            void *temp = arr[i];
            arr[i] = arr[select];
            arr[select] = temp;
        }
    }
    // Turn the sorted array back into a list and update the field in 
    // leaderboard.
    list_t* old_entries = leaderboard;
    list_t* new_entries = list_init(size, free);
    for (size_t i = 0; i < size; i ++) {list_add(new_entries, (entry_t*) arr[i]);}

    // Make sure to not free the entries being pointed to by the list!
    free(list_arr(old_entries));
    free(old_entries);

    return new_entries;
}


// Updates leaderboard with a new finisher.
void leaderboard_update(entry_t* entry)
{ 
    list_t* leaderboard = leaderboard_init();
    list_add(leaderboard, entry);
 
    leaderboard = leaderboard_sort(leaderboard);
    
    size_t size = list_size(leaderboard);
    // If the leaderboard exceeds the maximum size, the last entry is removed.
    if (size > DISPLAY_SIZE) 
    {
        list_remove(leaderboard, size - 1);
    }
    if(leaderboard_save(leaderboard) != 0) {printf("Leaderboard save error.");}
    
    list_free(leaderboard);
}

// Entry accessors --------------------------------------------------------------------------------
entry_t* entry_init(char* name)
{
    entry_t* entry = malloc(sizeof(entry_t));
    entry -> name = name;
    entry -> tokens = 0;
    entry -> kills = 0;
    return entry;
}

size_t entry_tokens(entry_t* entry)
{
    return entry -> tokens;
}

size_t entry_kills(entry_t* entry)
{
    return entry -> kills;
}

char* entry_name(entry_t* entry)
{
    return entry -> name;
}

void entry_set_kills(entry_t* entry, size_t kills)
{
    assert(entry != NULL);
    entry -> kills = kills;
}

void entry_set_tokens(entry_t* entry, size_t tokens)
{
    assert(entry != NULL);
    entry -> tokens = tokens;
}

void entry_set_name(entry_t* entry, char* name)
{
    entry -> name = name;
}
