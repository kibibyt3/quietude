#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "qdefs.h"
#include "qerror.h"

#include "dialogue.h"




static long file_size_get(FILE *);


static DialogueTree_t *dialogue_tree_create(/*@only@*/char *title,
		/*@only@*/DialogueBranch_t **branches, size_t sz)/*@*/;

static DialogueBranch_t *dialogue_branch_create(/*@only@*/char *header,
		/*@only@*/char *message, /*@only@*/DialogueObject_t **objects, size_t sz)
		/*@*/;

static DialogueObject_t *dialogue_object_create(/*@only@*/char *response,
		/*@only@*/DialogueCommand_t *commands, /*@only@*/char **args, 
		size_t sz)/*@*/;

/*@null@*/
static char *dialogue_file_to_string(FILE *fp)/*@modifies fileSystem, fp@*/;

static DialogueCommand_t string_to_dialogue_command(const char *s)/*@*/;




/**
 * Initialize the dialogue module.
 * @param[in] qdl_filename: the `*.qdl` file to parse.
 * @return #Q_OK or #Q_ERROR.
 */
int
dialogue_init(const char *qdl_filename) {
	int returnval = Q_OK;
	FILE *qdl_file;
	char *file_string_raw;

	if ((qdl_file = fopen(qdl_filename, "r")) == NULL) {
		Q_ERROR_SYSTEM("fopen()");
		return Q_ERROR;
	}

	if ((file_string_raw = dialogue_file_to_string(qdl_file)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	if (fclose(qdl_file) == EOF) {
		Q_ERROR_SYSTEM("fclose()");
		returnval = Q_ERROR;
	}
	

	/* FIXME: debug code */
	if (printf("%s", file_string_raw) < 0) {
		Q_ERROR_SYSTEM("printf()");
		returnval = Q_ERROR;
	}

	free(file_string_raw);
	return returnval;
}


/**
 * Create a #DialogueTree_t.
 * @param[in] title: @ref DialogueTree_t.title.
 * @param[in] branches: @ref DialogueTree_t.branches.
 * @param[in] sz: @ref DialogueTree_t.sz.
 * @return pointer to new #DialogueTree_t.
 */
DialogueTree_t *
dialogue_tree_create(char *title, 
		 DialogueBranch_t **branches, size_t sz) {
	
	DialogueTree_t *tree;

	if ((tree = calloc((size_t) 1, sizeof(*tree))) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		abort();
	}

	tree->title = title;
	tree->branches = branches;
	tree->sz = sz;

	return tree;

}


/**
 * Create a #DialogueBranch_t.
 * @param[in] header: @ref DialogueBranch_t.header.
 * @param[in] message: @ref DialogueBranch_t.message.
 * @param[in] branches: @ref DialogueBranch_t.objects.
 * @param[in] sz: @ref DialogueBranch_t.sz.
 * @return pointer to new #DialogueBranch_t.
 */
DialogueBranch_t *
dialogue_branch_create(char *header, 
		char *message, DialogueObject_t **objects, size_t sz) {

	DialogueBranch_t *branch;

	if ((branch = calloc((size_t) 1, sizeof(*branch))) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		abort();
	}

	branch->header = header;
	branch->message = message;
	branch->objects = objects;
	branch->sz = sz;

	return branch;

}


/**
 * Create a #DialogueObject_t.
 * @param[in] response: @ref DialogueObject_t.response.
 * @param[in] commands: @ref DialogueObject_t.commands.
 * @param[in] args: @ref DialogueObject_t.args.
 * @param[in] sz: @ref DialogueObject_t.sz.
 * @return pointer to new #DialogueObject_t.
 */
DialogueObject_t *
dialogue_object_create(char *response, 
		DialogueCommand_t *commands, char **args, size_t sz) {

	DialogueObject_t *obj;

	if ((obj = calloc((size_t) 1, sizeof(*obj))) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		abort();
	}

	obj->response = response;
	obj->commands = commands;
	obj->args = args;
	obj->sz = sz;

	return obj;

}


/**
 * Convert a file's contents to a string.
 * @param[in] fp: pointer to `FILE` to parse.
 * return file contents stripped of redundant whitespace or `NULL` on error.
 */
char *
dialogue_file_to_string(FILE *fp) {
	long file_size; 
	char *s;

	if ((file_size = file_size_get(fp)) == Q_ERRORCODE_LONG) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	file_size++; /* for null-terminating char */

	if ((s = calloc((size_t) file_size, sizeof(*s))) 
			== NULL) {
		Q_ERROR_SYSTEM("calloc()");
		return NULL;
	}

	/* add every non-whitespace character to a string */
	int index = 0;
	int ch;
	bool isstring = false;
	bool isstring_proper = false;
	while ((ch = fgetc(fp)) != EOF) {

		/* 
		 * track if we're in a string or string-like sequence and only add 
		 * whitespace characters if we are
		 */
		if ((isspace(ch) == 0) || (isstring)) {
			s[index++] = (char) ch;
		}
		if ((char) ch == DIALOGUE_PARSE_CHAR_STRING) {
			isstring = !isstring;
			isstring_proper = !isstring_proper;
		} else if ((((char) ch == DIALOGUE_PARSE_CHAR_OBJECT_COMMANDS_BEG) 
					|| ((char) ch == DIALOGUE_PARSE_CHAR_OBJECT_COMMANDS_END)
					|| ((char) ch == DIALOGUE_PARSE_CHAR_TREE_TITLE_BEG) 
					|| ((char) ch == DIALOGUE_PARSE_CHAR_TREE_TITLE_END))
				&& (!isstring_proper)) {
			isstring = !isstring;
		}
	}
	
	/* check if file error indicator is set */
	if (ferror(fp) != 0) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}

	s[index] = '\0';
	
	return s;
}


/**
 * Check a file string for major syntax errors.
 * @param[in] s: string to validate.
 * @return -1 if there are no syntax errors, or the index in @p s of the first
 * unexpected @ref ParseChars character.
 */
int
dialogue_file_string_isvalid(const char *s) {
	int len = (int) strlen(s);
	char ch;
	int open_string_char_index = -1;
	int open_title_char_index = -1;
	int open_branch_char_index = -1;
	int open_command_char_index = -1;
	bool command_has_delimiter = false;

	int i;

	/* check for nested sequences and track any sections that never terminate */
	for (i = 0; i < len; i++) {
		ch = s[i];
		if (ch == DIALOGUE_PARSE_CHAR_STRING) {
			if (open_string_char_index != -1) {
				open_string_char_index = -1;
			} else {
				open_string_char_index = i;
			}
		} else if (open_string_char_index == -1) {
			switch (ch) {
			case DIALOGUE_PARSE_CHAR_TREE_TITLE_BEG:
				if (open_title_char_index != -1) {
					return i;
				}
				open_title_char_index = i;
				break;
			case DIALOGUE_PARSE_CHAR_TREE_TITLE_END:
				if (open_title_char_index == -1) {
					return i;
				}
				open_title_char_index = -1;
				break;
			case DIALOGUE_PARSE_CHAR_BRANCH_BEG:
				if (open_branch_char_index != -1) {
					return i;
				}
				open_branch_char_index = i;
				break;
			case DIALOGUE_PARSE_CHAR_BRANCH_END:
				if (open_branch_char_index == -1) {
					return i;
				}
				open_branch_char_index = -1;
				break;
			case DIALOGUE_PARSE_CHAR_OBJECT_COMMANDS_BEG:
				if (open_command_char_index != -1) {
					return i;
				}
				command_has_delimiter = false;
				open_command_char_index = i;
				break;
			case DIALOGUE_PARSE_CHAR_OBJECT_COMMANDS_END:
				if ((open_command_char_index == -1) || (!command_has_delimiter)) {
					return i;
				}
				open_command_char_index = -1;
				break;
			case DIALOGUE_PARSE_CHAR_COMMAND_DELIMITER:
				command_has_delimiter = true;
				break;
			}
		}
	}

	/* check if any *_BEG chars were left open */
	if (open_string_char_index != -1) {
		return open_string_char_index;
	
	} else if (open_title_char_index != -1) {
		return open_title_char_index;

	} else if (open_branch_char_index != -1) {
		return open_branch_char_index;
	
	} else if (open_command_char_index != -1) {
		return open_command_char_index;
	}

	return -1;	

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
