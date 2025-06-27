
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

extern game_state_type *game_state;
extern creature_type  *creature;

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

	if (game_state == NULL)
		game_state = malloc(sizeof(*game_state));
	if (game_state == NULL)
		errx(1, "Failed to allocate memory for game_state");

	if (creature == NULL)
		creature = malloc(sizeof(*creature));
	if (creature == NULL)
		errx(1, "Failed to allocate memory for creature");

	if (fread(game_state, sizeof(*game_state), 1, fp) != 1)
		errx(1, "Failed to read game state from file %s", file);
	if (fread(creature, sizeof(*creature), 1, fp) != 1)
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
	if (st.st_size < sizeof(struct database_info) + sizeof(game_state_type) + sizeof(creature_type))
		errx(1, "%s is too small to be a valid game file", optarg);
	struct database_info db_info;
	if (read(fd, &db_info, sizeof(db_info)) != sizeof(db_info))
		errx(1, "Failed to read database info from file %s", optarg);
	if (db_info.major != MAJOR || db_info.minor != MINOR || db_info.patch != PATCH || db_info.gamecode != GAMECODE)
		errx(1, "Incompatible game file version in %s", optarg);
	if (lseek(fd, sizeof(db_info), SEEK_SET) == -1)
		err(1, "Failed to seek in file %s", optarg);
	if (read(fd, game_state, sizeof(*game_state)) != sizeof(*game_state))
		errx(1, "Failed to read game state from file %s", optarg);
	if (read(fd, creature, sizeof(*creature)) != sizeof(*creature))
		errx(1, "Failed to read creature data from file %s", optarg);
	close(fd);
}