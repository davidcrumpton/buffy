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
 * save, loads, and verify use privsep to read and write the game
 *
 */
#include <sys/stat.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "playerio.h"
#include "buffy.h"
#include "gamestate.h"


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
void
load_game_state(const char *load_path, game_state_type * gamestate_g, size_t gs_len,
		creature_type * patient_g, size_t plen, char *character_name_g)
{
	int		pipefd[2];
	pid_t		pid;




	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {		/* Child process to read the file and send to
				 * parent */
		close(pipefd[0]);	/* close stdin since we are output */
#ifdef __OpenBSD__
	if (unveil(load_path, "r") == -1)
		err(1, "unveil");
	if (unveil(NULL, NULL) == -1)
		err(1, "lock unveil");

	if (pledge("stdio rpath", NULL) == -1)
		err(1, "pledge:");
#endif
		FILE	       *fp = fopen(load_path, "rb");
		if (fp == NULL)
			errx(1, "Unable to open file %s for reading", load_path);

		static struct database_info db_info;
		if (fread(&db_info, sizeof(db_info), 1, fp) != 1)
			errx(1, "Failed to read database info from file %s", load_path);

		if (db_info.major != MAJOR || db_info.minor != MINOR || db_info.patch != PATCH || db_info.gamecode != GAMECODE)
			errx(1, "Incompatible game file version in %s", load_path);

		game_state_type	gamestate;
		if (fread(&gamestate, sizeof(gamestate), 1, fp) != 1)
			errx(1, "Failed to read game state from file %s", load_path);
		else if (write(pipefd[1], &gamestate, sizeof(gamestate)) <= 0)
			errx(1, "Couldn't write gamestate to parent process");

		creature_type	patient;
		if (fread(&patient, sizeof(patient), 1, fp) != 1)
			errx(1, "Failed to read patient data from file %s", load_path);
		else if (write(pipefd[1], &patient, sizeof(patient)) <= 0)
			errx(1, "Couldn't write patient to parent process");


		if (gamestate.character_name != NULL) {
			size_t		len = 0;
			int		c;
			long		pos = ftell(fp);

			while ((c = fgetc(fp)) != EOF && c != '\0')
				len++;
			if (c == EOF)
				errx(1, "Unexpected EOF while reading character name from %s", load_path);
			len++;	/* Include null */
			fseek(fp, pos, SEEK_SET);
			gamestate.character_name = malloc(len);
			if (gamestate.character_name == NULL)
				errx(1, "Failed to allocate memory for character name");
			if (fread(gamestate.character_name, len, 1, fp) != 1)
				errx(1, "Failed to read character name from file %s", load_path);
			else if (write(pipefd[1], gamestate.character_name, strlen(gamestate.character_name) + 1) != (strlen(gamestate.character_name) + 1))
				errx(1, "Failed to write patient name to parent");
		}



		close(pipefd[1]);
		_exit(EXIT_SUCCESS);
	} else {		/* Parent process */
		close(pipefd[1]);	/* Close stdout */

		/* read from stdin pipe */

		if (read(pipefd[0], gamestate_g, gs_len) != gs_len)
			errx(1, "Failed to read game state from file %s", load_path);
		if (read(pipefd[0], patient_g, plen) != plen)
			errx(1, "Failed to read creature data from file %s", load_path);

		/* Read strings */
		int ch = 0, n = 0;
		do {
			if (read(pipefd[0], &ch, 1) != -1)
				character_name_g[n++] = ch;
		} while (ch != 0);

		close(pipefd[0]);
	}


	/*
	 * PARENT END
	 */
	return;
}



int
save_game_state(const char *save_path, const game_state_type * gamestate, size_t gs_len, const creature_type * patient, size_t plen)
{
	int		pipefd[2];
	if (pipe(pipefd) == -1)
		err(1, "pipe");

	pid_t		pid = fork();
	if (pid == -1) {
		err(1, "fork");
	} else if (pid == 0) {
		/* Writer subprocess (Child) */
		close(pipefd[1]);	/* Close stdout*/

		if (dup2(pipefd[0], STDIN_FILENO) == -1)
			err(1, "dup2");
		close(pipefd[0]);

#ifdef __OpenBSD__
		if (unveil(save_path, "rwc") == -1)
			err(1, "unveil");
		if (unveil(NULL, NULL) == -1)
			err(1, "lock unveil");

		if (pledge("stdio wpath cpath", NULL) == -1)
			err(1, "pledge:");
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
		_exit(EXIT_SUCCESS);	/* Success */
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


		close(pipefd[1]);

		int		status;
		while (waitpid(pid, &status, 0) == -1) {
			if (errno == EINTR)
				continue;
			warn("waitpid failed");

   		return 1;	
 		}
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		else if (WIFSIGNALED(status))
			warnx("writer killed by signal %d", WTERMSIG(status));
	}
	return 0;
}





/*
 * validate_game_file (subprocess) environment creates subprocess which
 * returns 0 if valid and 1 if not
 */
int
validate_game_file(const char *file)
{
	pid_t		pid = fork();

	if (pid == -1) {
		errx(1, "fork");
	} else if (pid == 0) {
		/* child */
#ifdef __OpenBSD__
		if (unveil(file, "r") == -1)
			err(1, "unveil");
		if (unveil(NULL, NULL) == -1)
			err(1, "lock unveil");

		if (pledge("stdio rpath", NULL) == -1)
			err(1, "pledge");
#endif



		struct stat	st;
		int		fd;
		int		isvalid = 1;

		if ((fd = stat(optarg, &st)) == -1) {
			err(1, "unable to stat %s", optarg);
			goto validation_err;
		}
		if (!S_ISREG(st.st_mode)) {
			err(1, "%s is not a regular file", optarg);
			goto validation_err;
		}
		if ((fd = open(optarg, O_RDONLY)) == -1) {
			err(1, "unable to open %s", optarg);
			goto validation_err;
		}
		if (st.st_size < sizeof(struct database_info) + sizeof(game_state_type) + sizeof(creature_type)) {
			err(1, "%s is too small to be a valid game file", optarg);
			goto validation_err;
		}
		struct database_info db_info;
		if (read(fd, &db_info, sizeof(db_info)) != sizeof(db_info)) {
			err(1, "Failed to read database info from file %s", optarg);
			goto validation_err;
		}
		if (db_info.major != MAJOR || db_info.minor != MINOR || db_info.patch != PATCH || db_info.gamecode != GAMECODE) {
			err(1, "Incompatible game file version in %s", optarg);
			goto validation_err;
		}
		if (lseek(fd, sizeof(db_info), SEEK_SET) == -1) {
			err(1, "Failed to seek in file %s", optarg);
			goto validation_err;
		}
		static game_state_type game_state_v;
		if (read(fd, &game_state_v, sizeof(game_state_v)) != sizeof(game_state_v)) {
			err(1, "Failed to read game state from file %s", optarg);
			goto validation_err;
		}
		static creature_type patient;
		if (read(fd, &patient, sizeof(patient)) != sizeof(patient)) {
			err(1, "Failed to read creature data from file %s", optarg);
			goto validation_err;
		}
		if (game_state_v.flouride < 0 || game_state_v.tool_dip < 0 || game_state_v.tool_effort < 0 || game_state_v.flouride_used < 0 || game_state_v.bflag < 0 || game_state_v.daggerset < 0) {
			err(1, "Invalid game state in %s", optarg);
			goto validation_err;
		}
		isvalid = 0;
		printf("valid file %s", file);
		_exit(isvalid);

validation_err:

		close(fd);
		_exit(isvalid);	/* child ends */
	} else {		/* Parent */
		int		status;
		while (waitpid(pid, &status, 0) == -1) {
			if (errno == EINTR)
				continue;
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

