#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "qdefs.h"
#include "qerror.h"

#include "dialogue.h"




static long file_size_get(FILE *);

static DialogueTree_t *dialogue_tree_create(const char *title,
		const DialogueBranch_t **branches, size_t sz);

static DialogueBranch_t *dialogue_branch_create(const char *mnemonic,
		const DialogueBranch_t **branches, size_t sz);

static DialogueObject_t *dialogue_object_create(char *response,
		const DialogueCommand_t *commands, const char **args, size_t sz);

static DialogueCommand_t string_to_dialogue_command(const char *s);




/**
 * Initialize the dialogue module.
 * @param[in] qdl_filename: the `*.qdl` file to parse.
 * @return #Q_OK or #Q_ERROR.
 */
int
dialogue_init(const char *qdl_filename) {
	int returnval = Q_OK;
	FILE *qdl_file;
	int ch;
	long file_size;
	char *file_string_raw;

	if ((qdl_file = fopen(qdl_filename, "r")) == NULL) {
		Q_ERROR_SYSTEM("fopen()");
		return Q_ERROR;
	}

	if ((file_size = file_size_get(qdl_file)) == Q_ERRORCODE_LONG) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		if (fclose(qdl_file) == EOF) {
			Q_ERROR_SYSTEM("fclose()");
		}
		return Q_ERROR;
	}

	file_size++; /* for null-terminating char */

	if ((file_string_raw = calloc((size_t) file_size, sizeof(*file_string_raw))) 
			== NULL) {
		Q_ERROR_SYSTEM("calloc()");
		if (fclose(qdl_file) == EOF) {
			Q_ERROR_SYSTEM("fclose()");
		}
		return Q_ERROR;
	}

	/* add every non-whitespace character to a string */
	int index = 0;
	while ((ch = fgetc(qdl_file)) != EOF) {
		if (isspace(ch) == 0) { /* that is, if ch is not a whitespace character */
			file_string_raw[index++] = (char) ch;
		}
	}
	
	/* check if file error indicator is set */
	if (ferror(qdl_file) != 0) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	
	if (fclose(qdl_file) == EOF) {
		Q_ERROR_SYSTEM("fclose()");
		returnval = Q_ERROR;
	}
	
	file_string_raw[index] = '\0';

	/* FIXME: debug code */
	if (printf("%s", file_string_raw) < 0) {
		Q_ERROR_SYSTEM("printf()");
		returnval = Q_ERROR;
	}

	free(file_string_raw);
	return returnval;
}


/**
 * Get the size of an open file.
 * @param[in] fp: pointer to open file.
 * @return size of @p fp or #Q_ERRORCODE_LONG.
 */
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


/**
 * Convert a `char *` to a #DialogueCommand_t.
 * @param[in] s: string in question.
 * @return converted #DialogueCommand_t or Q_ERRORCODE_ENUM on failure.
 */
DialogueCommand_t
string_to_dialogue_command(const char *s) {

	if (strcmp(s, DIALOGUE_STRING_COMMAND_GOTO) == 0) {
		return DIALOGUE_COMMAND_GOTO;
	} else if (strcmp(s, DIALOGUE_STRING_COMMAND_BECOME) == 0) {
		return DIALOGUE_COMMAND_BECOME;
	} else if (strcmp(s, DIALOGUE_STRING_COMMAND_EXIT) == 0) {
		return DIALOGUE_COMMAND_EXIT;
	} else {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return (DialogueCommand_t) Q_ERRORCODE_ENUM;
	}
}
