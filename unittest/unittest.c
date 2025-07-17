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
 * unittest.c: CUnit tests for the Buffy game.
 *
 */
#ifdef __UNIT_TEST__

/*
 * CUnit tests for Buffy
 *
 */

#include "CUnit/Basic.h"

int		startup = 0;
int		isclean = 0;

game_state_type game_state;
patient_type	patient;

/* Pointer to the file used by the tests. */

/*
 * The suite initialization function. Opens the temporary file used by the
 * tests. Returns zero on success, non-zero otherwise.
 */
int
init_suite1(void)
{
	startup = 1;
	return 0;
}

/*
 * The suite cleanup function. Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
clean_suite1(void)
{
	isclean = 1;
	return 0;
}

void
testINIT_GAME_STATE(void)
{
	init_game_state(1);
	CU_ASSERT(game_state.daggerset == 0);
	CU_ASSERT(game_state.flouride == 200);
	CU_ASSERT(game_state.tool_dip > 1);
	CU_ASSERT(game_state.tool_effort > 1);
	CU_ASSERT(game_state.flouride_used == 0);
	CU_ASSERT(game_state.bflag == 1);
	CU_ASSERT(game_state.score == 10);
	CU_ASSERT(game_state.turns == 0);
}

void
testCHOOSE_RANDOM_TOOL(void)
{
	int		tool_index = choose_random_tool(1);
	CU_ASSERT(tool_index >= 0 && tool_index < 6);	/* Assuming there are 6
							 * tools */
}

void
testCHOOSE_RANDOM_TOOL_NON_DAGGER(void)
{
	int		tool_index = choose_random_tool(0);
	CU_ASSERT(tool_index >= 0 && tool_index < 3);	/* Assuming there are 3
							 * non-dagger tools */
}

void
testRANDOMIZE_FANGS(void)
{
	patient_init(&patient);
	randomize_fangs(&patient, 4);
	for (int i = 0; i < 4; i++) {
		CU_ASSERT(patient.fangs[i].length >= 4 && patient.fangs[i].length <= 6);
		CU_ASSERT(patient.fangs[i].sharpness >= 5 && patient.fangs[i].sharpness <= 8);
		CU_ASSERT(patient.fangs[i].health >= 60 && patient.fangs[i].health <= 100);
	}
}

void
testPRINT_FANG_INFO(void)
{
	patient_init(&patient);
	randomize_fangs(&patient, 4);
	for (int i = 0; i < 4; i++) {
		print_fang_info(i, &patient.fangs[i], 0);
		CU_ASSERT(patient.fangs[i].length >= 4 && patient.fangs[i].length <= 6);
		CU_ASSERT(patient.fangs[i].sharpness >= 5 && patient.fangs[i].sharpness <= 8);
		CU_ASSERT(patient.fangs[i].health >= 60 && patient.fangs[i].health <= 100);
	}
}

void testSAVE_GAME_STATE(void)
{
	/* Initialize game state and patient */
	init_game_state(1);
	patient_init(&patient);
	randomize_fangs(&patient, 4);
	/* Assuming the game state is saved to a file */
	const char *save_path = "test_game_state.dat";
	save_game_state(save_path, &game_state, sizeof(game_state), &patient, sizeof(patient));
	/* Check if the file exists and has the expected size */
	struct stat st;
	CU_ASSERT(stat(save_path, &st) == 0);
	CU_ASSERT(st.st_size > 0);	/* Ensure the file is not empty */
}

void testLOAD_GAME_STATE(void)
{
/* Create our own storage for the game state and patient */
	game_state_type game_state;
	patient_type patient;
	patient_init(&patient);
	randomize_fangs(&patient, 4);
	/* Assuming the game state is loaded from a file */
	const char *load_path = "test_game_state.dat";
	load_game_state(load_path, &game_state, sizeof(game_state), &patient, sizeof(patient),
			"test_character_name");
	CU_ASSERT(game_state.flouride >= 0);
	CU_ASSERT(game_state.tool_dip >= 0);
	CU_ASSERT(game_state.tool_effort >= 0);
	CU_ASSERT(game_state.flouride_used >= 0);
	CU_ASSERT(game_state.bflag >= 0);
	CU_ASSERT(game_state.score >= 0);
	CU_ASSERT(game_state.turns >= 0);
	CU_ASSERT(game_state.tool_in_use >= 0 && game_state.tool_in_use < 6);	/* Assuming 6 tools */
}

void
testPRINT_CREATURE_INFO(void)
{
	patient_init(&patient);
	randomize_fangs(&patient, 4);
	print_patient_info(&patient, 0);
	/*
	 * The patient is randomly chosen so DEFAULTs can not be tested with
	 * assertions here.
	 */
	CU_ASSERT(patient.age >= 90);	/* Assuming age is set to 100 or more */
	CU_ASSERT(patient.fangs != NULL);
	for (int i = 0; i < 4; i++) {
		CU_ASSERT(patient.fangs[i].length >= 4 && patient.fangs[i].length <= 6);
		CU_ASSERT(patient.fangs[i].sharpness >= 5 && patient.fangs[i].sharpness <= 8);
		CU_ASSERT(patient.fangs[i].health >= 60 && patient.fangs[i].health <= 100);
	}
}

void
testPRINT_TOOL_INFO(void)
{
	game_state.tool_in_use = 0;	/* Assuming tool index 0 is valid */
	print_tool_info();
	/*
	 * The dagger is randomly chosen in the latest version of the code so
	 * we will just check if the tool in use is a dagger and has valid
	 * properties
	 */
	CU_ASSERT(game_state.tool_in_use >= 0 && game_state.tool_in_use < 6);	/* Assuming 6 tools */
	CU_ASSERT(tools[game_state.tool_in_use].dip_amount >= 0);
	CU_ASSERT(tools[game_state.tool_in_use].effort >= 0);
	CU_ASSERT(tools[game_state.tool_in_use].durability >= 0);
	CU_ASSERT(tools[game_state.tool_in_use].name != NULL);
	CU_ASSERT(tools[game_state.tool_in_use].description != NULL);
	CU_ASSERT(strlen(tools[game_state.tool_in_use].name) > 0);
	CU_ASSERT(strlen(tools[game_state.tool_in_use].description) > 0);
}

void
testPRINT_FLUORIDE_INFO(void)
{
	game_state.flouride = 100;	/* Set a test value */
	game_state.flouride_used = 0;	/* Reset used flouride for testing */
	print_fluoride_info();
	CU_ASSERT(game_state.flouride == 100);
}

void
testFANG_IDX_TO_NAME(void)
{
	CU_ASSERT(strcmp(fang_idx_to_name(0), "Maxillary Right Canine") == 0);
	CU_ASSERT(strcmp(fang_idx_to_name(1), "Maxillary Left Canine") == 0);
	CU_ASSERT(strcmp(fang_idx_to_name(2), "Mandibular Left Canine") == 0);
	CU_ASSERT(strcmp(fang_idx_to_name(3), "Mandibular Right Canine") == 0);
	CU_ASSERT(strcmp(fang_idx_to_name(4), "Unknown Fang") == 0);	/* unkown fang index */
}

void
testCONCAT_PATH(void)
{
	char	       *path = return_concat_path(DEFAULT_SAVE_FILE);
	CU_ASSERT(path != NULL);
	CU_ASSERT(strlen(path) > 0);
}

void testVALIDATE_GAME_FILE(void) 
{
	char *failed_path = "/dev/null";
	CU_ASSERT(validate_game_file(failed_path) == 1);
}
int
main()
{
	CU_pSuite	pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	/* add the tests to the suite */
	if ((NULL == CU_add_test(pSuite, "test of init_game_state)", testINIT_GAME_STATE)) ||
	    (NULL == CU_add_test(pSuite, "test of choose_random_tool)", testCHOOSE_RANDOM_TOOL)) ||
	    (NULL == CU_add_test(pSuite, "test of choose_random_tool_non_dagger)", testCHOOSE_RANDOM_TOOL_NON_DAGGER)) ||
	    (NULL == CU_add_test(pSuite, "test of randomize_fangs()", testRANDOMIZE_FANGS)) ||
	    (NULL == CU_add_test(pSuite, "test of print_fang_info()", testPRINT_FANG_INFO)) ||
	    (NULL == CU_add_test(pSuite, "test of print_patient_info()", testPRINT_CREATURE_INFO)) ||
	    (NULL == CU_add_test(pSuite, "test of print_tool_info()", testPRINT_TOOL_INFO)) ||
	    (NULL == CU_add_test(pSuite, "test of print_fluoride_info()", testPRINT_FLUORIDE_INFO)) ||
	    (NULL == CU_add_test(pSuite, "test of fang_idx_to_name()", testFANG_IDX_TO_NAME)) ||
		(NULL == CU_add_test(pSuite, "test validate game_file()", testVALIDATE_GAME_FILE)) ||		
	    (NULL == CU_add_test(pSuite, "test of return_concat_path()", testCONCAT_PATH))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	return CU_get_error();
}

#endif				/* __UNIT_TEST__ */
