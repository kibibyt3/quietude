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



static /*@null@*/QobjType_t *qwalk_logic_walk_field_sanitize(/*@in@*/QwalkField_t *)/*@*/;

static void qwalk_logic_qobj_type_destroy(/*@only@*/QobjType_t *);

static int qwalk_logic_find_qobj_index(QobjType_t *obj_types_parse, QobjType_t obj_type_search)/*@*/;


ModeSwitchData_t *
qwalk_logic_subtick(QwalkField_t *walk_field, QwalkCommand_t walk_command)
/*@modifies *walk_field@*/ {
	
	/*
	 * Dynamic array of every #QwalkObject_t #QobjType_t #Qattr_t.
	 * Indexes are assumed to be in lockstep with walk_field.
	 */
	QobjType_t *obj_types;
	obj_types = qwalk_logic_walk_field_sanitize(walk_field);

	/* Pointer to the memory that holds the player #QwalkObject_t */
	QwalkObject_t *player = NULL;


	qwalk_logic_qobj_type_destroy(obj_types);
	obj_types = NULL;
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
		
		if ((walk_field->objects[i]).x < QWALK_AREA_MINIMUM_COORD) {
			Q_ERRORFOUND(QERROR_NEGATIVE_VALUE_UNEXPECTED);
			return NULL;
		}
		if ((walk_field->objects[i]).y < QWALK_AREA_MINIMUM_COORD) {
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
