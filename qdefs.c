/**
 * @file qdefs.c
 * Program file for the qdefs 'module'.
 * Logic for game-wide types.
 */

#include <stdlib.h>

#include "qdefs.h"

/**
 * Create a datameta.
 * @param[in] count: number of elements allocated to the given pointer
 * @param[in] data:  pointer to allocated memory that stores the data
 * @param[in] type:  type of @c data
 * @return newly created #Qdatameta_t
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
