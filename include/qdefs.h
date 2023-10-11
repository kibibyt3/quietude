/**
 * @file qdefs.h
 * Header file for game-wide definitions. Depends on stdint.h.
 */



#define Q_OK     0 /**< General OK return code for Q */
#define Q_ERROR -1 /**< General ERROR return code for Q */

/** Returned by functions that return @c size_t if an error occurs */
#define Q_ERRORCODE_SIZE 0

/** Returned by functions that return an internal enum if an error occurs */
#define Q_ERRORCODE_ENUM -1

/**
 * The value of the first enum constant in an enum declaration.
 * This is to facilitate checking for undefined enums, since they will default
 * to 0 upon initialization.
 */
#define Q_ENUM_VALUE_START  1

/**
 * Type for representing data types.
 * Used primarily for error-checking and casting when data type is unknown.
 */
typedef enum QdataType_t {
	/** int type */
	QDATA_TYPE_INT = Q_ENUM_VALUE_START,
	QDATA_TYPE_FLOAT,   /**< float type */
	QDATA_TYPE_STRING,  /**< int* type (when used to represent characters) */

	/**
	 * Total enumerated #QdataType_t values.
	 * Must be defined via the final proper enum.
	 */
	QDATA_TYPE_COUNT = QDATA_TYPE_STRING
} QdataType_t;

/** 
 * Type for holding arbitrary amounts of data to pass between modules. Meant to
 * be used in a pointer; it should never be called in isolation prior to being
 * cast to a proper type.
 */
typedef void Qdata_t;

/** Type for holding #Qdata_t and its size */
typedef struct Qdatameta_t {
	
	/** Pointer to the beginning of the data compoment of the #Qdatameta_t */
	/*@owned@*/Qdata_t    *datap; 
	
	/**
	 * Number of elements in data.
	 * Used when @c datap is cast back to its proper @c type.
	 */
	size_t      count; 
	
	QdataType_t type;    /**< Type of the data. */
} Qdatameta_t;




/** Create a #Qdatameta_t    */
extern /*@null@*//*@partial@*/Qdatameta_t *qdatameta_create(QdataType_t, size_t);

/** Destroy a #Qdatameta_t   */
extern void qdatameta_destroy(/*@only@*/Qdatameta_t *);

/** Get the datap member from a #Qdatameta_t */
extern /*@observer@*//*@null@*/Qdata_t *qdatameta_datap_get(const Qdatameta_t *);

/** Get the count member from a #Qdatameta_t */
extern size_t qdatameta_count_get(const Qdatameta_t *);

/** Get the type member from a #Qdatameta_t  */
extern QdataType_t qdatameta_type_get(const Qdatameta_t *);

/** Get the size of a #QdataType_t */
extern size_t qdata_type_size_get(QdataType_t);
