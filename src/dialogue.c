/**
 * @file dialogue.c
 * Wrapper program file for dialogue.
 * Controls interactions between the I/O and logic modules of dialogue.
 */



#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "qdefs.h"
#include "qerror.h"

#include "dialogue.h"



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
