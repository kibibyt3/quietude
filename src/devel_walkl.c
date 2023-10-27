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