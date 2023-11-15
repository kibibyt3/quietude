/**
 * @file dialogue.c
 * Wrapper program file for dialogue.
 * Controls interactions between the I/O and logic modules of dialogue.
 */



#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"
#include "splint_types.h"

#include "dialogue.h"



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

	/* Allocate and set the header_active for the tree. */
	size_t default_header_active_sz = strlen(DIALOGUE_HEADER_ACTIVE_DEFAULT) +
		(size_t) 1;
	if ((tree->header_active = calloc(default_header_active_sz,
					sizeof(tree->header_active))) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		abort();
	}
	
	strcpy(tree->header_active, DIALOGUE_HEADER_ACTIVE_DEFAULT);

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
 * Destroy a #DialogueTree_t.
 * @param[out] tree: #DialogueTree_t to destroy.
 */
void
dialogue_tree_destroy(DialogueTree_t *tree) {
	size_t sz;
	sz = dialogue_tree_sz_get(tree);

	for (int i = 0; (size_t) i < sz; i++) {
		dialogue_branch_destroy(tree->branches[i]);
	}

	free(tree->branches);
	free(tree->header_active);
	free(tree->title);

	free(tree);
	return;
}


/**
 * Destroy a #DialogueBranch_t.
 * @param[out] branch: #DialogueBranch_t to destroy.
 */
void
dialogue_branch_destroy(DialogueBranch_t *branch) {
	size_t sz;
	sz = dialogue_branch_sz_get(branch);

	for (int i = 0; (size_t) i < sz; i++) {
		dialogue_object_destroy(branch->objects[i]);
	}

	free(branch->objects);
	free(branch->header);
	free(branch->message);

	free(branch);
	return;
}


/**
 * Destroy a #DialogueObject_t.
 * @param[out] obj: #DialogueObject_t to destroy.
 */
void
dialogue_object_destroy(DialogueObject_t *obj) {
	size_t sz;
	sz = dialogue_object_sz_get(obj);

	for (int i = 0; (size_t) i < sz; i++) {
		free(obj->args[i]);
	}

	free(obj->args);
	free(obj->response);
	free(obj->commands);

	free(obj);
	return;
}


/**
 * Search through every header in a tree and check for a match.
 * @param[in] tree: #DialogueTree_t to search through.
 * @param[in] header: @ref DialogueBranch_t.header to search for.
 * @return #DialogueObject_t whose member @ref DialogueBranch_t.header was a
 * match for @p header, or NULL if an error occurred or @p header's match could
 * not be found.
 */
DialogueBranch_t *
dialogue_branch_search_from_header(const DialogueTree_t* tree, const char *header) {
	size_t sz;
	DialogueBranch_t *branch;

	sz = dialogue_tree_sz_get(tree);
	for (int i = 0; (size_t) i < sz; i++) {
		if ((branch = dialogue_tree_branch_get(tree, i)) == NULL) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return NULL;
		}
		if (strcmp(dialogue_branch_header_get(branch), header) == 0) {
			return branch;
		}
	}

	return NULL;
}


/**
 * Get the currently active #DialogueBranch_t in a #DialogueTree_t.
 * @param[in] tree: #DialogueTree_t whose active #DialogueBranch_t is to be
 * found.
 * @return the active #DialogueBranch_t or `NULL` on error.
 */
DialogueBranch_t *
dialogue_tree_active_branch_get(const DialogueTree_t *tree){

	DialogueBranch_t *branch;
	char *header_active;

	header_active = dialogue_tree_header_active_get(tree);
	if ((branch = dialogue_branch_search_from_header(tree, header_active)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	return branch;
}


/**
 * Get @ref DialogueTree_t.header_active.
 * @param[in] tree: parent #DialogueTree_t.
 * @return requested header.
 */
char *
dialogue_tree_header_active_get(const DialogueTree_t *tree) {
	return tree->header_active; 
}


/**
 * Get @ref DialogueTree_t.title.
 * @param[in] tree: parent #DialogueTree_t.
 * @return requested title.
 */
char *
dialogue_tree_title_get(const DialogueTree_t *tree) {
	return tree->title;
}


/**
 * Get a #DialogueBranch_t from @ref DialogueTree_t.branches.
 * @param[in] tree: parent #DialogueTree_t.
 * @param[in] index: index of #DialogueBranch_t.
 * @return requested #DialogueBranch_t or `NULL`.
 */
DialogueBranch_t *
dialogue_tree_branch_get(const DialogueTree_t *tree, int index) {
	if ((index >= (int) tree->sz) || (index < 0)) {
		Q_ERRORFOUND(QERROR_INDEX_OUTOFRANGE);
		return NULL;
	}
	return tree->branches[index];
}


/**
 * Get @ref DialogueTree_t.sz.
 * @param[in] tree: parent #DialogueTree_t.
 * @return requested `size_t`.
 */
size_t
dialogue_tree_sz_get(const DialogueTree_t *tree) {
	return tree->sz;
}


/**
 * Get @ref DialogueBranch_t.header.
 * @param[in] branch: parent #DialogueBranch_t.
 * @return requested header.
 */
char *
dialogue_branch_header_get(const DialogueBranch_t *branch) {
	return branch->header;
}


/**
 * Get @ref DialogueBranch_t.message.
 * @param[in] branch: parent #DialogueBranch_t.
 * @return requested message.
 */
char *
dialogue_branch_message_get(const DialogueBranch_t *branch) {
	return branch->message;
}


/**
 * Get a #DialogueObject_t from @ref DialogueBranch_t.objects.
 * @param[in] branch: parent #DialogueBranch_t.
 * @param[in] index: index of #DialogueObject_t.
 * @return requested #DialogueObject_t or `NULL`.
 */
DialogueObject_t *
dialogue_branch_object_get(const DialogueBranch_t *branch, int index) {
	if ((index >= (int) branch->sz) || (index < 0)) {
		Q_ERRORFOUND(QERROR_INDEX_OUTOFRANGE);
		return NULL;
	}
	return branch->objects[index];
}


/**
 * Get @ref DialogueBranch_t.sz.
 * @param[in] branch: parent #DialogueBranch_t.
 * @return requested `size_t`.
 */
size_t
dialogue_branch_sz_get(const DialogueBranch_t *branch) {
	return branch->sz;
}


/**
 * Get @ref DialogueObject_t.response.
 * @param[in] obj: parent #DialogueObject_t.
 * @return requested response.
 */
char *
dialogue_object_response_get(const DialogueObject_t *obj) {
	return obj->response;
}


/**
 * Get a #DialogueCommand_t from @ref DialogueObject_t.commands.
 * @param[in] obj: parent #DialogueObject_t.
 * @param[in] index: index of #DialogueCommand_t.
 * @return requested #DialogueCommand_t or #Q_ERRORCODE_ENUM.
 */
DialogueCommand_t
dialogue_object_command_get(const DialogueObject_t *obj, int index) {
	if ((index >= (int) obj->sz) || (index < 0)) {
		Q_ERRORFOUND(QERROR_INDEX_OUTOFRANGE);
		return (DialogueCommand_t) Q_ERRORCODE_ENUM;
	}
	return obj->commands[index];
}

/**
 * Get an argument from @ref DialogueObject_t.args.
 * @param[in] obj: parent #DialogueObject_t.
 * @param[in] index: index of argument.
 * @return requested arument or #Q_ERRORCODE_ENUM.
 */
char *
dialogue_object_arg_get(const DialogueObject_t *obj, int index) {
	if ((index >= (int) obj->sz) || (index < 0)) {
		Q_ERRORFOUND(QERROR_INDEX_OUTOFRANGE);
		return NULL;
	}
	return obj->args[index];
}


/**
 * Get @ref DialogueObject_t.sz.
 * @param[in] obj: parent #DialogueObject_t.
 * @return requested `size_t`.
 */
size_t
dialogue_object_sz_get(const DialogueObject_t *obj) {
	return obj->sz;
}
