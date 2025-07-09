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
 * playerio.c: Provides functions for input/output operations in the Buffy
 * game.
 *
 */

#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <err.h>

#include "playerio.h"
#include "buffy.h"

static int	using_curses = 0;
static int	color_mode = 0;

static WINDOW * game_win = NULL;
static WINDOW * stats_win = NULL;
static WINDOW * err_win = NULL;
static WINDOW * inp_win = NULL;

void
my_werase()
{
	if (using_curses)
		werase(game_win);
	else
		putchar('\n');
}
void
my_clear()
{
	if (using_curses)
		wclear(game_win);
	else
		putchar('\n');
}
void
my_refresh()
{
	if (using_curses)
		wrefresh(game_win);
	else
		putchar('\n');
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

void redraw_game_screen() {
    clear(); // Clear the entire screen
    // Get current dimensions
    // int max_y, max_x;
    // getmaxyx(stdscr, max_y, max_x);

    // If you have specific windows, you'll need to resize and reposition them
    // For example, if you had a 'game_window' and a 'score_window':
    wresize(game_win, 16, 80);
    mvwin(game_win, 0, 0); // Reposition if needed
    wclear(game_win);
    wclear(stats_win);
	wclear(err_win);
	wclear(inp_win);

    // redraw_game_elements(game_window);
    wrefresh(game_win);
    wrefresh(stats_win);
	wrefresh(err_win);
	wrefresh(inp_win);	

    refresh(); // Refresh the standard screen (stdscr)
}
void
get_input(const char *prompt, char *buffer, size_t size)
{
	if (using_curses) {
		int		prompt_row = 0;
		int 	ch = -1;
		vw_printw(inp_win, prompt, NULL);

		do {
			wrefresh(inp_win);
			curs_set(1);
			wmove(inp_win, prompt_row, strlen(prompt));
			ch = wgetnstr(inp_win, buffer, size - 1);
			if(ch == KEY_RESIZE)
				my_print_err("I see a resize!\n");
				redraw_game_screen();
		} while(ch == -1);
		curs_set(0);
		werase(inp_win);
		wrefresh(inp_win);
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
		werase(err_win);

		wattron(err_win, A_BOLD | COLOR_PAIR(3));
		vw_printw(err_win, format, args);

		wattroff(err_win, A_BOLD | COLOR_PAIR(3));
		wrefresh(err_win);
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
		printf("Fluoride: %d, Score: %d, Turn: %d\n", fluoride_level, score, turns);
		return;
	}

	werase(stats_win);

	mvwprintw(stats_win, 0, 1, "Fluoride: %d", fluoride_level);
	mvwprintw(stats_win, 0, COLS / 3, "Score: %d", score);
	mvwprintw(stats_win, 0, (COLS * 2) / 3, "Turn: %d", turns);

	wrefresh(stats_win);
}


void
initalize_curses(void)
{
	if (!using_curses)
		return;
	if (using_curses) {
		initscr();
		keypad(stdscr, TRUE);
		if (LINES < 24 || COLS < 80) {
			end_curses();
			errx(1, "please resize your window from %d/%d to 80x24", COLS, LINES);
		}
	}

	if (using_curses && color_mode && has_colors()) {
		start_color();
		init_pair(PATTERN_GAME_COLOR, COLOR_RED, COLOR_BLACK);	/* red and black is a
									 * friend of Jack */
		init_pair(PATTERN_STATUS_COLOR, COLOR_BLUE, COLOR_BLACK);	/* status */
		init_pair(PATTERN_ERROR_COLOR, COLOR_YELLOW, COLOR_BLACK);	/* error */
		init_pair(PATTERN_PROMPT_COLOR, COLOR_WHITE, COLOR_BLACK);	/* prompt_color */
	}

	int		game_win_height = LINES - 1;
	int		game_win_width = COLS;

	game_win = newwin(game_win_height, game_win_width, 0, 0);
	stats_win = newwin(1, COLS, LINES - 1, 0);
	err_win = newwin(5, COLS, LINES - 5, 0);
	inp_win = newwin(1, COLS, LINES - 2, 0);
	if (color_mode) {
		wattron(game_win, COLOR_PAIR(PATTERN_GAME_COLOR));
		wattron(stats_win, A_BOLD | COLOR_PAIR(PATTERN_STATUS_COLOR));
		wattron(err_win, COLOR_PAIR(PATTERN_ERROR_COLOR));
		wattron(inp_win, COLOR_PAIR(PATTERN_PROMPT_COLOR));
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

	if (game_win) {
		delwin(game_win);
		game_win = NULL;
	}
	if (stats_win) {
		delwin(stats_win);
		stats_win = NULL;
	}

	if (err_win) {
		delwin(err_win);
		err_win = NULL;
	}

	if (inp_win) {
		delwin(inp_win);
		inp_win = NULL;
	}

	sleep(2);
	refresh();
	endwin();

	using_curses = 0;
	return 0;
}
