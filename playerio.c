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
#include <signal.h>

#include "playerio.h"
#include "buffy.h"

static int	using_curses = 0;
static int	color_mode = 0;

static WINDOW * info_win = NULL;
static WINDOW * fang_win = NULL;
static WINDOW * stats_win = NULL;
static WINDOW * err_win = NULL;
static WINDOW * inp_win = NULL;

void
my_werase()
{
	if (using_curses) {
		werase(fang_win);
		werase(info_win);
		werase(stats_win);
	} else
		putchar('\n');
}
void
my_clear()
{
	if (using_curses) {
		wclear(fang_win);
		wclear(info_win);
		wclear(stats_win);
	} else
		putchar('\n');
}
void
my_refresh()
{
	if (using_curses) {
		wrefresh(fang_win);
		wrefresh(info_win);
		wrefresh(stats_win);
	} else
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

void
redraw_game_screen()
{
	clear();

	int		max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);

	mvprintw(0, 0, "Current terminal size: %dx%d", max_x, max_y);
	mvprintw(2, 0, "This game is designed for 80x24.");
	if (max_x < 80 || max_y < 24) {
		mvprintw(4, 0, "Warning: Window is smaller than 80x24. Display may be truncated.");
	} else if (max_x > 80 || max_y > 24) {
		mvprintw(4, 0, "Note: Window is larger than 80x24. Extra space available.");
	}
	mvprintw(6, 0, "To continue the game, enter your response as prompted.");

	wresize(fang_win, 16, max_x);
	wclear(fang_win);
	wrefresh(fang_win);

	mvwin(inp_win, max_y - 2, 0);
	wclear(inp_win);

	mvwin(info_win, max_y - 9, 0);
	wclear(info_win);
	mvwin(info_win, max_y - 9, 0);
	wrefresh(info_win);

	mvwin(stats_win, max_y - 1, 0);
	wclear(stats_win);
	mvwin(stats_win, max_y - 1, 0);
	wrefresh(stats_win);

	wclear(err_win);
	mvwin(err_win, max_y - 5, 0);
	wrefresh(err_win);

	refresh();
}


void
get_input(const char *prompt, char *buffer, size_t size)
{
	if (using_curses) {
		int		prompt_row = 0;
		int		ch = ERR;


		do {
			vw_printw(inp_win, prompt, NULL);
			wrefresh(inp_win);

			wmove(inp_win, prompt_row, strlen(prompt));
			curs_set(1);
			ch = wgetnstr(inp_win, buffer, size - 1);
			if (ch == ERR)
				break;
			if (ch == KEY_RESIZE)
				redraw_game_screen();
		} while (ch != OK);

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
		vw_printw(fang_win, format, args);
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
		vw_printw(fang_win, format, args);
	} else {
		vprintf(format, args);
	}
	va_end(args);
}


void
my_putchar(char c)
{
	if (using_curses) {
		wmove(fang_win, 0, 0);	/* top left */
		waddch(fang_win, c);	/* add the character at the current
					 * cursor position */
		wrefresh(fang_win);
	} else {
		putchar(c);
		fflush(stdout);
	}
}

void
print_highlighted(WINDOW * win, const char *line, const char *word, const int *color_pair)
{
	const char     *p = strstr(line, word);
	if (!p) {
		wprintw(win, "%s", line);
		return;
	}

	wprintw(win, "%.*s", (int)(p - line), line);	/* Print before word */
	wattron(win, COLOR_PAIR(*color_pair));
	wprintw(win, "%s", word);
	wattroff(win, COLOR_PAIR(*color_pair));
	wprintw(win, "%s", p + strlen(word));	/* Print after word */
}



void
print_stats_info(int fluoride_level, int score, int turns)
{
	if (!using_curses) {
		my_printf("Fluoride: %d, Score: %d, Turn: %d\n", fluoride_level, score, turns);
		return;
	}

	mvwprintw(stats_win, 0, 1, "Fluoride: %d", fluoride_level);
	mvwprintw(stats_win, 0, COLS / 3, "Score: %d", score);
	mvwprintw(stats_win, 0, (COLS * 2) / 3, "Turn: %d", turns);
}

void
print_working_info(const char *format,...)
{
	va_list		args;
	va_start(args, format);
	if (using_curses) {
		vw_printw(info_win, format, args);
	} else {
		vprintf(format, args);
	}
	va_end(args);
}

void
handle_exit_signal(int signo)
{
	end_curses();

	_exit(1);
}

void
setup_signal_handlers(void)
{
	struct sigaction sa;
	sa.sa_handler = handle_exit_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGHUP, &sa, NULL);

}


void
initalize_curses(void)
{
	if (!using_curses)
		return;
	if (using_curses) {
		initscr();
		if (LINES < 24 || COLS < 80) {
			end_curses();
			errx(1, "please resize your window from %d/%d to 80x24", COLS, LINES);
		}
		setup_signal_handlers();
	}

	if (using_curses && color_mode && has_colors()) {
		start_color();
		init_pair(PATTERN_GAME_COLOR, COLOR_RED, COLOR_BLACK);	/* red and black is a
									 * friend of Jack */
		init_pair(PATTERN_STATUS_COLOR, COLOR_BLUE, COLOR_BLACK);	/* status */
		init_pair(PATTERN_ERROR_COLOR, COLOR_YELLOW, COLOR_BLACK);	/* error */
		init_pair(PATTERN_PROMPT_COLOR, COLOR_WHITE, COLOR_BLACK);	/* prompt_color */
		init_pair(PATTERN_INFO_COLOR, COLOR_CYAN, COLOR_BLACK);
	}

	int		fang_win_height = LINES - 1;
	int		fang_win_width = COLS;

	info_win = newwin(2, COLS, LINES - 9, 0);
	fang_win = newwin(fang_win_height, fang_win_width, 0, 0);
	stats_win = newwin(1, COLS, LINES - 1, 0);
	err_win = newwin(5, COLS, LINES - 5, 0);
	inp_win = newwin(1, COLS, LINES - 2, 0);

	if (color_mode) {
		wattron(fang_win, COLOR_PAIR(PATTERN_GAME_COLOR));
		wattron(stats_win, A_BOLD | COLOR_PAIR(PATTERN_STATUS_COLOR));
		wattron(err_win, COLOR_PAIR(PATTERN_ERROR_COLOR));
		wattron(inp_win, COLOR_PAIR(PATTERN_PROMPT_COLOR));
		wattron(info_win, COLOR_PAIR(PATTERN_INFO_COLOR));
	}
	wrefresh(fang_win);
	wrefresh(stats_win);
}

int
end_curses(void)
{
	if (!using_curses) {
		return 0;
	}
	if (info_win) {
		delwin(info_win);
		info_win = NULL;
	}
	if (fang_win) {
		delwin(fang_win);
		fang_win = NULL;
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
