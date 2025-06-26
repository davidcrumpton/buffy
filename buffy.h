// Compiler: This project is compiled with clang version 17.0.0.
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
    char *character_name; // Name of the character, e.g., "Buffy"
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
        int health; // health of the fangs from 0 to 10
    } fangs[3];
};

int save_game(char *file);
int load_game(char *file);

#define DEFAULT_CHARACTER_NAME "Buffy"
// Define default values as constants
#define DEFAULT_DAGGERSET 0
#define DEFAULT_FLOURIDE 200
#define DEFAULT_DAGGER_DIP 10
#define DEFAULT_DAGGER_EFFORT 5
#define DEFAULT_FLOURIDE_USED 0
#define DEFAULT_SCORE 10
#define DEFAULT_TURNS 0

#define MAX_HEALTH 100

#define BONUS_ALL_HEALTH 100
#define BONUS_FANG_CLEANED 1
#define BONUS_FANG_HEALTH 3


#define DEFAULT_CREATURE_AGE 100 // Default age for the creature
#define DEFAULT_CREATURE_NAME "Dracula"
#define DEFAULT_CREATURE_SPECIES "Vampire"

#endif // BUFFY_H
