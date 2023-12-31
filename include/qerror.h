/**
 * @file qerror.h
 * Header file for qerror.
 * Depends on stdio.h and qdefs.h.
 */



/**
 * Print a #Qerror_t and its info to @c stderr if an expression is @c true.
 * @param[in] check: the expression whose truth is to be checked
 * @param[in] error: the error whose corresponding message should be printed to
 * @c stdout.
 */
#define Q_IFERROR(check, error) \
	if ((check)) {qerror_internal((Qerror_t) (error), __FILE__, __func__, __LINE__);}

/**
 * Print a #Qerror_t and its info to @c stderr.
 * @param[in] error: the error whose corresponding message should be printed to
 * @c stdout.
 */
#define Q_ERRORFOUND(error) \
	qerror_internal((error), __FILE__, __func__, __LINE__)

/**
 * Print a system error and its info to @c stderr.
 * @param[in] func_name: name of current function.
 */
#define Q_ERROR_SYSTEM(func_name) \
	{perror(func_name); \
		qerror_internal((QERROR_SYSTEM), __FILE__, __func__, __LINE__);}

/**
 * Print a stringified argument.
 * @param[in] arg: argument to stringify.
 */
#define IF_STRINGIFY(arg) \
	if ((arg)) {fprintf(stderr, "%s", (#arg));}



/**
 * Internal error for Q.
 */
typedef enum Qerror_t {
	/** Error for unexpected @c NULL pointer.                                 */
	QERROR_NULL_POINTER_UNEXPECTED = Q_ENUM_VALUE_START,
	
	/** Error for unexpected non-`NULL` pointer.                              */
	QERROR_NONNULL_POINTER_UNEXPECTED,

	/** Error for unexpected @c NULL value.                                   */
	QERROR_NULL_VALUE_UNEXPECTED,

	/** Error for a system memory function failing.                           */
	QERROR_SYSTEM_MEMORY,

	/** Error for an unexpected zero value.                                   */
	QERROR_ZERO_VALUE_UNEXPECTED,

	/** Error for an unexpected negative value.                               */
	QERROR_NEGATIVE_VALUE_UNEXPECTED,

	/** Error for a `define` const whose constraints are not met.             */
	QERROR_BADDEFINE,

	/** Error for invalid @c enum constant.                                   */
	QERROR_ENUM_CONSTANT_INVALID,

	/** Error for an @c enum constant set to zero.                            */
	QERROR_ENUM_CONSTANT_INVALID_ZERO,

	/** Error for a @c struct not entirely filled (namely when it should be). */
	QERROR_STRUCT_INCOMPLETE,

	/** Error for indexing a location that does not exist.                    */
	QERROR_INDEX_OUTOFRANGE,

	/** Error for an impossible parameter.                                    */
	QERROR_PARAMETER_INVALID,

	/** Error for an already-initialized module.                              */
	QERROR_MODULE_INITIALIZED,

	/** Error for an uninitialized module.                                    */
	QERROR_MODULE_UNINITIALIZED,

	/** Error for violating internal file read/write permissions.             */
	QERROR_FILE_MODE,

	/** Error for a #Qdatameta_t whose usage is incompatible with its type.   */
	QERROR_QDATAMETA_TYPE_INCOMPATIBLE,

	/** Error for a #Qdatameta_t whose @c count disagrees with its @c type.   */
	QERROR_QDATAMETA_TYPE_COUNT_INCOMPATIBLE,
	
	/** Special error for use in dealing with functions that use `errno`.     */
	QERROR_SYSTEM,

	/**
	 * Catch-all error to propogate error values
	 * Sometimes for functions from libraries that don't belong to Q.
	 */
	QERROR_ERRORVAL,
	
	/**
	 * Amount of possible errors.
	 * Set equal to the final enum constant.
	 */
	QERROR_COUNT = QERROR_ERRORVAL
} Qerror_t;



/**
 * Internal version of #Q_IFERROR that the macro invokes.
 */
extern void qerror_internal(Qerror_t, const char *, const char *, int line);
