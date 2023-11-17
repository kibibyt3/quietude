/**
 * @file devel_walkl.c
 * Logic submodule for devel_walk.
 */



#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "mode.h"
#include "qattr.h"
#include "qwalk.h"
#include "devel_walk.h"



static int  devel_walkl_cursor_move(int *, DevelWalkCmd_t);
static bool devel_walkl_coords_arevalid(int, int, int)/*@*/;



/* 
 * TODO: maybe write individual wrappers for each command. this starts to get
 * messy.
 */
/**
 * Pass a logic tick for devel_walk.
 * @param[out] walk_area: #QwalkArea_t to operate on.
 * @param[out] curs_loc:  y, x, z coords of cursor.
 * @param[in]  cmd:       #DevelWalkCmd_t to execute on @p walk_area.
 * @return #Q_OK, #Q_ERROR, or #Q_ERROR_NOCHANGE if nothing was pasted.
 */
int
devel_walkl_tick(QwalkArea_t *walk_area, int *curs_loc, DevelWalkCmd_t cmd) {

	/*@only@*/static QattrList_t *attr_list_clipboard = NULL;
	QattrList_t *attr_listr;

	/* param validation */
	if ((walk_area == NULL) || (curs_loc == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if ((cmd < (DevelWalkCmd_t) Q_ENUM_VALUE_START) || (cmd > DEVEL_WALK_CMD_COUNT)) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERROR;
	}

	/* check if cmd is a cursor movement command */
	if ((cmd >= DEVEL_WALK_CMD_MOVE_MIN) && (cmd <= DEVEL_WALK_CMD_MOVE_MAX)) {
		if (devel_walkl_cursor_move(curs_loc, cmd) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
	}

	/* check if cmd is a modify command */
	if ((cmd >= DEVEL_WALK_CMD_MODIFY_MIN) && (cmd <= DEVEL_WALK_CMD_MODIFY_MAX)) {



		/* handle the copy command */
		if (cmd == DEVEL_WALK_CMD_COPY) {
			if ((attr_listr = devel_walkl_loc_attr_list_get(walk_area, curs_loc)) == NULL) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return Q_ERROR;
			}
		
			if (attr_list_clipboard != NULL) {
				if (qattr_list_destroy(attr_list_clipboard) == Q_ERROR) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
					return Q_ERROR;
				}
				attr_list_clipboard = NULL;
			}

			if ((attr_list_clipboard = qattr_list_clone(attr_listr)) == NULL) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return Q_ERROR;
			}

			if (devel_walkio_message_print(DEVEL_WALKIO_MESSAGE_COPY_SUCCESS) == Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return Q_ERROR;
			}
			return Q_OK;
		}



		/* handle the paste command */
		if (cmd == DEVEL_WALK_CMD_PASTE) {

			/* prevent user from pasting from an empty clipboard */
			if (attr_list_clipboard == NULL) {
				if (devel_walkio_message_print(DEVEL_WALKIO_MESSAGE_PASTE_ERROR) == Q_ERROR) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
					return Q_ERROR;
				}
				return Q_ERROR_NOCHANGE;
			}

			/*@only@*/QattrList_t *attr_list_paste;
			if ((attr_list_paste = qattr_list_clone(attr_list_clipboard)) == NULL) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return Q_ERROR;
			}

			if (devel_walkl_loc_attr_list_set(walk_area, curs_loc, attr_list_paste) == Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				abort();
			}

			if (devel_walkio_message_print(DEVEL_WALKIO_MESSAGE_PASTE_SUCCESS) == Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return Q_ERROR;
			}
			return Q_OK;
		}
		
		/* handle the attr delete command */
		if (cmd == DEVEL_WALK_CMD_ATTR_DELETE) {
			QattrList_t *attr_list;
			QattrKey_t key;
			if ((attr_list = devel_walkl_loc_attr_list_get(walk_area, curs_loc)) == NULL) {
				Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
				return Q_ERROR;
			}
			if ((key = qattr_list_attr_key_get(attr_list, devel_walkio_userint_get()))
					== (QattrKey_t) Q_ERRORCODE_ENUM) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return Q_ERROR;
			}
			/*@i1@*/if (qattr_list_attr_delete(&attr_list, key) == Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return Q_ERROR;
			}
			return Q_OK;
		}
		
		/* handle the edit command */
		if (cmd == DEVEL_WALK_CMD_EDIT) {
			QattrList_t *attr_list;
			QattrKey_t key;
			Qdatameta_t *datameta;
			QobjType_t obj_type;
			QobjType_t *obj_typep;
			char  *char_data; 
			size_t userstring_len;

			if ((attr_list = devel_walkl_loc_attr_list_get(walk_area, curs_loc)) == NULL) {
				Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
				return Q_ERROR;
			}

			/* devel_walkio sets userint to the index of the changed attribute */
			if ((key = qattr_list_attr_key_get(attr_list, devel_walkio_userint_get()))
					== (QattrKey_t) Q_ERRORCODE_ENUM) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return Q_ERROR;
			}
			
			switch (key) {
			case QATTR_KEY_QOBJECT_TYPE:
				/* 
				 * devel_walkio sets userstring to the string version of the new value
				 */
				/*@i1@*/if ((obj_type = qobj_string_to_type(devel_walkio_userstring_get()))
						== (QobjType_t) Q_ERRORCODE_ENUM) {
					if (devel_walkio_message_print(DEVEL_WALKIO_MESSAGE_MODIFY_ERROR) == Q_ERROR) {
						Q_ERRORFOUND(QERROR_ERRORVAL);
						return Q_ERROR;
					}
					return Q_OK;
				}

				if ((obj_typep = calloc((size_t) 1, sizeof(QobjType_t))) == NULL) {
					Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
					return Q_ERROR;
				}

				*obj_typep = obj_type;
				if ((datameta = qdatameta_create((Qdata_t *) obj_typep, 
								QDATA_TYPE_QOBJECT_TYPE, (size_t) 1)) == NULL) {
					Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
					abort();
				}
				
				break;


			case QATTR_KEY_CANMOVE:
				
				bool *boolptr;
				if ((boolptr = calloc((size_t) 1, sizeof(*boolptr))) == NULL) {
					Q_ERRORFOUND(QERROR_SYSTEM_MEMORY);
					return Q_ERROR;
				}
				
				/*
				 * devel_walkio sets userstring to the string version of the new value
				 */
				*boolptr = charstring_to_bool(devel_walkio_userstring_get());

				if ((datameta = qdatameta_create((Qdata_t *) boolptr, 
							QDATA_TYPE_BOOL, (size_t) 1)) == NULL) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
					abort();
				}
				break;


			case QATTR_KEY_NAME:
			/*@fallthrough@*/
			case QATTR_KEY_DESCRIPTION_BRIEF:
			/*@fallthrough@*/
			case QATTR_KEY_DESCRIPTION_LONG:
				/*@i1@*/userstring_len = strlen(devel_walkio_userstring_get()) + (size_t) 1;
				if ((char_data = calloc(userstring_len, sizeof(*char_data))) == NULL) {
					Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
					return Q_ERROR;
				}
				/*@i2@*/strcpy(char_data, devel_walkio_userstring_get());

				if ((datameta = qdatameta_create((Qdata_t *) char_data,
								QDATA_TYPE_CHAR_STRING, userstring_len)) == NULL) {
					Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
					return Q_ERROR;
				}

				break;
			
			default:
				Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
				return Q_ERROR;
			}
			/*@i1@*/if (qattr_list_attr_modify(attr_list, key, datameta) == Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
			}
		}
	}
	return Q_OK;
}


/**
 * Move the cursor according to a command.
 * @param[out] curs_loc: y, x, z coords.
 * @param[in]  cmd:      relevant #DevelWalkCmd_t.
 * @return #Q_OK, #Q_ERROR, or #Q_ERROR_NOCHANGE if the move wasn't allowed.
 */
int
devel_walkl_cursor_move(int *curs_loc, DevelWalkCmd_t cmd) {
	int y_new = curs_loc[0];
	int x_new = curs_loc[1];
	int z_new = curs_loc[2];

	if (!devel_walkl_coords_arevalid(y_new, x_new, z_new))	{
		Q_ERRORFOUND(QERROR_INDEX_OUTOFRANGE);
		return Q_ERROR;
	}
	
	switch (cmd) {
	case DEVEL_WALK_CMD_CURSOR_MOVE_NORTH:
		y_new += (QDIRECTION_MULTIPLIER_DEFAULT * QDIRECTION_NORTH_Y_MULTIPLICAND);
		break;
	case DEVEL_WALK_CMD_CURSOR_MOVE_EAST:
		x_new += (QDIRECTION_MULTIPLIER_DEFAULT * QDIRECTION_EAST_X_MULTIPLICAND);
		break;
	case DEVEL_WALK_CMD_CURSOR_MOVE_SOUTH:
		y_new += (QDIRECTION_MULTIPLIER_DEFAULT * QDIRECTION_SOUTH_Y_MULTIPLICAND);
		break;
	case DEVEL_WALK_CMD_CURSOR_MOVE_WEST:
		x_new += (QDIRECTION_MULTIPLIER_DEFAULT * QDIRECTION_WEST_X_MULTIPLICAND);
		break;
	case DEVEL_WALK_CMD_CURSOR_TOGGLE_ALTITUDE:
		if (z_new == 0) {
			z_new = 1;
		} else if (z_new == 1) {
			z_new = 0;
		} else {
			Q_ERRORFOUND(QERROR_INDEX_OUTOFRANGE);
			return Q_ERROR;
		}
		break;
	default:
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERROR;
	}

	if (devel_walkl_coords_arevalid(y_new, x_new, z_new)) {
		curs_loc[0] = y_new;
		curs_loc[1] = x_new;
		curs_loc[2] = z_new;
		return Q_OK;
	} else {
		return Q_ERROR_NOCHANGE;
	}
}


/**
 * Get a #QattrList_t from a #QwalkArea_t according to coordinates.
 * @param[in] walk_area: contextual #QwalkArea_t.
 * @param[in] curs_loc:  y, x, z coords in @p walk_area.
 * @return desired #QattrList_t or @c NULL.
 */
QattrList_t *
devel_walkl_loc_attr_list_get(const QwalkArea_t *walk_area, const int *curs_loc) {
	QwalkLayer_t *layer;
	int index;
	QattrList_t *attr_list;

	if ((walk_area == NULL) || (curs_loc == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	if (!devel_walkl_coords_arevalid(curs_loc[0], curs_loc[1], curs_loc[2])) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return NULL;
	}

	if (curs_loc[2] == 0) {
		layer = qwalk_area_layer_earth_get(walk_area);
	} else if (curs_loc[2] == 1) {
		layer = qwalk_area_layer_floater_get(walk_area);
	} else {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return NULL;
	}

	if ((index = qwalk_coords_to_index(curs_loc[0], curs_loc[1])) == Q_ERRORCODE_INT) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}
	
	if ((attr_list = qwalk_layer_object_attr_list_get(layer, index)) == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}

	return attr_list;
}


/**
 * Set a #QattrList_t in a #QwalkArea_t according to coordinates.
 * The object's old #QattrList_t is free'd.
 * @param[out] walk_area: contextual #QwalkArea_t.
 * @param[in] curs_loc:  y, x, z coords in @p walk_area.
 * @param[in] attr_list: #QattrList_t reference to give to @p walk_area.
 * @return #Q_OK or #Q_ERROR.
 */
int
devel_walkl_loc_attr_list_set(QwalkArea_t *walk_area, const int *curs_loc, QattrList_t *attr_list) {
	QwalkLayer_t *layer;
	int index;

	if ((walk_area == NULL) || (curs_loc == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		if (qattr_list_destroy(attr_list) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
		return Q_ERROR;
	}
	if (!devel_walkl_coords_arevalid(curs_loc[0], curs_loc[1], curs_loc[2])) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		if (qattr_list_destroy(attr_list) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
		return Q_ERROR;
	}

	if (curs_loc[2] == 0) {
		layer = qwalk_area_layer_earth_get(walk_area);
	} else if (curs_loc[2] == 1) {
		layer = qwalk_area_layer_floater_get(walk_area);
	} else {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		if (qattr_list_destroy(attr_list) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
		return Q_ERROR;
	}

	if ((index = qwalk_coords_to_index(curs_loc[0], curs_loc[1])) == Q_ERRORCODE_INT) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		if (qattr_list_destroy(attr_list) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
		return Q_ERROR;
	}
	
	if (qattr_list_destroy(layer->objects[index].attr_list) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}

	layer->objects[index].attr_list = attr_list;

	return Q_OK;
}


/**
 * Check the validity of a set of coordinates.
 * @param[in] y: y coord.
 * @param[in] x: x coord.
 * @param[in] z: z coord.
 * @return @c true or @c false.
 */
bool
devel_walkl_coords_arevalid(int y, int x, int z) {
	/* check against minimum values */
	if ((y < QWALK_LAYER_COORD_MINIMUM)
			|| (x < QWALK_LAYER_COORD_MINIMUM)
			|| (z < QWALK_LAYER_COORD_MINIMUM)) {
		return false;
	/* check y and x maximum values */
	} else if ((y >= QWALK_LAYER_SIZE_Y) || (x >= QWALK_LAYER_SIZE_X)) {
		return false;
	/* check z maximum value */
	} else if (z > (QWALK_AREA_TOTAL_LAYER_COUNT - 1)) {
		return false;
	} else {
		return true;
	}

}
