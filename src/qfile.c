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

#include "qfile.h"

/**
 * Pointer to the current active file.
 * Modify via #QfileOpen() and #QfileClose().
 */
/*@null@*/static FILE *qfile_ptr;

/** Current #QfileMode_t */
static QfileMode_t qfile_mode = QFILE_MODE_INACTIVE;



/**
 * Opens a file for future access by the qfile module.
 * @param[in] filename: @c char* that the name of the file to be kept open
 * @param[in] mode: #QfileMode_t to open @c filename in
 * @return #Q_OK or #Q_ERROR
 */
int
qfile_open(char *filename, QfileMode_t mode) {
	if (qfile_mode != QFILE_MODE_INACTIVE) {
		return Q_ERROR;
	}
	if (mode == QFILE_MODE_WRITE) {
		qfile_ptr  = fopen(filename, "w");
		qfile_mode = QFILE_MODE_WRITE;
	} else if (mode == QFILE_MODE_READ) {
		qfile_ptr = fopen(filename, "r");
		qfile_mode = QFILE_MODE_READ;
	} else {
		return Q_ERROR;
	}
	if (qfile_ptr == NULL){
		qfile_mode = QFILE_MODE_INACTIVE;
		return Q_ERROR;
	}
	return Q_OK;
}

/**
 * Closes the file opened in the qfile module.
 * @return #Q_OK or #Q_ERROR
 */
int
qfile_close() {
	if (qfile_ptr == NULL) {
		return Q_ERROR;
	}
	if (qfile_mode == QFILE_MODE_INACTIVE) {
		return Q_ERROR;
	}
	if (fclose(qfile_ptr) == EOF) {
		return Q_ERROR;
	}
	qfile_mode = QFILE_MODE_INACTIVE;
	return Q_OK;
}

/**
 * Write a #Qdatameta_t to the file open in qfile.
 * @param[in] datameta: pointer to the #Qdatameta_t to be written
 * @return #Q_OK or #Q_ERROR
 */
int
qfile_qdatameta_write(Qdatameta_t *datameta) {
	size_t datameta_data_type_size;
	size_t data_written_count;
	
	if (qfile_ptr == NULL) {
		return Q_ERROR;
	}
	if (qfile_mode != QFILE_MODE_WRITE) {
		return Q_ERROR;
	}
	
	if ((datameta_data_type_size = qdata_type_size_get(datameta->type))
				== Q_DEFAULT_TYPE_SIZE) {
		return Q_ERROR;
	}
	
	data_written_count = fwrite((void *) (datameta->datap),
			datameta_data_type_size,
			datameta->count,
			qfile_ptr);

	if (data_written_count < datameta->count) {
		return Q_ERROR;
	}
	return Q_OK;
}

/**
 * Read to a #Qdatameta_t from the file open in qfile.
 * @param[out] datameta: pointer to the #Qdatameta_t to be read to.
 * Its members must be set in advance to allow @c fread() to parse the data.
 * @return #Q_OK or #Q_ERROR
 */
int
qfile_qdatameta_read(Qdatameta_t *datameta) {
	size_t data_read_count;
	size_t datameta_data_type_size;
	
	if (qfile_ptr == NULL) {
		return Q_ERROR;
	}
	
	if (qfile_mode != QFILE_MODE_READ) {
		return Q_ERROR;
	}

	if ((datameta_data_type_size = qdata_type_size_get(datameta->type))
				== Q_DEFAULT_TYPE_SIZE) {
		return Q_ERROR;
	}

	data_read_count = fread((void *) (datameta->datap),
			datameta_data_type_size,
			datameta->count,
			qfile_ptr);
	
	if (data_read_count < datameta->count) {
		return Q_ERROR;
	}
	return Q_OK;
}
