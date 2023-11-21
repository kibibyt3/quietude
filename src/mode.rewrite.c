/**
 * @file mode.c
 * Program file to control switching between the various modes of Q.
 */



#include <stdlib.h>
#include <bool.h>

#include "qdefs.h"
#include "qerror.h"

#include "mode.h"
#include "qattr.h"
#include "qwalk.h"



/** Current mode for Q. */
/*@null@*//*@partial@*/static ModeSwitchData_t *switch_data_curr = NULL;



/**
 * TODO: implement a switch-at-next-opportunity function. keep a var to store
 * the switch-at-next and then make a function to do the switch proper; this
 * should be called by main after every tick to check if the module needs to be
 * put away.
 */



int
mode_init()/*@modifies switch_data_curr@*/
{

	if ((switch_data_curr = calloc((size_t) 1, sizeof(*switch_data_curr)))
			== NULL) {
		Q_ERROR_SYSTEM("calloc()");
		return Q_ERROR;
	}

	if (mode_switch_data_curr_mode_set(MODE_T_INIT) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}

	if (mode_switch_data_curr_datameta_set(NULL) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}

	return Q_OK;

}


int
mode_switch(ModeSwitchData_t *mode_data_next) {

	Mode_t mode_curr;

	if ((mode_curr = mode_switch_data_curr_mode_get()) == (Mode_t) Q_ERRORCODE_ENUM) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	if (mode_curr != MODE_T_INIT) {

		/* Put away the previous mode unless we just initialized the game. */
		switch (mode_curr) {
		case MODE_T_WALK:
			if (qwalk_end() == Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				abort(); /* because this leads to a memory leak */
			}
			break;
		}
	}

	if (mode_switch_data_curr_mode_get() != MODE_T_EXIT) {
		switch (mode_switch_data_curr_mode_get()
	}
}
