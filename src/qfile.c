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
#include "qerror.h"

#include "qfile.h"

/**
 * Pointer to the current active file.
 * Modify via #qfile_open() and #qfile_close().
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
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERROR;
	}
	if (mode == QFILE_MODE_WRITE) {
		qfile_ptr  = fopen(filename, "w");
		qfile_mode = QFILE_MODE_WRITE;
	} else if (mode == QFILE_MODE_READ) {
		qfile_ptr = fopen(filename, "r");
		qfile_mode = QFILE_MODE_READ;
	} else{
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}
	if (qfile_ptr == NULL){
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
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
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if (qfile_mode == QFILE_MODE_INACTIVE) {
		Q_ERRORFOUND(QERROR_FILE_MODE);
		return Q_ERROR;
	}
	if (fclose(qfile_ptr) == EOF) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
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
qfile_qdatameta_write(const Qdatameta_t *datameta) {
	size_t datameta_data_type_size;
	size_t data_written_count;
	
	if (qfile_ptr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if (qfile_mode != QFILE_MODE_WRITE) {
		Q_ERRORFOUND(QERROR_FILE_MODE);
		return Q_ERROR;
	}
	
	if ((datameta_data_type_size = qdata_type_size_get(datameta->type))
				== Q_ERRORCODE_SIZE) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}
	
	data_written_count = fwrite((void *) (datameta->datap),
			datameta_data_type_size,
			datameta->count,
			qfile_ptr);

	if (data_written_count < datameta->count) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}
	return Q_OK;
}

/**
 * Read to a #Qdata_t * from the file open in qfile.
 * @param[in] type: type of #Qdata_t.
 * @param[in] count: amount of elements.
 * @return #Q_OK or #Q_ERROR.
 */
Qdata_t *
qfile_qdata_read(QdataType_t type, size_t count) {
	size_t data_read_count;
	size_t data_type_size;
	
	Qdata_t *data;
	if (qfile_ptr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	
	if (qfile_mode != QFILE_MODE_READ) {
		Q_ERRORFOUND(QERROR_FILE_MODE);
		return NULL;
	}

	if ((data_type_size = qdata_type_size_get(type))
				== Q_ERRORCODE_SIZE) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	data = calloc((size_t) count, data_type_size);
	if (data == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}

	data_read_count = fread((void *) (data), data_type_size, count, qfile_ptr);
	
	if (data_read_count < count) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		free(data);
		return NULL;
	}

	/*@i1@*/return data;
}
