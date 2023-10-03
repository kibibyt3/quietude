/**
 * qdefs.c
 * Program file for the qdefs 'module'. Currently only holds functions for
 * dealing with datametas.
 */

#include <stdlib.h>

#include "qdefs.h"

/**
 * Creates a datameta. Size must be the number of elements allocated to the
 * given pointer. data's memory must be allocated in advance and merely cast to
 * the form of Qdata_t*. Param type holds the type of data and count holds the
 * number of type in data.
 */ 
Qdatameta_t*
qdatameta_create(Qdata_t* data, QdataType_t type, size_t count) {
	Qdatameta_t datameta;
	datametap = &datameta;
	datametap->datap = data;
	datametap->count = count;
	datametap->type = type;
	return &datameta;
}
