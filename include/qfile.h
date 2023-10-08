/**
 * @file qfile.h
 * Header file for the file module of Q.
 */



/**
 * Type of a q file.
 * i.e., what module it belongs to and the format of its data.
 */
typedef enum QfileType_t {
	/** Raw qwalk file type */
	QFILE_TYPE_WALK_RAW = Q_ENUM_VALUE_START,
	QFILE_TYPE_TALK_RAW,     /**< Raw qtalk file type */
	QFILE_TYPE_CLI_RAW,      /**< Raw qcli  file type */
	QFILE_TYPE_SAIL_RAW,     /**< Raw qsail file type */
  /**
	 * Number of possible #QfileType_t.
	 * Must be defined via the final proper enum constant
	 */
	QFILE_TYPE_COUNT = QFILE_TYPE_SAIL_RAW
} QfileType_t;

/**
 * Mode of a qfile
 */
typedef enum QfileMode_t {
	/** Read mode  */
	QFILE_MODE_READ = Q_ENUM_VALUE_START, 
	QFILE_MODE_WRITE,    /**< Write mode */
	QFILE_MODE_INACTIVE, /**< File isn't open */

	/**
	 * Number of modes.
	 * Must be defined by final enum constant.
	 */
	QFILE_MODE_COUNT = QFILE_MODE_WRITE
} QfileMode_t;

/** Open a file for qfile */
/*@external@*/
extern int qfile_open(char*, QfileMode_t);

/** Close a file for qfile */
/*@external@*/
extern int qfile_close(void);

/** Write a #Qdata_t to a file  */
/*@external@*/
extern int qfile_qdatameta_write(Qdatameta_t*);

/** Read a #Qdata_t from a file */
/*@external@*/
extern int qfile_qdatameta_read(Qdatameta_t*);
