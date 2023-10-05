/**
 * @file qfile.c
 * Program file for the file module of Q. Responsible for directly reading and
 * writing to files.
 */



#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "qdefs.h"



/**
 * Pointer to the current active file.
 * Modify via #QfileOpen() and #QfileClose().
 */
static QFile_t *qfile_ptr;

/** Indicates whether qfile_ptr is currently active. */
static bool qfile_isactive = false;



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
	return Q_SUCCESS;

}

int
QfileClose(FILE *file) {
	
	if (qfile_isactive) {
		return Q_ERROR;
	}
	if (fclose(qfile_ptr) == EOF){
		return Q_ERROR;
	}
	qfile_isactive = false;
	return Q_SUCCESS;

}

int
QfileQdataWrite(Qdata_t *data, QdataType_t data_type) {
	
	if (!qfile_isactive) {
		return Q_ERROR;
	}


}

Qdata_t*
QfileQdataRead(QdataType_t data_type) {
	
}
