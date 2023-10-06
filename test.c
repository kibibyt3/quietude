#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "qdefs.h"
#include "qattr.h"
#include "qfile.h"

int main(int argc, char** argv) {
	QattrList_t *qattr_list;
	Qdatameta_t *datameta, *datameta_out;
	int *data, *data_out;
	int size;
	data = calloc((size_t) 4, sizeof(*data));
	data[0] = 3;
	data[1] = 1;
	data[2] = 9;
	data[3] = 7;
	datameta = qdatameta_create((Qdata_t *)data, QDATA_TYPE_INT, (size_t) 4);
	
	qattr_list = qattr_list_create((size_t) 7);
	
	assert(qattr_list_attr_set(qattr_list, QATTR_KEY_DEBUG, datameta) != Q_ERROR);

	datameta_out = qattr_list_value_get(qattr_list, QATTR_KEY_DEBUG);
	
	assert(datameta_out != NULL);
	data_out = (int *) datameta_out->datap;

	printf("%i, %i, %i, %i\n", data_out[0], data_out[1], data_out[2], data_out[3]); 

	return 0;
}
