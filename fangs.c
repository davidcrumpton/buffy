#include <stdio.h>
#include <string.h>

#include "playerio.h"
#include "fangs.h"

void print_fang_art(const char **fangs, int rows, int health_level_left, int health_level_right);

/* Dirtiest to cleanest: '#', '=', '*', '+', '-', ':', '.' */
const char health_markers[] = {'#', '=', '*', '+', '-', ':', '.'};
/* ASCII art for upper and lower fangs (R/L for upper, r/l for lower) */
const char *maxillary_fangs[FANG_ROWS_UPPER] = {    
    "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
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
    "@@@@R@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@L@@@@", 
    "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"};
const char *mandibular_fangs[FANG_ROWS_LOWER] = {
    "@@@@@@@@@@@@@@@@@r@@@@@@@@@@@@@@@@@@@@@@@l+@@@@@@@@@@@@@@@@@",
    "@@@@@@@@@@@@@@@@rr@@@@@@@@@@@@@@@@@@@@@@@ll#@@@@@@@@@@@@@@@@",
    "@@@@@@@@@@@@@@@rrr@@@@@@@@@@@@@@@@@@@@@@@lll@@@@@@@@@@@@@@@@",
    "@@@@@@@@@@@@@@*rrrr@@@@@@@@@@@@@@@@@@@@@lllll@@@@@@@@@@@@@@@",
    "@@@@@@@@@@@@@@rrrrrr@@@@@@@@@@@@@@@@@@@lllllll@@@@@@@@@@@@@@",
    "@.........%@@rrrrrrrr@@@@.........=@@@llllllll@@@..........@",         
    "...........@@rrrrrrrrr@@:..........@@lllllllll+@%..........@",
    "...........@@rrrrrrrrr*@:..........@*llllllllll@%..........@",
    "...........@@rrrrrrrrr*@:..........@*llllllllll@%..........@",
    "...........@@rrrrrrrrr*@:..........@*llllllllll@%..........@",
    "...........@@rrrrrrrrr*@:..........@*llllllllll@%..........@",
    "...........@@rrrrrrrrr@@:..........@@llllllllll@@..........@",
    "@:........@@@%rrrrrrr@@@@..........@@@llllllll@@@@........@@",
    "@@@@%+%@@@@@@@@@@@@@@@@@@@+......@@@@@@@@@@@@@@@@@@@@##@@@@@"
};

/* Substitute fang markers based on health level (60 = dirtiest, 100 = cleanest)
 * The health level is expected to be in the range 60 to 100, where 60 is the dirtiest
 * and 100 is the cleanest. The function inputs a character and returns
 * the corresponding health marker based on the health level of the left and right fangs.
 * If the character is 'R', 'L', 'r', or 'l', it substitutes it with the appropriate health marker
 * based on the health level of the left fang. If the character is not one of these,
 * it returns the character unchanged.  With a scale of 60 to 100, and 6 characters,
 * we can map   the health levels as follows:
 * 60-64 -> '#', 65-69 -> '=', 70-74 -> '*', 75-79 -> '+',
 * 80-84 -> '-', 85-95 -> ':', 96-100 -> '.'.   
 * The range is special on the high end, where 96-100 maps to the cleanest marker '.'.
 * 
 */

char substitute_marker(char c, int health_level_left, int health_level_right)
{
    /* Upper fangs: R/L, Lower fangs: r/l */
    int index = 0; /* Index for health_markers array */
    if (c == 'R' || c == 'L' || c == 'r' || c == 'l')
    {
        /* Ensure health_level is within bounds */
        if (c == 'L' || c == 'l') 
        {
            /* For upper fangs, use health_level_left */
            if (health_level_left < 60 || health_level_left > 100) {
                health_level_left = 60; /* Default to dirtiest if out of bounds */
            }
                index = (health_level_left - 60) / 5; /* Maps 60-64 to 0, ..., 96-100 to 6 */
        }
        if(c == 'R' || c == 'r') {
            /* For right fang, use health_level_right */
            if (health_level_right < 60 || health_level_right > 100) {
                health_level_right = 60; /* Default to dirtiest if out of bounds */
            }
            index = (health_level_right - 60) / 5; /* Maps 60-64 to 0, ..., 96-100 to 6 */
            }
        
        /* Ensure index is within bounds */
        if (index < 0) {
            index = 0; /* Default to dirtiest if out of bounds */
        } else if (index > 6) {
            index = 6; /* Default to cleanest if out of bounds */
        }
        return health_markers[index];     
    }

    return c;
}



void print_fang_art(const char **fangs, int rows, int health_level_left, int health_level_right) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; fangs[i][j] != '\0'; ++j) {
            my_putchar(substitute_marker(fangs[i][j], health_level_left, health_level_right  ));
        }
        my_putchar('\n');
    }
}
