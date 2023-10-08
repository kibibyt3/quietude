/**
 * @file mode.c
 * Program file for the modes module of the game.
 */

#include <assert.h>
#include <stddef.h>

#include "qdefs.h"

#include "mode.h"
#include "qattr.h"
#include "qwalk.h"
#include "qsail.h"
#include "qcli.h"
#include "qtalk.h"


/**
 * Initialize the mode module.
 * @return #Q_OK or #Q_ERROR.
 */
int
mode_init() {
	return Q_OK;
}

/**
 * Exit the mode module.
 * @return #Q_OK or #Q_ERROR.
 */
int
mode_exit(){
	return Q_OK;
}

/**
 * Switch from one mode to another.
 * The previous mode is put away and the next mode is brought out. Data to pass
 * between each mode is also traded here, wherever applicable. #MODE_T_INIT and
 * #MODE_T_EXIT are handled here. 
 * @param[in] mode_prev: the name of the previous mode. Must not be #MODE_T_EXIT.
 * @param[in] mode_data_next: name and data for the next mode. Data will be @c NULL
 * when #MODE_T_INIT is handled. Must not be #MODE_T_EXIT.
 * @return #Q_OK or #Q_ERROR.
 */
int
mode_switch(ModeSwitchData_t *mode_data_next, Mode_t mode_prev) {
	
	/* Validate params against 'impossible' values */
	assert(mode_prev              != MODE_T_EXIT);
	assert(mode_data_next->mode != MODE_T_INIT);
	assert(mode_prev              > 0 && mode_prev           < MODE_T_COUNT);
	assert(mode_data_next->mode > 0 && mode_data_next->mode < MODE_T_COUNT);
	
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
		case MODE_T_INIT:
			return Q_ERROR;
		case MODE_T_EXIT:
			return Q_ERROR;
		}
	}

	/* Bring out the next mode unless we're going to exit */
	if (mode_data_next->mode != MODE_T_EXIT){
		switch (mode_data_next->mode) {
		case MODE_T_WALK:
			qwalk_init(mode_data_next->datameta);
			break;
		case MODE_T_TALK:
			qtalk_init(mode_data_next->datameta);
			break;
		case MODE_T_CLI:
			qcli_init(mode_data_next->datameta);
			break;
		case MODE_T_SAIL:
			qsail_init(mode_data_next->datameta);
			break;
		case MODE_T_INIT:
			return Q_ERROR;
		case MODE_T_EXIT:
			return Q_ERROR;
		}	
	}

	return Q_OK;
}

/**
 * Cause a tick to pass.
 * Acts as an interface to the relevant tick functions in each module. Tracks
 * whether the tick function wants to switch control to a different module
 * by keeping the name of the mode for the next tick in @c mode_switch_data.
 * If modes are to be changed by the next tick, the datameta to pass to the
 * next tick is also returned in the #ModeSwitchData_t*.
 * @param[in] mode: the specific mode to tick forward.
 * @return switch data with the name of the next mode and data to pass to 
 * the next mode, if applicable; or NULL if an error occurs.
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
	case MODE_T_INIT:
		return NULL;
	case MODE_T_EXIT:
		return NULL;
	}

	return mode_switch_data;
}
