#ifdef _OpenBSD__

/*
 *  CUnit tests for edoas 
 *
 */

#include "CUnit/Basic.h"

int startup = 0;
int isclean = 0;

struct game_state game_state;
struct vampire vampire;

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
	if ( (NULL == CU_add_test(pSuite, "test of init_game_state)", testINIT_GAME_STATE) ) || (NULL == CU_add_test(pSuite, "test of testPASS()", testPASS) ) ) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	return CU_get_error();
}

#endif // _OpenBSD__