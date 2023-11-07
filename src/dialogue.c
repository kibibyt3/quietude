#include <stdio.h>
#include <ctype.h>

#include "qdefs.h"
#include "qerror.h"



static long file_size_get(FILE *);



/**
 * Initialize the dialogue module.
 * @param[in] qdl_filename: the `*.qdl` file to parse.
 * @return #Q_OK or #Q_ERROR.
 */
int
dialogue_init(const char *qdl_filename) {
	returnval = Q_OK;
	FILE *qdl_file;
	int ch;
	long file_size;
	char *file_string_raw;

	if ((qdl_file = fopen(qdl_file, "r")) == NULL) {
		Q_ERROR_SYSTEM("fopen()");
		return Q_ERROR;
	}

	if ((file_size = file_size_get(fp)) == Q_ERRORCODE_LONG) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	file_size++; /* for null-terminating char */

	if ((file_string_raw = calloc(file_size, sizeof(*file_string_raw))) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		return Q_ERROR;
	}

	index = 0;
	while ((ch = fgetc(qdl_file)) != EOF) {
		if (isspace(ch) == 0) { /* that is, if ch is not a whitespace character */
			file_string_raw[index++] = ch;
		}
	}
	file_string_raw[index] = '\0';

	/* check if file error indicator is set */
	if (ferror(qdl_file) != 0) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		free(file_string_raw);
		return Q_ERROR;
	}

	printf("%s", file_string_raw);

	return Q_OK;
}


long
file_size_get(FILE *fp) {
	long sz;
	if (fseek(fp, 0L, SEEK_END) != 0) {
		Q_ERROR_SYSTEM("fseek()");
		return Q_ERRORCODE_LONG; 
	}
	if ((sz = ftell(fp)) == -1L) {
		Q_ERROR_SYSTEM("ftell()");
		return Q_ERRORCODE_LONG;
	}
	rewind(fp);
	return sz + 1L;
}
