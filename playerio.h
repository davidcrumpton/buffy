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

#ifndef PLAYERIO_H
#define PLAYERIO_H

#define PATTERN_GAME_COLOR		1
#define PATTERN_STATUS_COLOR	2
#define PATTERN_ERROR_COLOR		3
#define PATTERN_PROMPT_COLOR	4
#define PATTERN_INFO_COLOR      5

void		my_werase();
void		my_clear();
void		update_stats_display(int fluoride_level, int score, int turns);
void		my_refresh();
void		get_input(const char *prompt, char *buffer, size_t size);
void		my_printf(const char *format,...);
void		mv_printw(int row, int col, const char *format,...);
void		my_putchar(char c);
void		initalize_curses(void);
void        print_info_display(const char *format,...);
int		    end_curses(void);
void		my_print_err(const char *format,...);
void		set_using_curses(int flag);
void		set_color_mode(int flag);
#endif
