/**
 * @file qwins.h
 * Header file for qwins.
 * Depends on ncurses.h and stdbool.h.
 */



/**
 * @defgroup QwinWalkMembers Qwin Walk Window Members
 * Dimensions & titles for each #Qwindow_t used for qwalk.
 * @{
 */

#define QWIN_WALK_WIN_SIZE_Y 40
#define QWIN_WALK_WIN_SIZE_X 120
#define QWIN_WALK_WIN_TITLE  "OVERWORLD"

#define QWIN_WALK_ENVIRONMENT_LOG_WIN_SIZE_Y (LINES - QWIN_WALK_WIN_SIZE_Y)
#define QWIN_WALK_ENVIRONMENT_LOG_WIN_SIZE_X COLS
#define QWIN_WALK_ENVIRONMENT_LOG_WIN_TITLE  "ENVIRONMENT LOG"

#define QWIN_WALK_DIALOGUE_WIN_SIZE_Y QWIN_WALK_WIN_SIZE_Y
#define QWIN_WALK_DIALOGUE_WIN_SIZE_X (COLS - QWIN_WALK_WIN_SIZE_X)
#define QWIN_WALK_DIALOGUE_WIN_TITLE  "DIALOGUE"

/** @} */



/**
 * Type to hold a complete window for Q.
 */
typedef struct Qwindow_t {
	WINDOW *border_win; /**< `WINDOW *` that holds the border. */
	WINDOW *win;        /**< `WINDOW *` that holds the window proper. */
	char *title;        /**< title to display on the window. */
	int size_y;         /**< Y-dimension of window. */
	int size_x;         /**< X-dimension of window. */
} Qwindow_t;



int qwins_walk_wins_init(Qwindow_t **walk_win, Qwindow_t **dialogue_win,
		Qwindow_t **environment_log_win);

/*@only@*/Qwindow_t *qwindow_create(char *title, int size_y, int size_x,
		int start_y, int start_x);

void qwindow_destroy(/*@only@*/Qwindow_t *qwin);

int qwin_border_title_display(Qwindow_t *qwin);
