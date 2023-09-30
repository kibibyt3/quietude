#include "qdefs.h"

#include "mode.h"

#define MODE_SWITCH_DATA_SIZE 500




/*
 * Initialize the mode module. Returns Q_OK or Q_ERROR.
 */
int
mode_init() {
	return Q_OK;
}

/*
 * Exit the mode module. Returns Q_OK or Q_ERROR.
 */
int
mode_exit(){
	return Q_OK;
}

/*
 * Switch from one mode to another; mode_prev stores the name of the previous
 * mode. mode_data_next must store the name of the next mode and data to pass
 * to the next mode. If the game has just been initialized, mode_data_next.data
 * is allowed to be NULL. This is passed, unsanitized, to the initialization 
 * function of the next mode. MODE_T_INIT and MODE_T_EXIT are handled here. Do
 * not treat MODE_T_INIT as the next mode or MODE_T_EXIT as the previous mode,
 * as this is impossible behaviour. Returns Q_OK or Q_ERROR.
 */
int
mode_switch(ModeSwitchData_t *mode_data_next, Mode_t mode_prev) {
	
	/* Validate params against 'impossible' values */
	assert(mode_prev              != MODE_T_EXIT);
	assert((*mode_data_next).mode != MODE_T_INIT);
	assert(mode_prev              > 0 && mode_prev           < MODE_T_COUNT);
	assert((*mode_data_next).mode > 0 && mode_data_next.mode < MODE_T_COUNT);
	
	/* Put away previous mode unless we just initialized the game */
	if (mode_prev != MODE_T_INIT){
		switch (mode_prev) {
		case MODE_T_WALK:
			qwalk_end();
			break;
		case MODE_T_TALK:
			qtalk_end();
			break;
		case MODE_T_CLI:
			qcli_end();
			break;
		case MODE_T_SAIL:
			qsail_end();
			break;
		}
	}

	/* Bring out the next mode unless we're going to exit */
	if ((*mode_data_next).mode != MODE_T_EXIT){
		switch ((*mode_data_next).mode) {
		case MODE_T_WALK:
			qwalk_init((*mode_data_next).datameta);
			break;
		case MODE_T_TALK:
			qtalk_init((*mode_data_next).datameta);
			break;
		case MODE_T_CLI:
			qcli_init((*mode_data_next).datameta);
			break;
		case MODE_T_SAIL:
			qsail_init((*mode_data_next).datameta);
			break;
		}	
	}

	return Q_OK;
}

/*
 * Cause a tick to pass in Mode_t mode; if the next tick must be in a different
 * mode, return the mode name and the data to pass on as a ModeSwitchData_t
 * pointer; otherwise, the returned pointer contains the current mode along
 * with junk data in the other members.
 */
ModeSwitchData_t*
mode_tick(Mode_t mode){
	ModeSwitchData_t *mode_switch_data;
	
	/* Pass the tick and aquire its mode switch data */
	switch (mode){
	case MODE_T_WALK:
		mode_switch_data = qwalk_tick();
		break;
	case MODE_T_TALK:
		mode_switch_data = qtalk_tick();
		break;
	case MODE_T_CLI:
		mode_switch_data = qcli_tick();
		break;
	case MODE_T_SAIL:
		mode_switch_data = qsail_tick();
		break;
	}

	return mode_switch_data;
}
