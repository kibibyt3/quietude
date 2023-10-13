/**
 * @file qdefs.h
 * Header file for game-wide definitions. Depends on stdint.h.
 */




#define Q_OK     0          /**< Generic OK return code for Q    */
#define Q_ERROR -1          /**< Generic ERROR return code for Q */

/** Generic error for functions that otherwise return a meaningful @c int */
#define Q_ERRORCODE_INT -1

/**
 * Not found error for functions that search for an @c int
 * Namely in the event that the value cannot be found. This error should be
 * handled gracefully; it's an expected and normal part of Q and is therefore
 * expected to be non-fatal. If encountering this value is indicative of a fatal
 * error, the onus is on the caller to deal with this, as this code is
 * especially used for generic utility functions. This does furthermore mean
 * that @c qerror functions and macros <i>should not</i> be called in tandem
 * with returning this value!
 */
#define Q_ERRORCODE_INT_NOTFOUND -2

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
	
	/* standard types */
	QDATA_TYPE_INT = Q_ENUM_VALUE_START, /**< @c int type   */
	QDATA_TYPE_FLOAT,                    /**< @c float type */
	
	/** `int *` type (when representing characters)
	 *  Usage of this type should be strictly external; inside-facing character
	 *  representations should use `char *` instead to save memory usage.
	 *  Outside-facing types use @c int to make full use of wide characters.
	 */
	QDATA_TYPE_INT_STRING,  
	
	/**
	 * `unsigned char *` type (when representing characters)
	 * Usage of this type should be strictly internal; outside-facing character
	 * representations should use #IOCh_t whose primitive is `int`.
	 */
	QDATA_TYPE_CHAR_STRING,
	
	/* typedeffed types */
	QDATA_TYPE_QOBJECT_TYPE, /**< #QobjType_t type */

	/**
	 * Total enumerated #QdataType_t values.
	 * Must be defined via the final proper enum.
	 */
	QDATA_TYPE_COUNT = QDATA_TYPE_QOBJECT_TYPE
} QdataType_t;




/**
 * Type ID for an object belonging to any Q module
 */
typedef enum QobjType_t {

	/** Player type */
	Q_OBJ_TYPE_PLAYER = Q_ENUM_VALUE_START,

	Q_OBJ_TYPE_TREE,  /**< Tree type */
	
	/**
	 * Empty type
	 * Default type for an object that lacks any specificity. Treated in-game as
	 * an empty space that is readily traversed, like a patch of short grass.
	 */
	Q_OBJ_TYPE_EMPTY,

	/**
	 * Number of available object types
	 * Must be defined via the final proper enum constant
	 */
	Q_OBJ_TYPE_COUNT = Q_OBJ_TYPE_EMPTY

} QobjType_t;




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
extern /*@observer@*//*@null@*/Qdata_t *qdatameta_datap_get(const Qdatameta_t *)/*@*/;

/** Get the count member from a #Qdatameta_t */
extern size_t qdatameta_count_get(const Qdatameta_t *)/*@*/;

/** Get the type member from a #Qdatameta_t  */
extern QdataType_t qdatameta_type_get(const Qdatameta_t *)/*@*/;

/** Get the size of a #QdataType_t */
extern size_t qdata_type_size_get(QdataType_t)/*@*/;
