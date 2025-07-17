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

#ifndef FANGS_H
#define FANGS_H



#define FANG_ROWS_UPPER 11
#define FANG_ROWS_LOWER 12

#define	MAXILLARY_RIGHT_CANINE 0
#define	MAXILLARY_LEFT_CANINE 1

#define	MANDIBULAR_RIGHT_CANINE 2
#define	MANDIBULAR_LEFT_CANINE 3

#define UPPER_FANGS     1
#define LOWER_FANGS     0


char *      fang_art(const int upper_fangs, int rows, int health_level_left, int health_level_right, int using_curses);
#endif				/* FANGS_H */
