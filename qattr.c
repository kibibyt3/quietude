/**
 * @file qattr.c
 * Program file for the attribute module.
 */

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "qdefs.h"

#include "qattr.h"



/**
 * Create a #QattrList_t
 * Allocates memory for a new #QattrList_t of a given size.
 * @param[in] count: the number of #Qattr_t to allocate memory for
 * @return pointer to a newly allocated #QattrList_t
 */
QattrList_t*
qattr_list_create(size_t count){
	assert(count < SIZE_MAX);
	QattrList_t *qattr_listp;
	qattr_listp        = calloc(1, sizeof(*qattr_listp));
	qattr_listp->attrp = calloc(count, sizeof(*(qattr_listp->attrp)));
	assert(qattr_listp->attrp != NULL);
	qattr_listp->count = count;
	/* Initialize index_ok to zero because it is the first available index */
	qattr_listp->index_ok = (size_t) 0;
	return qattr_listp;
}

/**
 * Destroy a #QattrList_t
 * @param[in] qattr_list: #QattrList_t to free from memory
 * @return #Q_OK or #Q_ERROR
 */
int
qattr_list_destroy(QattrList_t *qattr_list) {
	free(qattr_list);
	return Q_OK;
}

/**
 * Fetch an attribute value
 * Fetches the value associated with a #QattrKey_t in a #QattrList_t.
 * @param[in] attr_key: key whose value is to be found
 * @param[in] attr_list: list whose keys are to be parsed
 * @return #Qdatameta_t containing the value or @c NULL if the key doesn't exist.
 */ 
Qdatameta_t*
qattr_list_value_get(QattrList_t* attr_list, QattrKey_t attr_key) {	
	/* We need only iterate through those elements which have been properly added */
	for (int i = 0; i < (int) (attr_list->index_ok); i++){
		if (attr_list->attrp[i].key == attr_key){
			return attr_list->attrp[i].valuep;
		}
	}
	return NULL;
}

/**
 * Add key/value pair to a #QattrList_t.
 * @param[out] attr_list: list to gain a key/value pair. Its @c index_ok element
 * is incremented with a successful addition.
 * @param[in]  attr_key: key to add to the #QattrList_t
 * @param[out] datameta: datameta to add to the #QattrList_t
 * @return #Q_OK or #Q_ERROR.
 */
int
qattr_list_attr_set(QattrList_t *attr_list, QattrKey_t attr_key, Qdatameta_t *datameta) {
	size_t index_free = attr_list->index_ok; /* To ease readability */
	
	/* Validate attr_key */
	if (attr_key < 1 || attr_key > QATTR_KEY_COUNT) {
		return Q_ERROR;
	}

	attr_list->attrp[index_free].key = attr_key;
	attr_list->attrp[index_free].valuep = datameta;
	(attr_list->index_ok)++; /* Index is no longer available; move to the next */
	return Q_OK;
}
