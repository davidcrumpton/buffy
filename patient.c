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
 * Patient Reaction System: For dental cleanings, low effort on a tooth is
 * pleasing but too much low effort over too many turns can cause the patient
 * to become impatient. High effort makes a quick cleaning but causes more
 * discomfort angering the patient.
 *
 */




struct patient_reaction {
	int		effort_level;	/* 0 = low, 1 = medium, 2 = high */
	int		patience_level;	/* 0 = low, 1 = medium, 2 = high */
	char	       *comment;
};
struct patient_reaction reactions[] = {
	{0, 0, "%s bares fangs, eyes narrowed in ancient impatience."},
	{0, 1, "A low growl escapes-immortality has not made %s more patient."},
	{0, 2, "%s sighs theatrically, fangs glinting, clearly unimpressed."},
	{1, 0, "A sharp hiss-centuries of tolerance wearing thin for %s."},
	{1, 1, "%s arches a brow, lips curled in a wry, undead smirk."},
	{1, 2, "A nod of approval from %s, as regal as a creature of the night can muster."},
	{2, 0, "A guttural snarl-'Careful, mortal. %s does bite back.'"},
	{2, 1, "%s winces, but the sarcasm is sharper than the canines."},
	{2, 2, "A rare, genuine smile-'Efficient. You may live another night,' %s intones."}
};

int		patient_mood;
int		patience_level;

void
patient_reaction(char *reaction, size_t reaction_len, int *effort, int *patience, int *pain_tolerance, const int *fang_health, const int *tool_pain_factor, const char *patient_name, int *mood, int *pat_level)
{

	int		pain_inflicted;

	/* Decrease patience over time if fangs aren't fully cleaned */
	if (fang_health > 0) {
		*patience = (*patience > 0) ? (*patience - 1) : 0;
	}

	/*
	 * Calculate pain inflicted, with modifiers for pain tolerance and
	 * fang health
	 */
	pain_inflicted = (*effort * *tool_pain_factor) - *pain_tolerance;
	pain_inflicted += (100 - *fang_health) / 10;
	//Unhealthy fangs(lower health) cause more pain.
		if (pain_inflicted < 0) {
		pain_inflicted = 0;
	}

	/* Determine mood based on inflicted pain (more pain = angrier mood) */
	if (pain_inflicted > 8) {
		patient_mood = MOOD_ANGRY;
	} else if (pain_inflicted > 4) {
		patient_mood = MOOD_UNHAPPY;
	} else {
		patient_mood = MOOD_HAPPY;
	}


	/* Normalize patience to match reaction structure */

	if (*patience > 7) {
		patience_level = PATIENCE_BLISS;	/* High patience */
	} else if (*patience > 3) {
		patience_level = PATIENCE_CALM;	/* Medium patience */
	} else {
		patience_level = PATIENCE_IMPATIENT;	/* Low patience */
	}

	int		index = (patient_mood * 3) + patience_level;	/* Changed from effort
									 * to patient_mood */

	*pat_level = patience_level;
	*mood = patient_mood;

	if (index >= 0 && index < (int)(sizeof(reactions) / sizeof(reactions[0]))) {
		snprintf(reaction, reaction_len, reactions[index].comment, patient_name ? patient_name : "the patient");
		return;
	}

	*reaction = '\0';
	return;
}
