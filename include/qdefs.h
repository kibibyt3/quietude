/**
 * @file qdefs.h
 * Header file for game-wide definitions. Depends on stdint.h and stdbool.h.
 */


/** `char *` version of `true`. */
#define BOOL_STRING_TRUE  "true"
/** `char *` version of `false`. */
#define BOOL_STRING_FALSE "false"


#define Q_OK     0          /**< Generic OK return code for Q    */
#define Q_ERROR -1          /**< Generic ERROR return code for Q */



/**
 * @defgroup ExpectedErrors Expected errors
 * Errors that are expected to come up in normal operation; that is to say,
 * the returning of these errorcodes is intentional and should not be considered
 * buggy behaviour. They <i>may</i> nonetheless be checked for in the event a
 * circumstance should not happen, but the returning of these should, for
 * instance, never invoke #Q_ERRORFOUND().
 */

/**
 * @ingroup ExpectedErrors
 * Expected error code for functions that attempt and fail to modify something.
 */
#define Q_ERROR_NOCHANGE -2

/**
 * @ingroup ExpectedErrors
 * Not found expected error for functions that search for an @c int.
 */
#define Q_ERRORCODE_INT_NOTFOUND -2



/** Generic error for functions that otherwise return a meaningful @c int. */
#define Q_ERRORCODE_INT -1

/** Generic error for functions that otherwise return a meaningful @c char. */
#define Q_ERRORCODE_CHAR '\0'

/** Generic error for functions that otherwise return a meaningful @c long. */
#define Q_ERRORCODE_LONG -1L

/** Returned by functions that return @c size_t if an error occurs. */
#define Q_ERRORCODE_SIZE 0

/** Returned by functions that return `char *` if an error occurs. */
#define Q_ERRORCODE_CHARSTRING "ERROR"

/** Returned by functions that return `int *` if an error occurs.  */
#define Q_ERRORCODE_INTSTRING  "ERROR"


/**
 * Returned by functions that return an internal enum if an error occurs.
 * Expected to be casted to the typedeffed enum declared to be returned by the
 * called function.
 */
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
	QDATA_TYPE_BOOL,                     /**< @c bool type  */

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
	QDATA_TYPE_QWALK_AREA,   /**< #QwalkArea_t     */
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
	QOBJ_TYPE_PLAYER = Q_ENUM_VALUE_START,

	QOBJ_TYPE_GRASS, /**< A patch of grass */
	QOBJ_TYPE_TREE,  /**< A tree           */
	
	QOBJ_TYPE_VOID,  /**< Type for entirely empty space */

	QOBJ_TYPE_NPC_FRIENDLY, /**< Type for friendly (non-aggressive) NPCs. */

	/**
	 * Number of available object types
	 * Must be defined via the final proper enum constant
	 */
	QOBJ_TYPE_COUNT = QOBJ_TYPE_VOID

} QobjType_t;

/**
 * @defgroup ObjTypeStrings QobjType_t Strings
 * String versions of #QobjType_t constants.
 * @{
 */

/** String version of #QOBJ_TYPE_PLAYER. */
#define QOBJ_STRING_TYPE_PLAYER "player"
/** String version of #QOBJ_TYPE_GRASS. */
#define QOBJ_STRING_TYPE_GRASS  "grass"
/** String version of #QOBJ_TYPE_TREE. */
#define QOBJ_STRING_TYPE_TREE   "tree"
/** String version of #QOBJ_TYPE_VOID. */
#define QOBJ_STRING_TYPE_VOID   "void"
/** String version of #QOBJ_TYPE_NPC_FRIENDLY. */
#define QOBJ_STRING_TYPE_NPC_FRIENDLY "person"

/** @} */



/** 
 * Type for holding arbitrary amounts of data to pass between modules. Meant to
 * be used in a pointer; it should never be called in isolation prior to being
 * cast to a proper type.
 */
typedef void Qdata_t;




/** Type for holding #Qdata_t and its size */
typedef struct Qdatameta_t {
	
	/** Pointer to the data compoment of the #Qdatameta_t */
	/*@only@*/Qdata_t    *datap; 
	
	/**
	 * Number of elements in data.
	 * Used when @c datap is cast back to its proper @c type.
	 */
	size_t      count; 
	
	QdataType_t type;    /**< Type of the data. */
} Qdatameta_t;




/** Create a #Qdatameta_t.                    */
extern /*@null@*//*@only@*/Qdatameta_t *qdatameta_create(/*@keep@*//*@returned@*/Qdata_t *, QdataType_t, size_t);

/** Clone a #Qdatameta_t.                     */
extern /*@null@*//*@only@*/Qdatameta_t *qdatameta_clone(const Qdatameta_t *);

/** Destroy a #Qdatameta_t.                   */
extern int qdatameta_destroy(/*@only@*/Qdatameta_t *);

/** Get the datap member from a #Qdatameta_t. */
extern /*@observer@*//*@null@*/Qdata_t *qdatameta_datap_get(const Qdatameta_t *)/*@*/;

/** Get the count member from a #Qdatameta_t. */
extern size_t qdatameta_count_get(const Qdatameta_t *)/*@*/;

/** Get the type member from a #Qdatameta_t.  */
extern QdataType_t qdatameta_type_get(const Qdatameta_t *)/*@*/;

/** Get the size of a #QdataType_t.           */
extern size_t qdata_type_size_get(QdataType_t)/*@*/;

/** Convert a #QobjType_t to a string.        */
/*@unused@*/extern /*@observer@*/char *qobj_type_to_string(QobjType_t)/*@*/;

/** Convert a string to a #QobjType_t.        */
/*@unused@*/extern QobjType_t qobj_string_to_type(const char *)/*@*/;

/** Convert a @c bool to a string.            */
/*@unused@*/extern /*@observer@*/char *bool_to_string(bool)/*@*/;

/** Convert a string to a @c bool.            */
/*@unused@*/extern bool charstring_to_bool(/*@partial@*/const char *)/*@*/;
