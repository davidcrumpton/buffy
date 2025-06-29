#include "playerio.h"
#include "buffy.h"

extern game_state_type game_state;

/* Global flag to switch modes */

void
get_input(const char *prompt, char *buffer, size_t size)
{
	if (game_state.using_curses) {
		printw("%s", prompt);
		refresh();
		getnstr(buffer, size - 1);
		/* Safe input in curses mode */
	} else {
		printf("%s", prompt);
		fflush(stdout);
		fgets(buffer, size, stdin);
		buffer[strcspn(buffer, "\n")] = 0;
		/* Remove newline */
	}
}

void my_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    if (game_state.using_curses) {
        vw_printw(stdscr, format, args);
        refresh();
    } else {
        vprintf(format, args);
    }
    va_end(args);
}

void mv_printw(int row, int col, const char *format, ...) {
    va_list args;
    va_start(args, format);
    if (game_state.using_curses) {
        move(row, col); // Move to the desired position
        vw_printw(stdscr, format, args);
        refresh();
    } else {
        vprintf(format, args);
    }
    va_end(args);
}


void
initalize_curses(void)
{

	if (game_state.using_curses) {
		initscr();
	}
	if (game_state.using_curses && game_state.color_mode && has_colors()) {
		start_color();  /* Initialize color support */
		init_pair(1, COLOR_RED, COLOR_BLACK);  /* Example color pair */
		attron(COLOR_PAIR(1));  /* Use the color pair */
	}
}

int end_curses(void)
{	
	
	if (!game_state.using_curses) {
		return 0;  /* If not using curses, nothing to exit */
	}
	if (has_colors()) {
		start_color();  /* Initialize color support */
		init_pair(1, COLOR_RED, COLOR_BLACK);  /* Example color pair */
		attron(COLOR_PAIR(1));  /* Use the color pair */
	}

	if (game_state.using_curses) {
		sleep(2);
		refresh();   /* Ensure everything is drawn */
		def_prog_mode();  /* Save current terminal state */
		endwin();     /* Exit curses mode */
		reset_shell_mode();  /* Restore terminal without clearing screen */

		/* endwin(); */
		/* Initialize curses mode */
	}
	return 0;
}
