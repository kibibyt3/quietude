/*
 * qdefs.c
 * Program file for the qdefs 'module'. Currently only holds functions for
 * dealing with datametas.
 */

#include <stdlib.h>

#include "qdefs.h"

/*
 * Creates a datameta. Size must be given as a size in bytes (ideally using the
 * sizeof operator). It is not a 'count' as in other initializers.
 */ 
Qdatameta_t*
qdatameta_create(Qdata_t* data, size_t size) {
	Qdatameta_t datameta;
	datameta = malloc(size);
	assert(datameta != NULL);
	datameta->datap = data;
	datameta->size = size;
	return &datameta;
}

/*
 * Frees a datameta from memory.
 */
int
qdatameta_destroy(Qdatameta *datameta){
	free(datameta);
	return Q_SUCCESS;
}
