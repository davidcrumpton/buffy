#include <stdio.h>
#include <unistd.h>

#include "diagnostic.h"


void
log_game_turn(int turn, const game_state_type * state, const patient_type * patient, const char *comment)
{
	char		filename[64];
	snprintf(filename, sizeof(filename), "game_log_%d.csv", getpid());
	FILE	       *fp = fopen(filename, "a");
	if (!fp)
		return;

	static int	header_written = 0;
	if (!header_written) {
		fprintf(fp, "Turn,Character,PatientIdx,Score,Fluoride,ToolDip,ToolEffort,Mood,Patience,Fang1,Fang2,Fang3,Fang4,Annotation\n");
		header_written = 1;
	}

	fprintf(fp, "%d,%s,%d,%d,%d,%d,%d,%d,%d",
		turn,
		state->character_name,
		state->patient_idx,
		state->score,
		state->fluoride,
		state->tool_dip,
		state->tool_effort,
		patient->mood,
		patient->patience
	);

	for (int i = 0; i < 4; i++) {
		fprintf(fp, ",%d", patient->fangs[i].health);
	}

	fprintf(fp, ",\"%s\"\n", comment ? comment : "");
	fclose(fp);
}

