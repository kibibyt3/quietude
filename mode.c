#include "qdefs.h"

#include "mode.h"

#define MODE_SWITCH_DATA_SIZE 500




/*
 * Initialize the mode module. Returns Q_SUCCESS or Q_FAILURE.
 */
int
mode_init() {
	return Q_SUCCESS;
}

/*
 * Switch from one mode to another; mode_prev stores the name of the previous
 * mode. mode_data_next must store the name of the next mode and data to pass
 * to the next mode. If the game has just been initialized, mode_data_next.data
 * is allowed to be NULL. This is passed, unsanitized, to the initialization 
 * function of the next mode. MODE_T_INIT and MODE_T_EXIT are handled here. Do
 * not treat MODE_T_INIT as the next mode or MODE_T_EXIT as the previous mode,
 * as this is impossible behaviour. Returns Q_SUCCESS or Q_FAILURE.
 */
int
mode_switch(Mode_t mode_data_next, Mode_t mode_prev) {
	
	/* Validate params against 'impossible' values */
	assert(mode_prev      != MODE_T_EXIT);
	assert(mode_data_next != MODE_T_INIT);
	assert(mode_prev           > 0 && mode_prev           < MODE_T_COUNT);
	assert(mode_data_next.mode > 0 && mode_data_next.mode < MODE_T_COUNT);
	
	/* Put away previous mode unless we just initialized the game */
	if (mode_prev != MODE_T_INIT){
		switch (mode_prev) {
		case MODE_T_WALK:
			walk_end();
			break;
		case MODE_T_TALK:
			talk_end();
			break;
		case MODE_T_CLI:
			cli_end();
			break;
		case MODE_T_SAIL:
			sail_end();
			break;
		}
	}

	/* Bring out the next mode unless we're going to exit */
	if (mode_data_next.mode != MODE_T_EXIT){
		switch (mode_data_next.mode) {
		case MODE_T_WALK:
			walk_init(mode_data_next.data);
			break;
		case MODE_T_TALK:
			talk_init(mode_data_next.data);
			break;
		case MODE_T_CLI:
			cli_init(mode_data_next.data);
			break;
		case MODE_T_SAIL:
			sail_init(mode_data_next.data);
			break;
		}	
	}

	return Q_SUCCESS;
}
