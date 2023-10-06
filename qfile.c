/**
 * @file qfile.c
 * Program file for the file module of Q.
 * Responsible for directly reading and writing to files. This module keeps the
 * current file for I/O operations at a global scope; this avoids the need to
 * open & close a file every time a lone datam needs to be written.
 */



#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "qdefs.h"



/**
 * Pointer to the current active file.
 * Modify via #QfileOpen() and #QfileClose().
 */
static FILE *qfile_ptr;

/** Indicates whether qfile_ptr is currently active. */
static bool qfile_isactive = false;



/**
 * Opens a file for future access by the qfile module.
 * @param[in] filename: the name of the file to be kept open
 * @return #Q_OK or #Q_ERROR
 */
int
QfileOpen(int *filename) {
	if (!qfile_isactive){
		return Q_ERROR;
	}
	qfile_ptr = fopen((char *) filename);
	if (qfile_ptr == NULL){
		return Q_ERROR;
	}
	qfile_isactive = true;
	return Q_OK;
}

/**
 * Closes the file opened in the qfile module.
 * @param[out] file: the pointer to the @c FILE to be closed.
 * @return #Q_OK or #Q_ERROR
 */
int
QfileClose(FILE *file) {
	if (qfile_isactive) {
		return Q_ERROR;
	}
	if (fclose(qfile_ptr) == EOF){
		return Q_ERROR;
	}
	qfile_isactive = false;
	return Q_OK;
}

/**
 * Write a #Qdatameta_t to the file open in qfile.
 * @param[in] data_meta: pointer to the #Qdatameta_t to be written
 * @return #Q_OK or #Q_ERROR
 */
int
QfileQdatametaWrite(Qdatameta_t *data_meta) {
	size_t datameta_data_type_size;
	size_t data_written_count;
	if (!qfile_isactive) {
		return Q_ERROR;
	}
	
	if ((datameta_data_type_size = qdata_type_size_get(datameta->type)
				== Q_DEFAULT_TYPE_SIZE)) {
		return Q_ERROR;
	}
	
	data_written_count = fwrite((void *) (data_meta->datap),
			datameta_data_type_size,
			data_meta->count,
			qfile_ptr);
	
	if (data_written_count < data_meta->count) {
		return Q_ERROR;
	}
	return Q_OK;
}

/**
 * Read to a #Qdatameta_t from the file open in qfile.
 * @param[out] pointer to the #Qdatameta_t to be read to
 * @return #Q_OK or #Q_ERROR
 */
int
QfileQdataRead(Qdatameta_t *data_meta) {
	size_t data_read_count;
	size_t datameta_data_type_size;
	if (!qfile_isactive) {
		return Q_ERROR;
	}

	if ((datameta_data_type_size = qdata_type_size_get(datameta->type)
				== Q_DEFAULT_TYPE_SIZE)) {
		return Q_ERROR;
	}

	data_read_count = fread((void *) (data_meta->datap),
			datameta_data_type_size,
			data_meta->count,
			qfile_ptr);
	
	if (data_read_count < data_meta->count) {
		return Q_ERROR;
	}
	return Q_OK;
}
