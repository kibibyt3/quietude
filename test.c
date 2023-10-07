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
	p = calloc(COUNT, sizeof(*p));
	int r;

	for (int i = 0, val = 1; i < COUNT; i++, val *= 2) {
		p[i] = val;
	}

	Qdatameta_t *datameta;
	
	datameta = qdatameta_create((Qdata_t *) p, QDATA_TYPE_INT, (size_t) COUNT);
	assert(datameta != NULL);

	r = qfile_open(FILENAME, QFILE_MODE_WRITE);
	assert(r != Q_ERROR);
	
	r = qfile_qdatameta_write(datameta);
	assert(r != Q_ERROR);

	r = qfile_close();
	assert(r != Q_ERROR);

	return 0;
}
