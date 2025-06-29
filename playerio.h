#ifndef PLAYERIO_H
#define PLAYERIO_H

#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>



void get_input(const char *prompt, char *buffer, size_t size);
void my_printf(const char *format, ...);
void mv_printw(int row, int col, const char *format, ...);
void initalize_curses(void);
int end_curses(void);
#endif
