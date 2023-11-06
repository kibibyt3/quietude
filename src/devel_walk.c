/**
 * @file devel_walk.c
 * Development utility for qwalk; namely used for creating #QwalkArea_t to write
 * to files.
 */



#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
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
#define DEVEL_WALK_ATTR_COUNT 5 


/*@null@*//*@only@*/
static QwalkArea_t *devel_walk_area_default_create(void);
/*@null@*//*@only@*/
static QattrList_t *devel_attr_list_default_create(QwalkLayerType_t);
/*@null@*//*@only@*/
static QwalkArea_t *devel_walk_area_load(const char *);
static int          devel_walk_area_write(const QwalkArea_t *, const char *);
static void         devel_walk_print_help(void);



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

	strcpy(file_path, QFILE_DEVEL_WALK_DEFAULT);

	/* parse command line args */
	while ((opt = getopt(argc, argv, "hf:")) != -1) {
		switch (opt) {
		case 'h':
			devel_walk_print_help();
			exit(EXIT_SUCCESS);
		case 'f':
			strcpy(file_path, optarg); 
			break;
		default:
			devel_walk_print_help();
			exit(EXIT_FAILURE);
		}
	}
	if (access(file_path, F_OK) != 0) {
		walk_area = devel_walk_area_default_create();
	}	else {
		walk_area = devel_walk_area_load(file_path);
	}
	assert(walk_area != NULL);

	r = devel_walkio_init();
	assert(r != Q_ERROR);

	r = devel_walk_wins_init(&area_win, &area_border_win, &info_win, &info_border_win);
	assert(r != Q_ERROR);

	r = devel_walkio_wins_init(area_win, area_border_win, info_win, info_border_win);
	assert(r != Q_ERROR);

	while (cmd != DEVEL_WALK_CMD_EXIT) {
		
		assert(walk_area != NULL);
		r = devel_walkio_out(walk_area, curs_loc);
		assert(r != Q_ERROR);
		
		cmd = devel_walkio_in(walk_area, curs_loc);
		assert((cmd >= (DevelWalkCmd_t) Q_ENUM_VALUE_START) && (cmd <= DEVEL_WALK_CMD_COUNT));

		if (cmd == DEVEL_WALK_CMD_SAVE) {
			r = devel_walk_area_write(walk_area, file_path);
			if (r == Q_ERROR) {
				if (devel_walkio_message_print(DEVEL_WALKIO_MESSAGE_SAVE_ERROR) == Q_ERROR) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
				}
			} else {
				if (devel_walkio_message_print(DEVEL_WALKIO_MESSAGE_SAVE_SUCCESS) == Q_ERROR) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
				}
			}
		}

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
 * @param[in] filepath: the filepath to use to load.
 * @return newly loaded #QwalkArea_t.
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
 * Write a #QwalkArea_t to storage.
 * @param[in] walk_area: #QwalkArea_t to write.
 * @param[in] filepath:  filepath to write to.
 * @return #Q_OK or #Q_ERROR.
 */
int
devel_walk_area_write(const QwalkArea_t *walk_area, const char *filepath) {
	
	int returnval = Q_OK;

	if (qfile_open(filepath, QFILE_MODE_WRITE) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}
	
	if (qwalk_area_write(walk_area) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}

	if (qfile_close() == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}

	return returnval;
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
	char         salias[DEVEL_WALKIO_USERSTRING_LENGTH_MAX];
	char         *s;
	bool         *canmove;
	
	size_t        count;
	QdataType_t   data_type;
	QattrKey_t    key;

	Qdata_t      *data;
	Qdatameta_t  *datameta;
	QattrList_t  *attr_list;
	int r;


	if ((layer_type != QWALK_LAYER_TYPE_EARTH) && (layer_type != QWALK_LAYER_TYPE_FLOATER)) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return NULL;
	}

	attr_list = qattr_list_create((size_t) DEVEL_WALK_ATTR_COUNT);
	if (attr_list == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}


	/* initialize each default attribute */
	for (int i = 0; i < DEVEL_WALK_ATTR_COUNT; i++) {
	
		key = (QattrKey_t) Q_ERRORCODE_ENUM;	
		strcpy(salias, QATTR_KEY_STRING_DEFAULT);

		/* prep for keys that share a data type */
		switch (i) {

		/* char * prep */
		case 1:
			key = QATTR_KEY_NAME;
			if (layer_type == QWALK_LAYER_TYPE_EARTH) {
				strcpy(salias, QATTR_KEY_NAME_DEFAULT_QWALK_DEFAULT_EARTH);
			} else {
				strcpy(salias, QATTR_KEY_NAME_DEFAULT_QWALK_DEFAULT_FLOATER);
			}
			break;
		case 2:
			key = QATTR_KEY_DESCRIPTION_BRIEF;
			if (layer_type == QWALK_LAYER_TYPE_EARTH) {
				strcpy(salias, QATTR_KEY_DESCRIPTION_BRIEF_QWALK_DEFAULT_EARTH);
			} else {
				strcpy(salias, QATTR_KEY_DESCRIPTION_BRIEF_QWALK_DEFAULT_FLOATER);
			}
			break;
		case 3:
			key = QATTR_KEY_DESCRIPTION_LONG;
			if (layer_type == QWALK_LAYER_TYPE_EARTH) {
				strcpy(salias, QATTR_KEY_DESCRIPTION_LONG_QWALK_DEFAULT_EARTH);
			} else {
				strcpy(salias, QATTR_KEY_DESCRIPTION_LONG_QWALK_DEFAULT_FLOATER);
			}
			break;
		}
		


		/* post-prep initializations */
		switch (i) {
		

		/* default #QATTR_KEY_QOBJECT_TYPE init */
		case 0:
			key = QATTR_KEY_QOBJECT_TYPE;
			data_type = QDATA_TYPE_QOBJECT_TYPE;
			count     = (size_t) 1;

			obj_type = calloc(count, sizeof(*obj_type));
			if (obj_type == NULL) {
				Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
				abort();
			}
			if (layer_type == QWALK_LAYER_TYPE_EARTH) {
				*obj_type = (QobjType_t) QATTR_KEY_QOBJECT_TYPE_QWALK_DEFAULT_EARTH;
			} else {
				*obj_type = (QobjType_t) QATTR_KEY_QOBJECT_TYPE_QWALK_DEFAULT_FLOATER;
			}
			data = (Qdata_t *) obj_type;
			break;
		

		/* default #QATTR_KEY_NAME init */
		case 1:
		
		/*@fallthrough@*/
		/* default #QATTR_KEY_DESCRIPTION_BRIEF init */
		case 2:
			
		/*@fallthrough@*/
		/* default #QATTR_KEY_DESCRIPTION_LONG init  */
		case 3:
			data_type = QDATA_TYPE_CHAR_STRING;
			/* strlen() does not include the terminating character */
			count     = strlen(salias) + (size_t) 1;
			
			s = calloc(count, sizeof(*s));
			if (s == NULL) {
				Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
				abort();
			}

			strcpy(s, salias);
			data = (Qdata_t *) s;
			break;
		

		/* default #QATTR_KEY_CANMOVE init */
		case 4:
			key = QATTR_KEY_CANMOVE;
			data_type = QDATA_TYPE_BOOL;
			count     = (size_t) 1;

			canmove = calloc(count, sizeof(*canmove));
			if (canmove == NULL) {
				Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
				abort();
			}

			if (layer_type == QWALK_LAYER_TYPE_EARTH) {
				*canmove = QATTR_KEY_CANMOVE_QWALK_DEFAULT_EARTH;
			} else {
				*canmove = QATTR_KEY_CANMOVE_QWALK_DEFAULT_FLOATER;
			}
			data = (Qdata_t *) canmove;
			break;


		default:
			Q_ERRORFOUND(QERROR_INDEX_OUTOFRANGE);
			abort();
		}

		datameta = qdatameta_create((Qdata_t *) data, data_type, count);
		if (datameta == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			abort();
		}

		r = qattr_list_attr_set(attr_list, key, datameta);
		if (r == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
	}



	return attr_list;
}


/**
 * Print the help message to @c stdout and exit.
 */
void
devel_walk_print_help() {
	if (fprintf(stdout,
				"---QUIETUDE---\n"
				"\n"
				"Without a specified filename, projects are saved in walk_area.dat\n"
				"\n"
				"Usage: devel_walk [-f filename] [-h]\n"
				"\n"
				"-f <filename> Load from and use as save file\n"
				"-h            Print help (this message) and exit\n"
				) < 0) {
		Q_ERRORFOUND (QERROR_ERRORVAL);
	}
	return;
}
