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


/** Number of elements in #default_qwalk_overrides. */
#define QDEFAULT_QWALK_OVERRIDESC 1

/** Default overrides. */
static const QdefaultOverride_t 
default_qwalk_overrides[QDEFAULT_QWALK_OVERRIDESC] = {
	{QATTR_KEY_QDL_FILE, "dialogue.qdl"}
};


/** Default for #QOBJ_TYPE_TREE. */
static const QdefaultQwalkObject_t default_qwalk_tree = {
	QOBJ_TYPE_TREE, /* type */
	"tree",         /* name */
	"A tall, longing, whispering tree", /* brief description */
	"A horribly tall tree with fingers curling and winding between opportune"
		" gaps in the Glass and the air.", /* long description */
	false /* can move */
};

/** Default for #QOBJ_TYPE_GRASS. */
static const QdefaultQwalkObject_t default_qwalk_grass = {
	QOBJ_TYPE_GRASS, /* type */
	"grass",         /* name */
	"A patch of grass", /* brief description */
	"Long limbs and threads of a varying greenbrown grasp longingly from the"
	" earth. You wonder if they march toward you.", /* long description */
	false /* can move */
};

/** Default for #QOBJ_TYPE_VOID. */
static const QdefaultQwalkObject_t default_qwalk_void = {
	QOBJ_TYPE_VOID, /* type */
	"void",         /* name */
	"An empty space", /* brief description */
	"The wind here whirls in a mundane miniature vortex. You know this. You can"
	" smell it.", /* long description */
	false /* can move */
};

/** Collection of all defaults for qwalk. */
static const QdefaultQwalkObject_t *default_qwalk_objects[] = {
	&default_qwalk_tree, &default_qwalk_grass, &default_qwalk_void
};

/** Number of elements in #default_qwalk_objects. */
#define QDEFAULT_QWALK_OBJECTSC 3



/*@null@*//*@only@*/
static QattrList_t *qdefault_qwalk_default_attr_list_create(
		QobjType_t type_search);

/*@null@*//*@only@*/
static Qdatameta_t *qdefault_qwalk_default_datameta_create(
		QobjType_t type_search, QattrKey_t key)
	/*@globals default_qwalk_objects, default_qwalk_overrides@*/;

static int qdefault_qwalk_objects_index_search(QobjType_t type)
	/*@globals default_qwalk_objects@*/;

static int qdefault_qwalk_overrides_index_search(QattrKey_t key)
	/*@globals default_qwalk_overrides@*/;



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

	qattr_list_destroy(layer->objects[index].attr_list);

	if (qdefault_qwalk_layer_object(layer, index, default_type) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}

	return Q_OK;
}


/**
 * Create an object in a #QwalkLayer_t as a default.
 * Assumes that @p layer isn't fully defined and is in the process of being
 * defined.
 * @param[in] layer: #QwalkLayer_t to operate on.
 * @param[in] index: index in @p layer of object to update.
 * @param[in] default_type: the #QobjType_t to initialize to.
 * @return #Q_OK or #Q_ERROR.
 */
int
qdefault_qwalk_layer_object_incomplete(QwalkLayer_t *layer, int index,
		QobjType_t default_type) {

	QattrList_t *attr_list;

	if ((attr_list = qdefault_qwalk_default_attr_list_create(default_type))
			== NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	int *coords;

	if ((coords = qwalk_index_to_coords(index)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		qattr_list_destroy(attr_list);
		return Q_ERROR;
	}

	layer->objects[layer->index_ok].coord_y = coords[0];
	layer->objects[layer->index_ok].coord_x = coords[1];
	/*@i2@*/layer->objects[layer->index_ok].attr_list = attr_list;
	layer->index_ok++;

	free(coords);
	
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
{

	QattrList_t *attr_list;

	if ((attr_list = qdefault_qwalk_default_attr_list_create(default_type))
			== NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	layer->objects[index].attr_list = attr_list;

	return Q_OK;
}


/**
 * Create a #QattrList_t associated with a default qwalk #QobjType_t.
 * @param[in] type_search: #QobjType_t to associate.
 * @return completed #QattrList_t or `NULL`.
 */
QattrList_t *
qdefault_qwalk_default_attr_list_create(QobjType_t type_search) {

	QattrList_t *attr_list;

	if ((attr_list = qattr_list_create((size_t) QDEFAULT_QWALK_OBJECT_MEMBERC))
			== NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}


	Qdatameta_t *datameta;
	for (int i = 0; i < QDEFAULT_QWALK_OBJECT_MEMBERC; i++) {
		if ((datameta = qdefault_qwalk_default_datameta_create(
						type_search, qdefault_qwalk_object_keys[i])) == NULL) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			abort();
		}

		if (qattr_list_attr_set(attr_list, qdefault_qwalk_object_keys[i], datameta)
				== Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			abort();
		}
	}

	return attr_list;
}


/**
 * Add a default attribute to a #QwalkLayer_t object.
 * @param[in] layer: relevant #QwalkLayer_t.
 * @param[in] index: index in @p layer to operate on.
 * @param[in] default_type: #QobjType_t to default in the context of.
 * @param[in] key: #QattrKey_t to add the default #Qattr_t of.
 * @return #Q_OK or #Q_ERROR.
 */
int
qdefault_qwalk_attr_list_attr_default(QwalkLayer_t *layer, int index,
		QobjType_t default_type, QattrKey_t key)
/*@globals default_qwalk_objects, default_qwalk_overrides@*/
{

	Qdatameta_t *datameta;
	QattrList_t *attr_list;

	if ((datameta = qdefault_qwalk_default_datameta_create(default_type, key))
			== NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	if ((attr_list = qwalk_layer_object_attr_list_get(layer, index)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		qdatameta_destroy(datameta);
		return Q_ERROR;
	}

	if (qattr_list_attr_set(attr_list, key, datameta) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	return Q_OK;
}


/**
 * Create a #Qdatameta_t associated with a default qwalk #QattrKey_t.
 * @param[in] type_search: #QobjType_t dictionary to use.
 * @param[in] key: specific #QattrKey_t default value in the @p type_search 
 * dictionary to use.
 * @return newly-created default #Qdatameta_t or `NULL` on error.
 */
Qdatameta_t *
qdefault_qwalk_default_datameta_create(QobjType_t type_search, QattrKey_t key) {

	QobjType_t typealias = (QobjType_t) Q_ERRORCODE_ENUM;
	QobjType_t *type;
	const char *salias = NULL;
	char *s;
	bool balias = false;
	bool *b;

	int index;

	QdataType_t data_type;
	size_t count;
	Qdata_t *data;
	Qdatameta_t *datameta;

	salias = NULL;

	bool isoverride = false;

	if ((index = qdefault_qwalk_overrides_index_search(key))
			!= Q_ERRORCODE_INT_NOTFOUND) {
		isoverride = true;
	} else if ((index = qdefault_qwalk_objects_index_search(type_search))
			== Q_ERRORCODE_INT_NOTFOUND) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return NULL;
	}

	if (isoverride) {
		salias = default_qwalk_overrides[index].value;
	} else {

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
	}

	if (isoverride) {
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
	} else {
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
	}

	if ((datameta = qdatameta_create(data, data_type, count)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	return datameta;
}


/**
 * Find the index in #default_qwalk_overrides of a specific #QobjType_t.
 * @param[in] key: #QattrKey_t to search for.
 * @return index or #Q_ERRORCODE_INT_NOTFOUND if the value can't be found.
 */
int
qdefault_qwalk_overrides_index_search(QattrKey_t key)
/*@globals default_qwalk_overrides@*/
{

	for (int i = 0; i < QDEFAULT_QWALK_OVERRIDESC; i++) {
		if (default_qwalk_overrides[i].key == key) {
			return i;
		}
	}

	return Q_ERRORCODE_INT_NOTFOUND;
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
