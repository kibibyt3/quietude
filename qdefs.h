/**
 * @file qdefs.h
 * Header file for game-wide definitions. Depends on stdint.h.
 */



#define Q_OK     0 /**< General OK return code for Q */
#define Q_ERROR -1 /**< General ERROR return code for Q */

/**
 * Type for representing data types.
 * Used primarily for error-checking and casting when data type is unknown.
 */
typedef enum QdataType_t {
	QDATA_TYPE_INT = 1, /**< int type   */
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
	Qdata_t    *datap; 
	
	/**
	 * Number of elements in data.
	 * Used when @c datap is cast back to its proper @c type.
	 */
	size_t      count; 
	
	QdataType_t type; /**< Type of the data. */
} Qdatameta_t;

/* Create a datameta */
Qdatameta_t *qdatameta_create(Qdata_t*, QdataType_t, size_t);
