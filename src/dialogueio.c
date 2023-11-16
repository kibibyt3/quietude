/**
 * @file dialogueio.c
 * Program file for the I/O half of NPC dialogue and related functions.
 */



#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"
#include "splint_types.h"

#include "dialogue.h"



/*@null@*//*@dependent@*/
static WINDOW *win = NULL;



static char dialogue_io_tick(DialogueBranch_t *branch, const char *title,
		int choice)/*@modifies win@*/;



/**
 * Initialize the dialogue I/O module.
 * @param[in] win: the `WINDOW *` to use for I/O functionality.
 */
void
dialogue_io_init(WINDOW *argwin)
/*@modifies win@*/
{
	win = argwin;
	return;
}


/**
 * Output a #DialogueTree_t to the screen and take a user-input choice.
 * More accurately, output a #DialogueBranch_t derived from the state of @p
 * tree.
 * @param[in] tree: #DialogueTree_t to output.
 * @return the index of the choice made by the user or #Q_ERRORCODE_INT.
 */
int
dialogue_io_event(DialogueTree_t *tree)
/*@modifies win@*/
{
	DialogueBranch_t *branch;
	char *header_active;
	size_t optc;
	char ch;
	int choice = 0;
	char *title;

	title = dialogue_tree_title_get(tree);

	header_active = dialogue_tree_header_active_get(tree);
	if ((branch = dialogue_branch_search_from_header(tree, header_active))
			== NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERRORCODE_INT;
	}

	optc = dialogue_branch_sz_get(branch);
	do {
		if ((ch = dialogue_io_tick(branch, title, choice)) == Q_ERRORCODE_CHAR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		
		/* handle movement input */
		if (ch == DIALOGUE_ICH_MOVE_UP) {
			if (choice > 0) {
				choice--;
			}
		} else if (ch == DIALOGUE_ICH_MOVE_DOWN) {
			if ((size_t) choice < (optc - 1)) {
				choice++;
			}
		} else if (ch == DIALOGUE_ICH_SELECT) {
			continue;
		} else {
			/** TODO: implement this! should be module-agnostic
			io_usererror("Input misunderstood!");
			*/
			continue;
		}
	} while (ch != DIALOGUE_ICH_SELECT);

	return choice;
}


/**
 * Execute a single input/output step for dialogue.
 */
char
dialogue_io_tick(DialogueBranch_t *branch, const char *title, int choice) {
	
	size_t sz;
	char *message;
	DialogueObject_t *obj;
	char *response;
	int line = 0;
	int ch;

	if (win == NULL) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERRORCODE_CHAR;
	}

	message = dialogue_branch_message_get(branch);
	sz = dialogue_branch_sz_get(branch);

	if (wclear(win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}

	if (mvwprintw(win, line, 0, "%s", title) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}

	/* 
	 * this because the concept padding assumes by default a minimum one newline
	 */
	line += (DIALOGUE_IO_PADDING_DIVIDER + 1);

	if (mvwprintw(win, line, 0, "%s", DIALOGUE_IO_DIVIDER) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}

	line += (DIALOGUE_IO_PADDING_DIVIDER + 1);

	if (mvwprintw(win, line, 0, "%s", message) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERRORCODE_CHAR;
	}

	line += (DIALOGUE_IO_PADDING_MESSAGE_RESPONSE + 1);

	for (int i = 0; (size_t) i < sz; i++) {
		
		if ((obj = dialogue_branch_object_get(branch, i)) == NULL) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERRORCODE_CHAR;
		}
		response = dialogue_object_response_get(obj);

		if (i == choice) {
			if (wattr_on(win, WA_REVERSE, NULL) == ERR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
			}
		}

		if (mvwprintw(win, line, 0, "%s", response) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERRORCODE_CHAR;
		}
		
		if (i == choice) {
			if (wattr_off(win, WA_REVERSE, NULL) == ERR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
			}
		}

		line += (DIALOGUE_IO_PADDING_RESPONSE_RESPONSE + 1);
	}

	if (wrefresh(win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERRORCODE_CHAR;
	}

	if ((ch = wgetch(win)) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERRORCODE_CHAR;
	}

	return (char) ch;
}
