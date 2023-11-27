/**
 * @file main.c
 * Main file for Q.
 */



#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "qdefs.h"
#include "qerror.h"

#include "qattr.h"
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

	int *switch_init_data;

	if ((switch_init_data = calloc((size_t) 1, sizeof(*switch_init_data)))
			== NULL) {
		Q_ERROR_SYSTEM("calloc()");
		exit(EXIT_FAILURE);
	}

	*switch_init_data = 0;

	Qdatameta_t *switch_init_datameta;

	if ((switch_init_datameta = qdatameta_create(
					(Qdata_t *) switch_init_data, QDATA_TYPE_INT, (size_t) 1)) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		exit(EXIT_FAILURE);
	}

	if (mode_buffer_switch(MODE_T_WALK, switch_init_datameta) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}

	if (mode_switch() == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}

	Mode_t mode_curr;

	while (true) {
		if (mode_switch() == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}

		/* check if we should exit */
		if ((mode_curr = mode_curr_get()) == (Mode_t) Q_ERRORCODE_ENUM) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
		if (mode_curr == MODE_T_EXIT) {
			break;
		}

		if (mode_tick() == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
	}

	exit(EXIT_SUCCESS);
}
