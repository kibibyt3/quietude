/**
 * @file qattr.c
 * Program file for the attribute module.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "qdefs.h"
#include "qerror.h"

#include "qattr.h"
#include "qfile.h"



/**
 * Create a #QattrList_t
 * Allocates memory for a new #QattrList_t of a given size.
 * @param[in] count: the number of #Qattr_t to allocate memory for
 * @return pointer to a newly allocated #QattrList_t or @c NULL upon failure
 * @allocs{2} for returned pointer and its attrp member.
 */
QattrList_t*
qattr_list_create(size_t count){
	QattrList_t *qattr_listp;
	qattr_listp        = calloc((size_t) 1, sizeof(*qattr_listp));
	if (qattr_listp == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	qattr_listp->attrp = calloc(count, sizeof(*(qattr_listp->attrp)));
	if (qattr_listp->attrp == NULL){
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		free(qattr_listp);
		return NULL;
	}

	qattr_listp->count = count;
	/* Initialize index_ok to zero because it is the first available index */
	qattr_listp->index_ok = (size_t) 0;
	return qattr_listp;
}


/**
 * Clone a #QattrList_t.
 * @param[in] attr_listr: pointer to #QattrList_t to clone.
 * @return clone of @p attr_list.
 */
QattrList_t *
qattr_list_clone(const QattrList_t *attr_listr) {
	QattrList_t *attr_list;
	size_t count;
	QattrKey_t attr_key;
	Qdatameta_t *datameta;
	int r;

	count = qattr_list_count_get(attr_listr);
	if (count == (size_t) Q_ERRORCODE_SIZE) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	attr_list = qattr_list_create(count);
	if (attr_list == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}

	for (int i = 0; i < (int) count; i++) {
		attr_key = qattr_list_attr_key_get(attr_listr, i);
		if (attr_key == (QattrKey_t) Q_ERRORCODE_ENUM) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			abort();
		}
		
		datameta = qdatameta_clone(qattr_list_value_get(attr_listr, attr_key));
		if (datameta == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);;
			abort();
		}

		r = qattr_list_attr_set(attr_list, attr_key, datameta);
		if (r == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			abort();
		}
	}

	return attr_list;
}


/**
 * Destroy a #QattrList_t
 * @param[in] qattr_list: #QattrList_t to free from memory
 * @return #Q_OK or #Q_ERROR
 */
int
qattr_list_destroy(QattrList_t *qattr_list) {
	for (int i = 0; i < (int) qattr_list->index_ok; i++) {
		if (qattr_list->attrp[i].key != QATTR_KEY_EMPTY) {
			if (qdatameta_destroy(qattr_list->attrp[i].valuep) == Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
			}
			qattr_list->attrp[i].valuep = NULL;
		}
	}

	/*@i1@*/free(qattr_list->attrp);
	free(qattr_list);
	return Q_OK;
}


/**
 * Write a #QattrList_t to storage.
 * @param[in] attr_list: #QattrList_t to write; must be completely filled out.
 * @return #Q_OK or #Q_ERROR.
 */
int
qattr_list_write(const QattrList_t *attr_list) {
	int r;
	int returnval = Q_OK;

	if (attr_list == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}

	r = qfile_size_write(attr_list->count);
	if (r == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}

	for (int i = 0; i < (int) attr_list->count; i++) {
		r = qfile_qattr_key_write(attr_list->attrp[i].key);
		if (r == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			returnval = Q_ERROR;
		}
		
		r = qfile_qdatameta_write(attr_list->attrp[i].valuep);
		if (r == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			returnval = Q_ERROR;
		}
	}

	return returnval;
}


/**
 * Read a #QattrList_t from storage.
 * @return new #QattrList_t.
 */
QattrList_t *
qattr_list_read() {
	QattrList_t *attr_list;
	size_t count;
	QattrKey_t attr_key;
	Qdatameta_t *datameta;
	int r;

	count = qfile_size_read();
	if (count == (size_t) Q_ERRORCODE_SIZE) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	attr_list = qattr_list_create(count);
	if (attr_list == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}

	for (int i = 0; i < (int) count; i++) {
		attr_key = qfile_qattr_key_read();
		if (attr_key == (QattrKey_t) Q_ERRORCODE_ENUM) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			abort();
		}
		
		datameta = qfile_qdatameta_read();
		if (datameta == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);;
			abort();
		}

		r = qattr_list_attr_set(attr_list, attr_key, datameta);
		if (r == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			abort();
		}
	}

	return attr_list;
}


/**
 * Fetch an attribute value
 * Fetches the value associated with a #QattrKey_t in a #QattrList_t.
 * @param[in] attr_key: key whose value is to be found
 * @param[in] attr_list: list whose keys are to be parsed
 * @return #Qdatameta_t containing the value or @c NULL if the key doesn't exist.
 */ 
Qdatameta_t*
qattr_list_value_get(const QattrList_t *attr_list, QattrKey_t attr_key) {

	Qdatameta_t *value = NULL;
	if (attr_list == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	/* We need only iterate through those elements which have been properly added */
	for (int i = 0; i < (int) (attr_list->index_ok); i++){
		if (attr_list->attrp[i].key == attr_key){
			value = attr_list->attrp[i].valuep;
			break;
		}
	}
	return value;
}


/**
 * Get @ref QattrList_t.count.
 * @param[in] attr_list: relevant attr_list.
 * @return @ref QattrList_t.count or #Q_ERRORCODE_SIZE.
 */ 
size_t
qattr_list_count_get(const QattrList_t *attr_list) {
	if (attr_list == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return (size_t) Q_ERRORCODE_SIZE;
	}
	return attr_list->count;
}


/**
 * Get @ref QattrList_t.index_ok.
 * @param[in] attr_list: relevant attr_list.
 * @return @ref QattrList_t.index_ok or #Q_ERRORCODE_SIZE.
 */ 
size_t
qattr_list_index_ok_get(const QattrList_t *attr_list) {
	if (attr_list == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return (size_t) Q_ERRORCODE_SIZE;
	}
	return attr_list->index_ok;
}


/**
 * Get @ref Qattr_t.key in a #QattrList_t from an index.
 * @param[in] attr_list: relevant attr_list.
 * @param[in] index: index of @ref Qattr_t.key in @p attr_list
 * @return @ref Qattr_t.key or #Q_ERRORCODE_ENUM.
 */ 
QattrKey_t
qattr_list_attr_key_get(const QattrList_t *attr_list, int index) {
	if (attr_list == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return (QattrKey_t) Q_ERRORCODE_ENUM;
	}
	if (index >= (int) qattr_list_index_ok_get(attr_list)) {
		Q_ERRORFOUND(QERROR_INDEX_OUTOFRANGE);
		return (QattrKey_t) Q_ERRORCODE_ENUM;
	}

	return attr_list->attrp[index].key;
}


/*
 * Remove a #Qattr_t from a #QattrList_t.
 * The @c valuep member of the #Qattr_t is set to be equal to @c NULL and the @c
 * key member to #QATTR_KEY_EMPTY when an error does not occur.
 * @param[in] attr_key: key whose value is to be found
 * @param[in] attr_list: list whose keys are to be parsed
 * @return #Qdatameta_t containing the value or @c NULL if the key doesn't exist.
 */
/*
Qdatameta_t*
qattr_list_attr_remove(QattrList_t* attr_list, QattrKey_t attr_key) {

	Qdatameta_t *value = NULL;
	if (attr_list == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	* We need only iterate through those elements which have been properly added *
	for (int i = 0; i < (int) (attr_list->index_ok); i++){
		if (attr_list->attrp[i].key == attr_key){
			value = attr_list->attrp[i].valuep;
			attr_list->attrp[i].valuep = NULL;
			attr_list->attrp[i].key = QATTR_KEY_EMPTY;
			break;
		}
	}
	return value;
}
*/

/**
 * Add key/value pair to a #QattrList_t.
 * @param[out] attr_list: list to gain a key/value pair. Its @ref
 * #QattrList_t.index_ok is incremented with a successful addition.
 * @param[in]  attr_key: key to add to the #QattrList_t
 * @param[in]  datameta: datameta to add to the #QattrList_t
 * @return #Q_OK or #Q_ERROR.
 */
int
qattr_list_attr_set(QattrList_t *attr_list, QattrKey_t attr_key, Qdatameta_t *datameta) {
	size_t index_free = attr_list->index_ok; /* To ease readability */
	
	/* Validate attr_key */
	assert((attr_key > (QattrKey_t) Q_ENUM_VALUE_START) || (attr_key < QATTR_KEY_COUNT));

	attr_list->attrp[index_free].key = attr_key;
	attr_list->attrp[index_free].valuep = datameta;
	(attr_list->index_ok)++; /* Index is no longer available; move to the next */
	return Q_OK;
}


/**
 * Alter the value of an already-existing key in a #QattrList_t.
 * @param[out] attr_list: #QattrList_t whose key is to be altered.
 * @param[in] attr_key: #QattrKey_t whose value will be set to @p datameta.
 * @param[in] datameta: #Qatameta_t to add to @p attr_list.
 * @return #Q_ERROR_NOCHANGE if @p attr_key could not be found, or #Q_OK or 
 * #Q_ERROR.
 */
int
qattr_list_attr_modify(QattrList_t *attr_list, QattrKey_t attr_key, Qdatameta_t *datameta) {
	/* Validate attr_key */
	if ((attr_key < (QattrKey_t) Q_ENUM_VALUE_START) || (attr_key > QATTR_KEY_COUNT)) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
	}
	
	/*
	 * when the matching key is found, destroy its value and replace it with the
	 * new one.
	 */
	for (int i = 0; i < (int) qattr_list_count_get(attr_list); i++) {
		if (qattr_list_attr_key_get(attr_list, i) == attr_key) {
			if (qdatameta_destroy(attr_list->attrp[i].valuep) == Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
			}
			attr_list->attrp[i].valuep = datameta;
			return Q_OK;
		}
	}

	/* 
	 * to avoid memory leaks, destroy the parameter if it wasn't successfully set.
	 */
	if (qdatameta_destroy(datameta) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}
	return Q_ERROR_NOCHANGE;
}


/**
 * Convert a @ref Qattr_t.valuep to a string, if possible.
 * @param[in] attr_list: #QattrList_t to find @p key in.
 * @param[in] key: key of value to convert to a string.
 * @return `char *` version of value of @p key.
 */
char *
qattr_value_to_string(const QattrList_t *attr_list, QattrKey_t key) {
	Qdatameta_t *datameta;
	size_t count;
	QdataType_t type;
	Qdata_t *data;

	/* collect and sanitize value metadata */
	if ((datameta = qattr_list_value_get(attr_list, key)) == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERRORCODE_CHARSTRING; 
	}
	if ((count = qdatameta_count_get(datameta)) == Q_ERRORCODE_SIZE) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERRORCODE_CHARSTRING;
	}
	if ((data = qdatameta_datap_get(datameta)) == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERRORCODE_CHARSTRING; 
	}
	if((type = qdatameta_type_get(datameta)) == (QdataType_t) Q_ERRORCODE_ENUM) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERRORCODE_CHARSTRING;
	}

	switch (key) {
		case QATTR_KEY_QOBJECT_TYPE:
			return qobj_type_to_string(*((QobjType_t *) data));
		case QATTR_KEY_NAME:
			return (char *) data;
		case QATTR_KEY_DESCRIPTION_BRIEF:
			return (char *) data;
		case QATTR_KEY_DESCRIPTION_LONG:
			return (char *) data;
		case QATTR_KEY_CANMOVE:
			return bool_to_string(*((bool *) data));
		case QATTR_KEY_EMPTY:
			return Q_ERRORCODE_CHARSTRING;
		case QATTR_KEY_DEBUG:
			return Q_ERRORCODE_CHARSTRING;
		default:
			Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
			return QATTR_STRING_KEY_UNRECOGNIZED;
	}
}


/**
 * Convert a #QattrKey_t to a @c char * from @ref AttrKeyStrings.
 * @param[in] key: relevant #QattrKey_t.
 * @return string @p key or #QATTR_STRING_KEY_UNRECOGNIZED.
 */
char *
qattr_key_to_string(QattrKey_t key) {
	switch (key) {
		case QATTR_KEY_QOBJECT_TYPE:
			return QATTR_STRING_KEY_QOBJECT_TYPE;
		case QATTR_KEY_NAME:
			return QATTR_STRING_KEY_NAME;
		case QATTR_KEY_DESCRIPTION_BRIEF:
			return QATTR_STRING_KEY_DESCRIPTION_BRIEF;
		case QATTR_KEY_DESCRIPTION_LONG:
			return QATTR_STRING_KEY_DESCRIPTION_LONG;
		case QATTR_KEY_CANMOVE:
			return QATTR_STRING_KEY_CANMOVE;
		case QATTR_KEY_EMPTY:
			return QATTR_STRING_KEY_EMPTY;
		case QATTR_KEY_DEBUG:
			return QATTR_STRING_KEY_DEBUG;
		default:
			Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
			return QATTR_STRING_KEY_UNRECOGNIZED;
	}
}


/**
 * Convert a @c char * from @ref AttrKeyStrings to a #QattrKey_t.
 * @param[in] s: relevant string.
 * @return requested #QattrKey_t or #Q_ERRORCODE_ENUM.
 */
QattrKey_t
qattr_string_to_key(const char *s) {
	if (strcmp(s, QATTR_STRING_KEY_QOBJECT_TYPE) == 0) {
		return QATTR_KEY_QOBJECT_TYPE;
	} else if (strcmp(s, QATTR_STRING_KEY_NAME) == 0) {
		return QATTR_KEY_NAME;
	} else if (strcmp(s, QATTR_STRING_KEY_DESCRIPTION_BRIEF) == 0) {
		return QATTR_KEY_DESCRIPTION_BRIEF;
	} else if (strcmp(s, QATTR_STRING_KEY_DESCRIPTION_LONG) == 0) {
		return QATTR_KEY_DESCRIPTION_LONG;
	} else if (strcmp(s, QATTR_STRING_KEY_CANMOVE) == 0) {
		return QATTR_KEY_CANMOVE;
	} else if (strcmp(s, QATTR_STRING_KEY_EMPTY) == 0) {
		return QATTR_KEY_EMPTY;
	} else if (strcmp(s, QATTR_STRING_KEY_DEBUG) == 0) {
		return QATTR_KEY_DEBUG;
	} else {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return (QattrKey_t) Q_ERRORCODE_ENUM;
	}
}
