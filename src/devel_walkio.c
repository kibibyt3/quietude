/**
 * @file devel_walkio.c
 * I/O submodule for devel_walk.
 * Does not handle internal logic; takes in and output what it's told to.
 */



#include <stdlib.h>
#include <stddef.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "mode.h"
#include "qattr.h"
#include "qwalk.h"
#include "devel_walk.h"



/*@null@*/static WINDOW *area_win = NULL;
/*@null@*/static WINDOW *info_win = NULL;



static int devel_walkio_area_out(const QwalkArea_t *, const int *); 
static int devel_walkio_info_out(const QwalkArea_t *, const int *);



/**
 * Initialize devel_walkio.
 */
int
devel_walkio_init(WINDOW *area_argwin, WINDOW *info_argwin) {
	if ((area_argwin == NULL) || (info_argwin == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if ((area_win != NULL) || (info_win != NULL)) {
		Q_ERRORFOUND(QERROR_NONNULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	area_win = area_argwin;
	info_win = info_argwin;
	return Q_OK;
}

/**
 * Terminate devel_walkio.
 */
int
devel_walkio_end() {
	return Q_OK;
}


/**
 * Take input from the user.
 */
DevelWalkCmd_t
devel_walkio_in() {
	if ((area_win == NULL) || (info_win == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
	}
	int ch;
	DevelWalkCmd_t cmd;
	ch = wgetch(area_win);
	cmd = devel_walkio_input_to_command(ch);
	if (cmd == (DevelWalkCmd_t) Q_ERRORCODE_ENUM) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
	}
	return cmd;
}


/**
 * Output a #QwalkArea_t and focus object info.
 * @param[in] walk_area: relevant #QwalkArea_t.
 * @param[in] curs_loc: y, x, and z coords of cursor.
 * @return #Q_OK or #Q_ERROR.
 */
int
devel_walkio_out(const QwalkArea_t *walk_area, const int *curs_loc) {
	int returnval = Q_OK;
	int r;
	
	if ((area_win == NULL) || (info_win == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERROR;
	}

	r = devel_walkio_area_out(walk_area, curs_loc);
	if (r != Q_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	
	r = devel_walkio_info_out(walk_area, curs_loc);
	if (r != Q_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}

	return returnval;
}


/**
 * Output the state to the screen.
 * @param[in] walk_area: #QwalkArea_t to output to the screen.
 * @param[in] curs_loc: y, x, z coords of the cursor.
 * @return #Q_OK or #Q_ERROR.
 */
int
devel_walkio_area_out(const QwalkArea_t *walk_area, const int *curs_loc) {
	/*@observer@*/QattrList_t *layer_object_attr_list;
	/*@observer@*/Qdatameta_t *datameta_value;
	QobjType_t *obj_typep;
	chtype outch;
	int *coords;
	int r;
	int returnval = Q_OK;

	/* routine error-checking */
	if ((area_win == NULL) || (info_win == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERROR;
	}
	if ((walk_area == NULL) || (curs_loc == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if ((!(qwalk_logic_coords_arevalid(curs_loc[0], curs_loc[1])))
			|| ((curs_loc[2] != 0) && (curs_loc[2] != 1))) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}

	/*@observer@*/QwalkLayer_t *layer_earth; 
	layer_earth = qwalk_area_layer_earth_get(walk_area);
	if (layer_earth == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	
	
	/*@observer@*/QwalkLayer_t *layer_floater; 
	layer_floater = qwalk_area_layer_floater_get(walk_area);
	if (layer_floater == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	
	/*
	 * iterate through both layers and print their contents to the screen;
	 * print everything on layer_earth and then print layer_floater non-void
	 * objects
	 */
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < QWALK_LAYER_SIZE; j++) {
			if (i == 0) {
				layer_object_attr_list = qwalk_layer_object_attr_list_get(layer_earth, j);
			} else {
				layer_object_attr_list = qwalk_layer_object_attr_list_get(layer_floater, j);
			}
			if (layer_object_attr_list == NULL) {
				Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
				return Q_ERROR;
			}
	
			datameta_value = qattr_list_value_get(layer_object_attr_list, QATTR_KEY_QOBJECT_TYPE);
			if (datameta_value == NULL) {
				Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
				return Q_ERROR;
			}
	
			if (qdatameta_type_get(datameta_value) != QDATA_TYPE_QOBJECT_TYPE) {
				Q_ERRORFOUND(QERROR_QDATAMETA_TYPE_INCOMPATIBLE);
				abort();
			}
	
			obj_typep = ((QobjType_t *) (qdatameta_datap_get(datameta_value)));
			if (obj_typep == NULL) {
				Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
				return Q_ERROR;
			}
			
			/* if *obj_typep isn't an empty space, print it to the screen */
			if (*obj_typep != QOBJ_TYPE_VOID) {
				
				outch = qwalk_obj_type_to_chtype(*obj_typep);
				if (outch == (chtype) ERR) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
					abort();
				}
	
				coords = qwalk_index_to_coords(j);
				r = mvwaddch(area_win, coords[0], coords[1], outch);
				free(coords);
				if (r != OK) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
					return Q_ERROR;
				}
			}
		}
	}
	/* TODO: maybe change cursor style depending on curs_loc[2]. */
	if (wmove(area_win, curs_loc[0], curs_loc[1]) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	return returnval;
}


int
devel_walkio_info_out(const QwalkArea_t *walk_area, const int *curs_loc) {
	
	/*@observer@*/QwalkLayer_t *layer_active;
	/*@observer@*/QattrList_t *attr_list;
	int returnval = Q_OK;
	
	/* routine error-checking */
	if ((area_win == NULL) || (info_win == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERROR;
	}
	if ((walk_area == NULL) || (curs_loc == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if ((!(qwalk_logic_coords_arevalid(curs_loc[0], curs_loc[1])))
			|| ((curs_loc[2] != 0) && (curs_loc[2] != 1))) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}

	if (curs_loc[2] == 0) {
		layer_active = qwalk_area_layer_earth_get(walk_area);
	}
	else if (curs_loc[2] == 1) {
		layer_active = qwalk_area_layer_floater_get(walk_area);
	} else {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}

	/* ensure attr_list is entirely filled out. */
	attr_list = qwalk_layer_object_attr_list_get(layer_active,
			qwalk_coords_to_index(curs_loc[0], curs_loc[1]));
	if (attr_list == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if (qattr_list_index_ok_get(attr_list) != qattr_list_count_get(attr_list)) {
		Q_ERRORFOUND(QERROR_STRUCT_INCOMPLETE);
		return Q_ERROR;
	}


	QattrKey_t key;
	Qdatameta_t *valuep;
	Qdata_t *datap;
	
	/* print all key-value pairs. */
	/* TODO: key-to-string and value-to-string conversion. */
	for (int i = 0; i < (int) qattr_list_count_get(attr_list); i++) {
		
		if ((key = qattr_list_attr_key_get(attr_list, i))
				== (QattrKey_t) Q_ERRORCODE_ENUM) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}

		if ((valuep = qattr_list_value_get(attr_list, key)) == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			return Q_ERROR;
		}

		if ((datap = qdatameta_datap_get(valuep)) == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			return Q_ERROR;
		}

		if (wprintw(info_win, "%i : %i\n", (int) key, *((int *) datap)) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			returnval = Q_ERROR;
		}
	}

	return returnval;
}
