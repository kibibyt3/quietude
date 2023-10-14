/**
 * @file qwalkl.c
 * Program file for the logic section of the qwalk module.
 */



#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "qdefs.h"
#include "qerror.h"

#include "qwalk.h"
#include "qattr.h"


/**
 * Direction type.
 * Stored here for now; might be worth moving to qwalk.h eventually?
 */
typedef enum Qdirection_t {
	
	/** North */
	QDIRECTION_NORTH = Q_ENUM_VALUE_START,
	
	QDIRECTION_NORTHEAST, /**< Northeast */
	QDIRECTION_EAST,      /**< East      */
	QDIRECTION_SOUTHEAST, /**< Southeast */
	QDIRECTION_SOUTH,     /**< South     */
	QDIRECTION_SOUTHWEST, /**<	Southwest */
	QDIRECTION_WEST,      /**< West      */
	QDIRECTION_NORTHWEST, /**< Northwest */

	/**
	 * Amount of possible values for a #Qdirection_t
	 * Must be defined by the final @c enum constant.
	 */
	QDIRECTION_COUNT = QDIRECTION_NORTHWEST
} Qdirection_t;

/**
 * Translate north to a y coordinate multiplicand (which is then reduced to an
 * augend depending on its factor, if applicable).
 */
#define QDIRECTION_NORTH_Y_MULTIPLICAND -1

/**
 * Translate east to an x coordinate multiplicand (which is then reduced to an
 * augend depending on its factor, if applicable).
 */
#define QDIRECTION_EAST_X_MULTIPLICAND   1

/**
 * Translate south to a y coordinate multiplicand (which is then reduced to an
 * augend depending on its factor, if applicable)
 */
#define QDIRECTION_SOUTH_Y_MULTIPLICAND  1

/**
 * Translate west to a x coordinate multiplicand (which is then reduced to an
 * augend depending on its factor, if applicable)
 */
#define QDIRECTION_WEST_X_MULTIPLICAND  -1


/**
 * Default multiplier for the multiplicands in the event that it isn't otherwise
 * specified
 */
#define QDIRECTION_MULTIPLIER_DEFAULT    1




static           int         qwalk_move_obj(QwalkObject_t *, Qdirection_t);

static /*@null@*/QobjType_t *qwalk_logic_walk_field_sanitize(/*@in@*/QwalkField_t *)/*@*/;

static void qwalk_logic_qobj_type_destroy(/*@only@*/QobjType_t *);

static int qwalk_logic_find_qobj_index(QobjType_t *obj_types_parse, QobjType_t obj_type_search)/*@*/;

static Qdirection_t qwalk_logic_command_move_to_direction(QwalkCommand_t)/*@*/;

static int qwalk_logic_coords_arevalid(int, int);



int
qwalk_logic_subtick(QwalkField_t *walk_field, QwalkCommand_t walk_command, ModeSwitchData_t *switch_data)
/*@modifies walk_field->objects[].coord_x walk_field->objects[].coord_y *switch_data@*/ {
	
	/* Pointer to the memory that holds the player #QwalkObject_t */
	QwalkObject_t *player = NULL;
	
	/* Index of the player in walk_field and obj_types */
	int player_index;

	/* Player's direction, if applicable */
	Qdirection_t player_direction;

	/*
	 * Dynamic array of every #QwalkObject_t #QobjType_t #Qattr_t.
	 * Indexes are assumed to be in lockstep with walk_field.
	 */
	QobjType_t *obj_types;
	obj_types = qwalk_logic_walk_field_sanitize(walk_field);
	if (obj_types == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}


	/* find the player's index */
	/*
	 * Very possible bug could arise here if there are multiple players! This code
	 * assumes there's either 1 or 0 players on the map!
	 */
	player_index = qwalk_logic_find_qobj_index(obj_types, QOBJ_PLAYER);
	if (player_index == Q_ERRORCODE_INT) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		qwalk_logic_obj_type_destroy(obj_types);
		return Q_ERROR;
	}
	if (player_index == Q_ERRORCODE_INT_NOTFOUND) {
		/* 
		 * This because Q_ERRORCODE_INT_NOTFOUND is returned by
		 * qwalk_logic_find_obj_index() when it finds zero instances of the search
		 * query. This is impossible; we must have a player.
		 */
		Q_ERRORFOUND(QERROR_ZERO_VALUE_UNEXPECTED);
		qwalk_logic_obj_type_destroy(obj_types);
		return Q_ERROR;
	}


	/* check for & handle movement commands */
	if ((cmd >= QWALK_COMMAND_MOVE_MIN) && (cmd <= QWALK_COMMAND_MOVE_MAX)) {
		
		player_direction = qwalk_logic_command_move_to_direction(cmd);
		if (player_direction == Q_ERRORCODE_ENUM) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			qwalk_logic_obj_type_destroy(obj_types);
			return Q_ERROR;
		}
		
		player = qwalk_field_object_get(walk_field, player_index);
		if (player == NULL) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			qwalk_logic_obj_type_destroy(obj_types);
			return Q_ERROR;
		}

		if ( qwalk_move_obj(player, player_direction)  ==  NULL ) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			qwalk_logic_obj_type_destroy(obj_types);
			return Q_ERROR;
		}

	}
	
	/* in future, this secion will handle checking whether the mode must change */
	switch_data->mode = MODE_T_WALK;

	return Q_OK;
}





	qwalk_logic_qobj_type_destroy(obj_types);
	obj_types = NULL;
}


/**
 * Try to move a #QwalkObject_t to a new location.
 * If the location to attempt to move to is valid, the move occurs. Otherwise,
 * the old coordinates remain and nothing is changed.
 * @param[out] walk_object: #QwalkObject_t on whose coords modification is
 * attempted
 * @param[in] direction:    #Qdirection_t to attempt to move in
 * @return #Q_OK, #Q_ERROR_NOCHANGE if the attempt to move resulted in illegal
 * behaviour (e.g. passing through walls), or #Q_ERROR.
 */
int
qwalk_move_obj(QwalkObject_t *walk_object, Qdirection_t direction)
/*@modifies *walk_object@*/ {
	int y_old;
	int x_old;

	int y_new;
	int x_new;
	
	y_old = qwalk_object_coord_y_get(walk_object);
	x_old = qwalk_object_coord_y_get(walk_object);
 	
	/* modify the coords and check if they're allowed */
	switch (Qdirection_t) {
	case QDIRECTION_NORTH;
		y_new = y_old + (QDIRECTION_MULTIPLIER_DEFAULT * QDIRECTION_NORTH_Y_MULTIPLICAND);
		break;
	case QDIRECTION_EAST;
		x_new = x_old + (QDIRECTION_MULTIPLIER_DEFAULT * QDIRECTION_EAST_X_MULTIPLICAND);
		break;
	case QDIRECTION_SOUTH;
		y_new = y_old + (QDIRECTION_MULTIPLIER_DEFAULT * QDIRECTION_SOUTH_Y_MULTIPLICAND);
		break;
	case QDIRECTION_WEST:
		x_new = x_old + (QDIRECTION_MULTIPLIER_DEFAULT * QDIRECTION_WEST_X_MULTIPLICAND);
		break;
	}
	if (!qwalk_logic_coords_arevalid(y_new, x_new)) {
		return Q_ERROR_NOCHANGE; 
	}


	qwalk_object_coord_y_set(walk_object, y_new);
	qwalk_object_coord_x_set(walk_object, x_new);
	return Q_OK;
}


/**
 * Error-check a #QwalkField_t and create an array of every #QobjType_t therein.
 * This is for ease of future indexing and to sanitize inputs to dispose of the
 * need for excessive error-checking code; i.e., this function's params and
 * return value are assumed to be error-free if a non-`NULL` value is returned.
 * The exception is everything under walk_field->objects[].attr_list, barring
 * the #QATTR_KEY_QOBJECT_TYPE value. The verb sanitize is certainly a
 * misnomer, but this function's identifier really should only be so long!
 * @param[in] walk_field: #QwalkField_t to parse and proof-read 
 * @return Dynamic #QobjType_t array or @c NULL if an error was encountered
 */
QobjType_t
qwalk_logic_walk_field_sanitize(QwalkField_t *walk_field) {
	QattrList_t           *attr_list;
	Qdatameta_t           *datameta_object_type;

	/*@only@*/QobjType_t  *obj_types;
	obj_types = calloc(QWALK_AREA_SIZE, sizeof(*obj_types));

	/* parse starting from the outside and travelling to the inside */
	if (walk_field == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}

	for (int i = 0; i < QWALK_AREA_SIZE; i++) {
		
		if ((walk_field->objects[i]).x < QWALK_AREA_COORD_MINIMUM) {
			Q_ERRORFOUND(QERROR_NEGATIVE_VALUE_UNEXPECTED);
			return NULL;
		}
		if ((walk_field->objects[i]).y < QWALK_AREA_COORD_MINIMUM) {
			Q_ERRORFOUND(QERROR_NEGATIVE_VALUE_UNEXPECTED);
			return NULL;
		}
    
		/*
		 * TODO: in future, implement an interface function to nab the attr_list
		 * that we can mark with the splint observer annotation!
		 */
		attr_list            = walk_field->objects[i].attr_list;
		if (attr_list == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			return NULL;
		}
		
		datameta_object_type = qattr_list_value_get(attr_list, QATTR_KEY_QOBJECT_TYPE);
		if (datameta_object_type == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			return NULL;
		}
		if (qdatameta_type_get(datameta_name) != QDATA_TYPE_OBJECT_TYPE) {
			Q_ERRORFOUND(QERROR_QDATAMETA_TYPE_INCOMPATIBLE);
			return NULL;
		}
		
		name = (QobjType_t *) qdatameta_datap_get(datameta_name);
		if (name == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			return NULL;
		}
		/* ensure QATTR_KEY_OBJECT_TYPE has size of exactly 1 */
		if (qdatameta_count_get != 1) {
			Q_ERRORFOUND(QERROR_QDATAMETA_TYPE_SIZE_INCOMPATIBLE);
			return NULL;
		}
		
		obj_types[i] = name;
	}
	return obj_types;
}


/**
 * Safely destory a dynamic #QobjType_t array.
 * @param[out] obj_types: array to destroy
 */
void
qwalk_logic_qobj_type_destroy(QobjType_t *obj_types) {
	free(obj_types);
	return;
}


/**
 * Find the index of a #QwalkObject_t according to a #QobjType_t search query.
 * The #QobjType_t is assumed to be occur either once or zero times; if an
 * object occurs more than once, this function will fail.
 * @param[in] obj_types_parse: QobjType_t array to parse
 * @param[in] obj_type_search: search query; #QobjType_t to check against
 * @return index of desired #QwalkObject_t, <i>OR</i> #Q_ERRORCODE_INT in the
 * event of a fatal error, <i>OR</i> #Q_ERRORCODE_INT_NOTFOUND if the match for
 * obj_type_search cannot be found.
 */
int
qwalk_logic_find_qobj_index(QobjType_t *obj_types_parse, QobjType_t obj_type_search) {
	
	if (obj_types_parse == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERRORCODE_INT;
	}
	if (obj_type_search < Q_ENUM_VALUE_START) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID_ZERO);
		return Q_ERRORCODE_INT;
	} else if (obj_type_search > Q_OBJ_TYPE_COUNT) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERRORCODE_INT;
	}

	for (int i = 0; i < QWALK_AREA_SIZE; i++) {
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
	if (y < QWALK_AREA_COORD_MINIMUM) {
		return false;
	} else if (y >= QWALK_AREA_SIZE_Y) {
		return false;
	} else if (x < QWALK_AREA_COORD_MINIMUM) {
		return false;
	} else if (x >= QWALK_AREA_SIZE_X) {
		return false;
	}
	return true;
}
