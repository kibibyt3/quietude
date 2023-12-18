/**
 * @file qfile.c
 * Program file for the file module of Q.
 * Responsible for reading and writing binary information to files. This module
 * keeps the current file for I/O operations at a global scope; this avoids the
 * need to open & close a file every time a lone datam needs to be written.
 */



#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "qdefs.h"
#include "qerror.h"

#include "qattr.h"
#include "item.h"
#include "qfile.h"



/**
 * Pointer to the current active file.
 * Modify via qfile_open() and qfile_close().
 */
/*@null@*/static FILE *qfile_ptr;

/** Current #QfileMode_t */
static QfileMode_t qfile_mode = QFILE_MODE_INACTIVE;



/**
 * Opens a file for future access by the qfile module.
 * @param[in] filename: name of the file to be kept open.
 * @param[in] mode: #QfileMode_t to open @p filename in.
 * @return #Q_OK or #Q_ERROR.
 */
int
qfile_open(const char *filename, QfileMode_t mode) {
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
 * #Qdatameta_t.count is also written.
 * @param[in] datameta: pointer to the #Qdatameta_t to be written.
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
	
	if (qfile_size_write(datameta->count) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	if (qfile_qdata_type_write(datameta->type) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	data_written_count = fwrite((void *) (datameta->datap),
			datameta_data_type_size,
			datameta->count,
			qfile_ptr);

	if (data_written_count != datameta->count) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	return Q_OK;
}


/**
 * Write an @c int to the file open in qfile.
 * @param[in] i: @c int to write.
 * @return #Q_OK or #Q_ERROR.
 */
int
qfile_int_write(int i) {
	size_t data_written_count;

	if (qfile_ptr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if (qfile_mode != QFILE_MODE_WRITE) {
		Q_ERRORFOUND(QERROR_FILE_MODE);
		return Q_ERROR;
	}

	data_written_count = fwrite((void *) &i, sizeof(i), (size_t) 1, qfile_ptr);
	if (data_written_count < (size_t) 1) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}
	
	return Q_OK;
}


/**
 * Write a @c size_t to the file open in qfile.
 * @return #Q_OK or #Q_ERROR.
 */
int
qfile_size_write(size_t size) {
	size_t data_written_count;

	if (qfile_ptr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if (qfile_mode != QFILE_MODE_WRITE) {
		Q_ERRORFOUND(QERROR_FILE_MODE);
		return Q_ERROR;
	}

	data_written_count = fwrite((void *) &size, sizeof(size), (size_t) 1, qfile_ptr);
	if (data_written_count < (size_t) 1) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}
	
	return Q_OK;
}


/**
 * Write a #QdataType_t to the file open in qfile.
 * @return #Q_OK or #Q_ERROR.
 */
int
qfile_qdata_type_write(QdataType_t type) {
	size_t data_written_count;

	if (qfile_ptr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if (qfile_mode != QFILE_MODE_WRITE) {
		Q_ERRORFOUND(QERROR_FILE_MODE);
		return Q_ERROR;
	}

	data_written_count = fwrite((void *) &type, sizeof(type), (size_t) 1, qfile_ptr);
	if (data_written_count < (size_t) 1) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}
	
	return Q_OK;
}


/**
 * Write a #QattrKey_t to the file open in qfile.
 * @return #Q_OK or #Q_ERROR.
 */
int
qfile_qattr_key_write(QattrKey_t attr_key) {

	size_t data_written_count;

	if (qfile_ptr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if (qfile_mode != QFILE_MODE_WRITE) {
		Q_ERRORFOUND(QERROR_FILE_MODE);
		return Q_ERROR;
	}

	data_written_count = fwrite((void *) &attr_key, sizeof(attr_key), (size_t) 1, qfile_ptr);
	if (data_written_count < (size_t) 1) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}
	
	return Q_OK;
}


/**
 * Write an #ItemID_t to storage.
 * @param[in] id: #ItemID_t to write.
 * @return #Q_OK or #Q_ERROR.
 */
int
qfile_item_id_write(ItemID_t id) {

	if (qfile_ptr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if (qfile_mode != QFILE_MODE_WRITE) {
		Q_ERRORFOUND(QERROR_FILE_MODE);
		return Q_ERROR;
	}

	if (fwrite((void *) &id, sizeof(id), (size_t) 1, qfile_ptr) < (size_t) 1) {
		Q_ERROR_SYSTEM("fwrite()");
		return Q_ERROR;
	}

	return Q_OK;
}


/**
 * Read a #Qdatameta_t from the file open in qfile.
 * @return new #Qdatameta_t.
 */
Qdatameta_t *
qfile_qdatameta_read() {
	Qdatameta_t *datameta;
	size_t count;
	QdataType_t type;
	Qdata_t *datap;

	if (qfile_ptr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}
	if (qfile_mode != QFILE_MODE_READ) {
		Q_ERRORFOUND(QERROR_FILE_MODE);
		return NULL;
	}

	count = qfile_size_read();
	if (count == (size_t) Q_ERRORCODE_SIZE) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	type = qfile_qdata_type_read();
	if (type == (QdataType_t) Q_ERRORCODE_ENUM) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}
	if ((type < (QdataType_t) Q_ENUM_VALUE_START) || (type > QDATA_TYPE_COUNT)) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return NULL;
	}

	datap = qfile_qdata_read(type, count);
	if (datap == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}

	datameta = qdatameta_create(datap, type, count);
	return datameta;
}


/**
 * Read an @c int from the file open in qfile.
 * @return newly-read @c int.
 */
int
qfile_int_read() {
	size_t data_read_count;
	int i;

	if (qfile_ptr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}
	if (qfile_mode != QFILE_MODE_READ) {
		Q_ERRORFOUND(QERROR_FILE_MODE);
		abort();
	}

	data_read_count = fread((void *) &i, sizeof(i), (size_t) 1, qfile_ptr);
	if (data_read_count < (size_t) 1) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}
	
	return i;
}


/**
 * Read a @c size_t from the file open in qfile.
 * @return newly-read @c size_t.
 */
size_t
qfile_size_read() {
	size_t data_read_count;
	size_t size;

	if (qfile_ptr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return (size_t) Q_ERRORCODE_SIZE;
	}
	if (qfile_mode != QFILE_MODE_READ) {
		Q_ERRORFOUND(QERROR_FILE_MODE);
		return (size_t) Q_ERRORCODE_SIZE;
	}

	data_read_count = fread((void *) &size, sizeof(size), (size_t) 1, qfile_ptr);
	if (data_read_count < (size_t) 1) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return (size_t) Q_ERRORCODE_SIZE;
	}
	
	return size;
}


/**
 * Read a #QdataType_t from the file open in qfile.
 * @return newly-read #QdataType_t or #Q_ERRORCODE_ENUM.
 */
QdataType_t
qfile_qdata_type_read() {
	size_t data_read_count;
	QdataType_t type;

	if (qfile_ptr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return (QdataType_t) Q_ERRORCODE_ENUM;
	}
	if (qfile_mode != QFILE_MODE_READ) {
		Q_ERRORFOUND(QERROR_FILE_MODE);
		return (QdataType_t) Q_ERRORCODE_ENUM;
	}

	data_read_count = fread((void *) &type, sizeof(type), (size_t) 1, qfile_ptr);
	if (data_read_count < (size_t) 1) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return (QdataType_t) Q_ERRORCODE_ENUM;
	}
	
	return type;
}


/**
 * Read a #QattrKey_t from the file open in qfile.
 * @return newly-read #QattrKey_t or #Q_ERRORCODE_ENUM.
 */
QattrKey_t
qfile_qattr_key_read() {
	size_t data_read_count;
	QattrKey_t attr_key;

	if (qfile_ptr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return (QattrKey_t) Q_ERRORCODE_ENUM;
	}
	if (qfile_mode != QFILE_MODE_READ) {
		Q_ERRORFOUND(QERROR_FILE_MODE);
		return (QattrKey_t) Q_ERRORCODE_ENUM;
	}

	data_read_count = fread((void *) &attr_key, sizeof(attr_key), (size_t) 1, qfile_ptr);
	if (data_read_count < (size_t) 1) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return (QattrKey_t) Q_ERRORCODE_ENUM;
	}
	
	return attr_key;
}


/**
 * Read an #ItemID_t from storage.
 * @return newly-read #ItemID_t or #Q_ERRORCODE_ENUM.
 */
ItemID_t
qfile_item_id_read() {

	ItemID_t id;

	if (qfile_ptr == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return (ItemID_t) Q_ERRORCODE_ENUM;
	}
	if (qfile_mode != QFILE_MODE_READ) {
		Q_ERRORFOUND(QERROR_FILE_MODE);
		return (ItemID_t) Q_ERRORCODE_ENUM;
	}

	if (fread((void *) &id, sizeof(id), (size_t) 1, qfile_ptr) < (size_t) 1) {
		Q_ERROR_SYSTEM("fread()");
		return (ItemID_t) Q_ERRORCODE_ENUM;
	}

	return id;
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
