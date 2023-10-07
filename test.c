#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "qdefs.h"
#include "qattr.h"
#include "qfile.h"

#define COUNT 5
#define FILENAME "test.sav"

int main(int argc, char** argv) {

	int *p;
	int *p2;

	p = calloc(COUNT, sizeof(*p));
	p2 = calloc(COUNT, sizeof(*p2));

	int r;

	for (int i = 0, val = 1; i < COUNT; i++, val *= 2) {
		p[i] = val;
	}

	Qdatameta_t *datameta;
	Qdatameta_t *datameta2;
	Qdata_t     *data;

	data = qdata_create(QDATA_TYPE_INT, COUNT);

	datameta  = qdatameta_create((Qdata_t *) p, QDATA_TYPE_INT, (size_t) COUNT);
	datameta2 = qdatameta_create((Qdata_t *) p2, QDATA_TYPE_INT, (size_t) COUNT);
	assert(datameta != NULL);
	assert(datameta2 != NULL);

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

	p2 = (int *) datameta2->datap;

	printf("%i, %i, %i, %i, %i", p2[0], p2[1], p2[2], p2[3], p2[4]);
	
	return 0;
}
