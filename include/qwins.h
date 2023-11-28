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

/** Y-dimension of walk `WINDOW`. */
#define QWIN_WALK_WIN_SIZE_Y 40
/** X-dimension of walk `WINDOW`. */
#define QWIN_WALK_WIN_SIZE_X 120
/** Title of walk `WINDOW`. */
#define QWIN_WALK_WIN_TITLE  "OVERWORLD"

/** Y-dimension of environment log `WINDOW`. */
#define QWIN_WALK_ENVIRONMENT_LOG_WIN_SIZE_Y (LINES - QWIN_WALK_WIN_SIZE_Y)
/** X-dimension of environment log `WINDOW`. */
#define QWIN_WALK_ENVIRONMENT_LOG_WIN_SIZE_X COLS
/** Title of environment log `WINDOW`. */
#define QWIN_WALK_ENVIRONMENT_LOG_WIN_TITLE  "ENVIRONMENT LOG"

/** Y-dimension of dialogue `WINDOW`. */
#define QWIN_WALK_DIALOGUE_WIN_SIZE_Y QWIN_WALK_WIN_SIZE_Y
/** X-dimension of dialogue `WINDOW`. */
#define QWIN_WALK_DIALOGUE_WIN_SIZE_X (COLS - QWIN_WALK_WIN_SIZE_X)
/** Title of dialogue `WINDOW`. */
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



/** Initialize windows for qwalk. */
extern int qwins_walk_wins_init(
		/*@partial@*/Qwindow_t **walk_win,
		/*@partial@*/Qwindow_t **dialogue_win,
		/*@partial@*/Qwindow_t **environment_log_win);

/** Terminate windows for qwalk. */
extern void qwins_walk_wins_end(
		/*@only@*/Qwindow_t *walk_win,
		/*@only@*/Qwindow_t *dialogue_win,
		/*@only@*/Qwindow_t *environment_log_win);

/*@unused@*/
/*@only@*//*@null@*/
extern Qwindow_t *qwindow_create(char *title,
		int size_y, int size_x, int start_y, int start_x);


/*@unused@*/
extern void qwindow_destroy(/*@only@*/Qwindow_t *qwin);

extern int qwindow_noutrefresh(Qwindow_t *qwin);
extern int qwindow_noutrefresh_noborder(Qwindow_t *qwin);

extern int qwin_border_title_display(Qwindow_t *qwin);
