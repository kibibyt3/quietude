/**
 * Program file for the wrapper section of the qwalk module.
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "qdefs.h"

#include "qwalk.h"
#include "qattr.c"

/** Pointer to current #QwalkField_t */
/*@owned@*/static QwalkField_t *walk_field;

/** Whether the qwalk module is currently initialized  */
           static bool          isinit = false; 


/**
 * Initialize the qwalk module.
 * Upon a successful inititialization, set #isinit to @c true. #walk_field is
 * updated.
 * @param[in] datameta: pointer to the #Qdatameta_t sent by the previous mode 
 * Should be a #QwalkField_t.
 * @return #Q_OK or #Q_ERROR
 */ 
int
qwalk_init(Qdatameta_t* datameta) {
	assert(isinit == false);
	isinit = true;
	if (datameta == NULL) {
		return Q_ERROR;
	}
	walk_field = (QwalkField_t *) datameta;
	return Q_OK;
}


/**
 * Safely exit the qwalk module.
 * @return #Q_OK or #Q_ERROR
 */ 
int
qwalk_end() {
	assert(isinit == true);
	isinit = false;
	return Q_OK;
}


/**
 * Pass a tick in qwalk.
 * @return #ModeSwitchData_t for determining and executing the mode for the
 * next tick.
 */
ModeSwitchData_t *
qwalk_tick() {
	QwalkCommand_t    cmd;
	ModeSwitchData_t *switch_data;
	cmd = qwalk_input_subtick();
	if ((cmd < Q_ENUM_VALUE_START) || (cmd > Q_WALK_COMMAND_COUNT)) {
		return NULL;
	}
	switch_data = qwalk_logic_subtick(walk_field, cmd);
	if (switch_data == NULL) {
		return NULL;
	}
	qwalk_output_subtick();
	return switch_data;
}


/**
 * Get the y coordinate of a #QwalkObject_t
 * @param[in] walk_object: pointer to #QwalkObject_t in question
 * @return y coordinate or #Q_ERROR if @c walk_object is @c NULL
 */
int
qwalk_object_coord_y_get(const QwalkObject_t *walk_object) {
	if (walk_object == NULL) {
		return Q_ERROR;
	}
	return walk_object->coord_y;
}


/**
 * Get the x coordinate of a #QwalkObject_t
 * @param[in] walk_object: pointer to #QwalkObject_t in question
 * @return x coordinate or #Q_ERROR if @c walk_object is @c NULL
 */
int
qwalk_object_coord_x_get(const QwalkObject_t *walk_object) {
	if (walk_object == NULL) {
		return Q_ERROR;
	}
	return walk_object->coord_x;
}


/**
 * Get the #QattrList_t of a #QwalkObject_t
 * @param[in] walk_object: pointer to #QwalkObject_t in question
 * @return #QattrList_t of all object's #Qattr_t or @c NULL if @c walk_object is
 * @c NULL
 */
QattrList_t*
qwalk_object_attr_list_get(const QwalkObject_t *walk_object) {
	if (walk_object == NULL) {
		return NULL;
	}	
	return walk_object->attr_list;
}
