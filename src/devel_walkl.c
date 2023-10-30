/**
 * @file devel_walkl.c
 * Logic submodule for devel_walk.
 */



#include <stdlib.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "mode.h"
#include "qattr.h"
#include "qwalk.h"
#include "devel_walk.h"



static int  devel_walkl_cursor_move(int *, DevelWalkCmd_t);
static bool devel_walkl_coords_arevalid(int, int, int)/*@*/;


/**
 * Pass a logic tick for devel_walk.
 * @param[out] walk_area: #QwalkArea_t to operate on.
 * @param[out] curs_loc:  y, x, z coords of cursor.
 * @param[in]  cmd:       #DevelWalkCmd_t to execute on @p walk_area.
 * @return #Q_OK or #Q_ERROR.
 */
int
devel_walkl_tick(QwalkArea_t *walk_area, int *curs_loc, DevelWalkCmd_t cmd) {

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
		
		/* handle the edit command */
		if (cmd == DEVEL_WALK_CMD_EDIT) {
			QattrList_t *attr_list;
			QattrKey_t key;
			Qdatameta_t *datameta;
			QobjType_t obj_type;
			QobjType_t *obj_typep;
			/* char  *char_data;      */
			/* size_t userstring_len; */

			/*
			 * TODO: implement this properly! this section will be invoked whenever we
			 * need to directly copy player input into a char * value.
			 *
			userstring_len = strlen(devel_walkio_userstring_get() + 1);
			if ((data = calloc(userstring_len, sizeof(*data))) == NULL) {
				Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
				return Q_ERROR;
			}

			strcpy(data, devel_walkio_userstring_get());
			*/

			
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
				break;
			default:
				Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
				return Q_ERROR;
			}
			if ((datameta = qdatameta_create((Qdata_t *) obj_typep,
							QDATA_TYPE_QOBJECT_TYPE, (size_t) 1)) == NULL) {
				Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
				abort();
			}
			if (qattr_list_attr_modify(attr_list, key, datameta) == Q_ERROR) {
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
