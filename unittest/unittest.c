#ifdef __UNIT_TEST__

/*
 *  CUnit tests for edoas 
 *
 */

#include "CUnit/Basic.h"

int startup = 0;
int isclean = 0;

struct game_state game_state;
struct creature creature;

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
	CU_ASSERT(game_state.dagger_dip == 0);
	CU_ASSERT(game_state.dagger_effort == 0);
	CU_ASSERT(game_state.flouride_used == 0);
	CU_ASSERT(game_state.bflag == 1);
	CU_ASSERT(game_state.score == 0);
	CU_ASSERT(game_state.turns == 0);
}

void 
testCHOOSE_RANDOM_TOOL(void)
{
	int tool_index = choose_random_tool(1);
	CU_ASSERT(tool_index >= 0 && tool_index < 6); // Assuming there are 6 tools
}

void testCHOOSE_RANDOM_TOOL_NON_DAGGER(void)
{
	int tool_index = choose_random_tool(0);
	CU_ASSERT(tool_index >= 0 && tool_index < 3); // Assuming there are 3 non-dagger tools
}

void testRANDOMIZE_FANGS(void)
{
	creature_init(&creature);
	randomize_fangs(&creature, 4);
	for (int i = 0; i < 4; i++) {
		CU_ASSERT(creature.fangs[i].length >= 4 && creature.fangs[i].length <= 6);
		CU_ASSERT(creature.fangs[i].sharpness >= 5 && creature.fangs[i].sharpness <= 8);
		CU_ASSERT(creature.fangs[i].health >= 60 && creature.fangs[i].health <= 100);
		CU_ASSERT(creature.fangs[i].color != NULL);
	}
}

void testPRINT_FANG_INFO(void)
{
	creature_init(&creature);
	randomize_fangs(&creature, 4);
	for (int i = 0; i < 4; i++) {
		print_fang_info(i, &creature.fangs[i]);
		CU_ASSERT(creature.fangs[i].length >= 4 && creature.fangs[i].length <= 6);
		CU_ASSERT(creature.fangs[i].sharpness >= 5 && creature.fangs[i].sharpness <= 8);
		CU_ASSERT(creature.fangs[i].health >= 60 && creature.fangs[i].health <= 100);
	}
}

void testPRINT_CREATURE_INFO(void)
{
	creature_init(&creature);
	randomize_fangs(&creature, 4);
	print_creature_info(&creature);
	/* The creature is randomly chosen so DEFAULTs can not be tested with 
	   assertions here.
	*/
	CU_ASSERT(creature.age >= 100); // Assuming age is set to 100 or more
	CU_ASSERT(creature.name != NULL);
	CU_ASSERT(strlen(creature.name) > 0);
	CU_ASSERT(creature.species != NULL);
	CU_ASSERT(strlen(creature.species) > 0);
	CU_ASSERT(creature.fangs != NULL);	
	for (int i = 0; i < 4; i++) {
		CU_ASSERT(creature.fangs[i].length >= 4 && creature.fangs[i].length <= 6);
		CU_ASSERT(creature.fangs[i].sharpness >= 5 && creature.fangs[i].sharpness <= 8);
		CU_ASSERT(creature.fangs[i].health >= 60 && creature.fangs[i].health <= 100);
	}
}

void testPRINT_TOOL_INFO(void)
{
	game_state.tool_in_use = 0; // Assuming tool index 0 is valid
	print_tool_info();
	/* 
	* The dagger is randomly chosen in the latest version of the code
	* so we will just check if the tool in use is a dagger and has valid properties
	*/
	CU_ASSERT(game_state.tool_in_use >= 0 && game_state.tool_in_use < 6); // Assuming there are 6 tools
	CU_ASSERT(tools[game_state.tool_in_use].dip_amount >= 0);
	CU_ASSERT(tools[game_state.tool_in_use].effort >= 0);
	CU_ASSERT(tools[game_state.tool_in_use].durability >= 0);
	CU_ASSERT(tools[game_state.tool_in_use].name != NULL);
	CU_ASSERT(tools[game_state.tool_in_use].description != NULL);
	CU_ASSERT(strlen(tools[game_state.tool_in_use].name) > 0);
	CU_ASSERT(strlen(tools[game_state.tool_in_use].description) > 0);	
}

void testPRINT_FLOURIDE_INFO(void)
{
	game_state.flouride = 100; // Set a test value
	print_flouride_info();
	CU_ASSERT(game_state.flouride == 100);
}

void testFANG_IDX_TO_NAME(void)
{
	CU_ASSERT(strcmp(fang_idx_to_name(0), "Maxillary Right Canine") == 0);
	CU_ASSERT(strcmp(fang_idx_to_name(1), "Maxillary Left Canine") == 0);
	CU_ASSERT(strcmp(fang_idx_to_name(2), "Mandibular Left Canine") == 0);
	CU_ASSERT(strcmp(fang_idx_to_name(3), "Mandibular Right Canine") == 0);
	CU_ASSERT(strcmp(fang_idx_to_name(4), "Unknown Fang") == 0); // Invalid index
}

void 
testPASS(void)
{
	CU_ASSERT(1 == 1);
}
int 
main()
{
	CU_pSuite 	pSuite = NULL;

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
	if ( (NULL == CU_add_test(pSuite, "test of init_game_state)", testINIT_GAME_STATE) ) ||
	     (NULL == CU_add_test(pSuite, "test of choose_random_tool)", testCHOOSE_RANDOM_TOOL) ) ||
	     (NULL == CU_add_test(pSuite, "test of choose_random_tool_non_dagger)", testCHOOSE_RANDOM_TOOL_NON_DAGGER) ) ||
	     (NULL == CU_add_test(pSuite, "test of randomize_fangs()", testRANDOMIZE_FANGS) ) ||
	     (NULL == CU_add_test(pSuite, "test of print_fang_info()", testPRINT_FANG_INFO) ) ||
	     (NULL == CU_add_test(pSuite, "test of print_creature_info()", testPRINT_CREATURE_INFO) ) ||
	     (NULL == CU_add_test(pSuite, "test of print_tool_info()", testPRINT_TOOL_INFO) ) ||
	     (NULL == CU_add_test(pSuite, "test of print_flouride_info()", testPRINT_FLOURIDE_INFO) ) ||
	     (NULL == CU_add_test(pSuite, "test of fang_idx_to_name()", testFANG_IDX_TO_NAME)  ) ) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	return CU_get_error();
}

#endif // __UNIT_TEST__
