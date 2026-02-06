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

#include <stdio.h>
#include "fangs.h"



/* Dirtiest to cleanest: '#', '=', '*', '+', ':', '-', '.' */
static const char health_markers[] = {'#', '=', '*', '+', ':', '-', '.'};
/* ASCII art for upper and lower fangs (R/L for upper, r/l for lower) */
static const char *maxillary_fangs[FANG_ROWS_UPPER] = {
	"@@@@@@@@@@@.... @@@-....@@@......@@@....:@@@.....@@@@@@@@@@@",
	"@RRRRRRR@@.......@.......@........@.......@.......@@LLLLLLL@",
	"RRRRRRRRR@.......@.......%........@.......@.......@LLLLLLLL@",
	"RRRRRRRRR@.......@.......%........@.......@.......@LLLLLLLL@",
	":RRRRRRRR@.......@.......%........@.......@.......@LLLLLLLL@",
	"@RRRRRRRR@.......@+......@@......@@......-@.......@LLLLLLLL@",
	"@RRRRRRR@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@LLLLLLL@",
	"@:RRRRR@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@LLLLLL@",
	"@@RRRR@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@LLLL@@",
	"@@@RR-@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+LL@@@",
	"@@@@R@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@L@@@@"
};
static const char *mandibular_fangs[FANG_ROWS_LOWER] = {
	"@@@@@@@@@@@@@@@@rr@@@@@@@@@@@@@@@@@@@@@@@ll#@@@@@@@@@@@@@@@@",
	"@@@@@@@@@@@@@@@rrr@@@@@@@@@@@@@@@@@@@@@@@lll@@@@@@@@@@@@@@@@",
	"@@@@@@@@@@@@@@*rrrr@@@@@@@@@@@@@@@@@@@@@lllll@@@@@@@@@@@@@@@",
	"@@@@@@@@@@@@@@rrrrrr@@@@@@@@@@@@@@@@@@@lllllll@@@@@@@@@@@@@@",
	"@.........%@@rrrrrrrr@@@@.........=@@@llllllll@@@..........@",
	"...........@@rrrrrrrrr@@:..........@@lllllllll+@%..........@",
	"...........@@rrrrrrrrr*@:..........@*llllllllll@%..........@",
	"...........@@rrrrrrrrr*@:..........@*llllllllll@%..........@",
	"...........@@rrrrrrrrr*@:..........@*llllllllll@%..........@",
	"%.........%@@rrrrrrrrr*@:..........@*llllllllll@%..........@",
	"%%........%@@rrrrrrrrr@@:..........@@llllllllll@@..........@",
	"@@.......@%@@%rrrrrrr@@@@%........%@@@llllllll@@@@........@@"
};

/*
 * Substitute fang markers based on health level (60 = dirtiest, 100 =
 * cleanest) The health level is expected to be in the range 60 to 100, where
 * 60 is the dirtiest and 100 is the cleanest. The function inputs a
 * character and returns the corresponding health marker based on the health
 * level of the left and right fangs. If the character is 'R', 'L', 'r', or
 * 'l', it substitutes it with the appropriate health marker based on the
 * health level of the left fang. If the character is not one of these, it
 * returns the character unchanged.  With a scale of 60 to 100, and 6
 * characters, we can map   the health levels as follows: 60-64 -> '#', 65-69
 * -> '=', 70-74 -> '*', 75-79 -> '+', 80-84 -> ':', 85-95 -> '-', 96-100 ->
 * '.'.   The range is special on the high end, where 96-100 maps to the
 * cleanest marker '.'.
 *
 */
static const
char
substitute_marker(char c, int health_level_left, int health_level_right)
{
	/* Upper fangs: R/L, Lower fangs: r/l */
	if (c == 'R' || c == 'L' || c == 'r' || c == 'l') {
		int		health_level = (c == 'R' || c == 'r') ? health_level_right : health_level_left;

		if (health_level < FANG_HEALTH_MIN)
			health_level = FANG_HEALTH_MIN;
		if (health_level > FANG_HEALTH_MAX)
			health_level = FANG_HEALTH_MAX;

		int		index = (health_level - FANG_HEALTH_MIN) / 5;
		if (index < 0)
			index = 0;
		if (index > 6)
			index = 6;

		return health_markers[index];

	}

	return c;
}





char	       *
fang_art(const int upper_fangs, int rows, int health_level_left, int health_level_right)
{
	const char    **fangs;

	if (upper_fangs)
		fangs = maxillary_fangs;
	else
		fangs = mandibular_fangs;

	static char	buffer[FANG_ROWS_LOWER * 62];
	buffer[0] = '\0';
	size_t		idx = 0;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; fangs[i][j] != '\0'; ++j) {
			buffer[idx++] = substitute_marker(fangs[i][j], health_level_left, health_level_right);
		}
		buffer[idx++] = '\n';
	}
	buffer[idx] = '\0';
	return buffer;
}
