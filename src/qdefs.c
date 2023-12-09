/**
 * @file qdefs.c
 * Program file for the qdefs 'module'.
 * Logic for game-wide types.
 */



#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "splint_types.h"
#include "dialogue.h"
#include "mode.h"
#include "qattr.h"
#include "qwalk.h"



/**
 * Create a #Qdatameta_t.
 * @param[in] datap: pointer to raw data.
 * @param[in] type:  type of @c data
 * @param[in] count: number of elements allocated to the given pointer
 * @return newly created #Qdatameta_t or @c NULL pointer 
 * @allocs{2} for returned pointer and its datap member.
 */ 
Qdatameta_t *
qdatameta_create(Qdata_t *datap, QdataType_t type, size_t count) {
	Qdatameta_t *datameta;
	datameta = calloc((size_t) 1, sizeof(*datameta));
	if (datameta == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}
	datameta->datap = datap;
	datameta->count = count;
	datameta->type = type;
	return datameta;
}


/**
 * Clone a #Qdatameta_t.
 * @param[in] datametar: #Qdatameta_t to clone.
 * @return clone of @p datameta.
 */
Qdatameta_t *
qdatameta_clone(const Qdatameta_t *datametar) {
	Qdata_t *data;
	Qdata_t *datar;
	QdataType_t type;
	size_t count;
	size_t size;
	Qdatameta_t *datameta;

	if ((count = qdatameta_count_get(datametar)) == Q_ERRORCODE_SIZE) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	if ((type = qdatameta_type_get(datametar)) == (QdataType_t ) Q_ERRORCODE_ENUM) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

 	if ((size = qdata_type_size_get(type)) == Q_ERRORCODE_SIZE) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}
	
	if ((datar = qdatameta_datap_get(datametar)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	if ((data = calloc(count, size)) == NULL) {
		Q_ERRORFOUND(QERROR_SYSTEM_MEMORY);
		return NULL;
	}

	memcpy(data, datar, count * size);

	if ((datameta = qdatameta_create(data, type, count)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	return datameta;
}



/**
 * Recursively destroy a #Qdatameta_t.
 * @param[out] datameta: #Qdatameta_t to free from memory.
 */
void
qdatameta_destroy(Qdatameta_t *datameta) {

	if (datameta->datap == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		free(datameta);
		return;
	}

	if ((datameta->type < (QdataType_t) Q_ENUM_VALUE_START) || (datameta->type > QDATA_TYPE_COUNT)) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		free(datameta->datap);
		free(datameta);
		return;
	}

	
	/* Find the type of the data and call its appropriate destructor */
	switch(datameta->type) {

	/* Non-string typedeffed types */
	case QDATA_TYPE_QWALK_AREA:
		qwalk_area_destroy((QwalkArea_t *) datameta->datap);
		break;
	
	default:
		free(datameta->datap);
	}


	free(datameta);
	return;
}


/**
 * Get the size of a #QdataType_t.
 * Specifically the size in bytes. This is merely for acquiring the size of a
 * singly dereferenced pointer when the size cannot be normally acquired (e.g.
 * when stored as `void *` or #Qdata_t *. The <i>amount</i> of these
 * primitive types should be acquired elsewhere.
 * @param[in] data_type: the relevant #QdataType_t
 * @return the size of the data type in bytes or #Q_ERRORCODE_SIZE if the
 * type doesn't exist.
 */
size_t
qdata_type_size_get(QdataType_t data_type) {
	switch (data_type) {
	case QDATA_TYPE_INT:
		return sizeof(int);
	case QDATA_TYPE_FLOAT:
		return sizeof(float);
	case QDATA_TYPE_BOOL:
		return sizeof(bool);
	case QDATA_TYPE_INT_STRING:
		return sizeof(int);
	case QDATA_TYPE_CHAR_STRING:
		return sizeof(char);
	case QDATA_TYPE_QWALK_AREA:
		return sizeof(QwalkArea_t);
	case QDATA_TYPE_QOBJECT_TYPE:
		return sizeof(QobjType_t);
	default:
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERRORCODE_SIZE;
	}
}


/**
 * Get datap member of a #Qdatameta_t
 * @param[in] datameta: relevant #Qdatameta_t
 * @return @c datap or @c NULL upon failure
 */
Qdata_t *
qdatameta_datap_get(const Qdatameta_t *datameta) {
	if (datameta == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	return datameta->datap;
}


/**
 * Get count member of a #Qdatameta_t
 * @param[in] datameta: relevant #Qdatameta_t
 * @return @c count or #Q_ERRORCODE_SIZE upon failure
 */
size_t
qdatameta_count_get(const Qdatameta_t *datameta) {
	if (datameta == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return (size_t) Q_ERRORCODE_SIZE;
	}
	return datameta->count;
}


/**
 * Get type member of a #Qdatameta_t
 * @param[in] datameta: relevant #Qdatameta_t
 * @return @c type or #Q_ERRORCODE_ENUM upon failure
 */
QdataType_t
qdatameta_type_get(const Qdatameta_t *datameta) {
	if (datameta == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return (QdataType_t) Q_ERRORCODE_ENUM;
	}
	return datameta->type;
}


/**
 * Convert a #QobjType_t to a `char *`.
 * @param[in] type: relevant #QobjType_t.
 * @return `char *` version of @p type.
 */
char *
qobj_type_to_string(QobjType_t type) {
	if ((type < (QobjType_t) Q_ENUM_VALUE_START) || (type > QOBJ_TYPE_COUNT)) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERRORCODE_CHARSTRING;
	}
	switch (type) {
	case QOBJ_TYPE_PLAYER:
		return QOBJ_STRING_TYPE_PLAYER;
	case QOBJ_TYPE_GRASS:
		return QOBJ_STRING_TYPE_GRASS;
	case QOBJ_TYPE_TREE:
		return QOBJ_STRING_TYPE_TREE;
	case QOBJ_TYPE_VOID:
		return QOBJ_STRING_TYPE_VOID;
	case QOBJ_TYPE_NPC_FRIENDLY:
		return QOBJ_STRING_TYPE_NPC_FRIENDLY; 
	default:
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERRORCODE_CHARSTRING;
	}
}


/**
 * Convert a `char *` to a #QobjType_t.
 * @param[in] s: relevant `char *`.
 * @return #QobjType_t version of @p s.
 */
QobjType_t
qobj_string_to_type(const char *s) {
	
	if (strcmp(s, QOBJ_STRING_TYPE_PLAYER) == 0) {
		return QOBJ_TYPE_PLAYER;
	} else if (strcmp(s, QOBJ_STRING_TYPE_GRASS) == 0) {
		return QOBJ_TYPE_GRASS;
	} else if (strcmp(s, QOBJ_STRING_TYPE_TREE) == 0) {
		return QOBJ_TYPE_TREE;
	} else if (strcmp(s, QOBJ_STRING_TYPE_VOID) == 0) {
		return QOBJ_TYPE_VOID;
	} else if (strcmp(s, QOBJ_STRING_TYPE_NPC_FRIENDLY) == 0) {
		return QOBJ_TYPE_NPC_FRIENDLY;
	} else {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return (QobjType_t) Q_ERRORCODE_ENUM;
	}

}


/**
 * Convert a @c bool to a `char *`.
 * @param[in] b: relevant @c bool.
 * @return @c char * version of @p b.
 */
char *
bool_to_string(bool b) {
	if (b) {
		return BOOL_STRING_TRUE;
	} else {
		return BOOL_STRING_FALSE;
	}
}


/**
 * Convert a `char *` to a bool.
 * @param[in] s: relevant `char *`.
 * @return @c bool version of @p s. If @p s isn't a possible value, @c false
 * will be returned anyway.
 */
bool
charstring_to_bool(const char *s) {
	if (strcmp(s, BOOL_STRING_TRUE) == 0) {
		return true;
	} else {
		return false;
	}
}
