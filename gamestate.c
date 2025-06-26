
/*
  Saves the game structures to file specified by filename

*/

#include <stdio.h>
#include <err.h>
#include "buffy.h"
#include "gamestate.h"

extern struct game_state game_state;
extern struct creature creature;

struct database_info
{
	int gamecode;

	char major;
	char minor;
	char patch;
};

static void
init_db_info(struct database_info *db_info)
{
	db_info->major = MAJOR;
	db_info->minor = MINOR;
	db_info->patch = PATCH;
	db_info->gamecode = GAMECODE;
}

int save_game(char *file)
{
	/* open file for writing, use err/errx if failed
	   read database_info structiure
	   validate structure looks right or err/errx if not
	   write struct game_state
	   write struct creature
	*/
	FILE *fp = fopen(file, "wb");
	if (fp == NULL)
		errx(1, "Unable to open file %s for writing", file);
	struct database_info db_info;
	init_db_info(&db_info);
	if (fwrite(&db_info, sizeof(db_info), 1, fp) != 1)
		errx(1, "Failed to write database info to file %s", file);
	if (fwrite(&game_state, sizeof(game_state), 1, fp) != 1)
		errx(1, "Failed to write game state to file %s", file);
	if (fwrite(&creature, sizeof(creature), 1, fp) != 1)
		errx(1, "Failed to write creature data to file %s", file);
	fclose(fp);
	printf("Game saved successfully to %s\n", file);
	return 0;
}

int load_game(char *file)
{
	/*
	open file for reading using err/errx if failed
		read database_info structiure
	validate structure looks right or err/errx if not
	read struct game_state
	read struct creature
	*/
	FILE *fp = fopen(file, "rb");
	if (fp == NULL)
		errx(1, "Unable to open file %s for reading", file);
	struct database_info db_info;
	if (fread(&db_info, sizeof(db_info), 1, fp) != 1)
		errx(1, "Failed to read database info from file %s", file);

	if (db_info.major != MAJOR || db_info.minor != MINOR || db_info.patch != PATCH || db_info.gamecode != GAMECODE)
		errx(1, "Incompatible game file version in %s", file);

	if (fread(&game_state, sizeof(game_state), 1, fp) != 1)
		errx(1, "Failed to read game state from file %s", file);
	if (fread(&creature, sizeof(creature), 1, fp) != 1)
		errx(1, "Failed to read creature data from file %s", file);
	fclose(fp);
	printf("Game loaded successfully from %s\n", file);
	return 0;
}
