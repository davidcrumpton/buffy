/* Compiler: This project is compiled with clang version 17.0.0. */
#ifndef BUFFY_H
#define BUFFY_H

struct game_state
{
    int daggerset;
    int flouride;
    int dagger_dip;
    int dagger_effort;
    int flouride_used;
    int bflag;
    int score;
    int turns;
    int tool_in_use; /* Index of the tool currently in use */
    char *character_name; /* Name of the character, e.g., "Buffy" */
};

struct creature
{
    int age;
    char *name;
    char *species;
    struct creature_fangs
    {
        int length;
        int sharpness;
        char *color;
        int health; /* health of the fangs from 0 to 10 */
    } fangs[4];
};



int save_game(char *file);
int load_game(char *file);

#define DEFAULT_CHARACTER_NAME "Buffy"
/* Define default values as constants */
#define DEFAULT_DAGGERSET       0
#define DEFAULT_FLOURIDE        200
#define DEFAULT_DAGGER_DIP      10
#define DEFAULT_DAGGER_EFFORT    5
#define DEFAULT_FLOURIDE_USED    0
#define DEFAULT_SCORE           10
#define DEFAULT_TURNS           0

#define MAX_HEALTH              100

#define BONUS_ALL_HEALTH        100
#define BONUS_FANG_CLEANED      1
#define BONUS_FANG_HEALTH       3

#define FANG_COLOR_HIGH          "white"
#define FANG_COLOR_MEDIUM        "dull"
#define FANG_COLOR_LOW           "yellow"

#define DEFAULT_CREATURE_AGE         100 /* Default age for the creature */
#define DEFAULT_CREATURE_NAME       "Dracula"
#define DEFAULT_CREATURE_SPECIES        "Vampire"

/* We need a structure to hold tools such as daggers and other instruments 
   instead of hard coding the dagger.  The dagger is only used when 
   daggerset is defined so we need a non dagger tooth cleaner.  
   
   The struct should hold values that alter the how the tool works
   with effort values and dip amount. It should describe the tool
   by name and description and include the dagger length. */

typedef struct tool
{
    char *name;          /* Name of the tool, e.g., "Dagger" */
    char *description;   /* Description of the tool */
    int length;         /* Length of the tool, e.g., 10 for a dagger */
    int dip_amount;     /* Amount of fluoride the tool can hold */
    int effort;         /* Effort required to use the tool */

    int effectiveness;  /* Effectiveness of the tool, e.g., 5 for a dagger */
    int durability;     /* Durability of the tool, e.g., 100 for a dagger */
    int used;           /* Amount of fluoride used with the tool */
} tool;

#endif /* BUFFY_H */
