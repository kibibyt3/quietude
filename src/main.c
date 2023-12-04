/**
 * @file main.c
 * Main file for Q.
 */



#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "splint_types.h"
#include "dialogue.h"
#include "qattr.h"
#include "mode.h"
#include "qwalk.h"



/**
 * Main function of Q.
 * @param[in] argc: argument count
 * @param[in] argv: argument vector
 * @return 0 on success and 1 on error.
 */
int main(/*@unused@*/int argc, /*@unused@*/char** argv) {

	if (mode_init() == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		exit(EXIT_FAILURE);
	}

	char *switch_init_data;

	if ((switch_init_data = calloc(
					strlen(QWALK_AREA_FILENAME_DEFAULT) +
					(size_t) 1, sizeof(*switch_init_data)))
			== NULL) {
		Q_ERROR_SYSTEM("calloc()");
		exit(EXIT_FAILURE);
	}

	strcpy(switch_init_data, QWALK_AREA_FILENAME_DEFAULT);

	Qdatameta_t *switch_init_datameta;

	if ((switch_init_datameta = qdatameta_create(
					(Qdata_t *) switch_init_data, QDATA_TYPE_CHAR_STRING,
					(size_t) 1)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		exit(EXIT_FAILURE);
	}

	/* ncurses initializations */
	if (initscr() == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		exit(EXIT_FAILURE);
	}
	if (cbreak() == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		exit(EXIT_FAILURE);
	}
	if (noecho() == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
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


	/* ncurses cleanup */
	if (endwin() == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
