/**
 * @file devel_walk.c
 * Development utility for qwalk; namely used for creating #QwalkArea_t to write
 * to files.
 */



#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "mode.h"
#include "qattr.h"
#include "qfile.h"
#include "qwalk.h"
#include "devel_walk.h"




/*@only@*/static QwalkArea_t *walk_area;




int main(int argc, char **argv) {
	int opt;
	DevelWalkCmd_t cmd = DEVEL_WALK_CMD_INIT;
	int r;
	char file_path[QFILE_MAX_PATH_SIZE];

	if (strcpy(file_path, QFILE_DEVEL_DIR QFILE_DEVEL_WALK_AREA_DIR QFILE_DEVEL_WALK_DEFAULT) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		exit(EXIT_FAILURE);		
	}

	/* parse command line args */
	while ((opt = getopt(argc, argv, "f:")) != -1) {
		switch (opt) {
		case 'f':
			if (strcpy(file_path, QFILE_DEVEL_DIR QFILE_DEVEL_WALK_AREA_DIR) == NULL) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				exit(EXIT_FAILURE);
			}
			if (strcat(file_path, optarg) == NULL) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				exit(EXIT_FAILURE);
			}
			break;
		default:
			fprintf(stderr, "Usage: %s [-f filename]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	
	if (access(file_path, F_OK) != 0) {
		walk_area = devel_walk_area_default_init();
	}	else {
		walk_area = devel_walk_area_load(file_path);
	}
	
	r = devel_walkio_init();
	assert(r != Q_ERROR);

	while (cmd != DEVEL_WALK_CMD_EXIT) {
		
		r = devel_walkio_out(walk_area);
		assert(r != Q_ERROR);
		
		cmd = devel_walkio_in();
		assert((cmd >= Q_ENUM_VALUE_START) && (cmd <= DEVEL_WALK_CMD_INIT));

		if (cmd != DEVEL_WALK_CMD_INIT) {		
			r = devel_walkl_tick(walk_area, cmd);
			assert(r != Q_ERROR);
		}
	}
	
	r = devel_walkio_end();
	assert(r != Q_ERROR);
	return 0;
}
