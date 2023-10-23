/**
 * @file qfile.h
 * Header file for the file module of Q.
 */




#define QFILE_SAVES_DIR "saves"     /**< Directory in which to store saves */
#define QFILE_WALK_NAME "qwalk.dat" /**< Filename for qwalk save data      */
#define QFILE_TALK_NAME "qtalk.dat" /**< Filename for qtalk save data      */
#define QFILE_CLI_NAME  "qcli.dat"  /**< Filename for qcli  save data      */
#define QFILE_SAIL_NAME "qsail.dat" /**< Filename for qsail save data      */



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

/** Open a file for qfile.            */
extern           int          qfile_open(char*, QfileMode_t);

/** Close a file for qfile.           */
extern           int          qfile_close(void);

/** Write a #Qdatameta_t to a file.   */
extern           int          qfile_qdatameta_write(const Qdatameta_t *);

/** Write an @c int to storage.       */
/*@unused@*/extern           int      qfile_int_write(int);

/** Write a @c size_t to storage.     */
extern           int          qfile_size_write(size_t);

/** Write a #QdataType_t to storage.  */
/*@unused@*/extern           int          qfile_qdata_type_write(QdataType_t);

/** Write a #QattrKey_t to storage.   */
extern           int          qfile_qattr_key_write(QattrKey_t);


/** Read a #Qdatameta_t from a file.  */
extern /*@null@*/Qdatameta_t *qfile_qdatameta_read(void);

/** Read an @c int from storage.      */
/*@unused@*/extern           int         qfile_int_read(void);

/** Read a @c size_t from storage.    */
extern           size_t      qfile_size_read(void);

/** Read a #QdataType_t from storage. */
/*@unused@*/extern           QdataType_t qfile_qdata_type_read(void);

/** Read a #QattrKey_t from storage.  */
extern           QattrKey_t  qfile_qattr_key_read(void);

/** Read a #Qdata_t from a file.      */
extern /*@null@*/Qdata_t    *qfile_qdata_read(QdataType_t, size_t);
