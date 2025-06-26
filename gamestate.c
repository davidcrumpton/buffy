
/*
 * Saves the game structures to file specified by filename
 *
 */
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "buffy.h"
#include "gamestate.h"

extern struct game_state game_state;
extern struct creature creature;

struct database_info {
	int		gamecode;

	char		major;
	char		minor;
	char		patch;
};

static void
init_db_info(struct database_info *db_info)
{
	db_info->major = MAJOR;
	db_info->minor = MINOR;
	db_info->patch = PATCH;
	db_info->gamecode = GAMECODE;
}

int
save_game(char *file)
{
	/*
	 * open file for writing, use err/errx if failed read database_info
	 * structiure validate structure looks right or err/errx if not write
	 * struct game_state write struct creature
	 */
	FILE	       *fp = fopen(file, "wb");
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

int
load_game(char *file)
{
	/*
	 * open file for reading using err/errx if failed read database_info
	 * structiure validate structure looks right or err/errx if not read
	 * struct game_state read struct creature
	 */
	FILE	       *fp = fopen(file, "rb");
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


void
validate_game_file(char *optarg)
{
	struct stat	st;
	int		fd;

	if ((fd = stat(optarg, &st)) == -1)
		err(1, "unable to stat %s", optarg);
	if (!S_ISREG(st.st_mode))
		errx(1, "%s is not a regular file", optarg);
	if ((fd = open(optarg, O_RDONLY)) == -1)
		err(1, "unable to open %s", optarg);
	if (read(fd, &game_state.flouride, sizeof(game_state.flouride)) != sizeof(game_state.flouride))
		err(1, "unable to read fluoride from %s", optarg);
	if (read(fd, &game_state.dagger_dip, sizeof(game_state.dagger_dip)) != sizeof(game_state.dagger_dip))
		err(1, "unable to read dagger dip from %s", optarg);
	if (read(fd, &game_state.dagger_effort, sizeof(game_state.dagger_effort)) != sizeof(game_state.dagger_effort))
		err(1, "unable to read dagger effort from %s", optarg);
	if (read(fd, &game_state.flouride_used, sizeof(game_state.flouride_used)) != sizeof(game_state.flouride_used))
		err(1, "unable to read fluoride used from %s", optarg);
	if (read(fd, &game_state.bflag, sizeof(game_state.bflag)) != sizeof(game_state.bflag))
		err(1, "unable to read bflag from %s", optarg);
	if (read(fd, &game_state.daggerset, sizeof(game_state.daggerset)) != sizeof(game_state.daggerset))
		err(1, "unable to read daggerset from %s", optarg);
	if (read(fd, &game_state.score, sizeof(game_state.score)) != sizeof(game_state.score))
		err(1, "unable to read score from %s", optarg);
	if (read(fd, &game_state.turns, sizeof(game_state.turns)) != sizeof(game_state.turns))
		err(1, "unable to read turns from %s", optarg);
	if (read(fd, &game_state.character_name, sizeof(game_state.character_name)) != sizeof(game_state.character_name))
		err(1, "unable to read character name from %s", optarg);
	if (read(fd, &creature.name, sizeof(creature.name)) != sizeof(creature.name))
		err(1, "unable to read creature name from %s", optarg);
	if (read(fd, &creature.age, sizeof(creature.age)) != sizeof(creature.age))
		err(1, "unable to read creature age from %s", optarg);
	if (read(fd, &creature.species, sizeof(creature.species)) != sizeof(creature.species))
		err(1, "unable to read creature species from %s", optarg);
	if (read(fd, &creature.fangs, sizeof(creature.fangs)) != sizeof(creature.fangs))
		err(1, "unable to read creature fangs from %s", optarg);
	close(fd);
}
