/**
 * @file qerror.c
 * Program file for qerror.
 * Responsible for dealing with errors
 */



#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "qdefs.h"

#include "qerror.h"

 

/** Maximum number of characters in a qerror string.      */
#define QERROR_ERROR_LENGTH_MAX 100



/** String for #QERROR_NULL_POINTER_UNEXPECTED.           */
#define QERROR_STRING_NULL_POINTER_UNEXPECTED \
	"Encountered unexpected NULL pointer"

/** String for #QERROR_NONNULL_POINTER_UNEXPECTED.        */
#define QERROR_STRING_NONNULL_POINTER_UNEXPECTED \
	"Encountered unexpected non-NULL pointer"

/** String for #QERROR_NULL_VALUE_UNEXPECTED.             */
#define QERROR_STRING_NULL_VALUE_UNEXPECTED \
	"Encountered unexpected NULL value"

/** String for #QERROR_ZERO_VALUE_UNEXPECTED.             */
#define QERROR_STRING_ZERO_VALUE_UNEXPECTED \
	"Encountered unexpected zero value"

/** String for #QERROR_NEGATIVE_VALUE_UNEXPECTED.         */
#define QERROR_STRING_NEGATIVE_VALUE_UNEXPECTED \
	"Encountered unexpected negative value"

/** String for #QERROR_BADDEFINE.                         */
#define QERROR_STRING_BADDEFINE \
	"#define constant constraints not met"

/** String for #QERROR_ENUM_CONSTANT_INVALID.             */
#define QERROR_STRING_ENUM_CONSTANT_INVALID \
	"Invalid enum constant encountered"

/** String for #QERROR_ENUM_CONSTANT_INVALID_ZERO.        */
#define QERROR_STRING_ENUM_CONSTANT_INVALID_ZERO \
	"Enum constant likely never explicitly defined"

/** String for #QERROR_STRUCT_INCOMPLETE.                 */
#define QERROR_STRING_STRUCT_INCOMPLETE \
	"Struct required to be fully defined is not fully defined"

/** String for #QERROR_INDEX_OUTOFRANGE.                  */
#define QERROR_STRING_INDEX_OUTOFRANGE \
	"Index is out of range"

/** String for #QERROR_PARAMETER_INVALID.                 */
#define QERROR_STRING_PARAMETER_INVALID \
	"Invalid parameter"

/** String for #QERROR_MODULE_INITIALIZED.                */
#define QERROR_STRING_MODULE_INITIALIZED \
	"Module initialized (did you already initialize this module?)"

/** String for #QERROR_MODULE_UNINITIALIZED.              */
#define QERROR_STRING_MODULE_UNINITIALIZED \
	"Module not properly initialized"

/** String for #QERROR_FILE_MODE.                         */
#define QERROR_STRING_FILE_MODE \
	"Tried to operate on a file that was opened in an incompatible mode"

/** String for #QERROR_QDATAMETA_TYPE_INCOMPATIBLE.       */
#define QERROR_STRING_QDATAMETA_TYPE_INCOMPATIBLE \
	"Qdatameta usage is incompatible with its type"

/** String for #QERROR_QDATAMETA_TYPE_COUNT_INCOMPATIBLE. */
#define QERROR_STRING_QDATAMETA_TYPE_COUNT_INCOMPATIBLE \
	"Qdatameta_t members type and size disagree"

/** String for #QERROR_ERRORVAL.                          */
#define QERROR_STRING_ERRORVAL \
	"An error value was caught returning from a function"

/**
 * String for a meta error where #Q_IFERROR is passed a nonexistent #Qerror_t.
 */
#define QERROR_STRING_QERROR_USAGE \
	"Error in invocation of QERROR; error does not exist"


/**
 * Internal version of #Q_IFERROR.
 * Should only be invoked indirectly by #Q_IFERROR.
 * @param[in] error: the error whose corresponding message should be printed to
 * @c stderr.
 * @param[in] file: the filename to print in the error message
 * @param[in] func: the function name to print in the error message
 * @param[in] line: the line number to print in the error message
 */
void
qerror_internal(Qerror_t error, const char *file, const char *func, int line) {
	
	char error_string[QERROR_ERROR_LENGTH_MAX];

	switch (error) {
	case QERROR_NULL_POINTER_UNEXPECTED:
		strcpy(error_string, QERROR_STRING_NULL_POINTER_UNEXPECTED);
		break;
	case QERROR_NONNULL_POINTER_UNEXPECTED:
		strcpy(error_string, QERROR_STRING_NONNULL_POINTER_UNEXPECTED);
		break;
	case QERROR_NULL_VALUE_UNEXPECTED:
		strcpy(error_string, QERROR_STRING_NULL_VALUE_UNEXPECTED);
		break;
	case QERROR_NEGATIVE_VALUE_UNEXPECTED:
		strcpy(error_string, QERROR_STRING_NEGATIVE_VALUE_UNEXPECTED);
		break;
	case QERROR_BADDEFINE:
		strcpy(error_string, QERROR_STRING_BADDEFINE);
		break;
	case QERROR_ZERO_VALUE_UNEXPECTED:
		strcpy(error_string, QERROR_STRING_ZERO_VALUE_UNEXPECTED);
		break;
	case QERROR_ENUM_CONSTANT_INVALID:
		strcpy(error_string, QERROR_STRING_ENUM_CONSTANT_INVALID);
		break;
	case QERROR_ENUM_CONSTANT_INVALID_ZERO:
		strcpy(error_string, QERROR_STRING_ENUM_CONSTANT_INVALID_ZERO);
		break;
	case QERROR_STRUCT_INCOMPLETE:
		strcpy(error_string, QERROR_STRING_STRUCT_INCOMPLETE);
		break;
	case QERROR_INDEX_OUTOFRANGE:
		strcpy(error_string, QERROR_STRING_INDEX_OUTOFRANGE);
		break;
	case QERROR_PARAMETER_INVALID:
		strcpy(error_string, QERROR_STRING_PARAMETER_INVALID);
		break;
	case QERROR_MODULE_INITIALIZED:
		strcpy(error_string, QERROR_STRING_MODULE_INITIALIZED);
		break;
	case QERROR_MODULE_UNINITIALIZED:
		strcpy(error_string, QERROR_STRING_MODULE_UNINITIALIZED);
		break;
	case QERROR_FILE_MODE:
		strcpy(error_string, QERROR_STRING_FILE_MODE);
		break;
	case QERROR_QDATAMETA_TYPE_INCOMPATIBLE:
		strcpy(error_string, QERROR_STRING_QDATAMETA_TYPE_INCOMPATIBLE);
		break;
	case QERROR_QDATAMETA_TYPE_COUNT_INCOMPATIBLE:
		strcpy(error_string, QERROR_STRING_QDATAMETA_TYPE_COUNT_INCOMPATIBLE);
		break;
	case QERROR_ERRORVAL:
		strcpy(error_string, QERROR_STRING_ERRORVAL);
		break;
	default:
		strcpy(error_string, QERROR_STRING_QERROR_USAGE);
		break;
	}

	fprintf(stderr, "Internal error: %s in file %s, in function %s, on line %i\n",
			error_string, file, func, line);

	return;
}
