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
} game_state;

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
} vampire;

int save_game(char *file);
int load_game(char *file);
#endif // BUFFY_H
