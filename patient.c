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
 * patient.c: functions to express player mood and present appropriate
 * comments
 *
*/
#include "stdio.h"
#include "patient.h"

 /* 
  * Patient Reaction System:
  * For dental cleanings, low effort on a tooth is pleasing but too much
  * low effort over too many turns can cause the patient to become impatient.
  * High effort makes a quick cleaning but causes more discomfort angering
  * the patient.
  *  
 */

 


struct patient_reaction {
    int effort_level;  // 0 = low, 1 = medium, 2 = high
    int patience_level; // 0 = low, 1 = medium, 2 = high
    char *comment;
};
struct patient_reaction reactions[] = {
    {0, 0, "The %s bares fangs, eyes narrowed in ancient impatience."},
    {0, 1, "A low growl escapes—immortality has not made %s more patient."},
    {0, 2, "%s sighs theatrically, fangs glinting, clearly unimpressed."},
    {1, 0, "A sharp hiss—centuries of tolerance wearing thin for %s."},
    {1, 1, "%s arches a brow, lips curled in a wry, undead smirk."},
    {1, 2, "A nod of approval from %s, as regal as a creature of the night can muster."},
    {2, 0, "A guttural snarl—'Careful, mortal. %s does bite back.'"},
    {2, 1, "%s winces, but the sarcasm is sharper than the canines."},
    {2, 2, "A rare, genuine smile—'Efficient. You may live another night,' %s intones."}
};

static char reactstr[160];

/* Patient Responses:
    * The patient will make comments about the care.  Some comments can be body 
    * language comments as well.
    * Comments will range from low (angry/impatient to pleased/happy)
    * There will be a structure of comments with values to identify what to pick
    * based on provider performance.
    * Low fang_health will also affect the comments.
    * Patient will decrement patience based on high effort and low patience.
    * If fang_health is high, the patient will be more pleased.
    * If fang_health is low, the patient will be more displeased.
*/

char *
patient_reaction(int effort, int *patience, int fang_health, int turn, int tool_pain_factor, const char *patient_name) {
    int eff = effort;
    int pat = *patience;

    // Adjust patience based on effort
    if (effort == 2 && pat > 0) {
        (*patience)--;
        pat = *patience;
    } else if (effort == 0 && pat < 2) {
        (*patience)++;
        pat = *patience;
    }

    // Clamp patience to valid range
    if (pat < 0) pat = 0;
    if (pat > 2) pat = 2;

    // Adjust reaction for fang_health
    int mood = pat;
    if (fang_health < 3) {
        if (mood > 0) mood--;
    } else if (fang_health > 7) {
        if (mood < 2) mood++;
    }

    // Adjust mood based on tool pain factor (from buffy.c definitions)
    // Assume: TOOL_PAIN_NONE = 0, TOOL_PAIN_LOW = 1, TOOL_PAIN_MED = 2, TOOL_PAIN_HIGH = 3
    // Higher pain factor decreases mood, lower pain increases mood
    if (tool_pain_factor >= 2 && mood > 0) {
        mood--;
    } else if (tool_pain_factor == 0 && mood < 2) {
        mood++;
    }

    // Only print comment if mood changed or it's the first turn
    if (mood != last_mood || turn == 0) {
        int index = (eff * 3) + mood;
        if (index >= 0 && index < (int)(sizeof(reactions) / sizeof(reactions[0]))) {
            char *r;
            snprintf(reactstr, sizeof(reactstr), reactions[index].comment, patient_name);
            r = reactstr;
            return r;
        }
        last_mood = mood;
        last_turn = turn;
    }
    return NULL;
}