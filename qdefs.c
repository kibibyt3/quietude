/**
 * @file qdefs.c
 * Program file for the qdefs 'module'.
 * Logic for game-wide types.
 */

#include <stdlib.h>

#include "qdefs.h"

/**
 * Create a #Qdatameta_t.
 * @param[in] count: number of elements allocated to the given pointer
 * @param[in] data:  pointer to allocated memory that stores the data
 * @param[in] type:  type of @c data
 * @return newly created #Qdatameta_t or @c NULL pointer 
 */ 
Qdatameta_t*
qdatameta_create(Qdata_t* data, QdataType_t type, size_t count) {
	Qdatameta_t *datametap; 
	datametap = calloc(1, sizeof(*datametap));
	datametap->datap = data;
	datametap->count = count;
	datametap->type = type;
	return datametap;
}

/**
 * Create a #Qdata_t.
 * @param[in] type: the type of the #Qdata_t
 * @param[in] count: the amount of each @c type
 * @return pointer to newly created #Qdata_t
 */
Qdata_t*
qdata_create(QdataType_t type, size_t count){
	Qdata_t *datap;
	datap = calloc(count, qdata_type_size_get(type));
	return datap;
}

/**
 * Get the size of a #QdataType_t.
 * Specifically the size in bytes. This is merely for acquiring the size of a
 * singly dereferenced pointer when the size cannot be normally acquired (e.g.
 * when stored as `void *` or #Qdata_t *. The <i>amount</i> of these
 * primitive types should be acquired elsewhere.
 * @param[in] data_type: the relevant #QdataType_t
 * @return the size of the data type in bytes or #Q_DEFAULT_TYPE_SIZE if the
 * type doesn't exist.
 */
size_t
qdata_type_size_get(QdataType_t data_type) {
	switch (data_type) {
	case QDATA_TYPE_INT:
		return sizeof(int);
	case QDATA_TYPE_FLOAT:
		return sizeof(float);
	case QDATA_TYPE_STRING:
		return sizeof(int);
	default:
		return Q_DEFAULT_TYPE_SIZE;
	}
}
