/**
 * @file qwalkl.c
 * Program file for the logic section of the qwalk module.
 */



#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "mode.h"
#include "qattr.h"
#include "qwalk.h"




static int qwalk_logic_interact(QwalkLayer_t *layer_earth,
		QwalkLayer_t *layer_floater, int object_index);
static           int          qwalk_logic_obj_move(/*@null@*/QwalkLayer_t *, int, Qdirection_t);
static           int          qwalk_logic_objs_locs_trade(/*@null@*/QwalkLayer_t *, int, int);
static /*@null@*/QobjType_t  *qwalk_logic_walk_layer_sanitize(QwalkLayer_t *)/*@*/;
static           void         qwalk_logic_qobj_type_destroy(/*@only@*/QobjType_t *);
static           bool         qwalk_logic_layer_object_canmove(const QwalkLayer_t *, int);
/*@observer@*/
static /*@null@*/Qdatameta_t *qwalk_logic_layer_object_attr_value_get(const QwalkLayer_t *, int, QattrKey_t);
/*@observer@*/
static /*@null@*/QattrList_t *qwalk_logic_layer_object_attr_list_get(const QwalkLayer_t *, int); 
static           int          qwalk_logic_find_qobj_index(/*@null@*/QobjType_t *, QobjType_t)/*@*/;
static           Qdirection_t qwalk_logic_command_move_to_direction(QwalkCommand_t)/*@*/;




/**
 * Pass a logical tick for qwalk.
 * @param[out] walk_area: #QwalkArea_t to pass the tick on.
 * @param[in]  walk_command: #QwalkCommand_t to execute on @p walk_area.
 * @return #Q_OK or #Q_ERROR.
 */ 
int
qwalk_logic_subtick(QwalkArea_t *walk_area, QwalkCommand_t walk_command) {

	/* Index of the player in walk_layer and obj_types */
	int player_index;

	/* Player's direction, if applicable */
	Qdirection_t player_direction;

	/*
	 * Dynamic arrays of every #QwalkObj_t #QobjType_t #Qattr_t for each
	 * #QwalkLayer_t.
	 * Indexes are assumed to be in lockstep with each #QwalkLayer_t.
	 */
	QobjType_t *obj_types_layer_earth;
	QobjType_t *obj_types_layer_floater;

	QwalkLayer_t *layer_earth;
	QwalkLayer_t *layer_floater;

	if (walk_area == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}

	/*
	 * if the player is exiting, we don't need to worry about the rest of the
	 * logic.
	 */
	if (walk_command == QWALK_COMMAND_EXIT) {
		if (mode_buffer_switch(MODE_T_EXIT, NULL) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		return Q_OK;
	}
	
	layer_earth   = qwalk_area_layer_earth_get(walk_area);
	layer_floater = qwalk_area_layer_floater_get(walk_area);

	obj_types_layer_earth = qwalk_logic_walk_layer_sanitize(layer_earth);
	if (obj_types_layer_earth == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	
	obj_types_layer_floater = qwalk_logic_walk_layer_sanitize(layer_floater);
	if (obj_types_layer_floater == NULL) {
		qwalk_logic_qobj_type_destroy(obj_types_layer_earth);
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}


	/* find the player's index */
	/*
	 * Very possible bug could arise here if there are multiple players! This code
	 * assumes there's either 1 or 0 players on the map!
	 */
	player_index = qwalk_logic_find_qobj_index(obj_types_layer_floater, QOBJ_TYPE_PLAYER);
	if (player_index == Q_ERRORCODE_INT) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		qwalk_logic_qobj_type_destroy(obj_types_layer_earth);
		qwalk_logic_qobj_type_destroy(obj_types_layer_floater);
		return Q_ERROR;
	}
	if (player_index == Q_ERRORCODE_INT_NOTFOUND) {
		/* 
		 * This because Q_ERRORCODE_INT_NOTFOUND is returned by
		 * qwalk_logic_find_obj_index() when it finds zero instances of the search
		 * query. This is impossible; we must have a player.
		 */
		Q_ERRORFOUND(QERROR_ZERO_VALUE_UNEXPECTED);
		qwalk_logic_qobj_type_destroy(obj_types_layer_earth);
		qwalk_logic_qobj_type_destroy(obj_types_layer_floater);
		return Q_ERROR;
	}


	/* check for & handle movement commands */
	if ((walk_command >= QWALK_COMMAND_MOVE_MIN) && (walk_command <= QWALK_COMMAND_MOVE_MAX)) {
		
		player_direction = qwalk_logic_command_move_to_direction(walk_command);
		if ((int) player_direction == Q_ERRORCODE_ENUM) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			qwalk_logic_qobj_type_destroy(obj_types_layer_earth);
			qwalk_logic_qobj_type_destroy(obj_types_layer_floater);
			return Q_ERROR;
		}

		if (qwalk_logic_obj_move(qwalk_area_layer_floater_get(walk_area),
				player_index, player_direction)
				==  Q_ERROR ) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			qwalk_logic_qobj_type_destroy(obj_types_layer_earth);
			qwalk_logic_qobj_type_destroy(obj_types_layer_floater);
			return Q_ERROR;
		}
	}

	/* interact command */
	if (walk_command == QWALK_COMMAND_INTERACT) {
		int object_index = qwalk_io_buffer_int_get();
		if (qwalk_logic_interact(layer_earth, layer_floater, object_index) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			qwalk_logic_qobj_type_destroy(obj_types_layer_earth);
			qwalk_logic_qobj_type_destroy(obj_types_layer_floater);
			return Q_ERROR;
		}
	}

	qwalk_logic_qobj_type_destroy(obj_types_layer_earth);
	obj_types_layer_earth = NULL;
	
	qwalk_logic_qobj_type_destroy(obj_types_layer_floater);
	obj_types_layer_floater = NULL;

	return Q_OK;
}


/**
 * Have the player interact with a given layer object.
 * @param[out] layer_earth: earth #QwalkLayer_t of the #QwalkArea_t.
 * @param[out] layer_floater: floater #QwalkLayer_t of the #QwalkArea_t.
 * @param[in] object_index: index of the object to interact with.
 * @return #Q_ERROR or #Q_OK.
 */
int
qwalk_logic_interact(QwalkLayer_t *layer_earth, QwalkLayer_t *layer_floater,
		int object_index) {

	QobjType_t earth_object_type, floater_object_type, active_object_type;

	QwalkLayer_t *active_layer;

	if ((earth_object_type = qwalk_layer_object_type_get(
					layer_earth, object_index)) == (QobjType_t) Q_ERRORCODE_ENUM) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	if ((floater_object_type = qwalk_layer_object_type_get(
					layer_floater, object_index)) == (QobjType_t) Q_ERRORCODE_ENUM) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	/* 
	 * if the floater type is void, we operate on the earth layer. otherwise, we
	 * operate on the floater layer.
	 */
	if (floater_object_type == QOBJ_TYPE_VOID) {
		active_layer = layer_earth;
		active_object_type = earth_object_type;
	} else {
		active_layer = layer_floater;
		active_object_type = floater_object_type;
	}

	switch (active_object_type) {
		case QOBJ_TYPE_NPC_FRIENDLY:
			if (qwalk_dialogue(active_layer, object_index) == Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return Q_ERROR;
			}
			break;
		default:
			Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
			return Q_ERROR;
	}

	return Q_OK;
}


/**
 * Try to move a #QwalkObj_t to a new location.
 * Moreover, move the new location's previous occupant to the moving object's
 * previous location (this must be done due to implemenation limitations).
 * If the location to attempt to move to is valid, the move occurs. Otherwise,
 * the old coordinates remain and nothing is changed.
 * @param[out] walk_layer: walk_layer to operate on
 * @param[in]  index:      index in walk_layer of the object to attempt to move
 * @param[in] direction:   #Qdirection_t to attempt to move in
 * @return #Q_OK, #Q_ERROR_NOCHANGE if the attempt to move resulted in illegal
 * behaviour (e.g. passing through walls), or #Q_ERROR.
 */
int
qwalk_logic_obj_move(QwalkLayer_t *walk_layer, int index, Qdirection_t direction) {
	int returnval = Q_OK;

	int y_old;
	int x_old;

	int y_new;
	int x_new;
	
	/* Previous occupant of the square to move walk_layer[index] to */
	int coord_occupant_old_index;

	/* The object trying to move, i.e. walk_layer[index] */
	int coord_occupant_mover_index;

	y_old = qwalk_layer_object_coord_y_get(walk_layer, index);
	x_old = qwalk_layer_object_coord_x_get(walk_layer, index);
 	
	y_new = y_old;
	x_new = x_old;

	if (walk_layer == NULL) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}
	
	/* modify the coords and check if they're allowed */
	switch (direction) {
	case QDIRECTION_NORTH:
		y_new = y_old + (QDIRECTION_MULTIPLIER_DEFAULT * QDIRECTION_NORTH_Y_MULTIPLICAND);
		break;
	case QDIRECTION_EAST:
		x_new = x_old + (QDIRECTION_MULTIPLIER_DEFAULT * QDIRECTION_EAST_X_MULTIPLICAND);
		break;
	case QDIRECTION_SOUTH:
		y_new = y_old + (QDIRECTION_MULTIPLIER_DEFAULT * QDIRECTION_SOUTH_Y_MULTIPLICAND);
		break;
	case QDIRECTION_WEST:
		x_new = x_old + (QDIRECTION_MULTIPLIER_DEFAULT * QDIRECTION_WEST_X_MULTIPLICAND);
		break;
	default:
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERROR;
	}
	if (!qwalk_logic_coords_arevalid(y_new, x_new)) {
		return Q_ERROR_NOCHANGE; 
	}

	coord_occupant_mover_index = qwalk_coords_to_index(y_old, x_old);
	coord_occupant_old_index = qwalk_coords_to_index(y_new, x_new);

	/* only trade spots if the previous occupant is allowed to move */
	if (qwalk_logic_layer_object_canmove(walk_layer, coord_occupant_old_index)) {	
		if (qwalk_logic_objs_locs_trade(walk_layer, 
					coord_occupant_mover_index, coord_occupant_old_index) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			returnval = Q_ERROR;
		}
	}

	return returnval;
}


/**
 * Make two #QwalkObj_t on the same #QwalkLayer_t exchange places.
 * This is implemented in this fashion in order to adhere to the rule: every
 * #QwalkLayer_t must have <i>exactly</i> #QWALK_LAYER_SIZE many #QwalkObj_t on
 * it at any given time; therefore, if a #QwalkObj_t moves, it must exchange
 * places with a different #QwalkObj_t, even if one of the aforementioned is
 * merely a #QOBJ_TYPE_VOID.
 * @param[out] walk_layer:  #QwalkLayer_t to operate on
 * @param[in] mover_index:  index of that #QwalkObj_t which dislocates another.
 * @param[in] movend_index: index of that #QwalkObj_t which is dislocated by another.
 * @return #Q_OK or #Q_ERROR.
 */
int
qwalk_logic_objs_locs_trade(QwalkLayer_t *walk_layer, int mover_index, int movend_index) {
	if (walk_layer == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if ((mover_index < 0) || (mover_index >= QWALK_LAYER_SIZE)
			|| (movend_index < 0) || (movend_index >= QWALK_LAYER_SIZE)) {
		Q_ERRORFOUND(QERROR_INDEX_OUTOFRANGE);
		return Q_ERROR;
	}

	/*
	 * Exchange each param's attr_lists (yielding the effect of switching
	 * places).
	 */
	/*@only@*/QattrList_t *attr_list_buffer;
	attr_list_buffer = walk_layer->objects[mover_index].attr_list;
	walk_layer->objects[mover_index].attr_list = walk_layer->objects[movend_index].attr_list;
	walk_layer->objects[movend_index].attr_list = attr_list_buffer;
	attr_list_buffer = NULL;
	return Q_OK;
}


/**
 * Error-check a #QwalkLayer_t and create an array of every #QobjType_t therein.
 * This is for ease of future indexing and to sanitize inputs to dispose of the
 * need for excessive error-checking code; i.e., this function's params and
 * return value are assumed to be error-free if a non-`NULL` value is returned.
 * The exception is everything under walk_layer->objects[].attr_list, barring
 * the #QATTR_KEY_QOBJECT_TYPE value. The verb sanitize is certainly a
 * misnomer, but this function's identifier really should only be so long!
 * @param[in] walk_layer: #QwalkLayer_t to parse and proof-read 
 * @return Dynamic #QobjType_t array or @c NULL if an error was encountered
 */
QobjType_t *
qwalk_logic_walk_layer_sanitize(QwalkLayer_t *walk_layer) {
	QattrList_t           *attr_list;
	Qdatameta_t           *datameta_object_type;
	QobjType_t            *object_type;

	/*@only@*/QobjType_t  *obj_types;
	obj_types = calloc((size_t) QWALK_LAYER_SIZE, sizeof(*obj_types));

	if (obj_types == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	/* parse starting from the outside and travelling to the inside */
	if (walk_layer == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		free(obj_types);
		return NULL;
	}

	for (int i = 0; i < QWALK_LAYER_SIZE; i++) {
		
		if (walk_layer->objects[i].coord_x < QWALK_LAYER_COORD_MINIMUM) {
			Q_ERRORFOUND(QERROR_NEGATIVE_VALUE_UNEXPECTED);
			free(obj_types);
			return NULL;
		}
		if (walk_layer->objects[i].coord_y < QWALK_LAYER_COORD_MINIMUM) {
			Q_ERRORFOUND(QERROR_NEGATIVE_VALUE_UNEXPECTED);
			free(obj_types);
			return NULL;
		}
    
		/*
		 * TODO: in future, implement an interface function to nab the attr_list
		 * that we can mark with the splint observer annotation!
		 */
		attr_list            = walk_layer->objects[i].attr_list;
		if (attr_list == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			free(obj_types);
			return NULL;
		}
		
		datameta_object_type = qattr_list_value_get(attr_list, QATTR_KEY_QOBJECT_TYPE);
		if (datameta_object_type == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			free(obj_types);
			return NULL;
		}
		if (qdatameta_type_get(datameta_object_type) != QDATA_TYPE_QOBJECT_TYPE) {
			Q_ERRORFOUND(QERROR_QDATAMETA_TYPE_INCOMPATIBLE);
			free(obj_types);
			return NULL;
		}
		
		/* ensure QATTR_KEY_OBJECT_TYPE has size of exactly 1 */
		if (qdatameta_count_get(datameta_object_type) != (size_t) 1) {
			Q_ERRORFOUND(QERROR_QDATAMETA_TYPE_COUNT_INCOMPATIBLE);
			free(obj_types);
			return NULL;
		}

		object_type = (QobjType_t *) qdatameta_datap_get(datameta_object_type);
		if (object_type == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			free(obj_types);
			return NULL;
		}
		
		obj_types[i] = *object_type;
	}
	/*@i1@*/return obj_types;
}


/**
 * Safely destroy a dynamic #QobjType_t array.
 * @param[out] obj_types: array to destroy
 */
void
qwalk_logic_qobj_type_destroy(QobjType_t *obj_types) {
	free(obj_types);
	return;
}


/**
 * Check the value of a given object's #QATTR_KEY_CANMOVE attribute.
 * @param[in] layer: Pointer to the #QwalkLayer_t in question.
 * @param[in] index: Index in @p layer.
 * @return #QATTR_KEY_CANMOVE value or #QATTR_KEY_CANMOVE_QWALK_DEFAULT_FLOATER
 * on an error.
 */
bool
qwalk_logic_layer_object_canmove(const QwalkLayer_t *layer, int index) {
	/* check if old occupant can move. only move if it can. */
	Qdatameta_t *datameta;
	Qdata_t     *data;

	if ((datameta = qwalk_logic_layer_object_attr_value_get(layer, index, QATTR_KEY_CANMOVE)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return QATTR_KEY_CANMOVE_QWALK_DEFAULT_FLOATER;
	}
	if ((data = qdatameta_datap_get(datameta)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return QATTR_KEY_CANMOVE_QWALK_DEFAULT_FLOATER;
	}
	return *((bool *) data);
}


/**
 * Get a specific #QattrKey_t value from a #QwalkLayer_t and an index.
 * @param[in] layer: Pointer to the #QwalkLayer_t in question.
 * @param[in] index: Index in @p layer.
 * @param[in] key:  #QattrKey_t of the value to return.
 * @return desired value or NULL on error.
 */
Qdatameta_t *
qwalk_logic_layer_object_attr_value_get(const QwalkLayer_t *layer,
		int index, QattrKey_t key) {
	
	QattrList_t *attr_list;
	if ((attr_list = qwalk_logic_layer_object_attr_list_get(layer, index)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}
	return qattr_list_value_get(attr_list, key);
}


/**
 * Get the #QattrList_t of a given occupant of a #QwalkLayer_t.
 * @param[in] layer: Pointer to the #QwalkLayer_t in question.
 * @param[in] index: Index in @p layer.
 * @return desired #QattrList_t or @c NULL.
 */
QattrList_t *
qwalk_logic_layer_object_attr_list_get(const QwalkLayer_t *layer, int index) {
	return layer->objects[index].attr_list;
}


/**
 * Find the index of a #QwalkObj_t according to a #QobjType_t search query.
 * The #QobjType_t is assumed to be occur either once or zero times; if an
 * object occurs more than once, this function will fail.
 * @param[in] obj_types_parse: QobjType_t array to parse
 * @param[in] obj_type_search: search query; #QobjType_t to check against
 * @return index of desired #QwalkObj_t, <i>OR</i> #Q_ERRORCODE_INT in the
 * event of a fatal error, <i>OR</i> #Q_ERRORCODE_INT_NOTFOUND if the match for
 * obj_type_search cannot be found.
 */
int
qwalk_logic_find_qobj_index(QobjType_t *obj_types_parse, QobjType_t obj_type_search) {
	
	if (obj_types_parse == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERRORCODE_INT;
	}
	if (obj_type_search < (QobjType_t) Q_ENUM_VALUE_START) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID_ZERO);
		return Q_ERRORCODE_INT;
	} else if (obj_type_search > QOBJ_TYPE_COUNT) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERRORCODE_INT;
	}

	for (int i = 0; i < QWALK_LAYER_SIZE; i++) {
		if (obj_types_parse[i] == obj_type_search) {
			return i;
		}	
	}
	return Q_ERRORCODE_INT_NOTFOUND;
}


/**
 * Convert a #QwalkCommand_t to a cardinal direction.
 * Namely a movement-based #QwalkCommand_t
 * @param[in] cmd: input command
 * @return output direction or #Q_ERRORCODE_ENUM
 */
Qdirection_t
qwalk_logic_command_move_to_direction(QwalkCommand_t cmd) {
	switch (cmd) {
	case QWALK_COMMAND_MOVE_NORTH:
		return QDIRECTION_NORTH;
	case QWALK_COMMAND_MOVE_EAST:
		return QDIRECTION_EAST;
	case QWALK_COMMAND_MOVE_SOUTH:
		return QDIRECTION_SOUTH;
	case QWALK_COMMAND_MOVE_WEST:
		return QDIRECTION_WEST;
	default:
		return (Qdirection_t) Q_ERRORCODE_ENUM;
	}
}

/**
 * Tell caller if coordinates are valid for qwalk.
 * Specifically whether they index within the playable space. This function's
 * output is usually expected to be handled gracefully.
 * @param[in] y: y coord to check
 * @param[in] x: x coord to check
 */
bool
qwalk_logic_coords_arevalid(int y, int x) {
	if (y < QWALK_LAYER_COORD_MINIMUM) {
		return false;
	} else if (y >= QWALK_LAYER_SIZE_Y) {
		return false;
	} else if (x < QWALK_LAYER_COORD_MINIMUM) {
		return false;
	} else if (x >= QWALK_LAYER_SIZE_X) {
		return false;
	}
	return true;
}
