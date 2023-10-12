/**
 * @file qdefs.c
 * Program file for the qdefs 'module'.
 * Logic for game-wide types.
 */

#include <stdlib.h>
#include <stdio.h>

#include "qdefs.h"
#include "qerror.h"

/**
 * Create a #Qdatameta_t.
 * @param[in] type:  type of @c data
 * @param[in] count: number of elements allocated to the given pointer
 * @return newly created #Qdatameta_t or @c NULL pointer 
 * @allocs{2} for returned pointer and its datap member.
 */ 
Qdatameta_t *
qdatameta_create(QdataType_t type, size_t count) {
	Qdatameta_t *datameta;
	Qdata_t *datap;
	datameta = calloc((size_t) 1, sizeof(*datameta));
	if (datameta == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	datap = calloc(count, qdata_type_size_get(type));
	if (datap == NULL) {
		free(datameta);
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	datameta->datap = datap;
	datameta->count = count;
	datameta->type = type;
	return datameta;
}

/**
 * Destory a #Qdatameta_t.
 * This includes its @c datap member.
 * @param[out] datameta: #Qdatameta_t to free from memory
 */
void
qdatameta_destroy(Qdatameta_t *datameta) {
	free(datameta->datap);
	free(datameta);
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
	case QDATA_TYPE_STRING:
		return sizeof(int);
	default:
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
