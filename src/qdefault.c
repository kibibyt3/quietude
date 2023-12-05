/**
 * @file qdefault.c
 * Program file for qdefault.
 * Responsible for default initializations (particularly with regard to qattr).
 */



#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "splint_types.h"
#include "qattr.h"
#include "dialogue.h"
#include "qwalk.h"
#include "qdefault.h"



/**
 * QattrKey_t in-step with each member of #QdefaultQwalkObject_t.
 */
static const QattrKey_t qdefault_qwalk_object_keys[] = {
	QATTR_KEY_QOBJECT_TYPE,
	QATTR_KEY_NAME,
	QATTR_KEY_DESCRIPTION_BRIEF,
	QATTR_KEY_DESCRIPTION_LONG,
	QATTR_KEY_CANMOVE
};

/** Default for #QOBJ_TYPE_TREE. */
static const QdefaultQwalkObject_t default_qwalk_tree = {
	QOBJ_TYPE_TREE, /* type */
	"tree",         /* name */
	"A tall, longing, whispering tree.", /* brief description */
	"A horribly tall tree with fingers curling and winding between opportune"
		" gaps in the Glass and the air.", /* long description */
	false /* can move */
};

/** Collection of all defaults for qwalk. */
static const QdefaultQwalkObject_t *default_qwalk_objects[] = {
	&default_qwalk_tree
};

/** Number of elements in #default_qwalk_objects. */
#define QDEFAULT_QWALK_OBJECTSC 1



static int qdefault_qwalk_objects_index_search(QobjType_t type)
	/*@globals default_qwalk_objects@*/;



/**
 * Update an object in a #QwalkLayer_t to a default.
 * Assumes that the object was previously defined. Wrapper function with regards
 * to @ref qdefault_qwalk_layer_object().
 * @param[in] layer: #QwalkLayer_t to operate on.
 * @param[in] index: index in @p layer of object to update.
 * @param[in] default_type: the #QobjType_t to initialize to.
 * @return #Q_OK or #Q_ERROR.
 */
int
qdefault_qwalk_layer_object_replace(QwalkLayer_t *layer, int index,
		QobjType_t default_type) {

	if (qattr_list_destroy(layer->objects[index].attr_list) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}

	if (qdefault_qwalk_layer_object(layer, index, default_type) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}

	return Q_OK;
}


/**
 * Update an object in a #QwalkLayer_t to a default.
 * Assumes that the object was not previously defined.
 * @param[in] layer: #QwalkLayer_t to operate on.
 * @param[in] index: index in @p layer of object to update.
 * @param[in] default_type: the #QobjType_t to initialize to.
 * @return #Q_OK or #Q_ERROR.
 */
int
qdefault_qwalk_layer_object(QwalkLayer_t *layer, int index,
		QobjType_t default_type)
/*@globals qdefault_qwalk_object_keys@*/
{

	QattrList_t *attr_list;

	if ((attr_list = qattr_list_create((size_t) QDEFAULT_QWALK_OBJECT_MEMBERC))
			== NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}


	Qdatameta_t *datameta;
	for (int i = 0; i < QDEFAULT_QWALK_OBJECT_MEMBERC; i++) {
		if ((datameta = qdefault_qwalk_default_datameta_create(
						default_type, qdefault_qwalk_object_keys[i])) == NULL) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			abort();
		}

		if (qattr_list_attr_set(attr_list, qdefault_qwalk_object_keys[i], datameta)
				== Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			abort();
		}
	}

	layer->objects[index].attr_list = attr_list;

	return Q_OK;
}


/**
 * Create a datameta associated with a default qwalk #Qattr_t.
 * @param[in] type_search: #QobjType_t dictionary to use.
 * @param[in] key: specific #QattrKey_t default value in the @p type_search 
 * dictionary to use.
 * @return newly-created default #Qdatameta_t or `NULL` on error.
 */
Qdatameta_t *
qdefault_qwalk_default_datameta_create(QobjType_t type_search, QattrKey_t key)
/*@globals default_qwalk_objects@*/
{

	QobjType_t typealias = (QobjType_t) Q_ERRORCODE_ENUM;
	QobjType_t *type;
	char *salias = NULL;
	char *s;
	bool balias;
	bool *b = false;

	int index;

	QdataType_t data_type;
	size_t count;
	Qdata_t *data;
	Qdatameta_t *datameta;

	salias = NULL;

	if ((index = qdefault_qwalk_objects_index_search(type_search))
			== Q_ERRORCODE_INT) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return NULL;
	}

	switch (key) {
		case QATTR_KEY_QOBJECT_TYPE:
			typealias = default_qwalk_objects[index]->type;
			break;
		case QATTR_KEY_NAME:
			salias = default_qwalk_objects[index]->name;
			break;
		case QATTR_KEY_DESCRIPTION_BRIEF:
			salias = default_qwalk_objects[index]->description_brief;
			break;
		case QATTR_KEY_DESCRIPTION_LONG:
			salias = default_qwalk_objects[index]->description_long;
			break;
		case QATTR_KEY_CANMOVE:
			balias = default_qwalk_objects[index]->canmove;
			break;
		default:
			Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
			return NULL;
	}

	switch (key) {
		case QATTR_KEY_QOBJECT_TYPE:
			data_type = QDATA_TYPE_QOBJECT_TYPE;
			count = (size_t) 1;
			if ((type = calloc(count, sizeof(*type))) == NULL) {
				Q_ERROR_SYSTEM("calloc()");
				return NULL;
			}
			*type = typealias;
			data = (Qdata_t *) type;
			break;
		
		case QATTR_KEY_NAME:
		case QATTR_KEY_DESCRIPTION_BRIEF:
		case QATTR_KEY_DESCRIPTION_LONG:
			if (salias == NULL) {
				Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
				abort();
			}
			data_type = QDATA_TYPE_CHAR_STRING;
			count = strlen(salias) + (size_t) 1;
			if ((s = calloc(count, sizeof(*s))) == NULL) {
				Q_ERROR_SYSTEM("calloc()");
				return NULL;
			}
			strcpy(s, salias);
			data = (Qdata_t *) s;
			break;
		
		case QATTR_KEY_CANMOVE:
			data_type = QDATA_TYPE_BOOL;
			count = (size_t) 1;
			if ((b = calloc(count, sizeof(*b))) == NULL) {
				Q_ERROR_SYSTEM("calloc()");
				return NULL;
			}
			*b = balias;
			data = (Qdata_t *) b;
			break;

		default:
			Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
			return NULL;
	}

	if ((datameta = qdatameta_create(data, data_type, count)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	return datameta;
}


/**
 * Find the index in #default_qwalk_objects of a specific #QobjType_t.
 * @param[in] type: #QobjType_t to search for.
 * @return index or #Q_ERRORCODE_INT_NOTFOUND if the value can't be found.
 */
int
qdefault_qwalk_objects_index_search(QobjType_t type)
/*@globals default_qwalk_objects@*/
{

	for (int i = 0; i < QDEFAULT_QWALK_OBJECTSC; i++) {
		if (default_qwalk_objects[i]->type == type) {
			return i;
		}
	}

	return Q_ERRORCODE_INT_NOTFOUND;
}
