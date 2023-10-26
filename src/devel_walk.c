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



/** Total number of possible #QattrKey_t devel_walk can handle. */
#define DEVEL_WALK_ATTR_COUNT 2 



static /*@null@*//*@only@*/QwalkArea_t *devel_walk_area_default_create(void);
static /*@null@*//*@only@*/QattrList_t *devel_attr_list_default_create(QwalkLayerType_t);
static /*@null@*//*@only@*/QwalkArea_t *devel_walk_area_load(const char *);



/**
 * Main function for the devel_walk program.
 * @param[in] argc: argument count.
 * @param[in] argv: argument vector.
 * @return 0 on success or -1 otherwise.
 */
int main(int argc, char **argv) {
	/*@only@*//*@null@*/static QwalkArea_t *walk_area = NULL;
	int opt;
	DevelWalkCmd_t cmd = DEVEL_WALK_CMD_INIT;
	int r;
	char file_path[QFILE_MAX_PATH_SIZE];
	WINDOW *area_win, *area_border_win, *info_win, *info_border_win;
	int curs_loc[] = {0, 0, 0};

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
		walk_area = devel_walk_area_default_create();
	}	else {
		walk_area = devel_walk_area_load(file_path);
	}
	assert(walk_area != NULL);
	
	r = devel_walk_wins_init(&area_win, &area_border_win, &info_win, &info_border_win);
	assert(r != Q_ERROR);

	r = devel_walkio_init(area_win, info_win);
	assert(r != Q_ERROR);

	while (cmd != DEVEL_WALK_CMD_EXIT) {
		
		assert(walk_area != NULL);
		r = devel_walkio_out(walk_area, curs_loc);
		assert(r != Q_ERROR);
		
		cmd = devel_walkio_in();
		assert((cmd >= (DevelWalkCmd_t) Q_ENUM_VALUE_START) && (cmd <= DEVEL_WALK_CMD_COUNT));

		if (cmd != DEVEL_WALK_CMD_EXIT) {		
			r = devel_walkl_tick(walk_area, curs_loc, cmd);
			assert(r != Q_ERROR);
		}
	}
	
	r = devel_walk_wins_close(&area_win, &area_border_win, &info_win, &info_border_win);
	assert(r != Q_ERROR);

	r = devel_walkio_end();
	assert(r != Q_ERROR);
	return 0;
}


/**
 * Load a #QwalkArea_t from storage.
 * param[in] filepath: the filepath to use to load.
 * return newly loaded #QwalkArea_t.
 */
QwalkArea_t *
devel_walk_area_load(const char *filepath) {
	
	QwalkArea_t *walk_area;
	if (qfile_open(filepath, QFILE_MODE_READ) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	if ((walk_area = qwalk_area_read()) == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
	}

	if (qfile_close() == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}

	return walk_area;
}


/**
 * Generate a default #QwalkArea_t.
 * Used for initializing a default #QwalkArea_t; the specific values depend on
 * the QATTR_KEY_*_DEFAULT_* class of defines from @ref qfile.h.
 * @return newly-generated #QwalkArea_t.
 */
QwalkArea_t *
devel_walk_area_default_create() {
	QwalkLayer_t *walk_layer_earth;
	QwalkLayer_t *walk_layer_floater;
	QwalkArea_t  *walk_area;
	int *coords;
	int r;

	walk_layer_earth = qwalk_layer_create();
	walk_layer_floater = qwalk_layer_create();

	if ((walk_layer_earth == NULL) || (walk_layer_floater == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}
	

	for (int i = 0; i < QWALK_LAYER_SIZE; i++) {
		
		coords = qwalk_index_to_coords(i);
		if (coords == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			abort();
		}
		
		r = qwalk_layer_object_set(walk_layer_earth, coords[0], coords[1],
				devel_attr_list_default_create(QWALK_LAYER_TYPE_EARTH));
		if (r == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
		
		r = qwalk_layer_object_set(walk_layer_floater, coords[0], coords[1],
				devel_attr_list_default_create(QWALK_LAYER_TYPE_FLOATER));
		if (r == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}

		free(coords);
	}

	walk_area = qwalk_area_create(walk_layer_earth, walk_layer_floater);
	if (walk_area == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}

	return walk_area;
}


/**
 * Generate a default #QattrList_t.
 * Used for initializing a default #QattrList_t; the specific values depend on
 * the QATTR_KEY_*_DEFAULT_* class of defines from @ref qfile.h.
 * @param[in] layer_type: #QwalkLayerType_t returned #QattrList_t is meant for. 
 * @return newly-generated #QwalkArea_t.
 */
QattrList_t *
devel_attr_list_default_create(QwalkLayerType_t layer_type) {
	QobjType_t   *obj_type;
	bool         *canmove;
	Qdatameta_t  *datameta;
	/*@only@*/QattrList_t  *attr_list;

	int r;
	attr_list = qattr_list_create((size_t) DEVEL_WALK_ATTR_COUNT);
	if (attr_list == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}



	/* default #QATTR_KEY_QOBJECT_TYPE init. */
	obj_type = calloc((size_t) 1, sizeof(*obj_type));
	if (obj_type == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}
	if (layer_type == QWALK_LAYER_TYPE_EARTH) {
		*obj_type = (QobjType_t) QATTR_KEY_QOBJECT_TYPE_QWALK_DEFAULT_EARTH;
	} else if (layer_type == QWALK_LAYER_TYPE_FLOATER) {
		*obj_type = (QobjType_t) QATTR_KEY_QOBJECT_TYPE_QWALK_DEFAULT_FLOATER;
	} else {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		abort();
	}
	
	datameta = qdatameta_create((Qdata_t *) obj_type, QDATA_TYPE_QOBJECT_TYPE, (size_t) 1);
	if (datameta == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}

	r = qattr_list_attr_set(attr_list, QATTR_KEY_QOBJECT_TYPE, datameta);
	if (r == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}
	


	/* default #QATTR_KEY_CANMOVE init. */
	canmove = calloc((size_t) 1, sizeof(*canmove));
	if (canmove == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}
	if (layer_type == QWALK_LAYER_TYPE_EARTH) {
		*canmove = QATTR_KEY_CANMOVE_QWALK_DEFAULT_EARTH;
	} else if (layer_type == QWALK_LAYER_TYPE_FLOATER) {
		*canmove = QATTR_KEY_CANMOVE_QWALK_DEFAULT_FLOATER;
	} else {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		abort();
	}

	datameta = qdatameta_create((Qdata_t *) canmove, QDATA_TYPE_BOOL, (size_t) 1);
	if (datameta == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}

	r = qattr_list_attr_set(attr_list, QATTR_KEY_CANMOVE, datameta);
	if (r == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}



	return attr_list;
}
