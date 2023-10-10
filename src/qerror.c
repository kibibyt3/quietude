/**
 * @file qerror.c
 * Program file for qerror.
 * Responsible for dealing with errors
 */



#include <stdio.h>

#include "qdefs.h"

#include "qerror.h"

 

/** String for #QERROR_NULL_POINTER_UNEXPECTED */
#define QERROR_STRING_NULL_POINTER_UNEXPECTED \
	"Encountered unexpected NULL pointer"
/** String for #QERROR_NULL_VALUE_UNEXPECTED */
#define QERROR_STRING_NULL_VALUE_UNEXPECTED \
	"Encountered unexpect NULL value"
/** String for #QERROR_ENUM_CONSTANT_INVALID */
#define QERROR_STRING_ENUM_CONSTANT_INVALID \
	"Invalid enum constant encountered"
/** String for #QERROR_ENUM_CONSTANT_INVALID_ZERO */
#define QERROR_STRING_ENUM_CONSTANT_INVALID_ZERO \
	"Enum constant likely never explicitly defined"
/** String for a meta error where #QERROR is passed a nonexistent #Qerror_t */
#define QERROR_STRING_QERROR_USAGE \
	"Error in invocation of QERROR; error does not exist"

/**
 * Internal version of #QERROR.
 * Should only be invoked indirectly by #QERROR.
 * @param[in] error: the error whose corresponding message should be printed to
 * @c stderr.
 * @param[in] file: the filename to print in the error message
 * @param[in] func: the function name to print in the error message
 * @param[in] line: the line number to print in the error message
 */
void
qerror_internal(Qerror_t error, const char *file, const char *func, int line) {
	
	fprintf(stderr, "Internal error: ");

	switch (error) {
	case QERROR_NULL_POINTER_UNEXPECTED:
		fprintf(stderr, QERROR_STRING_NULL_POINTER_UNEXPECTED);
		break;
	case QERROR_NULL_VALUE_UNEXPECTED:
		fprintf(stderr, QERROR_STRING_NULL_VALUE_UNEXPECTED);
		break;
	case QERROR_ENUM_CONSTANT_INVALID:
		fprintf(stderr, QERROR_STRING_ENUM_CONSTANT_INVALID);
		break;
	case QERROR_ENUM_CONSTANT_INVALID_ZERO:
		fprintf(stderr, QERROR_STRING_ENUM_CONSTANT_INVALID_ZERO);
		break;
	default:
		fprintf(stderr, QERROR_STRING_QERROR_USAGE);
		break;
	}

	fprintf(stderr, " in file %s, in function %s, on line %i\n",
			file, func, line);

	return;
}
