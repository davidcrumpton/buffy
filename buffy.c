#include <sys/stat.h>

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

extern int  validate_game_file(char *optarg);
static int	exit_game(void);


char		character_name[256];
char		creature_name[256];
char 		save_path[FILENAME_MAX + 1];

game_state_type game_state;
creature_type	creature;



/* Array of fangs with proper dental names and numbers */
fang_info_type  fang_names[] = {
	{"Maxillary Right Canine", 6},
	{"Maxillary Left Canine", 11},
	{"Mandibular Left Canine", 22},
{"Mandibular Right Canine", 27}};

/* a structure to use for the selected tool */

tool tools[] = {
	{"Buffy's Fingernail", "A sharp fingernail for cleaning teeth", 5, 0, 10, 1, 50, 0},
	{"Rock", "A rough rock for scraping teeth", 8, 0, 15, 2, 30, 0},
	{"Shark Tooth", "A sharp shark tooth for cleaning teeth", 6, 0, 20, 3, 40, 0},
	{"Wooden Dagger", "A wooden dagger for applying fluoride", 10, DEFAULT_DAGGER_DIP, DEFAULT_DAGGER_EFFORT, 5, 100, 0},
	{"Bronze Dagger", "A bronze dagger for applying fluoride", 12, DEFAULT_DAGGER_DIP, DEFAULT_DAGGER_EFFORT, 7, 150, 0},
	{"Steel Dagger", "A steel dagger for applying fluoride", 14, DEFAULT_DAGGER_DIP, DEFAULT_DAGGER_EFFORT, 10, 200, 0}
};

/* we will have vampire, orc, werewolf, serpent, dragon */
struct creature creatures[] = {
	{DEFAULT_CREATURE_AGE, "Dracula",  "Vampire", {{0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}}},
	{DEFAULT_CREATURE_AGE, "Gorath", "Orc", {{0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}}},
	{DEFAULT_CREATURE_AGE, "Fenrir",  "Werewolf", {{0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}}},
	{DEFAULT_CREATURE_AGE + 50 /* Serpents are older */, "Nagini", "Serpent", {{0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}, {0, 0, NULL, 0}}},
	{DEFAULT_CREATURE_AGE + 100 /* Dragons are ancient */, "Smaug",  "Dragon", {{0, 0, NULL , 100}}} /* Dragon has max health fangs */
};

extern char    *__progname;

static int	__dead
usage(void)
{
	fprintf(stderr, "%s: [ -b | --not-named-buffy ] [ -f | --fluoride-file <file> ] [ --daggerset ]\n", __progname);
	exit(EXIT_FAILURE);
}

/* choose random tool returns one of the array index from 0 -5 */
int choose_random_tool(int isdaggerset)
{
	/* Generate a random number between 0 and 5 */
	if (isdaggerset) {
		return arc4random_uniform(3) + 3; /* Return index 3, 4, or 5 for daggers */
	} else {
		return arc4random_uniform(3); /* Return index 0, 1, or 2 for non-daggers */
	}
}

/* Initialize the game state with default values */
static void
init_game_state(int bflag)
{
	/* Initialize game state with default values */
	game_state.daggerset = DEFAULT_DAGGERSET;
	game_state.flouride = DEFAULT_FLOURIDE;
	game_state.dagger_dip = DEFAULT_DAGGER_DIP;
	game_state.dagger_effort = DEFAULT_DAGGER_EFFORT;
	game_state.flouride_used = DEFAULT_FLOURIDE_USED;
	game_state.bflag = bflag;
	game_state.score = DEFAULT_SCORE;
	game_state.turns = DEFAULT_TURNS;

	/* If bflag is set, we will use the user login name */
	if (bflag) {
		char		login_name[256];
	if (getlogin_r(login_name, sizeof(login_name)) != 0)
		err(1, "Unable to get login name");
	strlcpy(character_name, login_name, sizeof(game_state.character_name));
	} else
	strlcpy(character_name, DEFAULT_CHARACTER_NAME, sizeof(DEFAULT_CHARACTER_NAME));

	game_state.character_name = character_name;
	game_state.tool_in_use = choose_random_tool(game_state.daggerset);
	/* Initialize save path */
	const char *home = getenv("HOME");
	if (!home) {
		fprintf(stderr, "Unable to determine HOME directory.\n");
		exit_game();
	}
	
	snprintf(save_path, sizeof(save_path), "%s/%s", home, DEFAULT_SAVE_FILE);
}
static void
randomize_fangs(struct creature *fanged_beast, int count)
{
    for (int i = 0; i < count; i++) {
        // arc4random_uniform(n) returns 0 to n-1, so add offset as needed
        fanged_beast->fangs[i].length = 4 + arc4random_uniform(3);     // 4–6
        fanged_beast->fangs[i].sharpness = 5 + arc4random_uniform(4);  // 5–8

        // Bias health toward lower values (dirty teeth)
        int r = arc4random_uniform(MAX_HEALTH);
        if (r < 60)
            fanged_beast->fangs[i].health = 60 + arc4random_uniform(11); // 60–70
        else if (r < 90)
            fanged_beast->fangs[i].health = 71 + arc4random_uniform(10); // 71–80
        else
            fanged_beast->fangs[i].health = 90 + arc4random_uniform(11); // 90–100

        // Set color based on health
        if (fanged_beast->fangs[i].health >= 90)
            fanged_beast->fangs[i].color = "white";
        else if (fanged_beast->fangs[i].health >= 80)
            fanged_beast->fangs[i].color = "dull";
        else
            fanged_beast->fangs[i].color = "yellow";
    }
}



static void
ask_slayer(int *dagger_dip, int *dagger_effort)
{
	int		valid = 0;
	char 	ch = 0;

	while (!valid) {
		printf("How much to dip the %s in the fluoride? ", tools[game_state.tool_in_use].name);
		if (scanf("%d", dagger_dip) != 1 || *dagger_dip < 0) {
			fprintf(stderr, "Invalid input for %s dip. Please enter a non-negative integer.\n", tools[game_state.tool_in_use].name);
			while ( (ch=getchar()) != '\n' && ch != EOF)
				;
			/* clear input buffer */
			continue;
		}
		valid = 1;
	}

	valid = 0;
	while (!valid) {
		printf("How much effort to apply to the fang? ");
		if (scanf("%d", dagger_effort) != 1 || *dagger_effort < 0) {
			fprintf(stderr, "Invalid input for %s effort. Please enter a non-negative integer.\n", tools[game_state.tool_in_use].name);
			while ( (ch=getchar()) != '\n' && ch != EOF)
				;
			/* clear input buffer */
			continue;
		}
		valid = 1;
	}
}


int
calculate_flouride_used(int dagger_dip, int dagger_effort)
{
	/* Calculate the amount of fluoride used based on the selected tool, dip, effort, and creature species */
	if (dagger_dip < 0 || dagger_effort < 0) {
		fprintf(stderr, "Negative value for dagger dip or effort detected. Using default values instead.\n");
		dagger_dip = tools[game_state.tool_in_use].dip_amount;
		dagger_effort = tools[game_state.tool_in_use].effort;
	}

	int dip = (tools[game_state.tool_in_use].dip_amount > 0) ? dagger_dip : 0;
	int effort = (tools[game_state.tool_in_use].effort > 0) ? dagger_effort : 0;

	int used = (dip * 2) + (effort * 3);

	/* Adjust fluoride usage based on creature species */
	if (strcmp(creature.species, "Vampire") == 0) {
		used = (int)(used * 0.8); // Vampires need less fluoride
	} else if (strcmp(creature.species, "Orc") == 0) {
		used = (int)(used * 1.2); // Orcs need more fluoride
	} else if (strcmp(creature.species, "Werewolf") == 0) {
		used = (int)(used * 1.1); // Werewolves need a bit more
	} else if (strcmp(creature.species, "Serpent") == 0) {
		used = (int)(used * 0.9); // Serpents need slightly less
	} else if (strcmp(creature.species, "Dragon") == 0) {
		used = (int)(used * 1.5); // Dragons need much more
	}

	/* Some tools may not use fluoride at all */
	if (tools[game_state.tool_in_use].dip_amount == 0)
		used = 0;

	game_state.flouride_used = used;
	if (game_state.flouride_used > game_state.flouride) {
		printf("Fluoride used (%d) exceeds available fluoride (%d).\n",
			   game_state.flouride_used, game_state.flouride);
		fprintf(stderr, "Not enough fluoride available.\n");
		exit_game();
	}
	game_state.flouride -= game_state.flouride_used;
	return game_state.flouride_used;
}

void
calculate_fang_health(struct creature_fangs *fang, int dagger_dip, int dagger_effort)
{
	/* Calculate health based on dagger dip and effort, with creature species adjustment */
	if (dagger_dip < 0 || dagger_effort < 0) {
		fprintf(stderr, "%s dip and effort must be non-negative. Using default values instead.\n", tools[game_state.tool_in_use].name);
		dagger_dip = DEFAULT_DAGGER_DIP;
		dagger_effort = DEFAULT_DAGGER_EFFORT;
	}

	int health_gain = (dagger_dip / 2) + (dagger_effort / 3);

	/* Adjust health gain based on creature species */
	if (strcmp(creature.species, "Vampire") == 0) {
		health_gain += 2; // Vampires respond better to fluoride
	} else if (strcmp(creature.species, "Orc") == 0) {
		health_gain -= 1; // Orcs have tougher fangs
	} else if (strcmp(creature.species, "Werewolf") == 0) {
		health_gain += 1; // Werewolves heal a bit faster
	} else if (strcmp(creature.species, "Serpent") == 0) {
		health_gain = (int)(health_gain * 0.8); // Serpents are less affected
	} else if (strcmp(creature.species, "Dragon") == 0) {
		health_gain = (int)(health_gain * 0.5); // Dragons are very resistant
	}

	fang->health += health_gain;
	if (fang->health > MAX_HEALTH)
		fang->health = MAX_HEALTH;
	else if (fang->health < 0)
		fang->health = 0;

	// Set color according to health
	if (fang->health >= 9)
		fang->color = FANG_COLOR_HIGH;
	else if (fang->health >= 8)
		fang->color = FANG_COLOR_MEDIUM;
	else
		fang->color = FANG_COLOR_LOW;
}

char	       *
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
print_fang_info(int index, struct creature_fangs *fang)
{
	printf("Fang %s:\n", fang_idx_to_name(index));
	printf("  Length: %d\n", fang->length);
	printf("  Sharpness: %d\n", fang->sharpness);
	printf("  Color: %s\n", fang->color);
	printf("  Health: %d\n", fang->health);
}
static void
print_creature_info(struct creature *fanged_beast)
{
	printf("Creature Name: %s\n", fanged_beast->name);
	printf("Creature Age: %d\n", fanged_beast->age);
	printf("Creature Species: %s\n", fanged_beast->species);
	for (int i = 0; i < 4; i++) {
		printf("Fang %s:\n", fang_idx_to_name(i));
		printf("  Length: %d\n", fanged_beast->fangs[i].length);
		printf("  Sharpness: %d\n", fanged_beast->fangs[i].sharpness);
		printf("  Color: %s\n", fanged_beast->fangs[i].color);
		printf("  Health: %d\n", fanged_beast->fangs[i].health);
	}
}
static void
print_tool_info(void)
{
	printf("Using tool: %s\n", tools[game_state.tool_in_use].name);
	printf("Tool Description: %s\n", tools[game_state.tool_in_use].description);
	printf("Tool Dip Amount: %d\n", tools[game_state.tool_in_use].dip_amount);
	printf("Tool Effort: %d\n", tools[game_state.tool_in_use].effort);
	printf("Tool Durability: %d\n", tools[game_state.tool_in_use].durability);
}
static void
print_flouride_info(void)
{
	printf("Remaining fluoride: %d\n", game_state.flouride);
}

static void
print_game_state(struct game_state *state)
{
	printf("Game State:\n");
	printf("  Did you use your dagger: %s\n", state->daggerset ? "Yes" : "No");
	printf("  Fluoride remaining: %d\n", state->flouride);
	printf("  Final %s Dip: %d\n", tools[game_state.tool_in_use].name, state->dagger_dip);
	printf("  Final %s Effort: %d\n", tools[game_state.tool_in_use].name, state->dagger_effort);
	printf("  Fluoride Used: %d\n", state->flouride_used);
	printf("  Score: %d\n", state->score);
	printf("  Turns: %d\n", state->turns);
}

static void
creature_init(struct creature *fanged_beast)
{
	// Choose a random creature from the creatures array
	int idx = arc4random_uniform(sizeof(creatures) / sizeof(creatures[0]));
	struct creature *chosen = &creatures[idx];

	// Copy chosen creature's data
	fanged_beast->age = chosen->age;
	strlcpy(creature_name, chosen->name, sizeof(creature_name));
	fanged_beast->name = creature_name;
	fanged_beast->species = chosen->species;

	// Randomize fangs for this creature
	randomize_fangs(fanged_beast, 4);
}


int
apply_fluoride_to_fangs(void)
{
	int		dagger_dip = DEFAULT_DAGGER_DIP;
	int		dagger_effort = DEFAULT_DAGGER_EFFORT;

	creature_init(&creature);
	/*
	 * Next we will loop through the fangs asking the user how much to
	 * dip the dagger in the flouride and how much effort to apply to the
	 * current fang
	 */


	printf("Creature Information:\n");
	printf("Creature Name: %s\n", creature.name);
	printf("Creature Age: %d\n", creature.age);
	printf("Creature Species: %s\n", creature.species);
	int		cleaning = 1;
	do {
		char		answer[4];
		print_flouride_info();
		for (int i = 0; i < 4; i++) {
			/* skip fangs that are already healthy */
				if (creature.fangs[i].health >= MAX_HEALTH) {
				printf("Fang %s is already healthy and shiny!\n", fang_idx_to_name(i));
				continue;
			}
			printf("Applying fluoride to %s's fang %s:\n", creature.name, fang_idx_to_name(i));
			printf("Fang %s details:\n", fang_idx_to_name(i));
			printf("  Length: %d\n", creature.fangs[i].length);
			printf("  Sharpness: %d\n", creature.fangs[i].sharpness);
			printf("  Color: %s\n", creature.fangs[i].color);
			printf("  Health: %d\n", creature.fangs[i].health);
			ask_slayer(&dagger_dip, &dagger_effort);
			calculate_fang_health(&creature.fangs[i], dagger_dip, dagger_effort);

			game_state.score += BONUS_FANG_CLEANED;

			if (creature.fangs[i].health >= MAX_HEALTH)
				game_state.score += BONUS_FANG_HEALTH;

			calculate_flouride_used(dagger_dip, dagger_effort);

			print_fang_info(i, &creature.fangs[i]);
		}
		/*
		 * the game should check all teeth to see if health is 10. if
		 * health is 10, then we should print succss message and some
		 * pleasing states for the user then exit
		 */
		int		all_fangs_healthy = 1;
		for (int i = 0; i < 4; i++) {
			if (creature.fangs[i].health < MAX_HEALTH) {
				all_fangs_healthy = 0;
				break;
			}
		}
		if (all_fangs_healthy) {
			printf("All of %s's fangs are now healthy and shiny!\n", creature.name);
			printf("%s has successfully applied fluoride to %s's fangs.\n", game_state.character_name, creature.name);
			printf("%s is ready to slay more fangs!\n", game_state.character_name);
			game_state.score += BONUS_ALL_HEALTH;
			goto success;
		}
		game_state.turns++;

		game_state.score += 5;
		printf("Apply fluoride to %s's fangs? (y/n/q/s): ", creature.name);
		scanf("%3s", answer);
		if (answer[0] == 'y' || answer[0] == 'Y') {
			if (game_state.daggerset) {
				printf("%s applies fluoride to %s's fangs with the %s.\n", game_state.character_name, creature.name, tools[game_state.tool_in_use].name);
				printf("Dagger dip: %d, %s effort: %d\n", dagger_dip, tools[game_state.tool_in_use].name, dagger_effort);
				game_state.flouride_used += calculate_flouride_used(dagger_dip, dagger_effort);
			}
		} else if (answer[0] == 'n' || answer[0] == 'N') {
			printf("%s decides not to apply fluoride to %s's fangs.\n", game_state.character_name, creature.name);
			cleaning = 0;
			/* Exit the loop */
		} else if (answer[0] == 'q' || answer[0] == 'Q') {
			printf("%s quits the game.\n", game_state.character_name);
			goto success;
		} else if (answer[0] == 's' || answer[0] == 'S') {
			save_game(save_path);
			printf("Game saved to %s\n", save_path);
			cleaning = 0;
			/* Exit the loop after saving */
		} else {
			fprintf(stderr, "Thanks for playing.\n");
			goto success;
		}
	} while (cleaning);
	printf("%s has finished applying fluoride to %s's fangs.\n", game_state.character_name, creature.name);

success:
	printf("Remaining fluoride: %d\n", game_state.flouride);
	print_creature_info(&creature);
	print_tool_info();
	print_game_state(&game_state);
	return EXIT_SUCCESS;
}

/*
 * We want to do certain things on game end, like printing the game state and
 * the creature information, so we define a function to do that
 */

static int	__dead
exit_game(void)
{
	printf("Exiting the game...\n");
	print_game_state(&game_state);
	print_creature_info(&creature);
	print_tool_info();
	printf("Thank you for playing Buffy the Fang Slayer: Fluoride Edition!\n");
	printf("Final Score: %d\n", game_state.score);
	printf("Turns taken: %d\n", game_state.turns);
	printf("Creature Age: %d\n", creature.age);
	print_flouride_info();
	exit(EXIT_SUCCESS);
}


static int
main_program(int reloadflag)
{
	/*
	 * If we are reloading the game state, we do not need to initialize
	 * it again
	 */
	if (!reloadflag)
		init_game_state(game_state.bflag);

	/* If daggerset is not set, we will not use the dagger */
	if (!game_state.daggerset)
		game_state.dagger_dip = 0;
	else
		game_state.dagger_dip = DEFAULT_DAGGER_DIP;
	/* Default dagger dip value */

	game_state.dagger_effort = DEFAULT_DAGGER_EFFORT;
	/* Default dagger effort value */

	printf("Welcome to Buffy the Fang Slayer: Fluoride Edition!\n");
	printf("%s is ready to apply fluoride to %s's fangs.\n", game_state.character_name, creature.name ? creature.name : "the patient");

	return apply_fluoride_to_fangs();
}

#ifndef __UNIT_TEST__
int
main(int argc, char *argv[])
{
	int		bflag = 0;
	int		ch;
	int		fflag = 0;
	char		login_name[256];

	/* options descriptor */
	static struct option longopts[] = {
		{"not-buffy", no_argument, NULL, 'b'},
		{"fluoride-file", required_argument, NULL, 'f'},
		{"daggerset", no_argument, &game_state.daggerset, 1},
	{NULL, 0, NULL, 0}};

	while ((ch = getopt_long(argc, argv, "bf:", longopts, NULL)) != -1)
		switch (ch) {
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

			/* use getpwent to get the user name and store in login_name */
			if (getlogin_r(login_name, sizeof(login_name)) != 0)
				err(1, "Unable to get login name");

			strlcpy(character_name, login_name, sizeof(game_state.character_name));
			game_state.character_name = character_name;

			fprintf(stderr, "%s is ready to apply fluoride to %s's fangs.\n", game_state.character_name, creature.name);


			bflag = 1;
			break;
		case 'f':
			fflag = 1;
			validate_game_file(optarg);

			if (game_state.flouride < 0 || game_state.dagger_dip < 0 || game_state.dagger_effort < 0 || game_state.flouride_used < 0 || game_state.bflag < 0 || game_state.daggerset < 0)
				errx(1, "Invalid game state in %s", optarg);
			fprintf(stderr, "Fluoride: %d, Tool Dip: %d, Tool Effort: %d, Fluoride Used: %d, Bflag: %d, Daggerset: %d\n",
				game_state.flouride, game_state.dagger_dip, game_state.dagger_effort,
				game_state.flouride_used, game_state.bflag, game_state.daggerset);

			if (game_state.flouride < 0 || game_state.dagger_dip < 0 || game_state.dagger_effort < 0 || game_state.flouride_used < 0 || game_state.bflag < 0 || game_state.daggerset < 0)
				errx(1, "Invalid game state in %s", optarg);
			break;
		case 0:
			if (game_state.daggerset)
				fprintf(stderr, "%s will use the %s to "
					"apply fluoride to %s's teeth\n", game_state.character_name, tools[game_state.tool_in_use].name, creature.name);
			break;
		default:
			usage();
		}
	argc -= optind;
	argv += optind;

	if (argc != 0)
		usage();

	/*
	 * Initialize game state if fflag is not set we do not care about any
	 * other options since options are stored in the game_state struct
	 */

	if (!fflag)
		init_game_state(bflag);
#ifdef __OpenBSD__

	if (pledge("stdio rpath wpath cpath unveil", NULL) == -1)
		err(1, "pledge");
	
	if (unveil(save_path, "rwc") == -1) {
		err(1, "unveil");
		return EXIT_FAILURE;
	}

	if (unveil(NULL, NULL) == -1) {
		err(1, "unveil lock");
		return EXIT_FAILURE;
	}
#endif
	exit(main_program(fflag));
}
#else
#include "unittest/unittest.c"
#endif
