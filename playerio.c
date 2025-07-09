/*
 * BSD Zero Clause License
 *
 * Copyright (c) 2025 David M Crumpton david.m.crumpton [at] gmail [dot] com
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/*
 * playerio.c: Provides functions for input/output operations in the Buffy game.
 *
 */

#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include "playerio.h"
#include "buffy.h"

static int using_curses = 0;
static int color_mode = 0;	

static WINDOW *game_win = NULL;
static WINDOW *stats_win = NULL;
static WINDOW *err_win = NULL; 

void my_werase()
{
	if(using_curses)
		werase(game_win);
}
void my_clear()
{
    if(using_curses)
        wclear(game_win);

}
void
my_refresh()
{
    if(using_curses)
        wrefresh(game_win);
}
void
set_using_curses(int flag)
{
	using_curses = flag;
}

void
set_color_mode(int flag)
{
	color_mode = flag;	
}
void
get_input(const char *prompt, char *buffer, size_t size)
{
    if (using_curses) {
        int prompt_row = LINES - 2; 

        mvwprintw(game_win, prompt_row, 0, "%*s", COLS, " "); 
        mvwprintw(game_win, prompt_row, 0, "%s", prompt);
        wrefresh(game_win); 

        wmove(game_win, prompt_row, strlen(prompt));
        curs_set(1); 
        wgetnstr(game_win, buffer, size - 1); 

        curs_set(0); 
        mvwprintw(game_win, prompt_row, 0, "%*s", COLS, " ");
        wrefresh(game_win); 
    } else {
        printf("%s", prompt);
        fflush(stdout);
        fgets(buffer, size, stdin);
    }
}



void
my_print_err(const char *format,...)
{
	va_list		args;
	va_start(args, format);
	if (using_curses) {
		wattron(err_win, A_BOLD | COLOR_PAIR(1));	
		mvwprintw(err_win, 1, 1, format, args);
		wattroff(err_win, A_BOLD | COLOR_PAIR(1));
		wrefresh(err_win);
		delwin(err_win);
	} else {
		vfprintf(stderr, format, args);
		fflush(stderr);
	}
	va_end(args);
}

void
my_printf(const char *format,...)
{
	va_list		args;
	va_start(args, format);
	if (using_curses) {
		vw_printw(game_win, format, args);
	} else {
		vprintf(format, args);
	}
	va_end(args);
}

void
mv_printw(int row, int col, const char *format,...)
{
	va_list		args;
	va_start(args, format);
	if (using_curses) {
		move(row, col);	
		vw_printw(game_win, format, args);
	} else {
		vprintf(format, args);
	}
	va_end(args);
}


void
my_putchar(char c)
{
	if (using_curses) {
		wmove(game_win, 0, 0);	/* top left */
		waddch(game_win, c);	/* add the character at the current
					 * cursor position */
		wrefresh(game_win);
	} else {
		putchar(c);	
		fflush(stdout);	
	}
}

void
update_stats_display(int fluoride_level, int score, int turns)
{
    if (!using_curses || !stats_win) { 
        printf("Fluoride: %d, Score: %d, Turns: %d\n", fluoride_level, score, turns);
        return;
    }

    werase(stats_win); 

    mvwprintw(stats_win, 0, 1, "Fluoride: %d", fluoride_level);
    mvwprintw(stats_win, 0, COLS / 3, "Score: %d", score);
    mvwprintw(stats_win, 0, (COLS * 2) / 3, "turns: %d", turns);

    wrefresh(stats_win);
}


void
initalize_curses(void)
{
    if(!using_curses)
        return;
	if (using_curses) {
		initscr();
	}
	if (using_curses && color_mode && has_colors()) {
		start_color();	
		init_pair(1, COLOR_RED, COLOR_BLACK);	/* red and black is a friend
			of Jack */
	       	init_pair(2, COLOR_BLUE, COLOR_BLACK); /* status */
	       	init_pair(3, COLOR_WHITE, COLOR_BLACK); /* status */
	}
    int game_win_height = LINES - 1; 
    int game_win_width = COLS;

    game_win = newwin(game_win_height, game_win_width, 0, 0);
    stats_win = newwin(1, COLS, LINES - 1, 0);
    err_win = newwin(1,COLS, LINES -3, 0);
    if(color_mode) {
		wattron(game_win, COLOR_PAIR(1));
    		wattron(stats_win, A_BOLD | COLOR_PAIR(2));
    }
    wrefresh(game_win);
    wrefresh(stats_win);
}

int
end_curses(void)
{


	if (!using_curses) {
		return 0;
	}
	if (using_curses) {
		sleep(2);
		refresh();
		def_prog_mode();
		endwin();
		reset_shell_mode();
	}
	using_curses = 0;	/* Reset the flag */
	return 0;
}
