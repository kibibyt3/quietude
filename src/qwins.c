/**
 * @file qwins.c
 * File for controlling each I/O window for Q.
 */



#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "ioutils.h"
#include "qwins.h"



/** Splint type for `/\*@only@*\/` #Qwindow_t. */
typedef /*@only@*/Qwindow_t *OnlyQwindowp_t;
/** Splint type for `/\*@null@*\/` #Qwindow_t. */
typedef /*@null@*/Qwindow_t *NullQwindowp_t;



/**
 * Initialize windows for qwalk.
 * @param[out] walk_win: #Qwindow_t for the overworld proper.
 * @param[out] dialogue_win: #Qwindow_t for dialogue.
 * @param[out] environment_log_win: #Qwindow_t environment info.
 * @return #Q_OK or #Q_ERROR.
 */
int
qwins_walk_wins_init(NullQwindowp_t *walk_win, NullQwindowp_t *dialogue_win,
		NullQwindowp_t *environment_log_win) {
	if ((*walk_win = qwindow_create(
					QWIN_WALK_WIN_TITLE,
					QWIN_WALK_WIN_SIZE_Y,
					QWIN_WALK_WIN_SIZE_X,
					0, 0))
			== NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	if ((*dialogue_win = qwindow_create(
					QWIN_WALK_DIALOGUE_WIN_TITLE,
					QWIN_WALK_DIALOGUE_WIN_SIZE_Y,
					QWIN_WALK_DIALOGUE_WIN_SIZE_X,
					0,
					QWIN_WALK_WIN_SIZE_X))
			== NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		qwindow_destroy(*walk_win);
		return Q_ERROR;
	}

	if ((*environment_log_win = qwindow_create(
					QWIN_WALK_ENVIRONMENT_LOG_WIN_TITLE,
					QWIN_WALK_ENVIRONMENT_LOG_WIN_SIZE_Y,
					QWIN_WALK_ENVIRONMENT_LOG_WIN_SIZE_X,
					QWIN_WALK_WIN_SIZE_Y,
					0))
			== NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		qwindow_destroy(*walk_win);
		qwindow_destroy(*dialogue_win);
		return Q_ERROR;
	}

	return Q_OK;
}


/**
 * Terminate the qwalk component of qwins.
 * Namely, free all associated memory.
 * @param[out] walk_win: #Qwindow_t for the overworld proper.
 * @param[out] dialogue_win: #Qwindow_t for dialogue.
 * @param[out] environment_log_win: #Qwindow_t environment info.
 */
void
qwins_walk_wins_end(Qwindow_t *walk_win, Qwindow_t *dialogue_win,
		Qwindow_t *environment_log_win) {
	qwindow_destroy(walk_win);
	qwindow_destroy(dialogue_win);
	qwindow_destroy(environment_log_win);
	return;
}


/**
 * Create and initialize associated memory for a #Qwindow_t.
 * @param[in] title: title of the #Qwindow_t.
 * @param[in] size_y: y-dimension of the #Qwindow_t.
 * @param[in] size_x: x-dimension of the #Qwindow_t.
 * @param[in] start_y: y-coord of the top-left corner of the #Qwindow_t.
 * @param[in] start_x: x-coord of the top-left corner of the #Qwindow_t.
 * @return new #Qwindow_t or `NULL` on error.
 */
Qwindow_t *
qwindow_create(char *title, int size_y, int size_x,
		int start_y, int start_x) {

	WINDOW *border_win, *win;
	Qwindow_t *qwin;
	int title_len;

	if (strlen(title) > (size_t) size_y) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return NULL;
	}
	title_len = (int) strlen(title);

	if ((size_y < 0) || (size_x < 0)) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return NULL;
	}

	if ((border_win = newwin(size_y, size_x, start_y, start_x)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return NULL;
	}

	if ((win = derwin(border_win, size_y - 2, size_x - 2, 1, 1)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		if (delwin(border_win) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			abort();
		}
		return NULL;
	}

	if ((qwin = calloc((size_t) 1, sizeof(*qwin))) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		if (delwin(border_win) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			abort();
		}
		if (delwin(win) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			abort();
		}
		return NULL;
	}

	if ((qwin->title = calloc((size_t) title_len + 1, sizeof(*title))) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		if (delwin(border_win) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			abort();
		}
		if (delwin(win) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			abort();
		}
		free(qwin);
		return NULL;
	}

	strcpy(qwin->title, title);
	
	qwin->border_win = border_win;
	qwin->win = win;
	qwin->size_y = size_y;
	qwin->size_x = size_x;

	return qwin;
}


/**
 * Destroy a specific #Qwindow_t.
 * @param[out] qwin: #Qwindow_t to destroy.
 */
void
qwindow_destroy(Qwindow_t *qwin) {
	if (delwin(qwin->win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}
	if (delwin(qwin->border_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}
	free(qwin->title);
	free(qwin);
	return;
}


/**
 * Update the border window and window proper of @p qwin.
 * This should be called arbitrarily many times and followed by a single call
 * to `doupdate()`.
 * @param[out] qwin: #Qwindow_t to update.
 * @return #Q_OK or #Q_ERROR.
 */
int
qwindow_noutrefresh(Qwindow_t *qwin) {

	int returnval = Q_OK;
	if (qwin_border_title_display(qwin) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	if (wnoutrefresh(qwin->border_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	if (wnoutrefresh(qwin->win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}

	return returnval;
}


/**
 * Update only the window proper of @p qwin.
 * This should be called arbitrarily many times and followed by a single call
 * to `doupdate()`.
 * @param[out] qwin: #Qwindow_t to update.
 * @return #Q_OK or #Q_ERROR.
 */
int
qwindow_noutrefresh_noborder(Qwindow_t *qwin) {

	if (wnoutrefresh(qwin->win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	return Q_OK;
}


/**
 * Print the border and title for the #Qwindow_t.
 * @param[in] qwin: relevant #Qwindow_t.
 * @return #Q_OK or #Q_ERROR.
 */
int
qwin_border_title_display(Qwindow_t *qwin) {
	
	int starty, startx;
	int returnval = Q_OK;

	io_centerof(0, qwin->size_x, 0, (int) strlen(qwin->title), &starty, &startx);

	box(qwin->border_win, 0, 0);

	if ((mvwprintw(qwin->border_win, starty, startx, "%s", qwin->title)) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}

	return returnval;
}
