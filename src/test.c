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
#include "mode.h"
#include "qwalk.h"

/*
#include "mode.h"
*/

#define COUNT 5
#define FILENAME "saves/test.sav"

int main(/*@unused@*/int argc, /*@unused@*/char** argv) {

	printf("\n-----BEGIN PHONY ERRORS-----\n");
	Q_IFERROR(true, (Qerror_t) QERROR_NULL_POINTER_UNEXPECTED);
	Q_IFERROR(true, (Qerror_t) QERROR_NULL_VALUE_UNEXPECTED);
	Q_IFERROR((1 == 2), (Qerror_t) QERROR_ENUM_CONSTANT_INVALID);
	Q_IFERROR((1 == 1), (Qerror_t) QERROR_ENUM_CONSTANT_INVALID_ZERO);
	Q_IFERROR(1 == 1 && 2 == 2, (Qerror_t) QERROR_MODULE_UNINITIALIZED);
	Q_IFERROR(true, 0);
	printf("------END PHONY ERRORS------\n\n\n");

	int r;


	Qdatameta_t *datameta;
	Qdatameta_t *datameta2;

	QwalkLayer_t *walk_layer_floater;
	QwalkLayer_t *walk_layer_earth;

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
	val = NULL;

	QattrList_t *attr_list;
	attr_list = qattr_list_create((size_t) 1);
	assert(attr_list != NULL);
	r = qattr_list_attr_set(attr_list, QATTR_KEY_NAME, datameta2);
	assert(r != Q_ERROR);
	datameta2 = NULL;	
	Qdatameta_t *datameta3;
	
	datameta3 = qattr_list_value_get(attr_list, QATTR_KEY_NAME);
	
	val = (int *) qdatameta_datap_get(datameta3);
	assert(val != NULL);
	printf("%i, %i, %i, %i, %i\n", val[0], val[1], val[2], val[3], val[4]);
	val = NULL;
	size_t s;
	QdataType_t data_type;
	data_type = qdatameta_type_get(datameta);
	s = qdatameta_count_get(datameta);
	printf("%i, %i\n", (int) s, (int) data_type);

	walk_layer_earth = qwalk_layer_create();
	assert(walk_layer_earth != NULL);
	walk_layer_floater = qwalk_layer_create();
	assert(walk_layer_floater != NULL);

	r = qattr_list_destroy(attr_list);
	assert(r != Q_ERROR);
	
	attr_list = NULL;
	QobjType_t *obj_type;
	for (int i = 0; i < QWALK_LAYER_SIZE; i++) {
		obj_type = calloc((size_t) 1, sizeof(*obj_type));
		assert(obj_type != NULL);
		*obj_type = QOBJ_TYPE_GRASS;
		attr_list = qattr_list_create((size_t) 1);
		assert(attr_list != NULL);
		datameta2 = qdatameta_create(QDATA_TYPE_QOBJECT_TYPE, (size_t) 1);
		assert(datameta2 != NULL);
		(datameta2->datap) = (Qdata_t *) obj_type;
		obj_type = NULL;
		r = qattr_list_attr_set(attr_list, QATTR_KEY_QOBJECT_TYPE, datameta2);
		assert(r != Q_ERROR);
		assert(attr_list != NULL);
		r = qwalk_layer_object_set(walk_layer_earth, i, i, attr_list);
		attr_list = NULL;
		assert(r != Q_ERROR);
	}
	for (int i = 0; i < QWALK_LAYER_SIZE; i++) {
		obj_type = calloc((size_t) 1, sizeof(*obj_type));
		assert(obj_type != NULL);
		*obj_type = QOBJ_TYPE_GRASS;
		attr_list = qattr_list_create((size_t) 1);
		assert(attr_list != NULL);
		datameta2 = qdatameta_create(QDATA_TYPE_QOBJECT_TYPE, (size_t) 1);
		assert(datameta2 != NULL);
		datameta2->datap = (Qdata_t *) obj_type;
		obj_type = NULL;
		r = qattr_list_attr_set(attr_list, QATTR_KEY_QOBJECT_TYPE, datameta2);
		assert(r != Q_ERROR);
		assert(attr_list != NULL);
		r = qwalk_layer_object_set(walk_layer_floater, i, i, attr_list);
		attr_list = NULL;
		assert(r != Q_ERROR);
	}
	r = qwalk_layer_destroy(walk_layer_earth);
	assert(r != Q_ERROR);
	r = qwalk_layer_destroy(walk_layer_floater);
	assert(r != Q_ERROR);
	qdatameta_destroy(datameta);
	assert(r != Q_ERROR);
	datameta = NULL;
	return 0;
}
