#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <err.h>

#include <time.h>

#include "buffy.h"

// Define a struct for detailed fang information
struct fang_info
{
	const char *name;
	int tooth_number; // Universal tooth numbering system
};

// Array of fangs with proper dental names and numbers
struct fang_info fang_names[] = {
	{"Maxillary Right Canine", 6},
	{"Maxillary Left Canine", 11},
	{"Mandibular Left Canine", 22},
	{"Mandibular Right Canine", 27}};

extern char *__progname;

static int __dead
usage(void)
{
	fprintf(stderr, "%s: [ -b | --buffy ] [ -f | --flouride <file> ] [ --daggerset ]\n", __progname);
	exit(EXIT_FAILURE);
}

static void
init_game_state(int bflag)
{
	// Initialize game state variables
	game_state.daggerset = 0;	  // Default dagger state
	game_state.flouride = 200;	  // Default fluoride amount
	game_state.dagger_dip = 0;	  // Default dagger dip
	game_state.dagger_effort = 0; // Default dagger effort
	game_state.flouride_used = 0; // Default fluoride used
	game_state.bflag = bflag;	  // Default bflag state
}
static void
randomize_fangs(struct vampire *vamp, int count)
{
	int i;
	srand((unsigned int)time(NULL));
	for (i = 0; i < count; i++)
	{
		vamp->fangs[i].length = 4 + rand() % 3;	   // 4-6
		vamp->fangs[i].sharpness = 5 + rand() % 4; // 5-8

		// Bias health toward lower values (dirty teeth)
		int r = rand() % 100;
		if (r < 60)
			vamp->fangs[i].health = 60 + rand() % 11; // 60-70 (60% chance)
		else if (r < 90)
			vamp->fangs[i].health = 71 + rand() % 10; // 71-80 (30% chance)
		else
			vamp->fangs[i].health = 90 + rand() % 11; // 90-100 (10% chance)

		if (vamp->fangs[i].health >= 90)
			vamp->fangs[i].color = "white"; // max health
		else if (vamp->fangs[i].health >= 80)
			vamp->fangs[i].color = "dull"; // medium health
		else
			vamp->fangs[i].color = "yellow"; // low health
	}
}

static void __dead
ask_slayer(int *dagger_dip, int *dagger_effort)
{
	printf("How much to dip the dagger in the fluoride? ");
	if (scanf("%d", dagger_dip) != 1)
	{
		fprintf(stderr, "Invalid input for dagger dip.\n");
		exit(EXIT_FAILURE);
	}

	printf("How much effort to apply to the fang? ");
	if (scanf("%d", dagger_effort) != 1)
	{
		fprintf(stderr, "Invalid input for dagger effort.\n");
		exit(EXIT_FAILURE);
	}
}

int calculate_flouride_used(int dagger_dip, int dagger_effort)
{
	// Calculate the amount of fluoride used based on dagger dip and effort
	if (dagger_dip < 0 || dagger_effort < 0)
	{
		fprintf(stderr, "Dagger dip and effort must be non-negative.\n");
		exit(EXIT_FAILURE);
	}

	game_state.flouride_used = (dagger_dip * 2) + (dagger_effort * 3);
	if (game_state.flouride_used > game_state.flouride)
	{
		printf("Fluoride used (%d) exceeds available fluoride (%d).\n",
			   game_state.flouride_used, game_state.flouride);
		fprintf(stderr, "Not enough fluoride available.\n");
		exit(EXIT_FAILURE);
	}
	game_state.flouride -= game_state.flouride_used;
	return game_state.flouride_used;
}
void calculate_fang_health(struct vampire_fangs *fang, int dagger_dip, int dagger_effort)
{
	// Calculate health based on dagger dip and effort
	if (dagger_dip < 0 || dagger_effort < 0)
	{
		fprintf(stderr, "Dagger dip and effort must be non-negative.\n");
		exit(EXIT_FAILURE);
	}

	fang->health += (dagger_dip / 2) + (dagger_effort / 3);
	if (fang->health > 100)
		fang->health = 100; // Cap health at 10
	else if (fang->health < 0)
		fang->health = 0; // Ensure health doesn't go below 0

	// Set color according to health
	if (fang->health >= 9)
		fang->color = "white"; // max health
	else if (fang->health >= 8)
		fang->color = "dull"; // medium health
	else
		fang->color = "yellow"; // low health
}

char *
fang_idx_to_name(int fang_index)
{
	switch (fang_index)
	{
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
print_fang_info(int index, struct vampire_fangs *fang)
{
	printf("Fang %s:\n", fang_idx_to_name(index));
	printf("  Length: %d\n", fang->length);
	printf("  Sharpness: %d\n", fang->sharpness);
	printf("  Color: %s\n", fang->color);
	printf("  Health: %d\n", fang->health);
}
static void
print_vampire_info(struct vampire *vamp)
{
	printf("Vampire Name: %s\n", vamp->name);
	printf("Vampire Age: %d\n", vamp->age);
	printf("Vampire Species: %s\n", vamp->species);
	for (int i = 0; i < 4; i++)
	{
		printf("Fang %s:\n", fang_idx_to_name(i));
		printf("  Length: %d\n", vamp->fangs[i].length);
		printf("  Sharpness: %d\n", vamp->fangs[i].sharpness);
		printf("  Color: %s\n", vamp->fangs[i].color);
		printf("  Health: %d\n", vamp->fangs[i].health);
	}
}

static void
print_flouride_info(void)
{
	printf("Remaining fluoride: %d\n", game_state.flouride);
}

int apply_fluoride_to_dracula(void)
{
	int dagger_dip, dagger_effort;

	if (game_state.daggerset)
		printf("Buffy will use her dagger to apply fluoride to Dracula's teeth\n");
	else
		printf("Buffy will not use her dagger to apply fluoride to Dracula's teeth\n");

	struct vampire vampire = {
		.age = 0,
		.name = NULL,
		.species = NULL,
		.fangs = {
			{0, 0, NULL, 0},
			{0, 0, NULL, 0},
			{0, 0, NULL, 0}}};

	randomize_fangs(&vampire, 4);

	vampire.age = 200;
	vampire.name = "Dracula";
	vampire.species = "Vampire";

	/* Next we will loop through the fangs asking the user how much to dip the dagger
	   in the flouride and how much effort to apply to the current fang */

	printf("Vampire Name: %s\n", vampire.name);
	printf("Vampire Age: %d\n", vampire.age);
	printf("Vampire Species: %s\n", vampire.species);
	int cleaning = 1;
	do
	{
		char answer[4];
		print_flouride_info();
		for (int i = 0; i < 4; i++)
		{
			ask_slayer(&dagger_dip, &dagger_effort);
			calculate_fang_health(&vampire.fangs[i], dagger_dip, dagger_effort);
			calculate_flouride_used(dagger_dip, dagger_effort);
			// &vampire.fangs[i].length, &vampire.fangs[i].sharpness, &vampire.fangs[i].color, &vampire.fangs[i].health);
			print_fang_info(i, &vampire.fangs[i]);
		}
		/* the game should check all teeth to see if health is 10.
			if health is 10, then we should print succss message
			and some pleasing states for the user
			then exit */
		int all_fangs_healthy = 1;
		for (int i = 0; i < 4; i++)
		{
			if (vampire.fangs[i].health < 100)
			{
				all_fangs_healthy = 0;
				break;
			}
		}
		if (all_fangs_healthy)
		{
			printf("All of Dracula's fangs are now healthy and shiny!\n");
			printf("Buffy has successfully applied fluoride to Dracula's fangs.\n");
			printf("Buffy is ready to slay more vampires!\n");
			goto success; // Exit the loop and finish the game
		}
		printf("Apply fluoride to Dracula's fangs? (y/n/q/s): ");
		scanf("%3s", answer);
		if (answer[0] == 'y' || answer[0] == 'Y')
		{
			if (game_state.daggerset)
			{
				printf("Buffy applies fluoride to Dracula's fangs with her dagger.\n");
				printf("Dagger dip: %d, Dagger effort: %d\n", dagger_dip, dagger_effort);
				game_state.flouride_used += calculate_flouride_used(dagger_dip, dagger_effort);
			}
			else if (game_state.bflag)
			{
				printf("Buffy applies fluoride to Dracula's fangs with her dagger.\n");
			}
			else
			{
				printf("Buffy applies fluoride to Dracula's fangs without a dagger.\n");
			}
		}
		else if (answer[0] == 'n' || answer[0] == 'N')
		{
			printf("Buffy decides not to apply fluoride to Dracula's fangs.\n");
			cleaning = 0; // Exit the loop
		}
		else if (answer[0] == 'q' || answer[0] == 'Q')
		{
			printf("Buffy quits the game.\n");
			exit(EXIT_SUCCESS);
		}
		else if (answer[0] == 's' || answer[0] == 'S')
		{
			char save_file[256];
			printf("Enter filename to save the game: ");
			scanf("%255s", save_file);
			save_game(save_file);
			cleaning = 0; // Exit the loop after saving
		}
		else
		{
			fprintf(stderr, "Thanks for playing.\n");
			goto success;
		}
	} while (cleaning);
	printf("Buffy has finished applying fluoride to Dracula's fangs.\n");

success:
	printf("Remaining fluoride: %d\n", game_state.flouride);
	print_vampire_info(&vampire);
	return EXIT_SUCCESS;
}

static int
main_program(int reloadflag)
{
	// If we are reloading the game state, we don't need to initialize it again
	if (!reloadflag)
		init_game_state(game_state.bflag);

	// If daggerset is not set, we will not use the dagger
	if (!game_state.daggerset)
		game_state.dagger_dip = 0;
	else
		game_state.dagger_dip = 10; // Default dagger dip value

	game_state.dagger_effort = 5; // Default dagger effort value

	printf("Welcome to Buffy the Vampire Slayer: Fluoride Edition!\n");
	printf("Buffy is ready to apply fluoride to Dracula's fangs.\n");

	return apply_fluoride_to_dracula();
}

#ifndef __UNIT_TEST__
int main(int argc, char *argv[])
{
	int bflag;
	int ch;
	int fd;
	int fflag = 0;

	struct stat st;

	/* options descriptor */
	static struct option longopts[] = {
		{"buffy", no_argument, NULL, 'b'},
		{"fluoride", required_argument, NULL, 'f'},
		{"daggerset", no_argument, &game_state.daggerset, 1},
		{NULL, 0, NULL, 0}};

	bflag = 0;
	while ((ch = getopt_long(argc, argv, "bf:", longopts, NULL)) != -1)
		switch (ch)
		{
		case 'b':
			bflag = 1;
			break;
		case 'f':
			fflag = 1;
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
			close(fd);
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
				fprintf(stderr, "Buffy will use her dagger to "
								"apply fluoride to dracula's teeth\n");
			break;
		default:
			usage();
		}
	argc -= optind;
	argv += optind;

	if (argc != 0)
		usage();

	// Initialize game state
	// if fflag is not set we don't care about any other options
	// since options are stored in the game_state struct

	if (!fflag)
		init_game_state(bflag);

	exit(main_program(fflag));
}
#else
#include "unittest/unittest.c"
#endif
