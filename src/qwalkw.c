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
/*@only@*//*@null@*/static QwalkArea_t *walk_area_curr = NULL;

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
	if (isinit) {
		Q_ERRORFOUND(QERROR_MODULE_INITIALIZED);
		qdatameta_destroy(datameta);
		return Q_ERROR;
	}
	isinit = true;
	if (datameta == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		qdatameta_destroy(datameta);
		return Q_ERROR;
	}
	if (datameta->type != QDATA_TYPE_QWALK_AREA) {
		Q_ERRORFOUND(QERROR_QDATAMETA_TYPE_INCOMPATIBLE);
		qdatameta_destroy(datameta);
		return Q_ERROR;
	}
	if (walk_area_curr != NULL) {
		Q_ERRORFOUND(QERROR_NONNULL_POINTER_UNEXPECTED);
		qdatameta_destroy(datameta);
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
	}

	if (walk_area_curr != NULL) {
		Q_ERRORFOUND(QERROR_NONNULL_POINTER_UNEXPECTED);
	} else {
		return Q_ERROR;
	}


	if (walk_area_curr->layer_earth == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		returncode = Q_ERROR;
	} else {
		if ((qwalk_layer_destroy(walk_area_curr->layer_earth)) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
		walk_area_curr->layer_earth = NULL;
	}

	if (walk_area_curr->layer_floater == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		returncode = Q_ERROR;
	} else {
		if ((qwalk_layer_destroy(walk_area_curr->layer_floater)) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
		walk_area_curr->layer_floater = NULL;
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
	
	if (walk_area_curr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if ((cmd < (QwalkCommand_t) Q_ENUM_VALUE_START) || (cmd > QWALK_COMMAND_COUNT)) {
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
	for (int i = 0; i < walk_layer->index_ok; i++) {
		/* destroy each QwalkObj_t and its contents */
		if (qattr_list_destroy(walk_layer->objects[i].attr_list) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
	}
	/*@i1@*/free(walk_layer->objects);
	free(walk_layer);
	return returnval;
}


/**
 * Set an object in a #QwalkLayer_t.
 * @param[out] walk_layer: relevant #QwalkLayer_t.
 * @param[in] y: #QwalkLayer_t->coord_y.
 * @param[in] x: #QwalkLayer_t->coord_x.
 * @param[out] attr_list: #QwalkLayer_t->attr_list
 * @return #Q_OK or #Q_ERROR.
 */
int
qwalk_layer_object_set(QwalkLayer_t *walk_layer, int y, int x, QattrList_t *attr_list) {
	if ((walk_layer == NULL) || (attr_list == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		if (attr_list != NULL) {
			if (qattr_list_destroy(attr_list) == Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
			}
		}
		return Q_ERROR;
	}
	if (walk_layer->objects == NULL) {
		if (qattr_list_destroy(attr_list) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
		return Q_ERROR;
	}
	if (walk_layer->index_ok >= QWALK_LAYER_SIZE) {
		if (qattr_list_destroy(attr_list) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
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
 * @param[in] walk_object: pointer to #QwalkLayer_t in question.
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
 * @param[in] walk_object: pointer to #QwalkLayer_t in question.
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
 * @param[in] walkobject: pointer to #QwalkLayer_t in question.
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


