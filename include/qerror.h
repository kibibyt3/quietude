/**
 * @file qerror.h
 * Header file for qerror.
 * Depends on stdio.h and qdefs.h.
 */



/**
 * Print a #Qerror_t and error information to @c stderr.
 * @param[in] error: the error whose corresponding message should be printed to
 * @c stdout.
 */
#define QERROR(error) qerror_internal((error), __FILE__, __func__, __LINE__)



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

	/**
	 * Amount of possible errors.
	 * Set equal to the final enum constant.
	 */
	QERROR_COUNT = QERROR_ENUM_CONSTANT_INVALID_ZERO
} Qerror_t;



/**
 * Internal version of #QERROR that the macro invokes.
 */
extern void qerror_internal(Qerror_t, const char *, const char *, int line);
