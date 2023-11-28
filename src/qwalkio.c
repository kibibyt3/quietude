/**
 * @file qwalkio.c
 * Program file for the I/O section of the qwalk module.
 */



#include <stdlib.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "qattr.h"
#include "mode.h"
#include "qwalk.h"



/**
 * @defgroup InputChars Input characters
 * Characters meant to be input by the user.
 * As opposed to @ref OutputChars. Character space betwixt these two overlaps;
 * this is intentional and acceptable behaviour.
 * @{
 */

/** Input character for #QWALK_COMMAND_MOVE_NORTH. */
#define QWALK_ICH_MOVE_NORTH 'w'
/** Input character for #QWALK_COMMAND_MOVE_EAST. */
#define QWALK_ICH_MOVE_EAST  'd'
/** Input character for #QWALK_COMMAND_MOVE_SOUTH. */
#define QWALK_ICH_MOVE_SOUTH 's'
/** Input character for #QWALK_COMMAND_MOVE_WEST. */
#define QWALK_ICH_MOVE_WEST  'a'
/** Input character for #QWALK_COMMAND_WAIT. */
#define QWALK_ICH_WAIT       '.'
/** Input character for #QWALK_COMMAND_EXIT. */
#define QWALK_ICH_EXIT       'q'

/** @} */



/**
 * @defgroup OutputChars Output characters
 * Characters meant to be output to the user.
 * As opposed to @ref InputChars. Character space betwixt these two overlaps;
 * this is intentional and acceptable behaviour.
 */

/**
 * @ingroup OutputChars
 * Output character for #QOBJ_TYPE_PLAYER.
 */
#define QWALK_OCH_PLAYER '@'
/**
 * @ingroup OutputChars
 * Output character for #QOBJ_TYPE_GRASS.
 */
#define QWALK_OCH_GRASS '.'
/**
 * @ingroup OutputChars
 * Output character for #QOBJ_TYPE_TREE.
 */
#define QWALK_OCH_TREE 'T'
/**
 * @ingroup OutputChars
 * Output character for #QOBJ_TYPE_VOID.
 */
#define QWALK_OCH_VOID ' '




/** qwalk's IO window. */
/*@null@*/static WINDOW *win = NULL;



static QwalkCommand_t qwalk_input_to_command(int)/*@*/;



/**
 * Initialize the IO module of qwalk.
 * @param[out] argwin: @c WINDOW to set as the qwalk window
 * @return #Q_OK or #Q_ERROR.
 */
int
qwalk_io_init(WINDOW *argwin)/*@modifies win@*/{

	int returnval = Q_OK;
	if (argwin == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	win = argwin;
	if (curs_set(0) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	return returnval;
}


/**
 * Terminate the I/O module of qwalk.
 */
void
qwalk_io_end()/*@modifies win@*/{
	win = NULL;
}

/**
 * Pass the subtick step of getting player input.
 * @return #QwalkCommand_t associated with player input.
 */
QwalkCommand_t
qwalk_input_subtick() {
	if (win == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return (QwalkCommand_t) Q_ERRORCODE_ENUM;
	}
	int ch;
	QwalkCommand_t cmd;
	ch = wgetch(win);
	cmd = qwalk_input_to_command(ch);
	if (cmd == (QwalkCommand_t) Q_ERRORCODE_ENUM) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return (QwalkCommand_t) Q_ERRORCODE_ENUM;
	}
	return cmd;
}


/**
 * Pass the subtick step of outputting the game state.
 * @return #Q_OK or #Q_ERROR.
 */
int
qwalk_output_subtick(const QwalkArea_t *walk_area) {
	/*@observer@*/QattrList_t *layer_object_attr_list;
	/*@observer@*/Qdatameta_t *datameta_value;
	QobjType_t *obj_typep;
	chtype outch;
	int *coords;
	int r;
	if (win == NULL) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
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
			
			/* if *obj_typep isn't a layer_floater void, print it to the screen */
			if ((*obj_typep != QOBJ_TYPE_VOID) || (i != 1)) {
				
				outch = qwalk_obj_type_to_chtype(*obj_typep);
				if (outch == (chtype) ERR) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
					abort();
				}
	
				coords = qwalk_index_to_coords(j);
				r = mvwaddch(win, coords[0], coords[1], outch);
				free(coords);
				if (r != OK) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
					return Q_ERROR;
				}
			}
		}
	}
	return Q_OK;
}


/**
 * Convert a raw player input to a #QwalkCommand_t.
 * @param[in] ch: raw input character from e.g. `getch()`.
 * @return #QwalkCommand_t associated with @p ch.
 */
QwalkCommand_t
qwalk_input_to_command(int ch) {
	switch (ch) {
	case QWALK_ICH_MOVE_NORTH:
		return QWALK_COMMAND_MOVE_NORTH;
	case QWALK_ICH_MOVE_EAST:
		return QWALK_COMMAND_MOVE_EAST;
	case QWALK_ICH_MOVE_SOUTH:
		return QWALK_COMMAND_MOVE_SOUTH;
	case QWALK_ICH_MOVE_WEST:
		return QWALK_COMMAND_MOVE_WEST;
	case QWALK_ICH_WAIT:
		return QWALK_COMMAND_WAIT;
	case QWALK_ICH_EXIT:
		return QWALK_COMMAND_EXIT;
	default:
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return (QwalkCommand_t) Q_ERRORCODE_ENUM;
	}
}


/**
 * Convert a #QobjType_t to a `chtype`.
 * #QOBJ_TYPE_VOID functions in-game as a transparent layer;
 * @param[in] obj_type: #QobjType_t to convert.
 * @return converted `chtype` or `ERR`.
 * @note `ERR` is used instead of `Q_ERRORCODE_INT` or some such thing to
 * enforce compatibility with ncurses.
 */
chtype
qwalk_obj_type_to_chtype(QobjType_t obj_type) {
	if ((obj_type < (QobjType_t) Q_ENUM_VALUE_START)
			|| (obj_type > (QobjType_t) QOBJ_TYPE_COUNT)) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return (chtype) ERR; 
	}

	/* conversion switch-case proper */
	switch (obj_type) {
	case QOBJ_TYPE_PLAYER:
		return (chtype) QWALK_OCH_PLAYER;
	case QOBJ_TYPE_GRASS:
		return (chtype) QWALK_OCH_GRASS;
	case QOBJ_TYPE_TREE: 
		return (chtype) QWALK_OCH_TREE;
	case QOBJ_TYPE_VOID: 
		return (chtype) QWALK_OCH_VOID;
	default:
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return (chtype) ERR;
	}
}
