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
 * Compiler: This project is compiled with clang version 17.0.0 on macOS and
 * 16.0.6 on OpenBSD 7.7
 */


#ifndef BUFFY_H
#define BUFFY_H

typedef struct game_state {
	int		daggerset;
	int		flouride;
	int		tool_dip;
	int		tool_effort;
	int		flouride_used;
	int		bflag;
	int		score;
	int		turns;
	int		using_curses;
	int		color_mode;	
	int		last_tool_dip;
	int		last_tool_effort;
	int		tool_in_use;
	int		creature_idx;
	char	       *character_name;	
}		game_state_type;

typedef struct creature {
	int		age;
	char 	*name;
	char	*species;
	struct creature_fangs {
		int		length;
		int		sharpness;
		char	       *color;
		int		health;
	}		fangs[4];
}		creature_type;

typedef struct tool {
	char	       *name;
	char	       *description;
	int		length;
	int		dip_amount;	
	int		effort;

	int		effectiveness;
	int		durability;
	int		used;
}		tool;


typedef struct fang_info {
	const char     *name;
	int		tooth_number;

}		fang_info_type;





#define DEFAULT_CHARACTER_NAME "Buffy"

#define DEFAULT_DAGGERSET       0
#define DEFAULT_FLOURIDE        200
#define DEFAULT_TOOL_DIP      5
#define DEFAULT_TOOL_EFFORT    2
#define DEFAULT_DAGGER_DIP     10
#define DEFAULT_DAGGER_EFFORT  5
#define DEFAULT_FLOURIDE_USED    0
#define DEFAULT_SCORE           10
#define DEFAULT_TURNS           1

#define MAX_HEALTH              100

#define BONUS_ALL_HEALTH        100
#define BONUS_FANG_CLEANED      1
#define BONUS_FANG_HEALTH       3

#define FANG_COLOR_HIGH          "white"
#define FANG_COLOR_MEDIUM        "dull"
#define FANG_COLOR_LOW           "yellow"

#define FANG_HEALTH_HIGH	90
#define FANG_HEALTH_MEDIUM  80
#define FANG_HEALTH_LOW		60

#define DEFAULT_CREATURE_AGE         100
#define CREATURE_NAME_MAX_LENGTH    25



#define DEFAULT_SAVE_FILE ".buffy_save.btfd"

#define VERSION "1.0.0"

#endif
