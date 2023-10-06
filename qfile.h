/**
 * @file qfile.h
 * Header file for the file module of Q.
 */



/**
 * Type of a q file.
 * i.e., what module it belongs to and the format of its data.
 */
typedef enum QfileType_t {
	QFILE_TYPE_WALK_RAW = 1, /**< Raw qwalk file type */
	QFILE_TYPE_TALK_RAW,     /**< Raw qtalk file type */
	QFILE_TYPE_CLI_RAW,      /**< Raw qcli  file type */
	QFILE_TYPE_SAIL_RAW,     /**< Raw qsail file type */
  /**
	 * Number of possible #QfileType_t.
	 * Must be defined via the final proper enum constant
	 */
	QFILE_TYPE_COUNT = QFILE_TYPE_SAIL_RAW
}

/** Open a file for qfile */
extern int QfileOpen(FILE*);

/** Close a file for qfile */
extern int QfileClose(FILE*);

/** Write a #Qdata_t to a file  */
extern int QfileQdataWrite(Qdatameta_t*, QdataType_t);

/** Read a #Qdata_t from a file */
extern QData_t* QfileQdataRead(QdataType_t);
