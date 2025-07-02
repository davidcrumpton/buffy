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
#include "playerio.h"
#include "buffy.h"

static int using_curses = 0;
static int color_mode = 0;	/* Global flag to indicate if curses is used */

/* Global flag to switch modes */

void
set_using_curses(int flag)
{
	using_curses = flag;	/* Set the global flag */
}

void
set_color_mode(int flag)
{
	color_mode = flag;	/* Set the global color mode flag */
}
void
get_input(const char *prompt, char *buffer, size_t size)
{
	if (using_curses) {
		printw("%s", prompt);
		refresh();
		getnstr(buffer, size - 1);
		/* Safe input in curses mode */
	} else {
		printf("%s", prompt);
		fflush(stdout);
		fgets(buffer, size, stdin);
		/* buffer[strcspn(buffer, "\n")] = 0; */
		/* Remove newline */
	}
}


void
my_print_err(const char *format,...)
{
	va_list		args;
	va_start(args, format);
	if (using_curses) {
		/* Use a separate window for error messages */
		WINDOW	       *err_win = newwin(3, COLS - 2, LINES - 3, 1);
		wattron(err_win, A_BOLD | COLOR_PAIR(1));	/* Use color pair 1 */
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
		vw_printw(stdscr, format, args);
		refresh();
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
		move(row, col);	/* Move to the desired position */
		vw_printw(stdscr, format, args);
		refresh();
	} else {
		vprintf(format, args);
	}
	va_end(args);
}


void
my_putchar(char c)
{
	if (using_curses) {
		wmove(stdscr, 0, 0);	/* move to (0, 0) position */
		waddch(stdscr, c);	/* add the character at the current
					 * cursor position */
		wrefresh(stdscr);	/* refresh the window */
	} else {
		putchar(c);	/* Print character to standard output */
		fflush(stdout);	/* Ensure it is printed immediately */
	}
}

void
initalize_curses(void)
{

	if (using_curses) {
		initscr();
	}
	if (using_curses && color_mode && has_colors()) {
		start_color();	/* Initialize color support */
		init_pair(1, COLOR_RED, COLOR_BLACK);	/* Example color pair */
		attron(COLOR_PAIR(1));	/* Use the color pair */
	}
}

int
end_curses(void)
{


	if (!using_curses) {
		return 0;	/* If not using curses, nothing to exit */
	}
	if (has_colors()) {
		start_color();	/* Initialize color support */
		init_pair(1, COLOR_RED, COLOR_BLACK);	/* Example color pair */
		attron(COLOR_PAIR(1));	/* Use the color pair */
	}

	if (using_curses) {
		sleep(2);
		refresh();	/* Ensure everything is drawn */
		def_prog_mode();	/* Save current terminal state */
		endwin();	/* Exit curses mode */
		reset_shell_mode();	/* Restore terminal without clearing
					 * screen */

		/* endwin(); */
		/* Initialize curses mode */
	}
	using_curses = 0;	/* Reset the flag */
	return 0;
}