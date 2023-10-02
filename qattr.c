/*
 * qattr.c
 * Program file for the attribute module.
 */

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "qdefs.h"

#include "qattr.h"

/*
 * Creates and allocates memory for a new attr_list of size count.
 */
QattrList_t*
qattr_list_create(size_t count){
	assert(count < SIZE_MAX);
	QattrList_t  qattr_dummy;
	QattrList_t *qattr_listp;
	qattr_listp = &qattr_dummy;
	qattr_listp->attrp = calloc(count, sizeof(*(qattr_listp->attrp)));
	assert(qattr_listp->attrp != NULL);
	qattr_listp->count = count;
	qattr_listp->index_ok = (size_t) 0; /* Initialize index_ok to zero because it is the first available index */
	return qattr_listp;
}

/*
 * Frees from memory a given attr_list. Returns Q_OK or Q_ERROR.
 */
int
qattr_list_destroy(QattrList_t *qattr_list) {
	free(qattr_list);
	return Q_OK;
}

/*
 * Fetches the value associated with param attr_key in attr_list. Returns NULL
 * if the key doesn't exist.
 */ 
Qdatameta_t*
qattr_list_value_get(QattrList_t* attr_list, QattrKey_t attr_key) {
	for (int i = 0; i < (int) (attr_list->index_ok); i++){ /* We need only iterate through those elements which have been properly added */
		if (attr_list->attrp[i]->key == attr_key){
			return attr_list->attrp[i]->valuep;
		}
	}
	return NULL;
}

/*
 * Adds key/value pair to a given attr_list. attr_list->index_ok gets 
 * incremented only when the attribute is successfully added. Returns Q_OK
 * or Q_ERROR.
 */
int
qattr_list_attr_set(QattrList_t *attr_list, QattrKey_t attr_key, Qdatameta_t *datameta) {
	size_t index_free = attr_list->index_ok; /* To ease readability */
	for (size_t i = 0; i < index_free; i++){ /* Hoping size_t increments by 1! */
		if (attr_list->attrp[i]->key == attr_key){ /* Check if key is already defined. This could be removed to improve performance. */
			return Q_ERROR;
		}
	}
	attr_list->attrp[index_free]->key = attr_key;
	attr_list->attrp[index_free]->valuep = datameta;
	(attr_list->index_ok)++; /* Index is no longer available; move to the next */
	return Q_OK;
}
