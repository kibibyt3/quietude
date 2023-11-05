#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <ncurses.h>

#include "qdefs.h"
#include "ioutils.h"
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
#define FILENAME2 "saves/test2.sav"

int main(/*@unused@*/int argc, /*@unused@*/char** argv) {

	fprintf(stderr, "\n-----BEGIN PHONY ERRORS-----\n");
	Q_IFERROR(true, (Qerror_t) QERROR_NULL_POINTER_UNEXPECTED);
	Q_IFERROR(true, (Qerror_t) QERROR_NULL_VALUE_UNEXPECTED);
	Q_IFERROR((1 == 2), (Qerror_t) QERROR_ENUM_CONSTANT_INVALID);
	Q_IFERROR((1 == 1), (Qerror_t) QERROR_ENUM_CONSTANT_INVALID_ZERO);
	Q_IFERROR(1 == 1 && 2 == 2, (Qerror_t) QERROR_MODULE_UNINITIALIZED);
	Q_IFERROR(true, 0);
	fprintf(stderr, "------END PHONY ERRORS------\n\n\n");

	int r;

	char str[] = "   MY NAME   IS   WWWOWW LITEL DGGO  GGIE    ";
	
	if (io_whitespace_trim(str) == Q_ERROR) {
		abort();
	}

	printf("%s\n", str);

	int *intstr = calloc((size_t) 5, sizeof(*intstr));
	assert(intstr != NULL);
	
	int *intstr2 = calloc((size_t) 5, sizeof(*intstr2));
	assert(intstr != NULL);

	Qdatameta_t *datameta;
	Qdatameta_t *datameta2;

	QwalkLayer_t *walk_layer_floater;
	QwalkLayer_t *walk_layer_earth;

	intstr[0] = 1;
	intstr[1] = 2;
	intstr[2] = 4;
	intstr[3] = 8;
	intstr[4] = 16;
	
	datameta  = qdatameta_create((Qdata_t *) intstr, QDATA_TYPE_INT, (size_t) COUNT);
	assert(datameta != NULL);
	intstr = NULL;


	r = qfile_open(FILENAME, QFILE_MODE_WRITE);
	assert(r != Q_ERROR);
	
	r = qfile_qdatameta_write(datameta);
	assert(r != Q_ERROR);

	r = qfile_close();
	assert(r != Q_ERROR);

	r = qfile_open(FILENAME, QFILE_MODE_READ);
	assert(r != Q_ERROR);

	Qdata_t *rawdata;
	rawdata = qfile_qdata_read(QDATA_TYPE_INT, (size_t) COUNT);
	assert(rawdata != NULL);
	
	r = qfile_close();
	assert(r != Q_ERROR);
	
	datameta2 = qdatameta_create(rawdata, QDATA_TYPE_INT, (size_t) COUNT);
	assert(datameta2 != NULL);
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
	
	/* create both layers */
	for (int i = 0; i < QWALK_LAYER_SIZE; i++) {
		obj_type = calloc((size_t) 1, sizeof(*obj_type));
		assert(obj_type != NULL);
		*obj_type = QOBJ_TYPE_GRASS;
		attr_list = qattr_list_create((size_t) 1);
		assert(attr_list != NULL);
		datameta2 = qdatameta_create((Qdata_t *) obj_type, QDATA_TYPE_QOBJECT_TYPE, (size_t) 1);
		assert(datameta2 != NULL);
		obj_type = NULL;
		r = qattr_list_attr_set(attr_list, QATTR_KEY_QOBJECT_TYPE, datameta2);
		assert(r != Q_ERROR);
		assert(attr_list != NULL);
		
		int *coords;
		coords = qwalk_index_to_coords(i);
		r = qwalk_layer_object_set(walk_layer_earth, coords[0], coords[1], attr_list);
		free(coords);
		attr_list = NULL;
		assert(r != Q_ERROR);
	}
	for (int i = 0; i < QWALK_LAYER_SIZE; i++) {
		obj_type = calloc((size_t) 1, sizeof(*obj_type));
		assert(obj_type != NULL);
		
		if (i == 100) {
			*obj_type = QOBJ_TYPE_PLAYER;
		} else {
			*obj_type = QOBJ_TYPE_VOID;
		}

		attr_list = qattr_list_create((size_t) 1);
		assert(attr_list != NULL);
		
		datameta2 = qdatameta_create((Qdata_t *) obj_type, QDATA_TYPE_QOBJECT_TYPE, (size_t) 1);
		assert(datameta2 != NULL);
		
		obj_type = NULL;
		
		r = qattr_list_attr_set(attr_list, QATTR_KEY_QOBJECT_TYPE, datameta2);
		assert(r != Q_ERROR);
		assert(attr_list != NULL);
		
		int *coords;
		coords = qwalk_index_to_coords(i);
		r = qwalk_layer_object_set(walk_layer_floater, coords[0], coords[1], attr_list);
		free(coords);
		attr_list = NULL;
		assert(r != Q_ERROR);
	}

	QwalkArea_t *walk_area, *walk_area1;
	walk_area1 = qwalk_area_create(walk_layer_earth, walk_layer_floater);
	assert(walk_area1 != NULL);
	walk_layer_earth = NULL;
	walk_layer_floater = NULL;

	r = qfile_open(FILENAME, QFILE_MODE_WRITE);
	assert(r != Q_ERROR);
	
	r = qwalk_area_write(walk_area1);
	assert(r != Q_ERROR);

	r = qfile_close();
	assert(r != Q_ERROR);

	r = qwalk_area_destroy(walk_area1);
	assert(r != Q_ERROR);
	
	walk_area1 = NULL;
	
	r = qfile_open(FILENAME2, QFILE_MODE_READ);
	assert(r != Q_ERROR);
	
	walk_area = qwalk_area_read();
	assert(walk_area != NULL);

	r = qfile_close();
	assert(r != Q_ERROR);

	
	Qdatameta_t *area_datameta;
	area_datameta = qdatameta_create((Qdata_t *) walk_area, QDATA_TYPE_QWALK_AREA, (size_t) 1);
	assert(area_datameta != NULL);
	walk_area = NULL;
	
	WINDOW *r_win;
	/* ncurses startup*/
	r_win = initscr();
	assert(r_win != NULL);
	r = noecho();
	assert(r != ERR);
	r = curs_set(0);
	assert(r != ERR);
	r = cbreak();
	assert(r != ERR);


	r = qwalk_init(area_datameta);
	assert(r != Q_ERROR);
	r = qwalk_io_init(stdscr);
	
	ModeSwitchData_t *switch_data;
	switch_data = calloc((size_t) 1, sizeof(*switch_data));

	assert(switch_data != NULL);
	
	switch_data->mode = MODE_T_INIT; 
	switch_data->datameta = NULL;

	for (int i = 0; i < 100; i++) {
		assert(switch_data != NULL);
		r = qwalk_tick(switch_data);
		assert(r != Q_ERROR);
	}

	/* TODO: implement a destructor function for ModeSwitchData_t */
	/*@i1@*/free(switch_data);
	
	free(intstr2);

	r = qdatameta_destroy(datameta);
	assert(r != Q_ERROR);
	datameta = NULL;
	r = endwin();
	assert(r != Q_ERROR);
	/*@i1@*/return 0;
}
