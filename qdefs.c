/*
 * qdefs.c
 * Program file for the qdefs 'module'. Currently only holds functions for
 * dealing with datametas.
 */

#include <stdlib.h>

#include "qdefs.h"

/*
 * Creates a datameta. Size must be the number of elements allocated to the
 * given pointer. data's memory must be allocated in advance and merely cast to
 * the form of Qdata_t*
 */ 
Qdatameta_t*
qdatameta_create(Qdata_t* data, size_t size) {
	Qdatameta_t datameta;
	datametap = &datameta;
	datametap->datap = data;
	datametap->size = size;
	return &datameta;
}
