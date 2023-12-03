/**
 * @file dialoguel.c
 * Program file for the dialogue module.
 * This module concerns conversations with NPCs.
 */



#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"
#include "splint_types.h"

#include "dialogue.h"



/** Argument to be accessed by other modules. */
/*@i1@*/static char arg_external[DIALOGUE_SECTION_SIZE_MAX] = "";

/** Command to be accessed by other modules. */
static DialogueCommand_t command_external = DIALOGUE_COMMAND_EMPTY;




static int dialogue_command_handler(DialogueTree_t *tree,
		DialogueCommand_t command, const char *arg)
		/*@modifies tree, command_external, arg_external@*/;

	/*@null@*/
static DialogueTree_t *dialogue_file_string_to_tree(const char *)/*@*/;


static int dialogue_tree_header_active_set(
		DialogueTree_t *tree, const char *header_active)
	/*@modifies tree->header_active@*/;

/*@null@*/
static char *dialogue_file_to_string(FILE *fp)/*@modifies fileSystem, fp@*/;

static int dialogue_file_string_isvalid(const char *s)/*@*/;

/** Find the amount of branches, objects, and commands in a QDL file string. */
static int dialogue_sections_count(const char *s,
		/*@out@*/int *branchesc, /*@partial@*/int **objectsc,
		/*@partial@*/int ***commandsc)
	/*@modifies branchesc, objectsc, commandsc@*/;

static void dialogue_sections_counter_destroy(int branchesc, /*@only@*/int *objectsc, /*@only@*/int **commandsc);

static long file_size_get(FILE *fp)/*@modifies fileSystem, fp@*/;

static DialogueCommand_t string_to_dialogue_command(const char *s)/*@*/;




/**
 * Initialize the dialogue module.
 * @param[in] qdl_filename: the `*.qdl` file to parse.
 * @return initialized #DialogueTree_t or `NULL` on error.
 */
DialogueTree_t *
dialogue_logic_init(const char *qdl_filename) {
	int r;
	FILE *qdl_file;
	char *file_string_raw;
	DialogueTree_t *tree;

	size_t qdl_path_len;
	qdl_path_len = strlen(DIALOGUE_QDL_PATH) + strlen(qdl_filename) + (size_t) 2;

	if (qdl_path_len > (size_t) DIALOGUE_QDL_PATH_SIZE_MAX) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return NULL;
	}

	char *qdl_path;
	if ((qdl_path = calloc(qdl_path_len, sizeof(*qdl_path))) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		return NULL;
	}

	strcpy(qdl_path, DIALOGUE_QDL_PATH);
	strcat(qdl_path, qdl_filename);

	if ((qdl_file = fopen(qdl_path, "r")) == NULL) {
		Q_ERROR_SYSTEM("fopen()");
		free(qdl_path);
		return NULL;
	}
	free(qdl_path);

	if ((file_string_raw = dialogue_file_to_string(qdl_file)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	if (fclose(qdl_file) == EOF) {
		Q_ERROR_SYSTEM("fclose()");
	}
	
	if ((r = dialogue_file_string_isvalid(file_string_raw)) != -1) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		fprintf(stderr, "Error found while parsing QDL file %s at character %c "
				"(character index %i after removal of redundant whitespace).\n",
				qdl_filename, file_string_raw[r], r);
	}

	if ((tree = dialogue_file_string_to_tree(file_string_raw)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}

	free(file_string_raw);
	return tree;
}


/**
 * Pass a tick in the dialogue logic module.
 * Specifically executes the tick on @p tree.
 * @param[in] tree: #DialogueTree_t to pass a tick in.
 * @param[in] choice: index of the object selected by the user.
 * @return #Q_OK or #Q_ERROR. 
 */
int
dialogue_logic_tick(DialogueTree_t *tree, int choice)
/*@modifies command_external, tree@*/
{
	
	DialogueBranch_t *branch;
	DialogueObject_t *obj_choice;
	DialogueCommand_t command;
	char *arg;
	size_t obj_sz;

	/* set the external command pair to its initialized value */
	command_external = DIALOGUE_COMMAND_EMPTY;

	if ((branch = dialogue_tree_active_branch_get(tree)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	if ((obj_choice = dialogue_branch_object_get(branch, choice)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	obj_sz = dialogue_object_sz_get(obj_choice);
	
	/* handle commands */
	for (int i = 0; (size_t) i < obj_sz; i++) {
		if ((command = dialogue_object_command_get(obj_choice, i))
				== (DialogueCommand_t) Q_ERRORCODE_ENUM) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		if ((arg = dialogue_object_arg_get(obj_choice, i)) == NULL) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		if (dialogue_command_handler(tree, command, arg) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
	}

	return Q_OK;
}


/**
 * Handle a #DialogueCommand_t/argument pair.
 * Specifically execute the pair on a #DialogueTree_t.
 * @param[out] tree: #DialogueTree_t in question.
 * @param[in] command: #DialogueCommand_t to execute.
 * @param[in] arg: argument to execute.
 * @return #Q_OK or #Q_ERROR.
 */
int
dialogue_command_handler(DialogueTree_t *tree,
		DialogueCommand_t command, const char *arg) {
	
	if ((command < (DialogueCommand_t) Q_ENUM_VALUE_START)
			|| (command > DIALOGUE_COMMAND_COUNT)) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERROR;
	}
	switch (command) {
	case DIALOGUE_COMMAND_GOTO:
		if (dialogue_tree_header_active_set(tree, arg) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		break;
	case DIALOGUE_COMMAND_EXIT:
		if (dialogue_tree_header_active_set(tree, DIALOGUE_HEADER_ACTIVE_EXIT)
				== Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		break;
	
	/* handle commands meant exclusively for the module using dialogue */
	default:

		/* check that we aren't supplied the address to arg_external */
		if (arg == arg_external) {
			Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
			return Q_ERROR;
		}
		command_external = command;
		/*@i2@*/strcpy(arg_external, arg);
		return Q_OK;
	}

	return Q_OK;
}


/**
 * Get the external command.
 * Should be handled directly after @ref dialogue_logic_tick() is called.
 * @return the external command. Set to #DIALOGUE_COMMAND_EMPTY if there was no
 * external command on the last tick.
 */
DialogueCommand_t
dialogue_command_external_get() 
/*@globals command_external@*/
{
	return command_external;
}


/**
 * Get the external argument.
 * @return the external argument.
 */
char *
dialogue_arg_external_get()
/*@globals arg_external@*/
{
	return arg_external;
}


/*@ignore@*/
/**
 * Convert a string from a file to a #DialogueTree_t.
 * @param[in] s: file string to convert.
 * @return newly converted #DialogueTree_t.
 */
DialogueTree_t *
dialogue_file_string_to_tree(const char *s) {

	int branchesc = 0;
	int *objectsc = NULL;
	int **commandsc = NULL;
	int slen;

	/* Generic buffers */
	/*@reldef@*/char container[DIALOGUE_SECTION_SIZE_MAX];
	container[0] = '\0';

	/* Tree buffers */
	DialogueTree_t *tree = NULL;
	char *title = NULL;
	DialogueBranch_t **branches = NULL;
	
	/* Branch buffers */
	char *header = NULL;
	char *message = NULL;
	DialogueObject_t **objects = NULL;

	/* Object buffers */
	char *response = NULL;
	DialogueCommand_t *commands = NULL;
	DialogueCommand_t command = (DialogueCommand_t) 0;
	/*@only@*/char **args = NULL;

	if (dialogue_sections_count(s, &branchesc, &objectsc, &commandsc) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	if ((branchesc == 0) || (objectsc == NULL) || (commandsc == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return NULL;
	}

	/* 
	 * we can expect a tree title here because that's the first thing in every QDL
	 * file
	 */
	DialogueParseMode_t parse_mode = DIALOGUE_PARSE_MODE_TREE_TITLE;

	int container_index = 0;
	/* Main parse loop. */
	slen = (int) strlen(s); 
	char ch;
	bool isstring = false;
	int branches_index = 0;
	int objects_index = 0;
	int commands_index = 0;

	if ((branches = calloc((size_t) branchesc, sizeof(*branches))) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		dialogue_sections_counter_destroy(branchesc, objectsc, commandsc);
		return NULL;
	}
	for (int i = 0; i < slen; i++) {
		ch = s[i];

		/* handle the string character and command character seperately */
		if (ch == DIALOGUE_PARSE_CHAR_STRING) {
			isstring = !isstring;

			/* save the branch message and start an object's response
		 	*  branch headers are saved here because their only terminating character is
		 	*  the beginning of another string (namely an object's response)
		 	*/
			if ((isstring == false)
					&& (parse_mode == DIALOGUE_PARSE_MODE_BRANCH_MESSAGE)) {
				
				/* save the branch header */
				parse_mode = DIALOGUE_PARSE_MODE_OBJECT_RESPONSE;
				container[container_index] = '\0';
				message = calloc(strlen(container) + (size_t) 1, sizeof(*header));
				if (message == NULL) {
					Q_ERROR_SYSTEM("calloc()");
					abort();
				}
				strcpy(message, container);
				container_index = 0;
				
				/* start an object */
				commands_index = 0;
				if (objects_index < objectsc[branches_index]) {
					commands = calloc((size_t) commandsc[branches_index][objects_index],
							sizeof(*commands));
					if (commands == NULL) {
						Q_ERROR_SYSTEM("calloc()");
						abort();
					}
					args = calloc((size_t) commandsc[branches_index][objects_index],
							sizeof(*args));
					if (args == NULL) {
						Q_ERROR_SYSTEM("calloc()");
						abort();
					}
				}
			}
		}



		if (!isstring) {

			switch (ch) {

			/* start the tree title */
			case DIALOGUE_PARSE_CHAR_TREE_TITLE_BEG:
				parse_mode = DIALOGUE_PARSE_MODE_TREE_TITLE;
				break;



			/* save the tree title and start a new branch */
			case DIALOGUE_PARSE_CHAR_TREE_TITLE_END:

				/* save the title */
				assert(parse_mode == DIALOGUE_PARSE_MODE_TREE_TITLE);
				parse_mode = DIALOGUE_PARSE_MODE_BRANCH_HEADER;
				container[container_index] = '\0';
				title = calloc(strlen(container) + (size_t) 1, sizeof(*title));
				if (title == NULL) {
					Q_ERROR_SYSTEM("calloc()");
					abort();
				}
				strcpy(title, container);
				container_index = 0;
				header = NULL;
				message = NULL;
				objects = NULL;

				/* start a new branch */
				objects_index = 0;
				objects = calloc((size_t) objectsc[branches_index], sizeof(*objects));
				if (objects == NULL) {
					Q_ERROR_SYSTEM("calloc()");
				}

				break;



			/* save the old branch and start a new one if there's another left */
			case DIALOGUE_PARSE_CHAR_BRANCH_END:
				parse_mode = DIALOGUE_PARSE_MODE_BRANCH_HEADER;

				/* check that none of the buffers to be used are empty */
				if ((header == NULL) || (message == NULL) || (objects == NULL)) {
					Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
					abort();
				}

				/* save the old branch */
				branches[branches_index] = dialogue_branch_create(header, message, objects, 
						(size_t) objectsc[branches_index]);
				if (branches[branches_index] == NULL) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
					abort();
				}
				header = NULL;
				message = NULL;
				objects = NULL;

				/* create the new branch (if applicable) */
				branches_index++;
				objects_index = 0;
				if (branches_index < branchesc) {
					objects = calloc((size_t) objectsc[branches_index], sizeof(*objects));
					if (objects == NULL) {
						Q_ERROR_SYSTEM("calloc()");
						abort();
					}
				}
				break;



			/* save the branch header */
			case DIALOGUE_PARSE_CHAR_BRANCH_BEG:
				assert(parse_mode == DIALOGUE_PARSE_MODE_BRANCH_HEADER);
				parse_mode = DIALOGUE_PARSE_MODE_BRANCH_MESSAGE;
				container[container_index] = '\0';
				header = calloc(strlen(container) + (size_t) 1, sizeof(*header));
				if (header == NULL) {
					Q_ERROR_SYSTEM("calloc()");
					abort();
				}
				strcpy(header, container);
				container_index = 0;
				break;



			/* save the object's response and start a command */
			case DIALOGUE_PARSE_CHAR_OBJECT_COMMANDS_BEG:

				/* save the object's response */
				assert(parse_mode == DIALOGUE_PARSE_MODE_OBJECT_RESPONSE);
				parse_mode = DIALOGUE_PARSE_MODE_OBJECT_COMMAND;
				container[container_index] = '\0';
				response = calloc(strlen(container) + (size_t) 1, sizeof(*response));
				if (response == NULL) {
					Q_ERROR_SYSTEM("calloc()");
					abort();
				}
				strcpy(response, container);
				container_index = 0;
			
				/* start a command */
				commands_index = 0;
				
				break;



			/* save the command and start an arg */
			case DIALOGUE_PARSE_CHAR_COMMAND_ARG_SEPARATOR:
				
				/* check that we're not just in the title */
				if (parse_mode == DIALOGUE_PARSE_MODE_TREE_TITLE) {
					container[container_index++] = ch;
					break;
				}

				container[container_index] = '\0';
				if (commands == NULL) {
					Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
					abort();
				}
				if (parse_mode == DIALOGUE_PARSE_MODE_OBJECT_COMMAND) {
					/* save the command */
					container[container_index] = '\0';
					if ((command = string_to_dialogue_command(container))
							== (DialogueCommand_t) Q_ERRORCODE_ENUM) {
						Q_ERRORFOUND(QERROR_ERRORVAL);
						abort();
					}
					commands[commands_index] = command;

					/* start an arg */
					parse_mode = DIALOGUE_PARSE_MODE_OBJECT_ARG;
					container_index = 0;
				}
				break;



			/* save the arg and, if possible, start a command */
			case DIALOGUE_PARSE_CHAR_COMMAND_DELIMITER:
				if (args == NULL) {
					Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
					abort();
				}
				
				/* save the arg */
				assert(parse_mode == DIALOGUE_PARSE_MODE_OBJECT_ARG);
				container[container_index] = '\0';
				args[commands_index] = calloc(strlen(container) + (size_t) 1,
						sizeof(**args));
				if (args[commands_index] == NULL) {
					Q_ERROR_SYSTEM("calloc()");
					abort();
				}
				strcpy(args[commands_index], container);
				commands_index++;

				/* start a command (if applicable) 
				 * (checking if it's applicable is handled by
				 * DIALOGUE_PARSE_CHAR_OBJECT_COMMANDS_END
				 */
				parse_mode = DIALOGUE_PARSE_MODE_OBJECT_COMMAND;
				container_index = 0;

				break;


			/* save the object and, if possible, start a new one */
			case DIALOGUE_PARSE_CHAR_OBJECT_COMMANDS_END:

				assert(parse_mode == DIALOGUE_PARSE_MODE_OBJECT_COMMAND);
				parse_mode = DIALOGUE_PARSE_MODE_OBJECT_RESPONSE;
				if ((response == NULL) || (commands == NULL)
						|| (args == NULL) || (objects == NULL)) {
					Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
					abort();
				}
				
				/* save the old object */
				objects[objects_index] = dialogue_object_create(response, commands, args,
						(size_t) commandsc[branches_index][objects_index]);
				if (objects[objects_index] == NULL) {
					Q_ERROR_SYSTEM("calloc()");
					abort();
				}
				response = NULL;
				commands = NULL;
				args = NULL;

				/* start a new object (if applicable) */
				objects_index++;
				commands_index = 0;
				if (objects_index < objectsc[branches_index]) {
					commands = calloc((size_t) commandsc[branches_index][objects_index],
							sizeof(*commands));
					if (commands == NULL) {
						Q_ERROR_SYSTEM("calloc()");
						abort();
					}
					/* 
					 * this breaks with the pattern because args is a pointer-to-pointer
					 */
					args = calloc((size_t) commandsc[branches_index][objects_index], sizeof(*args));
					if (args == NULL) {
						Q_ERROR_SYSTEM("calloc()");
						abort();
					}
				}
				break;



			default:
				/* 
				 * until we implement escape characters, string chars are always literal
				 */
				if (ch != DIALOGUE_PARSE_CHAR_STRING) {
					container[container_index++] = ch;
				}
				break;

			}
		} else {
			if (ch != DIALOGUE_PARSE_CHAR_STRING) {
				container[container_index++] = ch;
			}
		}
	}

	if (title == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		abort();
	}
	if ((tree = dialogue_tree_create(title, branches, (size_t) branchesc)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}

	dialogue_sections_counter_destroy(branchesc, objectsc, commandsc);

	return tree;
}
/*@end@*/


/**
 * Resize the memory associated with the old active header and set a new one.
 * @param[out] tree: #DialogueTree_t whose @ref DialogueTree_t.header_active
 * member is to be changed.
 * @param[in] header_active: string to assign to @p tree.
 * @return #Q_OK or #Q_ERROR.
 */
int
dialogue_tree_header_active_set(DialogueTree_t *tree,
		const char *header_active) {
	
	size_t sz = strlen(header_active) + (size_t) 1;
	if (sz > (size_t) DIALOGUE_SECTION_SIZE_MAX) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}
	
	if ((tree->header_active = realloc(tree->header_active, sz)) == NULL) {
		Q_ERROR_SYSTEM("realloc()");
		abort();
	}

	strcpy(tree->header_active, header_active);

	return Q_OK;
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
	bool isstring_fake = false;
	bool isstring_proper = false;
	int ch_prev = (int) '\0';
	while ((ch = fgetc(fp)) != EOF) {

		/* 
		 * track if we're in a string or string-like sequence and only add 
		 * whitespace characters if we are. also ensure that it isn't a stray
		 * whitespace within a command section.
		 */
		if ((isspace(ch) == 0) 
				|| ((isstring) && !((ch_prev == (int) DIALOGUE_PARSE_CHAR_COMMAND_DELIMITER)
						&& (isstring_fake)))) {
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
			isstring_fake = !isstring_fake;
		}

		ch_prev = ch;
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
 * Find the amount of branches, objects, and commands in a QDL file string.
 * @param[in] s: file string to parse.
 * @param[out] branchesc: address to store the number of #DialogueBranch_t
 * instances.
 * @param[out] objectsc: address to store a one-dimensional dynamic array of
 * #DialogueObject_t instances, indexed against @p branchesc.
 * @param[out] commandsc: address to store a two-dimensional dynamic array of
 * #DialogueCommand_t instances, indexed from the inside-out against @p objectsc
 * and @p branchesc, respectively.
 * @return #Q_OK or #Q_ERROR.
 */
int
dialogue_sections_count(const char *s, int *branchesc, PartialIntp_t *objectsc,
		PartialPartialIntpp_t *commandsc) {
	int i;
	char ch;
	int len = (int) strlen(s);
	bool isstring = false;

	/* handle branches */
	*branchesc = 0;
	/* branches pass */
	for (i = 0; i < len; i++) {
		ch = s[i];
		if (ch == DIALOGUE_PARSE_CHAR_STRING) {
			isstring = !isstring;
		} else if ((ch == DIALOGUE_PARSE_CHAR_BRANCH_END) && (!isstring)) {
			(*branchesc)++;
		}
	}

	/* handle objects */
	int branches_index = 0;

	/* relies on calloc()'s initialization of all bytes to zero */
	if ((*objectsc = calloc((size_t) *branchesc, sizeof(**objectsc))) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		abort();
	}
	/* objects pass */
	for (i = 0; i < len; i++) {
		ch = s[i];
		if (ch == DIALOGUE_PARSE_CHAR_STRING) {
			isstring = !isstring;
		} else if ((ch == DIALOGUE_PARSE_CHAR_BRANCH_END) && (!isstring)) {
			branches_index++;
		} else if ((ch == DIALOGUE_PARSE_CHAR_OBJECT_COMMANDS_END) && (!isstring)) {
			/* each object has only one commands section */
			/*
			 * prevent out-of-bounds memory access for incorrectly-written QDL files
			 */
			if (branches_index >= *branchesc) { 
				Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
				abort();
			}
			/*@i3@*/((*objectsc)[branches_index])++;
		}
	}

	/* handle commands */
	int objects_index = 0;
	branches_index = 0;
	/* alloc memory for the pointers (which correspond to objects) */
	if ((*commandsc = calloc((size_t) *branchesc, sizeof(**commandsc))) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		abort();
	}
	/* alloc memory for the commands proper and initialize to zero */
	for (i = 0; i < *branchesc; i++) {
		/*@i5@*/if (((*commandsc)[i] = calloc((size_t) (*objectsc)[i], sizeof(***commandsc))) == NULL) {
			Q_ERROR_SYSTEM("calloc()");
			abort();
		}
	}
	/* commands pass */
	for (i = 0; i < len; i++) {
		ch = s[i];
		if (ch == DIALOGUE_PARSE_CHAR_STRING) {
			isstring = !isstring;
		} else if ((ch == DIALOGUE_PARSE_CHAR_BRANCH_END) && (!isstring)) {
			branches_index++;
			objects_index = 0;
		} else if ((ch == DIALOGUE_PARSE_CHAR_OBJECT_COMMANDS_END) && (!isstring)) {
			if (branches_index >= *branchesc) {
				Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
				abort();
			}
			objects_index++;
		} else if ((ch == DIALOGUE_PARSE_CHAR_COMMAND_DELIMITER) && (!isstring)) {
			/*@i3@*/if (objects_index >= (*objectsc)[branches_index]) {
				Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
				abort();
			}
			((*commandsc)[branches_index][objects_index])++;
		}
	}

	return Q_OK;
}


/**
 * Destroy the counters created by @ref dialogue_sections_count().
 * @param[out] branchesc: address to store the number of #DialogueBranch_t
 * instances.
 * @param[out] objectsc: address to store a one-dimensional dynamic array of
 * #DialogueObject_t instances, indexed against @p branchesc.
 * @param[out] commandsc: address to store a two-dimensional dynamic array of
 * #DialogueCommand_t instances, indexed from the inside-out against @p objectsc
 * and @p branchesc, respectively.
 */
void
dialogue_sections_counter_destroy(int branchesc, int *objectsc, /*@only@*/OnlyIntp_t *commandsc) {
	int branches_index;

	for (branches_index = 0; branches_index < branchesc; branches_index++) {
		free(commandsc[branches_index]);
	}
	free(commandsc);
	free(objectsc);

	return;
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
 * @return converted #DialogueCommand_t or #Q_ERRORCODE_ENUM on failure.
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
