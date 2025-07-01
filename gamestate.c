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
#include <sys/wait.h>

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
load_game(const char *file)
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

	static struct database_info db_info;
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
save_game_state(const char *save_path, const game_state_type * gamestate, size_t gs_len, const creature_type * patient, size_t plen)
{
	int		pipefd[2];
	if (pipe(pipefd) == -1)
		err(1, "pipe");

	pid_t		pid = fork();
	if (pid == -1) {
		err(1, "fork");
	} else if (pid == 0) {
		/* Writer subprocess */
		close(pipefd[1]);	/* Close write end */

		if (dup2(pipefd[0], STDIN_FILENO) == -1)
			err(1, "dup2");
		close(pipefd[0]);

#ifdef __OpenBSD__
		if (unveil(save_path, "wc") == -1)
			err(1, "unveil");
		if (unveil(NULL, NULL) == -1)
			err(1, "lock unveil");

		if (pledge("stdio wpath cpath proc", NULL) == -1)
			err(1, "pledge");
#endif
		int		fd = open(save_path, O_WRONLY | O_TRUNC | O_CREAT, 0600);
		if (fd == -1)
			err(1, "open save file");

		char		buf[1024];
		ssize_t		n;
		while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
			if (write(fd, buf, n) != n)
				err(1, "write save");
		}

		close(fd);
		_exit(0);	/* Success */
	} else {
		/* Main process */
		close(pipefd[0]);	/* Close read end */

		struct database_info db_info;
		init_db_info(&db_info);

		if (write(pipefd[1], &db_info, sizeof(db_info)) != sizeof(db_info))
			errx(1, "Failed to write database info to file %s", save_path);

		if (write(pipefd[1], gamestate, gs_len) != (ssize_t) gs_len)
			warn("partial write");

		if (write(pipefd[1], patient, plen) != plen)
			errx(1, "Failed to write patient data to file %s", save_path);
		/* write all string pointers from structures */
		if (gamestate->character_name != NULL) {
			if (write(pipefd[1], gamestate->character_name, strlen(gamestate->character_name) + 1) != (strlen(gamestate->character_name) + 1))
				errx(1, "Failed to write character name to file %s", save_path);
		}
		if (patient->name != NULL) {
			if (write(pipefd[1], patient->name, strlen(patient->name) + 1) != (strlen(patient->name) + 1))
				errx(1, "Failed to write creature name to file %s", save_path);
		}
		if (patient->species != NULL) {
			if (write(pipefd[1], patient->species, strlen(patient->species) + 1) != (strlen(patient->species) + 1))
				errx(1, "Failed to write creature species to file %s", save_path);
		}
		for (int i = 0; i < 4; i++) {
			if (patient->fangs[i].color != NULL) {
				if (write(pipefd[1], patient->fangs[i].color, strlen(patient->fangs[i].color) + 1) != (strlen(patient->fangs[i].color) + 1))
					errx(1, "Failed to write fang color to file %s", save_path);
			}
		}


		close(pipefd[1]);

		int		status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			warnx("writer exited with code %d", WEXITSTATUS(status));
		else if (WIFSIGNALED(status))
			warnx("writer killed by signal %d", WTERMSIG(status));
	}
}





/*
 * validate_game_file (subprocess) environment creates subprocess
 * which returns 0 if valid and 1 if not
 */
int
validate_game_file(const char *file)
{
	pid_t		pid = fork();
#ifdef __OpenBSD__
	if (unveil(file, "r") == -1)
		err(1, "unveil file");
	if (unveil(NULL, NULL) == -1)
		err(1, "lock unveil");

	if (pledge("stdio rpath", NULL) == -1)
		err(1, "pledge");
#endif

	if (pid == -1) {
		errx(1, "fork");
	} else if (pid == 0) {
		/* child */

		struct stat	st;
		int		fd;
		int		isvalid = 1;

		if ((fd = stat(optarg, &st)) == -1) {
			err(1, "unable to stat %s", optarg);
			goto end_validation;
		}
		if (!S_ISREG(st.st_mode)) {
			err(1, "%s is not a regular file", optarg);
			goto end_validation;
		}
		if ((fd = open(optarg, O_RDONLY)) == -1) {
			err(1, "unable to open %s", optarg);
			goto end_validation;
		}
		if (st.st_size < sizeof(struct database_info) + sizeof(game_state_type) + sizeof(creature_type)) {
			err(1, "%s is too small to be a valid game file", optarg);
			goto end_validation;
		}
		struct database_info db_info;
		if (read(fd, &db_info, sizeof(db_info)) != sizeof(db_info)) {
			err(1, "Failed to read database info from file %s", optarg);
			goto end_validation;
		}
		if (db_info.major != MAJOR || db_info.minor != MINOR || db_info.patch != PATCH || db_info.gamecode != GAMECODE) {
			err(1, "Incompatible game file version in %s", optarg);
			goto end_validation;
		}
		if (lseek(fd, sizeof(db_info), SEEK_SET) == -1) {
			err(1, "Failed to seek in file %s", optarg);
			goto end_validation;
		}
		static game_state_type game_state;
		if (read(fd, &game_state, sizeof(game_state)) != sizeof(game_state)) {
			err(1, "Failed to read game state from file %s", optarg);
			goto end_validation;
		}
		if (read(fd, &creature, sizeof(creature)) != sizeof(creature)) {
			err(1, "Failed to read creature data from file %s", optarg);
			goto end_validation;
		}
		if (game_state.flouride < 0 || game_state.tool_dip < 0 || game_state.tool_effort < 0 || game_state.flouride_used < 0 || game_state.bflag < 0 || game_state.daggerset < 0) {
			err(1, "Invalid game state in %s", optarg);
			goto end_validation;
		}

		isvalid = 0;

end_validation:

		close(fd);

		return isvalid;	/* child ends */
	} else {
		int		status;
		if (waitpid(pid, &status, 0) == -1) {
			warn("waitpid failed");
			return 1;
		}

		if (WIFEXITED(status))
			return (WEXITSTATUS(status));

		if (WIFSIGNALED(status))
			fprintf(stderr, "Subprocess killed by signal %d\n", WTERMSIG(status));
		return 1;
	}
}
