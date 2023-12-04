/**
 * @file mode.c
 * Program file to control switching between the various modes of Q.
 */



#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "splint_types.h"
#include "dialogue.h"
#include "mode.h"
#include "qattr.h"
#include "qwalk.h"



/** Current mode for Q. */
/*@null@*//*@partial@*//*@only@*/
static ModeSwitchData_t *switch_data_curr = NULL;

/** Future mode for Q to be switched to at the earliest opportunity. */
/*@null@*//*@partial@*//*@only@*/
static ModeSwitchData_t *switch_data_next = NULL;

/**
 * Whether a mode change has been buffered.
 * Whether @ref mode_buffer_switch() has been called since the last call to
 * @ref mode_switch().
 */
static bool mode_switch_is_buffered = false;



/**
 * Initialize the mode module.
 */
int
mode_init()/*@modifies switch_data_curr, switch_data_next@*/
{
	
	if ((switch_data_curr != NULL) || (switch_data_next != NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_INITIALIZED);
		return Q_ERROR;
	}

	/*@i1@*/if ((switch_data_curr = calloc((size_t) 1, sizeof(*switch_data_curr)))
			== NULL) {
		Q_ERROR_SYSTEM("calloc()");
		return Q_ERROR;
	}
	switch_data_curr->mode = MODE_T_INIT;
	switch_data_curr->datameta = NULL;

	/*@i1@*/if ((switch_data_next = calloc((size_t) 1, sizeof(*switch_data_curr)))
			== NULL) {
		Q_ERROR_SYSTEM("calloc()");
		return Q_ERROR;
	}
	switch_data_next->mode = MODE_T_INIT;
	switch_data_next->datameta = NULL;


	return Q_OK;

}


/**
 * Exit the mode module.
 */
int
mode_exit()/*@modifies switch_data_curr, switch_data_next@*/
{

	int returnval = Q_OK;

	if ((switch_data_curr == NULL) || (switch_data_next == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERROR;
	}

	/* Free the datameta memory of both switch data containers. */
	if (switch_data_curr->datameta != NULL) {
		if (qdatameta_destroy(switch_data_curr->datameta) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
	}

	if (switch_data_next->datameta != NULL) {
		if (qdatameta_destroy(switch_data_next->datameta) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
	}

	/*@i1@*/free(switch_data_curr);
	/*@i1@*/free(switch_data_next);
	switch_data_curr = NULL;
	switch_data_next = NULL;

	return returnval;
}


/**
 * Pass a tick in the current mode.
 */
int
mode_tick()
/*@globals switch_data_curr, switch_data_next@*/
{

	if ((switch_data_curr == NULL) || (switch_data_next == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERROR;
	}

	Mode_t mode;

	mode = mode_switch_data_mode_get(switch_data_curr);

	switch (mode) {
	case MODE_T_WALK:
		if (qwalk_tick() == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		break;
	default:
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERROR;
	}

	return Q_OK;
}


/**
 * Buffer a mode switch in the mode module.
 * @param[in] mode: #Mode_t to switch to at next opportunity.
 * @param[in] datameta: #Qdatameta_t to give to @p mode at the switch.
 * @return #Q_OK or #Q_ERROR.
 */
int
mode_buffer_switch(Mode_t mode, Qdatameta_t *datameta)
/*@modifies switch_data_next, mode_switch_is_buffered@*/
/*@globals switch_data_curr@*/
{

	if ((switch_data_curr == NULL) || (switch_data_next == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		if (datameta != NULL) {
			if (qdatameta_destroy(datameta) == Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
			}
		}
		return Q_ERROR;
	}

	if (mode_switch_data_datameta_set(switch_data_next, datameta) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}

	if (mode_switch_data_mode_set(switch_data_next, mode) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}

	mode_switch_is_buffered = true;
	return Q_OK;
}


/**
 * Attempt to switch the mode.
 * Namely if a switch has been requested via @ref mode_buffer_switch().
 */ 
int
mode_switch()
/*@modifies switch_data_next, switch_data_curr, mode_switch_is_buffered@*/
{

	Mode_t mode_curr, mode_next;
	Qdatameta_t *datameta_next;

	if ((switch_data_curr == NULL) || (switch_data_next == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERROR;
	}


	/* Exit if a mode switch hasn't been buffered. */
	if (mode_switch_is_buffered == false) {
		return Q_ERROR_NOCHANGE;
	}

	datameta_next = mode_switch_data_datameta_get(switch_data_next);



	/* Terminate the previous mode. */
	mode_curr = mode_switch_data_mode_get(switch_data_curr);

	switch (mode_curr) {
	case MODE_T_INIT:
		break;
	case MODE_T_WALK:
		if (qwalk_end() == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		break;
	default:
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERROR;
	}


	/* Initialize the next mode. */
	mode_next = mode_switch_data_mode_get(switch_data_next);

	Qdata_t *datap;

	switch(mode_next) {
	case MODE_T_EXIT:
		break;
	case MODE_T_WALK:
		if (datameta_next == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			return Q_ERROR;
		}
		if (qdatameta_type_get(datameta_next) != QDATA_TYPE_CHAR_STRING) {
			Q_ERRORFOUND(QERROR_QDATAMETA_TYPE_INCOMPATIBLE);
			return Q_ERROR;
		}
		if ((datap = qdatameta_datap_get(datameta_next)) == NULL) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		if (qwalk_init((char *) datap) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		break;
	default:
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERROR;
	}


	switch_data_next->datameta = NULL;
	if (mode_switch_data_mode_set(switch_data_curr, mode_next) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}
	/*@i2@*/if (mode_switch_data_datameta_set(switch_data_curr, datameta_next)
			== Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort(); /* Leads to a memory leak. */
	}

	mode_switch_is_buffered = false;

	return Q_OK;
}


/**
 * Get the current active #Mode_t.
 * @return requested #Mode_t or #Q_ERRORCODE_ENUM on error.
 */
Mode_t
mode_curr_get()/*@globals switch_data_curr, switch_data_next@*/
{

	if ((switch_data_curr == NULL) || (switch_data_next == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return (Mode_t) Q_ERRORCODE_ENUM;
	}

	return mode_switch_data_mode_get(switch_data_curr);
}



/**
 * Set @ref ModeSwitchData_t.datameta.
 * @param[out] switch_data: relevant #ModeSwitchData_t.
 * @param[in] datameta: #Qdatameta_t to give to @p switch_data.
 * @return #Q_OK or #Q_ERROR.
 */
int
mode_switch_data_datameta_set(
		ModeSwitchData_t *switch_data, Qdatameta_t *datameta) {
	
	int returnval = Q_OK;
	if (switch_data->datameta != NULL) {
		if (qdatameta_destroy(switch_data->datameta) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			returnval = Q_ERROR;
		}
	}

	switch_data->datameta = datameta;

	return returnval;
}


/**
 * Set @ref ModeSwitchData_t.mode.
 * @param[out] switch_data: relevant #ModeSwitchData_t.
 * @param[in] mode: #Mode_t to give to @p switch_data.
 * @return #Q_OK or #Q_ERROR.
 */
int
mode_switch_data_mode_set(ModeSwitchData_t *switch_data, Mode_t mode) {

	if ((mode < (Mode_t) Q_ENUM_VALUE_START) || (mode > MODE_T_COUNT)) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERROR;
	}

	switch_data->mode = mode;

	return Q_OK;
}


/**
 * Get @ref ModeSwitchData_t.datameta.
 * @param[in] switch_data: relevant #ModeSwitchData_t.
 * @return requested #Qdatameta_t.
 */
Qdatameta_t *
mode_switch_data_datameta_get(ModeSwitchData_t *switch_data) {
	return switch_data->datameta;
}


/**
 * Get @ref ModeSwitchData_t.mode.
 * @param[in] switch_data: relevant #ModeSwitchData_t.
 * @return requested #Mode_t.
 */
Mode_t
mode_switch_data_mode_get(ModeSwitchData_t *switch_data) {
	return switch_data->mode;
}
