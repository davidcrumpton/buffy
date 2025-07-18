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
 * buffy.c: contains the main game loop and initialization code for the Buffy
 * game. It initializes the game state, handles command line arguments, and
 *
 */
#include <sys/stat.h>
#include <sys/param.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <time.h>

#include "buffy.h"
#include "fangs.h"
#include "playerio.h"
#include "gamestate.h"

#ifdef __FreeBSD__
#define __dead
#endif

#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX              64
#endif				/* End Login Name Max */

#define IS_UPPER_FANG	(i < 2)

char		character_name[LOGIN_NAME_MAX + 1];
char		save_path[FILENAME_MAX + 1];

game_state_type	game_state;
patient_type	patient;




fang_info_type	fang_names[] = {
	{"Maxillary Right Canine", 6},
	{"Maxillary Left Canine", 11},
	{"Mandibular Right Canine", 27},
	{"Mandibular Left Canine", 22},
	{"Unknown Fang", 0}
};



tool		tools[] = {
	{"Buffy's Fingernail", "A sharp fingernail for cleaning", 1, 1, 1, 1, 50, 0},
	{"Small Rock", "A small but rough rock for scraping", 3, 3, 15, 2, 30, 0},
	{"Shark Tooth", "A sharp shark tooth for precise cleaning", 6, 5, 8, 5, 40, 0},
	{"Wooden Dagger", "A wooden dagger for simply applying fluoride", 10, 8, 5, 5, 100, 0},
	{"Bronze Dagger", "A bronze dagger for applying fluoride", 12, 9, 5, 7, 150, 0},
	{"Steel Dagger", "A steel dagger for strongly applying fluoride", 14, 10, 5, 10, 200, 0}
};


struct patient	patients[] = {
	{90, "Dracula", "Vampire", {{0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}}},
	{110, "Gorath", "Orc", {{0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}}},
	{130, "Fenrir", "Werewolf", {{0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}}},
	{150, "Nagini", "Serpent", {{0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}}},
	{200, "Smaug", "Dragon", {{0, 0, NULL, 100}}}	/* Dragon has max health
							 * fangs */
};

enum species {
	VAMPIRE,
	ORC,
	WEREWOLF,
	SERPENT,
	DRAGON
};

extern char    *__progname;

static int	__dead
usage(void)
{
	fprintf(stderr, "%s: [ -b | --not-named-buffy ] [ -f | --fluoride-file <file> ] [ --daggerset ]\n", __progname);
	exit(EXIT_FAILURE);
}

static char    *
return_patient_name(const int idx)
{
	return (patients[idx]).name;
}

static char    *
return_patient_species(const int idx)
{
	return (patients[idx].species);
}

static char    *
return_concat_homedir(const char *append_str)
{
	const char     *home = getenv("HOME");
	if (!home) {
		fprintf(stderr, "Unable to determine HOME directory.\n");
		return NULL;
	}

	static char	home_dir[FILENAME_MAX];
	if (append_str) {
		snprintf(home_dir, sizeof(home_dir), "%s/%s", home, append_str);
	} else {
		strlcpy(home_dir, home, sizeof(home_dir));
	}
	return home_dir;
}


static int
choose_random_tool(int isdaggerset)
{

	if (isdaggerset) {
		return arc4random_uniform(3) + 3;	/* daggers idx 3, 4, or
							 * 5 */
	} else {
		return arc4random_uniform(3);
	}
}

/*
 * The game when running only saves the game in the default path so we will
 * need a default_game_save wrapper to set the path returns 0 on success and
 * -1 on failure calls return_concat_homedir to build out path
 */
static void
default_game_save(void)
{
	char	       *saved_pathname = return_concat_homedir(DEFAULT_SAVE_FILE);
	if (saved_pathname == NULL) {
		errx(1, "Unable to determine save path.\n");
	}
	strlcpy(save_path, saved_pathname, sizeof(save_path));
	my_printf("Saving game to: %s\n", save_path);
	if (save_game_state(save_path, &game_state, sizeof(game_state), &patient, sizeof(patient)) != 0) {
		errx(1, "Unable to save game state to %s", save_path);
	}
}

/* Initialize the game state with default values */
static void
init_game_state(int bflag)
{

	game_state.fluoride = DEFAULT_FLUORIDE;
	game_state.tool_dip = DEFAULT_TOOL_DIP;
	game_state.tool_effort = DEFAULT_TOOL_EFFORT;
	game_state.fluoride_used = DEFAULT_FLUORIDE_USED;
	game_state.bflag = bflag;
	game_state.score = DEFAULT_SCORE;
	game_state.turns = DEFAULT_TURNS;

	/* If bflag is set, we will use the user login name */
	if (bflag) {
		char		login_name[256];
		if (getlogin_r(login_name, sizeof(login_name)) != 0)
			errx(1, "Unable to get login name");
		strlcpy(character_name, login_name, sizeof(game_state.character_name));
	} else
		strlcpy(character_name, DEFAULT_CHARACTER_NAME, sizeof(DEFAULT_CHARACTER_NAME));

	game_state.character_name = character_name;
	game_state.tool_in_use = choose_random_tool(game_state.daggerset);
	/*
	 * Initialize save path with return_concat_homedir with default save
	 * game path and store it in save_path
	 */
	char	       *saved_pathname = return_concat_homedir(DEFAULT_SAVE_FILE);
	if (saved_pathname == NULL) {
		fprintf(stderr, "Unable to determine save path.\n");
		exit(EXIT_FAILURE);
	}
	strlcpy(save_path, saved_pathname, sizeof(save_path));
	game_state.last_tool_dip = DEFAULT_TOOL_DIP;
	game_state.last_tool_effort = DEFAULT_TOOL_EFFORT;
}
static void
randomize_fangs(struct patient *patient_ptr, int count)
{
	for (int i = 0; i < count; i++) {
		patient_ptr->fangs[i].length = 4 + arc4random_uniform(3);	/* 4–6 */
		patient_ptr->fangs[i].sharpness = 5 + arc4random_uniform(4);	/* 5–8 */

		/* Bias health toward lower values (dirty teeth) */
		int		r = arc4random_uniform(MAX_HEALTH);
		if (r < 60)
			patient_ptr->fangs[i].health = 60 + arc4random_uniform(11);	/* 60–70 */
		else if (r < 90)
			patient_ptr->fangs[i].health = 71 + arc4random_uniform(10);	/* 71–80 */
		else
			patient_ptr->fangs[i].health = 90 + arc4random_uniform(11);	/* 90–100 */
	}
}



static void
get_provider_input(int *tool_dip, int *tool_effort, int last_tool_dip, int last_tool_effort)
{
	int		valid = 0;
	char		input[32];
	char	       *endptr;
	char		prompt[128];

	/* Prompt for tool dip */
	while (!valid) {
		prompt[0] = 0;
		snprintf(prompt, sizeof(prompt), "How much to dip the %s in the fluoride [%d]? ", tools[game_state.tool_in_use].name, last_tool_dip);
		get_input(prompt, input, sizeof(input));
		if (strlen(input) == 0 && game_state.using_curses < 1) {
			my_print_err("Input error. Please try again.\n");
			continue;
		}
		/* If user just presses enter, use last value */
		if (input[0] == '\n' || strlen(input) == 0) {
			*tool_dip = last_tool_dip;
			valid = 1;
			continue;
		}
		*tool_dip = (int)strtol(input, &endptr, 10);
		if (endptr == input || *tool_dip < 0) {
			my_print_err("Invalid input for %s dip. Please enter a non-negative integer.\n", tools[game_state.tool_in_use].name);
			continue;
		}
		valid = 1;
	}

	valid = 0;
	/* Prompt for tool effort */
	while (!valid) {
		snprintf(prompt, sizeof(prompt), "How much effort to apply to the fang [%d]? ", last_tool_effort);
		get_input(prompt, input, sizeof(input));
		if (strlen(input) == 0 && game_state.using_curses < 1) {
			my_print_err("Input error. Please try again.\n");
			continue;
		}
		if (input[0] == '\n' || strlen(input) == 0) {
			*tool_effort = last_tool_effort;
			valid = 1;
			continue;
		}
		*tool_effort = (int)strtol(input, &endptr, 10);
		if (endptr == input || *tool_effort < 0) {
			my_print_err("Invalid input for %s effort. Please enter a non-negative integer.\n", tools[game_state.tool_in_use].name);
			continue;
		}
		valid = 1;
	}
}


static int
calculate_fluoride_used(int tool_dip, int tool_effort)
{
	/*
	 * Calculate the amount of fluoride used based on the selected tool,
	 * dip, effort, and patient species
	 */
	if (tool_dip < 0 || tool_effort < 0) {
		my_print_err("Negative value for tool dip or effort detected. Using default values instead.\n");
		tool_dip = tools[game_state.tool_in_use].dip_amount;
		tool_effort = tools[game_state.tool_in_use].effort;
	}

	int		dip = (tools[game_state.tool_in_use].dip_amount > 0) ? tool_dip : 0;
	int		effort = (tools[game_state.tool_in_use].effort > 0) ? tool_effort : 0;

	int		used = (dip * 2) + (effort * 3);

	/* Adjust fluoride usage based on patient species */
	

	if (game_state.patient_idx == VAMPIRE ) {
		used = (int)(used * 0.8);	/* Vampires need less
						 * fluoride */
	} else if (game_state.patient_idx == ORC ) {
		used = (int)(used * 1.2);	/* Orcs need more fluoride */
	} else if (game_state.patient_idx == WEREWOLF) {
		used = (int)(used * 1.1);	/* Werewolves need a bit more */
	} else if (game_state.patient_idx == SERPENT) {
		used = (int)(used * 0.9);	/* Serpents need slightly
						 * less */
	} else if (game_state.patient_idx == DRAGON) {
		used = (int)(used * 1.5);	/* Dragons need much more */
	}
	


	game_state.fluoride_used = used;
	if (game_state.fluoride_used > game_state.fluoride) {
		my_print_err("Fluoride used (%d) exceeds available fluoride (%d).\n",
			     game_state.fluoride_used, game_state.fluoride);
		return -1;
	}
	game_state.fluoride -= game_state.fluoride_used;
	return game_state.fluoride_used;
}

static void
calculate_fang_health(struct patient_fangs *fang, int tool_dip, int tool_effort)
{
	/*
	 * Calculate health based on tool dip and effort, with patient
	 * species adjustment
	 */
	if (tool_dip < 0 || tool_effort < 0) {
		my_print_err("%s dip and effort must be non-negative. Using default values instead.\n", tools[game_state.tool_in_use].name);
		tool_dip = DEFAULT_TOOL_DIP;
		tool_effort = DEFAULT_TOOL_EFFORT;
	}

	int		health_gain = (tool_dip / 2) + (tool_effort / 3);

	/* Adjust health gain based on patient species */
	if (game_state.patient_idx == VAMPIRE ) {
		health_gain += 2;	/* Vampires respond better to
					 * fluoride */
	} else if (game_state.patient_idx == ORC ) {
		health_gain -= 1;	/* Orcs have tougher fangs */
	} else if (game_state.patient_idx == WEREWOLF ) {
		health_gain += 1;	/* Werewolves heal a bit faster */
	} else if (game_state.patient_idx == SERPENT ) {
		health_gain = (int)(health_gain * 0.8);	/* Serpents are less
							 * affected */
	} else if (game_state.patient_idx == DRAGON ) {
		health_gain = (int)(health_gain * 0.5);	/* Dragons are very
							 * resistant */
	}

	fang->health += health_gain;
	if (fang->health > MAX_HEALTH)
		fang->health = MAX_HEALTH;
	else if (fang->health < 0)
		fang->health = 0;
}

static char    *
fang_health_to_color(int health)
{
	/* Convert fang health to color string */
	if (health >= FANG_HEALTH_HIGH)
		return FANG_COLOR_HIGH;	/* White */
	else if (health >= FANG_HEALTH_MEDIUM)
		return FANG_COLOR_MEDIUM;	/* Dull */
	else
		return FANG_COLOR_LOW;	/* Yellow */
}

static char    *
fang_idx_to_name(int fang_index)
{
	switch (fang_index) {
	case 0:
		return "Maxillary Right Canine";
	case 1:
		return "Maxillary Left Canine";
	case 2:
		return "Mandibular Left Canine";
	case 3:
		return "Mandibular Right Canine";
	}
	return "Unknown Fang";
}

static void
print_fang_logo(void)
{
	my_printf("  /\\     /\\\n");
	my_printf(" (  o___o  )\n");
	my_printf("  \\_ V V __/\n");
	my_printf("     | |\n");
	my_printf("    /   \\\n");
	my_printf("   V     V\n");

}


static void
print_fang_info(const int index, const struct patient_fangs *fang, const int compact_printing)
{
	if (fang == NULL) {
		my_print_err("Fang is NULL.\n");
		return;
	}
	if (compact_printing) {
		print_working_info("  %s: Length: %d, Sharpness: %d, Color: %s, Health: %d\n",
		     fang_idx_to_name(index), fang->length, fang->sharpness,
			  fang_health_to_color(fang->health), fang->health);
		return;
	} else {
		print_working_info("Fang %s:\n", fang_idx_to_name(index));
		print_working_info("  Length: %d\n", fang->length);
		print_working_info("  Sharpness: %d\n", fang->sharpness);
		print_working_info("  Color: %s\n", fang_health_to_color(fang->health));
		print_working_info("  Health: %d\n", fang->health);
	}
}
static void
print_patient_info(const struct patient *patient_ptr, const int compact_printing)
{
	if (compact_printing) {
		my_printf("Creature: %s, Age: %d, Species: %s\n", return_patient_name(game_state.patient_idx), patient_ptr->age, return_patient_species(game_state.patient_idx));

		return;
	} else {
		my_printf("Creature Name: %s\n", return_patient_name(game_state.patient_idx));
		my_printf("Creature Age: %d\n", patient_ptr->age);
		my_printf("Creature Species: %s\n", return_patient_species(game_state.patient_idx));
	}
}

static void
print_tool_info(void)
{
	my_printf("Using tool: %s\n", tools[game_state.tool_in_use].name);
	my_printf("Tool Description: %s\n", tools[game_state.tool_in_use].description);
	my_printf("Tool Dip Amount: %d\n", tools[game_state.tool_in_use].dip_amount);
	my_printf("Tool Effort: %d\n", tools[game_state.tool_in_use].effort);
	my_printf("Tool Durability: %d\n", tools[game_state.tool_in_use].durability);
}


static void
print_game_state(const struct game_state *state)
{
	my_printf("Game State:\n");
	my_printf("  Did you use your dagger: %s\n", state->daggerset ? "Yes" : "No");
	my_printf("  Fluoride remaining: %d\n", state->fluoride);
	my_printf("  Score: %d\n", state->score);
	my_printf("  Turns: %d\n", state->turns);
}



static void
patient_init(struct patient *patient_ptr)
{
	/* Choose a random patient from the patients array */
	int		idx = arc4random_uniform(sizeof(patients) / sizeof(patients[0]));
	struct patient *chosen = &patients[idx];

	/* Copy chosen patient's data */
	patient_ptr->age = chosen->age;
	game_state.patient_idx = idx;
	/* Randomize fangs for this patient */
	randomize_fangs(patient_ptr, 4);
}

/*
 * We want to do certain things on game end, like printing the game state and
 * the patient information, so we define a function to do that
 */

static void
continuation_err(void)
{
	char	       *fangs_formatted;

	my_printf("Ending the game early.\n");
	fangs_formatted = fang_art(UPPER_FANGS, FANG_ROWS_UPPER, patient.fangs[MAXILLARY_LEFT_CANINE].health, patient.fangs[MAXILLARY_RIGHT_CANINE].health, 0);
	my_printf("%s", fangs_formatted);
	fangs_formatted = fang_art(LOWER_FANGS, FANG_ROWS_LOWER, patient.fangs[MANDIBULAR_LEFT_CANINE].health, patient.fangs[MANDIBULAR_RIGHT_CANINE].health, 0);
	my_printf("%s", fangs_formatted);

	sleep(4);
	print_game_state(&game_state);
	print_patient_info(&patient, 0);

	print_fang_info(0, &patient.fangs[0], 1);
	print_fang_info(1, &patient.fangs[1], 1);
	print_fang_info(2, &patient.fangs[2], 1);
	print_fang_info(3, &patient.fangs[3], 1);
	print_tool_info();
	my_printf("Sorry you couldn't finish Buffy the Fluoride Dispenser: Fang Edition!\n");
}

static int inline 
all_fangs_healthy(const patient_type *patient)
{
		int four_healthy_fangs = 0;

		for (int i = 0; i < 4; i++) {
			if (patient->fangs[i].health < MAX_HEALTH) {
				four_healthy_fangs = -1 ;
				break;
			}
		}
		return four_healthy_fangs;
}

static int
apply_fluoride_to_fangs(void)
{
	int		tool_dip = DEFAULT_TOOL_DIP;
	int		tool_effort = DEFAULT_TOOL_EFFORT;

	/*
	 * Next we will loop through the fangs asking the user how much to
	 * dip the tool in the fluoride and how much effort to apply to the
	 * current fang
	 */
	print_fang_logo();
	my_printf("Welcome to Buffy the Fluoride Dispenser: Fang Edition!\n");
	print_patient_info(&patient, 1);


	my_refresh();
	sleep(4);

	int		cleaning = 1;
	do {
		char		answer[4];
		char	       *fangs_formatted;

		for (int i = 0; i < 4; i++) {
			/* skip fangs that are already healthy */
			if (patient.fangs[i].health >= MAX_HEALTH) {
				my_printf("Fang %s is already healthy and shiny!\n", fang_idx_to_name(i));
				continue;
			}
			/*
			 * determine if upper or lower fang and call
			 * fang_art() passing in values for left and right
			 * fang
			 */

			my_werase();

			if (IS_UPPER_FANG) {
				fangs_formatted = fang_art(UPPER_FANGS, FANG_ROWS_UPPER, patient.fangs[MAXILLARY_LEFT_CANINE].health, patient.fangs[MAXILLARY_RIGHT_CANINE].health, game_state.using_curses);
			} else {
				fangs_formatted = fang_art(LOWER_FANGS, FANG_ROWS_LOWER, patient.fangs[MANDIBULAR_LEFT_CANINE].health, patient.fangs[MANDIBULAR_RIGHT_CANINE].health, game_state.using_curses);
			}

			my_printf("%s", fangs_formatted);
			print_working_info("Applying fluoride to %s's fang %s:\n", return_patient_name(game_state.patient_idx), fang_idx_to_name(i));

			print_fang_info(i, &patient.fangs[i], 1);
			print_stats_info(game_state.fluoride, game_state.score, game_state.turns);
			my_refresh();
			get_provider_input(&tool_dip, &tool_effort, game_state.last_tool_dip, game_state.last_tool_effort);
			game_state.last_tool_dip = tool_dip;
			game_state.last_tool_effort = tool_effort;
			calculate_fang_health(&patient.fangs[i], tool_dip, tool_effort);

			game_state.score += BONUS_FANG_CLEANED;

			if (patient.fangs[i].health >= MAX_HEALTH)
				game_state.score += BONUS_FANG_HEALTH;

			if (calculate_fluoride_used(tool_dip, tool_effort) == -1)
				goto continuation_fail;

			if (game_state.using_curses)
				print_stats_info(game_state.fluoride, game_state.score, game_state.turns);
			my_refresh();
		}

		game_state.turns++;
		game_state.score += BONUS_TURN_COMPLETE;



		if (all_fangs_healthy(&patient) == 0)
			goto success;





		get_input("Continue applying fluoride to fangs? (y/q/s):", answer, sizeof(answer));
		if (answer[0] == 'y' || answer[0] == 'Y' || answer[0] == '\n' || strlen(answer) == 0) {
			/* All tools use some fluoride */
			my_printf("%s applies fluoride to %s's fangs with the %s.\n", game_state.character_name, return_patient_name(game_state.patient_idx), tools[game_state.tool_in_use].name);
			my_printf("%s dip effort: %d\n", tools[game_state.tool_in_use].name, tool_effort);
			game_state.fluoride_used += calculate_fluoride_used(tool_dip, tool_effort);
		} else if (answer[0] == 'q' || answer[0] == 'Q') {
			my_printf("%s quits the game.\n", game_state.character_name);
			cleaning = 0;
			goto quit_game;
		} else if (answer[0] == 's' || answer[0] == 'S') {
			goto save_game;
		} else {
			my_print_err("Thanks for playing.\n");
			goto success;
		}
	} while (cleaning);

success:
	end_curses();
	my_printf("%s has successfuly cleaned all of %s's fangs.\n", game_state.character_name, return_patient_name(game_state.patient_idx));

	game_state.score += BONUS_ALL_HEALTH;
	print_game_state(&game_state);
	return 0;

save_game:
	end_curses();
	default_game_save();
	print_game_state(&game_state);
	return 0;

quit_game:
	end_curses();
	my_printf("%s chooses to quit!\n", game_state.character_name);
	print_game_state(&game_state);
	return 0;

continuation_fail:
	end_curses();
	continuation_err();
	return 0;
}



static int
main_program(int reloadflag)
{
	/*
	 * If we are reloading the game state, we do not need to initialize
	 * it again
	 */
	if (!reloadflag) {
		init_game_state(game_state.bflag);
		patient_init(&patient);
	}

	/* Use dagger only with --daggerset option */
	if (!game_state.daggerset) {
		game_state.tool_dip = DEFAULT_TOOL_DIP;
		game_state.tool_effort = DEFAULT_TOOL_EFFORT;
	} else {
		game_state.tool_dip = DEFAULT_DAGGER_DIP;
		game_state.tool_effort = DEFAULT_DAGGER_EFFORT;
	}

	initalize_curses();

	return apply_fluoride_to_fangs();
}

#ifndef __UNIT_TEST__
int
main(int argc, char *argv[])
{
	int		bflag = 0;
	int		ch;
	int		fflag = 0;
	int		curses = 0;
	char		login_name[256];

	/* options descriptor */
	static struct option longopts[] = {
		{"not-buffy", no_argument, NULL, 'b'},
		{"curses", no_argument, NULL, 'c'},
		{"colorized", no_argument, &game_state.color_mode, 1},	/* Alias for curses with
									 * color */
		{"help", no_argument, NULL, '?'},
		{"version", no_argument, NULL, 'v'},
		{"fluoride-file", required_argument, NULL, 'f'},
		{"daggerset", no_argument, &game_state.daggerset, 1},
	{NULL, 0, NULL, 0}};

#ifdef __OpenBSD__

	if (pledge("stdio rpath wpath cpath unveil proc", NULL) == -1)
		errx(1, "pledge");
#endif

	while ((ch = getopt_long(argc, argv, "cbvf:", longopts, NULL)) != -1)
		switch (ch) {
		case 'v':
			printf("%s version %s\n", __progname, VERSION);
			exit(EXIT_SUCCESS);
		case 'c':
			curses++;
			if (curses > 1) {
				game_state.using_curses = 1;
				game_state.color_mode = 1;
				set_using_curses(game_state.using_curses);
				set_color_mode(game_state.color_mode);
			} else {
				game_state.using_curses = 1;
				game_state.color_mode = 0;
				set_using_curses(game_state.using_curses);
				set_color_mode(game_state.color_mode);
			}
			break;
		case 'b':
			/*
			 * not named buffy we will use the user login name
			 * and do not care if the user is root we will lookup
			 * the user name in the password database we use
			 * pledge and unveil so the game is safe for all
			 * users
			 */

			if (getlogin() == NULL)
				errx(1, "Unable to determine user name");

			/*
			 * use getpwent to get the user name and store in
			 * login_name
			 */
			if (getlogin_r(login_name, sizeof(login_name)) != 0)
				errx(1, "Unable to get login name");

			strlcpy(character_name, login_name, sizeof(game_state.character_name));
			game_state.character_name = character_name;

			fprintf(stderr, "%s is ready to apply fluoride to fangs.\n", game_state.character_name);

			bflag = 1;
			break;
		case 'f':
			fflag = 1;

			if (validate_game_file(optarg) == 1)
				errx(1, "Game file %s is not a valid file", optarg);
			/*
			 * If the file is valid, we will load all game data
			 * from it
			 */
			load_game_state(optarg, &game_state, sizeof(game_state), &patient, sizeof(patient), character_name);
			game_state.character_name = character_name;
			set_using_curses(game_state.using_curses);
			set_color_mode(game_state.color_mode);

			break;
		case 0:
			if (game_state.daggerset)
				fprintf(stderr, "Player will use a dagger to "
					"apply fluoride to fangs\n");
			if (game_state.color_mode == 1) {
				game_state.using_curses = 1;
				set_using_curses(game_state.using_curses);
				set_color_mode(game_state.color_mode);
			}
			break;
		default:
			usage();
		}
	argc -= optind;
	/* argv += optind; */

	if (argc != 0)
		usage();

	/*
	 * Initialize game state if fflag is not since we are not restoring a
	 * saved game
	 */

	if (!fflag)
		init_game_state(bflag);
#ifdef __OpenBSD__

	if (!fflag)
		if (unveil(save_path, "rwc") == -1) {
			errx(1, "unveil");
			return EXIT_FAILURE;

			if (unveil(NULL, NULL) == -1) {
				errx(1, "unveil lock");
				return EXIT_FAILURE;
			}
		}

	if (pledge("stdio rpath wpath cpath proc unveil", NULL) == -1)
		errx(1, "pledge");
#endif

	exit(main_program(fflag));
}
#else
#include "unittest/unittest.c"
#endif
