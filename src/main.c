/**
 * @file main.c
 * Main file for Q.
 */



#include <stdlib.h>
#include <stdbool.h>

#include "qdefs.h"
#include "qerror.h"

#include "mode.h"



/**
 * Main function of Q.
 * @param[in] argc: argument count
 * @param[in] argv: argument vector
 * @return 0 on success and 1 on error.
 */
int main(int argc, char** argv) {
	
	if (mode_init() == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		exit(EXIT_FAILURE);
	}

	ModeSwitchData_t  mode_switch_data_next = {MODE_T_INIT, NULL};
	ModeSwitchData_t *mode_switch_data_nextptr = &mode_switch_data_next;
	Mode_t mode_curr = MODE_T_INIT;

	while (mode_switch_data_next.mode != MODE_T_EXIT) {
		mode_switch(mode_switch_data_nextptr);
		mode_curr = mode_switch_data_nextptr->mode;
		while (mode_switch_data_nextptr->mode == mode_curr) {
			mode_tick(mode_switch_data_nextptr);
		}
	}

	exit(EXIT_SUCCESS);
}
