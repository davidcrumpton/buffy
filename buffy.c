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

extern void validate_game_file(char *optarg);
static int	exit_game(void);


char		character_name[256];
char		creature_name[256];



struct game_state game_state;
struct creature	creature;

/* Define a struct for detailed fang information */
struct fang_info {
	const char     *name;
	int		tooth_number;
		      /* Universal tooth numbering system */
};

/* Array of fangs with proper dental names and numbers */
struct fang_info fang_names[] = {
	{"Maxillary Right Canine", 6},
	{"Maxillary Left Canine", 11},
	{"Mandibular Left Canine", 22},
{"Mandibular Right Canine", 27}};

extern char    *__progname;

static int	__dead
usage(void)
{
	fprintf(stderr, "%s: [ -b | --not-named-buffy ] [ -f | --fluoride-file <file> ] [ --daggerset ]\n", __progname);
	exit(EXIT_FAILURE);
}

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
	while (!valid) {
		printf("How much to dip the dagger in the fluoride? ");
		if (scanf("%d", dagger_dip) != 1 || *dagger_dip < 0) {
			fprintf(stderr, "Invalid input for dagger dip. Please enter a non-negative integer.\n");
			while (getchar() != '\n')
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
			fprintf(stderr, "Invalid input for dagger effort. Please enter a non-negative integer.\n");
			while (getchar() != '\n')
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
	/* Calculate the amount of fluoride used based on dagger dip and effort */
		if (dagger_dip < 0 || dagger_effort < 0) {
		fprintf(stderr, "Negative value for dagger dip or effort detected. Using default values instead.\n");
		dagger_dip = DEFAULT_DAGGER_DIP;
		dagger_effort = DEFAULT_DAGGER_EFFORT;
	}

	game_state.flouride_used = (dagger_dip * 2) + (dagger_effort * 3);
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
	/* Calculate health based on dagger dip and effort */
		if (dagger_dip < 0 || dagger_effort < 0) {
		fprintf(stderr, "Dagger dip and effort must be non-negative. Using default values instead.\n");
		dagger_dip = DEFAULT_DAGGER_DIP;
		dagger_effort = DEFAULT_DAGGER_EFFORT;
	}

	fang->health += (dagger_dip / 2) + (dagger_effort / 3);
	if (fang->health > MAX_HEALTH)
		fang->health = MAX_HEALTH;
	/* Cap health at 10 */
		else if (fang->health < 0)
		fang->health = 0;
	/* Ensure health does not go below 0 */

		// Set color according to health
		if (fang->health >= 9)
		fang->color = "white";
	/* max health */
		else if (fang->health >= 8)
		fang->color = "dull";
	/* medium health */
		else
		fang->color = "yellow";
	/* low health */
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
	printf("  Final Dagger Dip: %d\n", state->dagger_dip);
	printf("  Final Dagger Effort: %d\n", state->dagger_effort);
	printf("  Fluoride Used: %d\n", state->flouride_used);
	printf("  Score: %d\n", state->score);
	printf("  Turns: %d\n", state->turns);
}

static void
creature_init(struct creature *fanged_beast)
{
	fanged_beast->age = DEFAULT_CREATURE_AGE;
	strlcpy(creature_name, DEFAULT_CREATURE_NAME, sizeof(DEFAULT_CREATURE_NAME));
	fanged_beast->name = creature_name;
	fanged_beast->species = DEFAULT_CREATURE_SPECIES;
	randomize_fangs(fanged_beast, 4);
}


int
apply_fluoride_to_fangs(void)
{
	int		dagger_dip = DEFAULT_DAGGER_DIP;
	int		dagger_effort = DEFAULT_DAGGER_EFFORT;

	if (game_state.daggerset)
		printf("%s will use her dagger to apply fluoride to %s's teeth\n", game_state.character_name, creature.name);
	else
		printf("%s will not use her dagger to apply fluoride to %s's teeth\n", game_state.character_name, creature.name);

	creature_init(&creature);
	/*
	 * Next we will loop through the fangs asking the user how much to
	 * dip the dagger in the flouride and how much effort to apply to the
	 * current fang
	 */

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
				printf("%s applies fluoride to %s's fangs with her dagger.\n", game_state.character_name, creature.name);
				printf("Dagger dip: %d, Dagger effort: %d\n", dagger_dip, dagger_effort);
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
			char		save_file[FILENAME_MAX + 1];
			printf("Enter filename to save the game: ");
			scanf("%1024s", save_file);
			save_game(save_file);
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
	printf("%s is ready to apply fluoride to %s's fangs.\n", game_state.character_name, creature.name);

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
			fprintf(stderr, "Fluoride: %d, Dagger Dip: %d, Dagger Effort: %d, Fluoride Used: %d, Bflag: %d, Daggerset: %d\n",
				game_state.flouride, game_state.dagger_dip, game_state.dagger_effort,
				game_state.flouride_used, game_state.bflag, game_state.daggerset);

			if (game_state.flouride < 0 || game_state.dagger_dip < 0 || game_state.dagger_effort < 0 || game_state.flouride_used < 0 || game_state.bflag < 0 || game_state.daggerset < 0)
				errx(1, "Invalid game state in %s", optarg);
			break;
		case 0:
			if (game_state.daggerset)
				fprintf(stderr, "%s will use the dagger to "
					"apply fluoride to %s's teeth\n", game_state.character_name, creature.name);
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
	const char     *home = getenv("HOME");
	if (!home) {
		err(1, "Unable to determine HOME directory.\n");
		return EXIT_FAILURE;
	}
	if (unveil(home, "rw") == -1) {
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
