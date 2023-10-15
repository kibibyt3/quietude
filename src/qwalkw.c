/**
 * @file qwalkw.c
 * Program file for the wrapper section of the qwalk module.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "qdefs.h"
#include "qerror.h"

#include "mode.h"
#include "qattr.h"
#include "qwalk.h"

/** Pointer to current #QwalkLayer_t */
/*@owned@*//*@null@*/static QwalkArea_t *walk_area_curr = NULL;

/** Whether the qwalk module is currently initialized  */
static bool          isinit = false; 


/**
 * Initialize the qwalk module.
 * Upon a successful inititialization, set #isinit to @c true. #walk_area_curr
 * is updated.
 * @param[in] datameta: pointer to the #Qdatameta_t sent by the previous mode.
 * `free`'d in the event of a successful execution. Must contain a
 * #QwalkLayer_t.
 * @return #Q_OK or #Q_ERROR
 */ 
int
qwalk_init(Qdatameta_t* datameta) {
	if (isinit == true) {
		Q_ERRORFOUND(QERROR_MODULE_INITIALIZED);
		free(datameta);
		return Q_ERROR;
	}
	isinit = true;
	if (datameta == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		free(datameta);
		return Q_ERROR;
	}
	if (datameta->type != QDATA_TYPE_QWALK_AREA) {
		Q_ERRORFOUND(QERROR_QDATAMETA_TYPE_INCOMPATIBLE);
		free(datameta);
		return Q_ERROR;
	}
	if (walk_area_curr != NULL) {
		Q_ERRORFOUND(QERROR_NONNULL_POINTER_UNEXPECTED);
		free(datameta);
		return Q_ERROR;
	}
	walk_area_curr = (QwalkArea_t *) datameta->datap;
	datameta->datap = NULL;
	free(datameta);

	return Q_OK;
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
		return Q_ERROR;
	}

	if (walk_area_curr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	
	if (walk_area_curr->layer_earth == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		returncode = Q_ERROR;
	} else {
		if (walk_area_curr->layer_earth->objects == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			returncode = Q_ERROR;
		} else {
			free(walk_area_curr->layer_earth);
		}
		free(walk_area_curr->layer_earth);
	}

	if (walk_area_curr->layer_floater == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		returncode = Q_ERROR;
	} else {
		free(walk_area_curr->layer_floater);
	}

	free(walk_area_curr);
	walk_area_curr = NULL;
	isinit = false;
	return returncode;
}


/**
 * Pass a tick in qwalk.
 * @param[out] switch_data: #ModeSwitchData_t to update for determining and
 * executing the mode for the next tick.
 * @return #Q_OK or #Q_ERROR
 */
int
qwalk_tick(ModeSwitchData_t *switch_data) {
	QwalkCommand_t    cmd;
	int               i;
//	cmd = qwalk_input_subtick();
	cmd = (QwalkCommand_t) 1; //TODO: remove this!!!
	
	if ((cmd < Q_ENUM_VALUE_START) || (cmd > QWALK_COMMAND_COUNT)) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERROR;
	}
	
	i = qwalk_logic_subtick(walk_area_curr, cmd, switch_data);
	if (i == Q_ERROR) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	
//	i = qwalk_output_subtick();
	assert(i != Q_ERROR);
	
	return Q_OK;
}


/**
 * Get #QwalkArea_t->layer_earth.
 * @param[in] walk_area: relevant #QwalkArea_t.
 * @return walk_area->layer_earth or @c NULL if an error occurs.
 */
QwalkLayer_t
*qwalk_area_layer_earth_get(const QwalkArea_t *walk_area) {
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
	walk_layer = calloc(1, sizeof(*walk_layer));
	walk_layer->objects = calloc(QWALK_LAYER_SIZE, sizeof(*(walk_layer->objects)));
	if (walk_layer == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}
	return walk_layer;
}

/**
 * Destroy a #QwalkLayer_t and its contents.
 * @return #Q_OK or #Q_ERROR.
 * @note
 * Relies on the fact that @c calloc() automatically initializes memory to 0,
 * which compares equal to @c NULL.
 */
int
qwalk_layer_destroy(QwalkLayer_t *walk_layer) {
	int returnval = Q_OK;
	if (walk_layer == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if (walk_layer->objects == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		free(walk_layer);
		return Q_ERROR;
	}
	for (int i = 0; i < QWALK_LAYER_SIZE; i++) {
		if (walk_layer->objects[i] != NULL) {
			qwalk_obj_destroy(walk_layer->objects[i]);
			walk_layer->objects[i] = NULL;
		} else {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			returnval = Q_ERROR;
		}
	}
	free(walk_layer->objects);
	free(walk_layer);
	return returnval;
}


/**
 * Set an object in a #QwalkLayer_t.
 * @param[out] walk_layer: relevant #QwalkLayer_t.
 * @param[out] walk_obj:   #QwalkObj_t to give to @p walk_layer.
 * @param[in]  index:      index in @p walk_layer to set @p walk_obj to.
 * @return #Q_OK or #Q_ERROR.
 */
int
qwalk_layer_object_set(QwalkLayer_t *walk_layer, QwalkObj_t *walk_obj, int index) {
	if ((walk_layer == NULL) || (walk_obj == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		if (walk_obj != NULL) {
			if (qwalk_obj_destroy(walk_obj) == Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
			}
		}
		return Q_ERROR;
	}
	walk_layer->objects[index] = walk_obj;
	return Q_OK;
}


/**
 * Get a specific #QwalkObj_t * from a #QwalkLayer_t.
 * @param[in] walk_layer: walk_layer to search inside of
 * @param[in] index: index to find
 * @return desired #QwalkObj_t or @c NULL.
 */
QwalkObj_t *
qwalk_layer_object_get(const QwalkLayer_t *walk_layer, int index) {
	if (walk_layer == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	if (index >= (QWALK_LAYER_SIZE + QWALK_LAYER_COORD_MINIMUM)) {
		Q_ERRORFOUND(QERROR_INDEX_OUTOFRANGE);
		return NULL;
	}
	return walk_layer->objects[index];
}


/**
 * Create a #QwalkObj_t.
 * @param[in]  y:          y coord of returned #QwalkObj_t.
 * @param[in]  x:          x coord of returned #QwalkObj_t.
 * @param[out] attr_list: #QattrList_t of returned #QwalkObj_t.
 */
QwalkObj_t *
qwalk_obj_create(int y, int x, QattrList_t *attr_list) {
	QwalkObj_t *walk_obj;
	if (attr_list == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	walk_obj = calloc(1, sizeof(walk_obj));
	if (walk_obj == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		if (qattr_list_destroy(attr_list) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
		return NULL;
	}
	return walk_obj;
}


/**
 * Destroy a #QwalkObj_t and its contents.
 * @param[in] walk_obj: #QwalkObj_t to destroy.
 * @return #Q_OK or #Q_ERROR.
 */
int
qwalk_obj_destroy(QwalkObj_t *walk_obj) {
	if (walk_obj == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if ((qattr_list_destroy(walk_obj->attr_list)) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		free(walk_obj);
		return Q_ERROR;
	}
	free(walk_obj);
	return Q_OK;
}

/**
 * Set the y coordinate of a #QwalkObj_t.
 * @param[in] walk_object: pointer to #QwalkObj_t in question
 * @param[in] coord:       value to set the y coordinate to
 * @return #Q_OK or #Q_ERROR
 */
int
qwalk_object_coord_y_set(QwalkObj_t *walk_object, int coord) {
	if (walk_object == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	walk_object->coord_y = coord;
	return Q_OK;
}


/**
 * Set the y coordinate of a #QwalkObj_t
 * @param[in] walk_object: pointer to #QwalkObj_t in question
 * @param[in] coord:       value to set the x coordinate to
 * @return #Q_OK or #Q_ERROR
 */
int
qwalk_object_coord_x_set(QwalkObj_t *walk_object, int coord) {
	if (walk_object == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	walk_object->coord_x = coord;
	return Q_OK;
}


/**
 * Get the y coordinate of a #QwalkObj_t.
 * @param[in] walk_object: pointer to #QwalkObj_t in question
 * @return y coordinate or #Q_ERRORCODE_INT if an error occurs
 */
int
qwalk_object_coord_y_get(const QwalkObj_t *walk_object) {
	if (walk_object == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERRORCODE_INT;
	}
	return walk_object->coord_y;
}


/**
 * Get the x coordinate of a #QwalkObj_t.
 * @param[in] walk_object: pointer to #QwalkObj_t in question
 * @return x coordinate or #Q_ERRORCODE_INT if an error occurs
 */
int
qwalk_object_coord_x_get(const QwalkObj_t *walk_object) {
	if (walk_object == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERRORCODE_INT;
	}
	return walk_object->coord_x;
}


/**
 * Get the #QattrList_t of a #QwalkObj_t.
 * @param[in] walk_object: pointer to #QwalkObj_t in question
 * @return #QattrList_t of all object's #Qattr_t or @c NULL if an error occurs.
 */
QattrList_t*
qwalk_object_attr_list_get(const QwalkObj_t *walk_object) {
	if (walk_object == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}	
	return walk_object->attr_list;
}


