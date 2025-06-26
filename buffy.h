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
};

struct vampire
{
    int age;
    char *name;
    char *species;
    struct vampire_fangs
    {
        int length;
        int sharpness;
        char *color;
        int health; // health of the fangs from 0 to 10
    } fangs[3];
};

int save_game(char *file);
int load_game(char *file);


// Define default values as constants
#define DEFAULT_DAGGERSET 0
#define DEFAULT_FLOURIDE 200
#define DEFAULT_DAGGER_DIP 10
#define DEFAULT_DAGGER_EFFORT 5
#define DEFAULT_FLOURIDE_USED 0
#define DEFAULT_SCORE 0
#define DEFAULT_TURNS 0

#define MAX_HEALTH 100

#define BONUS_ALL_HEALTH 100


#define DEFAULT_VAMPIRE_AGE 100 // Default age for the vampire
#define DEFAULT_VAMPIRE_NAME "Dracula"
#define DEFAULT_VAMPIRE_SPECIES "Vampire"

#endif // BUFFY_H
