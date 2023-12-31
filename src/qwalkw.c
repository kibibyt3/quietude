/**
 * @file qwalkw.c
 * Program file for the wrapper section of the qwalk module.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "splint_types.h"
#include "qutils.h"
#include "mode.h"
#include "qattr.h"
#include "qfile.h"
#include "qwins.h"
#include "dialogue.h"
#include "qwalk.h"
#include "qdefault.h"



/** Pointer to current #QwalkLayer_t */
/*@only@*//*@null@*/static QwalkArea_t *walk_area_curr = NULL;

/** Whether the qwalk module is currently initialized  */
static bool          isinit = false; 

/** #Qwindow_t for the main gameplay in walk. */
/*@only@*//*@null@*/static Qwindow_t *walk_win = NULL;
/** #Qwindow_t for NPC dialogue. */
/*@only@*//*@null@*/static Qwindow_t *walk_dialogue_win = NULL;
/** #Qwindow_t for the environment log. */
/*@only@*//*@null@*/static Qwindow_t *walk_environment_log_win = NULL;



/**
 * Initialize the qwalk module.
 * Upon a successful inititialization, set #isinit to @c true. #walk_area_curr
 * is updated.
 * @param[in] area_filename: filename of the file where the #QwalkArea_t is
 * saved.
 * @return #Q_OK or #Q_ERROR
 */ 
int
qwalk_init(const char *area_filename)/*@modifies walk_area_curr, isinit@*/{

	int returnval = Q_OK;

	if (isinit) {
		Q_ERRORFOUND(QERROR_MODULE_INITIALIZED);
		return Q_ERROR;
	}

	isinit = true;

	if (walk_area_curr != NULL) {
		Q_ERRORFOUND(QERROR_NONNULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}


	/* deal with logic initializations */
	if (qfile_open(area_filename, QFILE_MODE_READ) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	if ((walk_area_curr = qwalk_area_read()) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	if (qfile_close() == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}


	/* deal with I/O initializations */
	if (qwins_walk_wins_init(
			&walk_win, &walk_dialogue_win, &walk_environment_log_win) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	if ((walk_win == NULL) || (walk_environment_log_win == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		returnval = Q_ERROR;
	} else {
		if (qwalk_io_init(walk_win->win, walk_environment_log_win->win)
				== Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
	}

	return returnval;
}


/**
 * Safely exit the qwalk module.
 * @return #Q_OK or #Q_ERROR
 */ 
int
qwalk_end() {

	int returncode = Q_OK;

	/*
	 * ensure walk_area_curr and its two members aren't NULL and free them if
	 * possible
	 */
	if (isinit == false) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
	}

	if (walk_area_curr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}

	if ((walk_win == NULL) || (walk_dialogue_win == NULL)
			|| (walk_environment_log_win == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERROR;
	}


	/* logic cleanup */
	if (walk_area_curr->layer_earth == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		returncode = Q_ERROR;
	} else {
		qwalk_layer_destroy(walk_area_curr->layer_earth);
		walk_area_curr->layer_earth = NULL;
	}

	if (walk_area_curr->layer_floater == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		returncode = Q_ERROR;
	} else {
		qwalk_layer_destroy(walk_area_curr->layer_floater);
		walk_area_curr->layer_floater = NULL;
	}

	free(walk_area_curr);


	/* I/O cleanup */
	qwins_walk_wins_end(walk_win, walk_dialogue_win, walk_environment_log_win);

	walk_win = NULL;
	walk_dialogue_win = NULL;
	walk_environment_log_win = NULL;

	qwalk_io_end();

	walk_area_curr = NULL;
	isinit = false;
	return returncode;
}


/**
 * Pass a tick in qwalk.
 * Works in the order: output -> input -> logic
 * executing the mode for the next tick.
 * @return #Q_OK or #Q_ERROR
 */
int
qwalk_tick() {
	QwalkCommand_t    cmd;
	int               r;
	
	int returnval = Q_OK;

	int player_index;

	if (walk_area_curr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}

	if ((walk_win == NULL) || (walk_dialogue_win == NULL)
			|| (walk_environment_log_win == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_INITIALIZED);
		return Q_ERROR;
	}

	if (qwindow_noutrefresh(walk_win) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	if (qwindow_noutrefresh(walk_dialogue_win) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	if (qwindow_noutrefresh(walk_environment_log_win) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}

	if (doupdate() == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}

	r = qwalk_output_subtick(walk_area_curr);
	if (r == Q_ERROR) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}

	QwalkLayer_t *layer_floater;
	if ((layer_floater = qwalk_area_layer_floater_get(walk_area_curr)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	if ((player_index = qwalk_layer_obj_index_get(
					layer_floater, QOBJ_TYPE_PLAYER)) == Q_ERRORCODE_INT) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	} else if (player_index == Q_ERRORCODE_INT_NOTFOUND) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}

	cmd = qwalk_input_subtick(player_index);
	if (cmd == (QwalkCommand_t) Q_ERRORCODE_ENUM) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}
	if ((cmd < (QwalkCommand_t) Q_ENUM_VALUE_START) || (cmd > QWALK_COMMAND_COUNT)) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERROR;
	}

	r = qwalk_logic_subtick(walk_area_curr, cmd);
	if (r == Q_ERROR) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}


	return returnval;
}


/**
 * Wrapper function for qwalk to interface with dialogue.
 * @param[out] layer: #QwalkLayer_t with an NPC to speak to.
 * @param[in] player_index: index in @p layer of the player.
 * @param[in] npc_index: index in @p layer of the NPC to speak to.
 * @return #Q_OK, #Q_ERROR, or #Q_ERROR_NOCHANGE if the distance between the
 * player and NPC are too large.
 */
int
qwalk_dialogue(QwalkLayer_t *layer, int player_index, int npc_index) {

	Qdatameta_t *datameta;

	char *dialogue_filename;

	/* find distance between player and NPC */
	int *player_coords, *npc_coords;
	int distance;
	if ((player_coords = qwalk_index_to_coords(player_index)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}
	if ((npc_coords = qwalk_index_to_coords(npc_index)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		free(player_coords);
		return Q_ERROR;
	}
	if ((distance = qutils_distance_calculate(player_coords[0], player_coords[1],
					npc_coords[0], npc_coords[1])) == Q_ERRORCODE_INT) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		free(player_coords);
		free(npc_coords);
		return Q_ERROR;
	}
	free(player_coords);
	free(npc_coords);
	if (distance > QWALK_DIALOGUE_DISTANCE_MAX) {
		if (qwalk_log_print(QWALK_EXCESSIVE_DISTANCE_LOG_MESSAGE) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		if (qwalk_log_print("\n") == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		return Q_ERROR_NOCHANGE;
	}



	if ((walk_win == NULL) || (walk_dialogue_win == NULL)
			|| (walk_environment_log_win == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_INITIALIZED);
		return Q_ERROR;
	}

	if ((datameta = qwalk_layer_obj_attr_value_get(layer, npc_index,
					QATTR_KEY_QDL_FILE)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	if ((dialogue_filename = (char *) qdatameta_datap_get(datameta)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	DialogueTree_t *tree;
	if ((tree = dialogue_logic_init(dialogue_filename)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	dialogue_io_init(walk_dialogue_win->win);

	char *header_active;
	int choice;
	DialogueCommand_t dialogue_command_external;
	char *dialogue_arg_external;

	/* logic & I/O loop for dialogue */
	do {
		if ((choice = dialogue_io_event(tree)) == Q_ERRORCODE_INT) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			dialogue_tree_destroy(tree);
			return Q_ERROR;
		}
		if (dialogue_logic_tick(tree, choice) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			dialogue_tree_destroy(tree);
			return Q_ERROR;
		}

		/* handle external command */
		if ((dialogue_command_external = dialogue_command_external_get())
				!= DIALOGUE_COMMAND_EMPTY) {
			dialogue_arg_external = dialogue_arg_external_get();
			if (qwalk_dialogue_command_handler(
						layer, npc_index, dialogue_command_external, dialogue_arg_external)
					== Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
			}
		}
		header_active = dialogue_tree_header_active_get(tree);
	} while (strcmp(header_active, DIALOGUE_HEADER_ACTIVE_EXIT) != 0);


	dialogue_tree_destroy(tree);

	if (wclear(walk_dialogue_win->win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	return Q_OK;
}


/**
 * Handle an external command/argument pair sent from dialogue.
 * @param[in] layer: #QwalkLayer_t to execute @p command on.
 * @param[in] index: index in @p layer of the NPC speaker.
 * @param[in] command: #DialogueCommand_t to execute in qwalk.
 * @param[in] arg: argument for the execution of @p comkmand.
 * @return #Q_OK or #Q_ERROR.
 */
int
qwalk_dialogue_command_handler(QwalkLayer_t *layer, int index, 
		DialogueCommand_t command, const char *arg) {

	QobjType_t arg_obj_type;

	switch (command) {
	
	case DIALOGUE_COMMAND_BECOME:
		if ((arg_obj_type = qobj_string_to_type(arg)) 
				== (QobjType_t) Q_ERRORCODE_ENUM) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		if (qdefault_qwalk_layer_object_replace(layer, index, arg_obj_type)
				== Q_ERROR) {
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
 * Get the @ref Qattr_t.valuep from @ref walk_area_curr.
 * @param[in] index: index in @ref walk_area_curr to search.
 * @param[in] key: #QattrKey_t to find in @p index.
 * @return requested #Qdatameta_t or `NULL`.
 */
Qdatameta_t *
qwalk_area_curr_index_attr_value_get(int index, QattrKey_t key)
/*@globals walk_area_curr@*/
{

	QwalkLayer_t *layer_earth, *layer_floater, *layer_active;
	QobjType_t obj_type;

	if ((layer_earth = qwalk_area_layer_earth_get(walk_area_curr)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}
	if ((layer_floater = qwalk_area_layer_floater_get(walk_area_curr)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	layer_active = layer_floater;
	if ((obj_type = qwalk_layer_object_type_get(layer_floater, index))
			== (QobjType_t) Q_ERRORCODE_ENUM) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	if (obj_type == QOBJ_TYPE_VOID) {
		layer_active = layer_earth;
		if ((obj_type = qwalk_layer_object_type_get(layer_earth, index))
				== (QobjType_t) Q_ERRORCODE_ENUM) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return NULL;
		}
	}

	if (obj_type == QOBJ_TYPE_VOID) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return NULL;
	}

	Qdatameta_t *datameta;
	if ((datameta = qwalk_layer_obj_attr_value_get(layer_active, index, key))
			== NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	return datameta;
}


/**
 * Create a #QwalkArea_t.
 * @param[in] layer_earth:   @ref QwalkArea_t.layer_earth.
 * @param[in] layer_floater: @ref QwalkArea_t.layer_floater.
 * @return new #QwalkArea_t.
 */
QwalkArea_t *
qwalk_area_create(QwalkLayer_t *layer_earth, QwalkLayer_t *layer_floater) {
	
	QwalkArea_t *walk_area;
	
	if ((layer_earth == NULL) || (layer_floater == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}
	
	walk_area = calloc((size_t) 1, sizeof(*walk_area));
	if (walk_area == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}

	walk_area->layer_earth   = layer_earth;
	walk_area->layer_floater = layer_floater;

	return walk_area;
}


/**
 * Recursively destroy a #QwalkArea_t.
 * @param[out] walk_area: #QwalkArea_t to destroy.
 */
void
qwalk_area_destroy(QwalkArea_t *walk_area) {
	qwalk_layer_destroy(walk_area->layer_floater);
	qwalk_layer_destroy(walk_area->layer_earth);
	free(walk_area);
	return;
}


/**
 * Write a #QwalkArea_t to storage.
 * Follows the order #QwalkArea_t.layer_earth, #QwalkArea_t.layer_floater.
 * @param[in] walk_area: #QwalkArea_t to write
 * @return #Q_OK or #Q_ERROR.
 */
int
qwalk_area_write(const QwalkArea_t *walk_area) {
	int r;
	int returnval = Q_OK;
	
	if (walk_area == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	
	r = qwalk_layer_write(walk_area->layer_earth);
	if (r != Q_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	
	r = qwalk_layer_write(walk_area->layer_floater);
	if (r != Q_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	
	return returnval;
}


/**
 * Read a #QwalkArea_t from storage.
 * Follows the order #QwalkArea_t.layer_earth, #QwalkArea_t.layer_floater.
 * @return new #QwalkArea_t
 */
QwalkArea_t *
qwalk_area_read() {
	QwalkArea_t *walk_area;
	QwalkLayer_t *layer_earth;
	QwalkLayer_t *layer_floater;

	layer_earth = qwalk_layer_read();
	if (layer_earth == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}
	
	layer_floater = qwalk_layer_read();
	if (layer_floater == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}
	
	walk_area = qwalk_area_create(layer_earth, layer_floater);
	
	if (walk_area == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}

	return walk_area;
}


/**
 * Get #QwalkArea_t->layer_earth.
 * @param[in] walk_area: relevant #QwalkArea_t.
 * @return walk_area->layer_earth or @c NULL if an error occurs.
 */
QwalkLayer_t *
qwalk_area_layer_earth_get(const QwalkArea_t *walk_area) {
	if (walk_area == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	if (walk_area->layer_earth == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	return walk_area->layer_earth;
}


/**
 * Get #QwalkArea_t->layer_floater.
 * @param[in] walk_area: relevant #QwalkArea_t.
 * @return walk_area->layer_floater or @c NULL if an error occurs.
 */
QwalkLayer_t
*qwalk_area_layer_floater_get(const QwalkArea_t *walk_area) {
	if (walk_area == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	if (walk_area->layer_floater == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	return walk_area->layer_floater;
}


/**
 * Create an empty #QwalkLayer_t and set its contents to @c NULL.
 * @return new #QwalkLayer_t or @c NULL upon failure.
 * @allocs{2} for the new walk_layer and its objects member
 */
QwalkLayer_t *
qwalk_layer_create() {
	QwalkLayer_t *walk_layer;
	walk_layer = calloc((size_t) 1, sizeof(*walk_layer));
	if (walk_layer == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}
	walk_layer->objects = calloc((size_t) QWALK_LAYER_SIZE, sizeof(*(walk_layer->objects)));
	if (walk_layer->objects == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		free(walk_layer);
		return NULL;
	}

	walk_layer->index_ok = 0;
	return walk_layer;
}


/**
 * Destroy a #QwalkLayer_t and its contents.
 * @note
 * Relies on the fact that @c calloc() automatically initializes memory to 0,
 * which compares equal to @c NULL.
 */
void
qwalk_layer_destroy(QwalkLayer_t *walk_layer) {

	for (int i = 0; i < walk_layer->index_ok; i++) {
		/* destroy each QwalkObj_t and its contents */
		qattr_list_destroy(walk_layer->objects[i].attr_list);
	}
	/*@i1@*/free(walk_layer->objects);
	free(walk_layer);
	return;
}


/**
 * Write a #QwalkLayer_t to storage.
 * Only @ref QwalkObj_t.attr_list is written; this is because @ref
 * QwalkObj_t.coord_y and @ref QwalkObj_t.coord_x can be confidently converted to
 * and from their index.
 * @param[in] walk_layer: #QwalkLayer_t to write.
 * @return #Q_OK or #Q_ERROR.
 */
int
qwalk_layer_write(const QwalkLayer_t *walk_layer) {
	int r;
	int returnval = Q_OK;
	
	if (walk_layer == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if (walk_layer->objects == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}

	/* layers should ONLY be written when they are fully filled out! */
	if (walk_layer->index_ok != QWALK_LAYER_SIZE) {
		Q_ERRORFOUND(QERROR_STRUCT_INCOMPLETE);
		return Q_ERROR;
	}

	/* iterate through every layer object */
	for (int i = 0; i < QWALK_LAYER_SIZE; i++) {
		r = qattr_list_write(walk_layer->objects[i].attr_list);
		if (r != Q_OK) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			returnval = Q_ERROR;
		}
	}

	return returnval;
}


/**
 * Read a #QwalkLayer_t from storage.
 * @return new #QwalkLayer_t.
 */
QwalkLayer_t *
qwalk_layer_read() {
	QwalkLayer_t *walk_layer;
	QattrList_t  *attr_list;
	int *coords;
	int r;

	walk_layer = qwalk_layer_create();
	if (walk_layer == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}

	/* iterate through every layer object */
	for (int i = 0; i < QWALK_LAYER_SIZE; i++) {
		
		attr_list = qattr_list_read();
		if (attr_list == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			abort();
		}
		
		coords = qwalk_index_to_coords(i);
		if (coords == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			abort();
		}
		
		r = qwalk_layer_object_set(walk_layer, coords[0], coords[1], attr_list);
		if (r == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}

		free(coords);
	}

	return walk_layer;
}


/**
 * Set a #QwalkObj_t in a #QwalkLayer_t.
 * @param[out] walk_layer: relevant #QwalkLayer_t.
 * @param[in] y: @ref QwalkObj_t.coord_y.
 * @param[in] x: @ref QwalkObj_t.coord_x.
 * @param[out] attr_list: @ref QwalkObj_t.attr_list.
 * @return #Q_OK or #Q_ERROR.
 */
int
qwalk_layer_object_set(QwalkLayer_t *walk_layer, int y, int x, QattrList_t *attr_list) {
	if ((walk_layer == NULL) || (attr_list == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		if (attr_list != NULL) {
			qattr_list_destroy(attr_list);
		}
		return Q_ERROR;
	}
	if (walk_layer->objects == NULL) {
		qattr_list_destroy(attr_list);
		return Q_ERROR;
	}
	if (walk_layer->index_ok >= QWALK_LAYER_SIZE) {
		qattr_list_destroy(attr_list);
		return Q_ERROR;
	}
	walk_layer->objects[walk_layer->index_ok].coord_y = y;
	walk_layer->objects[walk_layer->index_ok].coord_x = x;
	walk_layer->objects[walk_layer->index_ok].attr_list = attr_list;
	walk_layer->index_ok++;
	return Q_OK;
}


/**
 * Get the y coordinate of a #QwalkObj_t in a #QwalkLayer_t.
 * @param[in] walk_layer: pointer to #QwalkLayer_t in question.
 * @param[in] index: index of #QwalkObj_t in question.
 * @return y coordinate or #Q_ERRORCODE_INT if an error occurs
 */
int
qwalk_layer_object_coord_y_get(const QwalkLayer_t *walk_layer, int index) {
	if (walk_layer == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERRORCODE_INT;
	}
	if ((index >= walk_layer->index_ok) || (index < 0)) {
		Q_ERRORFOUND(QERROR_INDEX_OUTOFRANGE);
		return Q_ERRORCODE_INT;
	}
	return walk_layer->objects[index].coord_y;
}


/**
 * Get the x coordinate of a #QwalkObj_t in a #QwalkLayer_t.
 * @param[in] walk_layer: pointer to #QwalkLayer_t in question.
 * @param[in] index: index of #QwalkObj_t in question.
 * @return x coordinate or #Q_ERRORCODE_INT if an error occurs
 */
int
qwalk_layer_object_coord_x_get(const QwalkLayer_t *walk_layer, int index) {
	if (walk_layer == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERRORCODE_INT;
	}
	if ((index >= walk_layer->index_ok) || (index < 0)) {
		Q_ERRORFOUND(QERROR_INDEX_OUTOFRANGE);
		return Q_ERRORCODE_INT;
	}
	return walk_layer->objects[index].coord_x;
}


/**
 * Get the #QattrList_t of a #QwalkObj_t in a #QwalkLayer_t.
 * @param[in] walk_layer: pointer to #QwalkLayer_t in question.
 * @param[in] index: index of #QwalkObj_t in question.
 * @return #QattrList_t or @c NULL if an error occurs
 */
QattrList_t *
qwalk_layer_object_attr_list_get(const QwalkLayer_t *walk_layer, int index) {
	if (walk_layer == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	if ((index >= walk_layer->index_ok) || (index < 0)) {
		Q_ERRORFOUND(QERROR_INDEX_OUTOFRANGE);
		return NULL;
	}
	if (walk_layer->objects[index].attr_list == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	return walk_layer->objects[index].attr_list;
}


/**
 * Get the #QobjType_t of an object in a #QwalkLayer_t.
 * @param[in] layer: #QwalkLayer_t to search.
 * @param[in] index: index of object in @p layer.
 * @return requested #QobjType_t or #Q_ERRORCODE_ENUM.
 */
QobjType_t
qwalk_layer_object_type_get(const QwalkLayer_t *layer, int index) {

	Qdatameta_t *datameta;
	QobjType_t  *obj_type;

	if ((datameta = qwalk_layer_obj_attr_value_get(
					layer, index, QATTR_KEY_QOBJECT_TYPE)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return (QobjType_t) Q_ERRORCODE_ENUM;
	}

	if (qdatameta_type_get(datameta) != QDATA_TYPE_QOBJECT_TYPE) {
		Q_ERRORFOUND(QERROR_QDATAMETA_TYPE_COUNT_INCOMPATIBLE);
		return (QobjType_t) Q_ERRORCODE_ENUM;
	}

	if ((obj_type = (QobjType_t *) qdatameta_datap_get(datameta)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return (QobjType_t) Q_ERRORCODE_ENUM;
	}

	return *obj_type;
}


/**
 * Wrapper function to access a particular #Qattr_t.valuep in a #QwalkLayer_t.
 * @param[in] layer: #QwalkLayer_t of the #Qattr_t.
 * @param[in] index: index in @p layer of the parent object.
 * @param[in] key:   #QattrKey_t of the #Qattr_t.valuep.
 * @return requested #Qattr_t.valuep or `NULL`.
 */
Qdatameta_t *
qwalk_layer_obj_attr_value_get(const QwalkLayer_t *layer, int index, 
		QattrKey_t key) {

	QattrList_t *attr_list;
	Qdatameta_t *datameta;

	if ((attr_list = qwalk_layer_object_attr_list_get(layer, index)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	if ((datameta = qattr_list_value_get(attr_list, key)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	return datameta;
}


/**
 * Find the index in a layer of a particular #QobjType_t.
 * The requested #QobjType_t should occur only one or zero times; any more will
 * cause unpredictable behaviour.
 * @param[in] parse_layer: #QwalkLayer_t to parse within.
 * @param[in] type_search: #QobjType_t to search for.
 * @return index of requested #QobjType_t, #Q_ERRORCODE_INT_NOTFOUND if @p
 * type_search had no match, or #Q_ERRORCODE_INT if an error occurred.
 */
int
qwalk_layer_obj_index_get(
		const QwalkLayer_t *parse_layer, const QobjType_t type_search) {

	QattrList_t *attr_list;
	Qdatameta_t *datameta;
	QobjType_t  *obj_type;

	for (int i = 0; i < QWALK_LAYER_SIZE; i++) {
		if ((attr_list = parse_layer->objects[i].attr_list) == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			return Q_ERRORCODE_INT;
		}
		if ((datameta = qattr_list_value_get(attr_list, QATTR_KEY_QOBJECT_TYPE))
				== NULL) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERRORCODE_INT;
		}
		if (qdatameta_type_get(datameta) != QDATA_TYPE_QOBJECT_TYPE) {
			Q_ERRORFOUND(QERROR_QDATAMETA_TYPE_COUNT_INCOMPATIBLE);
			return Q_ERRORCODE_INT;
		}

		if ((obj_type = (QobjType_t *) qdatameta_datap_get(datameta)) == NULL) {
			Q_ERRORFOUND(QERROR_QDATAMETA_TYPE_COUNT_INCOMPATIBLE);
			return Q_ERRORCODE_INT;
		}

		if (*obj_type == type_search) {
			return i;
		}
	}

	return Q_ERRORCODE_INT_NOTFOUND;
}


/**
 * Convert coordinates in qwalk to an index.
 * @param[in] y: y coordinate.
 * @param[in] x: x coordinate.
 * @return index or #Q_ERRORCODE_INT.
 */
int
qwalk_coords_to_index(int y, int x) {
	if ((y < QWALK_LAYER_COORD_MINIMUM) || (y >= QWALK_LAYER_SIZE_Y) ||
			(x < QWALK_LAYER_COORD_MINIMUM) || (x >= QWALK_LAYER_SIZE_X)) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERRORCODE_INT;
	}
	return ((y * QWALK_LAYER_SIZE_X) + x);
}

/**
 * Convert index in qwalk to coordinates.
 * @param[in] index: index to convert.
 * @return @c int array in the order: y, x.
 */
int *
qwalk_index_to_coords(int index) {
	int *vals;
	vals = calloc((size_t) 2, sizeof(*vals));
	assert(vals != NULL);
	if ((index >= QWALK_LAYER_SIZE) || (index < QWALK_LAYER_COORD_MINIMUM)) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		abort();
	}
	vals[0] = index / QWALK_LAYER_SIZE_X;
	vals[1] = index % QWALK_LAYER_SIZE_X;
	return vals;
}
