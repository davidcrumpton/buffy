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
/* refactor dagger as tool */
typedef struct game_state {
	int		daggerset;
	int		flouride;
	int		tool_dip;
	int		tool_effort;
	int		flouride_used;
	int		bflag;
	int		score;
	int		turns;
	int		using_curses;	/* Flag to indicate if curses is used */
	int		color_mode;	/* Flag to indicate if color mode is
					 * enabled */
	int		last_tool_dip;	/* Last dip amount used */
	int		last_tool_effort;	/* Last effort used */
	int		tool_in_use;	/* Index of the tool currently in use */
	char	       *character_name;	/* Name of the character, e.g.,
					 * "Buffy" */
}		game_state_type;

typedef struct creature {
	int		age;
	char	       *name;
	char	       *species;
	struct creature_fangs {
		int		length;
		int		sharpness;
		char	       *color;
		int		health;	/* health of the fangs from 0 to 10 */
	}		fangs[4];
}		creature_type;

typedef struct tool {
	char	       *name;	/* Name of the tool, e.g., "Dagger" */
	char	       *description;	/* Description of the tool */
	int		length;	/* Length of the tool, e.g., 10 for a dagger */
	int		dip_amount;	/* Amount of fluoride the tool can
					 * hold */
	int		effort;	/* Effort required to use the tool */

	int		effectiveness;	/* Effectiveness of the tool, e.g., 5
					 * for a dagger */
	int		durability;	/* Durability of the tool, e.g., 100
					 * for a dagger */
	int		used;	/* Amount of fluoride used with the tool */
}		tool;

/* Define a struct for detailed fang information */
typedef struct fang_info {
	const char     *name;
	int		tooth_number;
	/* Universal tooth numbering system */
}		fang_info_type;

void save_game_state(const char *save_path, const game_state_type * gamestate, size_t gs_len, const creature_type * patient, size_t plen);

int		load_game(const char *file);

#define DEFAULT_CHARACTER_NAME "Buffy"
/* Define default values as constants */
#define DEFAULT_DAGGERSET       0
#define DEFAULT_FLOURIDE        200
#define DEFAULT_TOOL_DIP      5
#define DEFAULT_TOOL_EFFORT    2
#define DEFAULT_DAGGER_DIP     10
#define DEFAULT_DAGGER_EFFORT  5
#define DEFAULT_FLOURIDE_USED    0
#define DEFAULT_SCORE           10
#define DEFAULT_TURNS           0

#define MAX_HEALTH              100

#define BONUS_ALL_HEALTH        100
#define BONUS_FANG_CLEANED      1
#define BONUS_FANG_HEALTH       3

#define FANG_COLOR_HIGH          "white"
#define FANG_COLOR_MEDIUM        "dull"
#define FANG_COLOR_LOW           "yellow"

#define DEFAULT_CREATURE_AGE         100	/* Default age for the
						 * creature */
#define CREATURE_NAME_MAX_LENGTH    25

#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX              64
#endif				/* LOGIN_NAME_MAX */
#define DEFAULT_SAVE_FILE ".buffy_save.btfd"

#define VERSION "1.0.0"
#endif				/* BUFFY_H */
