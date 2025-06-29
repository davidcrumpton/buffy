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
 * gamestate.c: saves the game structures to file specified by filename
 *
 */
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "playerio.h"
#include "buffy.h"
#include "gamestate.h"

extern game_state_type game_state;
extern creature_type creature;

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
	/* write all string pointers from structures */
	if (game_state.character_name != NULL) {
		if (fwrite(game_state.character_name, strlen(game_state.character_name) + 1, 1, fp) != 1)
			errx(1, "Failed to write character name to file %s", file);
	}
	if (creature.name != NULL) {
		if (fwrite(creature.name, strlen(creature.name) + 1, 1, fp) != 1)
			errx(1, "Failed to write creature name to file %s", file);
	}
	if (creature.species != NULL) {
		if (fwrite(creature.species, strlen(creature.species) + 1, 1, fp) != 1)
			errx(1, "Failed to write creature species to file %s", file);
	}
	for (int i = 0; i < 4; i++) {
		if (creature.fangs[i].color != NULL) {
			if (fwrite(creature.fangs[i].color, strlen(creature.fangs[i].color) + 1, 1, fp) != 1)
				errx(1, "Failed to write fang color to file %s", file);
		}
	}
	fclose(fp);
	my_printf("Game saved successfully to %s\n", file);
	return 0;
}


int
load_game(char *file)
{
	/*
	 * open file for reading using err/errx if failed read database_info
	 * structure, validate structure looks right or err/errx if not read
	 * struct game_state, read struct creature read all string pointers
	 * from structures
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

	/*
	 * Read all string pointers from structures character_name
	 */
	if (game_state.character_name != NULL) {
		size_t		len = 0;
		int		c;
		long		pos = ftell(fp);
		/* Find length of string (including null terminator) */
		while ((c = fgetc(fp)) != EOF && c != '\0')
			len++;
		if (c == EOF)
			errx(1, "Unexpected EOF while reading character name from %s", file);
		len++;		/* Include null terminator */
		fseek(fp, pos, SEEK_SET);
		game_state.character_name = malloc(len);
		if (game_state.character_name == NULL)
			errx(1, "Failed to allocate memory for character name");
		if (fread(game_state.character_name, len, 1, fp) != 1)
			errx(1, "Failed to read character name from file %s", file);
	}

	/* creature name */
	if (creature.name != NULL) {
		size_t		len = 0;
		int		c;
		long		pos = ftell(fp);
		while ((c = fgetc(fp)) != EOF && c != '\0')
			len++;
		if (c == EOF)
			errx(1, "Unexpected EOF while reading creature name from %s", file);
		len++;
		fseek(fp, pos, SEEK_SET);
		creature.name = malloc(len);
		if (creature.name == NULL)
			errx(1, "Failed to allocate memory for creature name");
		if (fread(creature.name, len, 1, fp) != 1)
			errx(1, "Failed to read creature name from file %s", file);
	}

	/* creature.species */
	if (creature.species != NULL) {
		size_t		len = 0;
		int		c;
		long		pos = ftell(fp);
		while ((c = fgetc(fp)) != EOF && c != '\0')
			len++;
		if (c == EOF)
			errx(1, "Unexpected EOF while reading creature species from %s", file);
		len++;
		fseek(fp, pos, SEEK_SET);
		creature.species = malloc(len);
		if (creature.species == NULL)
			errx(1, "Failed to allocate memory for creature species");
		if (fread(creature.species, len, 1, fp) != 1)
			errx(1, "Failed to read creature species from file %s", file);
	}

	/* Read fang colors */
	for (int i = 0; i < 4; i++) {
		if (creature.fangs[i].color != NULL) {
			size_t		len = 0;
			int		c;
			long		pos = ftell(fp);
			while ((c = fgetc(fp)) != EOF && c != '\0')
				len++;
			if (c == EOF)
				errx(1, "Unexpected EOF while reading fang color from %s", file);
			len++;
			fseek(fp, pos, SEEK_SET);
			creature.fangs[i].color = malloc(len);
			if (creature.fangs[i].color == NULL)
				errx(1, "Failed to allocate memory for fang color");
			if (fread(creature.fangs[i].color, len, 1, fp) != 1)
				errx(1, "Failed to read fang color from file %s", file);
		}
	}

	fclose(fp);
	my_printf("Game loaded successfully from %s\n", file);
	return 0;
}



void
validate_game_file(char *optarg)
{
	struct stat	st;
	int		fd;

	if ((fd = stat(optarg, &st)) == -1)
		errx(1, "unable to stat %s", optarg);
	if (!S_ISREG(st.st_mode))
		errx(1, "%s is not a regular file", optarg);
	if ((fd = open(optarg, O_RDONLY)) == -1)
		errx(1, "unable to open %s", optarg);
	if (st.st_size < sizeof(struct database_info) + sizeof(game_state_type) + sizeof(creature_type))
		errx(1, "%s is too small to be a valid game file", optarg);
	struct database_info db_info;
	if (read(fd, &db_info, sizeof(db_info)) != sizeof(db_info))
		errx(1, "Failed to read database info from file %s", optarg);
	if (db_info.major != MAJOR || db_info.minor != MINOR || db_info.patch != PATCH || db_info.gamecode != GAMECODE)
		errx(1, "Incompatible game file version in %s", optarg);
	if (lseek(fd, sizeof(db_info), SEEK_SET) == -1)
		err(1, "Failed to seek in file %s", optarg);
	if (read(fd, &game_state, sizeof(game_state)) != sizeof(game_state))
		errx(1, "Failed to read game state from file %s", optarg);
	if (read(fd, &creature, sizeof(creature)) != sizeof(creature))
		errx(1, "Failed to read creature data from file %s", optarg);
	if (game_state.flouride < 0 || game_state.dagger_dip < 0 || game_state.dagger_effort < 0 || game_state.flouride_used < 0 || game_state.bflag < 0 || game_state.daggerset < 0)
		errx(1, "Invalid game state in %s", optarg);
	close(fd);
}
