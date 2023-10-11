#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "qdefs.h"
#include "qattr.h"
#include "qfile.h"
#include "qerror.h"

/*
#include "mode.h"
*/

#define COUNT 5
#define FILENAME "saves/test.sav"

int main(/*@unused@*/int argc, /*@unused@*/char** argv) {

	Q_IFERROR(true, (Qerror_t) QERROR_NULL_POINTER_UNEXPECTED);
	Q_IFERROR(true, (Qerror_t) QERROR_NULL_VALUE_UNEXPECTED);
	Q_IFERROR((1 == 2), (Qerror_t) QERROR_ENUM_CONSTANT_INVALID);
	Q_IFERROR((1 == 1), (Qerror_t) QERROR_ENUM_CONSTANT_INVALID_ZERO);
	Q_IFERROR(1 == 1 && 2 == 2, (Qerror_t) QERROR_MODULE_UNINITIALIZED);
	Q_IFERROR(true, 0);
	
	int r;


	Qdatameta_t *datameta;
	Qdatameta_t *datameta2;

	datameta  = qdatameta_create(QDATA_TYPE_INT, (size_t) COUNT);
	datameta2 = qdatameta_create(QDATA_TYPE_INT, (size_t) COUNT);
	assert(datameta != NULL);
	assert(datameta2 != NULL);

	((int *)(datameta->datap))[0] = 1;
	((int *)(datameta->datap))[1] = 2;
	((int *)(datameta->datap))[2] = 4;
	((int *)(datameta->datap))[3] = 8;
	((int *)(datameta->datap))[4] = 16;


	r = qfile_open(FILENAME, QFILE_MODE_WRITE);
	assert(r != Q_ERROR);
	
	r = qfile_qdatameta_write(datameta);
	assert(r != Q_ERROR);

	r = qfile_close();
	assert(r != Q_ERROR);

	r = qfile_open(FILENAME, QFILE_MODE_READ);
	assert(r != Q_ERROR);

	r = qfile_qdatameta_read(datameta2);
	assert(r != Q_ERROR);

	int *val = (int *) qdatameta_datap_get(datameta2);
	assert(val != NULL);
	printf("%i, %i, %i, %i, %i\n", val[0], val[1], val[2], val[3], val[4]);
	
	printf("%i, %i\n", (int) qdatameta_type_get(datameta2), (int) qdatameta_count_get(datameta2));
	qdatameta_destroy(datameta);
	qdatameta_destroy(datameta2);
	return 0;
}
