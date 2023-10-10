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
	if ((check)) {qerror_internal((error), __FILE__, __func__, __LINE__);}



/**
 * Internal error for Q.
 */
typedef enum Qerror_t {
	/** Error for unexpected @c null pointer      */
	QERROR_NULL_POINTER_UNEXPECTED = Q_ENUM_VALUE_START,

	/** Error for unexpected @c null value        */
	QERROR_NULL_VALUE_UNEXPECTED,

	/** Error for invalid @c enum constant        */
	QERROR_ENUM_CONSTANT_INVALID,

	/** Error for an @c enum constant set to zero */
	QERROR_ENUM_CONSTANT_INVALID_ZERO,

	/** Error for an uninitialized module         */
	QERROR_MODULE_UNINITIALIZED,

	/**
	 * Amount of possible errors.
	 * Set equal to the final enum constant.
	 */
	QERROR_COUNT = QERROR_MODULE_UNINITIALIZED
} Qerror_t;



/**
 * Internal version of #QERROR that the macro invokes.
 */
extern void qerror_internal(Qerror_t, const char *, const char *, int line);
