/**
 * @file ioutils.c
 * General I/O utilities.
 */



#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "ioutils.h"



/**
 * @defgroup CursEscSeqs Cursor Escape Sequences
 * Escape sequences to change the terminal cursor.
 * @{
 */

/** Escape sequence for #CURS_STYLE_BLINKING_BLOCK.	*/
#define CURS_STYLE_STRING_BLINKING_BLOCK "\033[\x32 q"
/** Escape sequence for #CURS_STYLE_STEADY_BLOCK. 	*/
#define CURS_STYLE_STRING_STEADY_BLOCK   "\033[\x31 q"
/** Escape sequence for #CURS_STYLE_BLINKING_UL.		*/
#define CURS_STYLE_STRING_BLINKING_UL    "\033[\x34 q"
/** Escape sequence for #CURS_STYLE_STEADY_UL. 			*/
#define CURS_STYLE_STRING_STEADY_UL      "\033[\x33 q"
/** Escape sequence for #CURS_STYLE_BLINKING_BAR. 	*/
#define CURS_STYLE_STRING_BLINKING_BAR   "\033[\x36 q"
/** Escape sequence for #CURS_STYLE_STEADY_BAR. 		*/
#define CURS_STYLE_STRING_STEADY_BAR     "\033[\x35 q"

/** @} */



/*@external@*/
extern int /*@alt void@*/wborder(WINDOW *, chtype, chtype, chtype, chtype, chtype, chtype, chtype, chtype);



/**
 * Prompt the user to make a choice from multiple options.
 * Create the menu thereof in the center of the screen.
 * @param[in] optc: number of strings in @p optv.
 * @param[in] optv: array of strings that comprise the options.
 * @param[in] title: title to place on the menu.
 * @return the selected option or #Q_ERRORCODE_INT.
 */
int
io_choice_from_selection(int optc, const char **optv, const char *title) {
	WINDOW *win, *border_win;
	int border_starty;
	int border_startx;

	int border_height = IO_CHOICE_MENU_HEIGHT + 2;
	int border_width = IO_CHOICE_MENU_WIDTH + 2;

	size_t title_len = strlen(title);

	if (optc > IO_CHOICE_MENU_HEIGHT) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERRORCODE_INT;
	}

	if (title_len > (size_t) IO_CHOICE_MENU_WIDTH) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERRORCODE_INT;
	}

	io_centerof(LINES, COLS, border_height, border_width,
			&border_starty, &border_startx);

	if ((border_win = newwin(border_height, border_width, border_starty,
					border_startx)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERRORCODE_INT;
	}

	box(border_win, 0, 0);

	/* print the title of the window on the border */
	io_centerof(0, border_height, 0, (int) title_len,
			&border_starty, &border_startx);
	if (mvwprintw(border_win, border_starty, border_startx, "%s", title) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}

	if ((win = derwin(border_win, IO_CHOICE_MENU_HEIGHT, IO_CHOICE_MENU_WIDTH,
					1, 1)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		if (delwin(border_win) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			abort(); /* this would lead to a memory leak */
		}
		return Q_ERRORCODE_INT;
	}

	if (keypad(win, TRUE) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}

	int choice = 0;
	int ch;

	if (wrefresh(border_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}
	if (curs_set(0) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}
	do {
		for (int i = 0; i < optc; i++) {
			if (strlen(optv[i]) > (size_t) IO_CHOICE_MENU_WIDTH) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
			
				if (delwin(win) == ERR) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
					abort();
				}

				if (delwin(border_win) == ERR) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
					abort();
				}

				if (curs_set(1) == ERR) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
				}

				return Q_ERRORCODE_INT;
			}

			if (i == choice) {
				if (wattr_on(win, WA_REVERSE, NULL) == ERR) {
					Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
				}
			}

			if (mvwprintw(win, i, 0, "%s", optv[i]) == ERR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
			}

			if (i == choice) {
				if (wattr_off(win, WA_REVERSE, NULL) == ERR) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
				}
			}
		}
		if (wrefresh(win) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
		if ((ch = wgetch(win)) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
		if (wclear(win) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
		/* handle controls */
		switch (ch) {
		case IO_CHOICE_MENU_ICH_MOVE_UP:
		/*@fallthrough@*/
		case IO_CHOICE_MENU_ICH_MOVE_UP_ALT:
			if (choice > 0) {
				choice--;
			}
			break;
		case IO_CHOICE_MENU_ICH_MOVE_DOWN:
		/*@fallthrough@*/
		case IO_CHOICE_MENU_ICH_MOVE_DOWN_ALT:
			if (choice < (optc - 1)) {
				choice++;
			}
			break;
		case IO_CHOICE_MENU_ICH_CONFIRM_CHOICE:
			break;
		default:
			break;
		}

	} while (ch != (int) IO_CHOICE_MENU_ICH_CONFIRM_CHOICE);

	if (delwin(win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}
	if (delwin(border_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}
	return choice;
}



/**
 * Trim leading, trailing, and excess whitespace.
 * e.g. `" This     is a   string. "` -> `"This is a string."`.
 * @param[out] s: string to edit. Must contain null-terminating character.
 * @return #Q_OK or #Q_ERROR.
 */
int
io_whitespace_trim(char *s) {
	char ch = (char) 1; /* default value; only used to compare against '\0' */
	int returnval = Q_OK;
	bool prev_isspace = false;
	int snew_index = 0;
	size_t slen;

	if (s == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}

	/* Check if string is empty. */
	if ((slen = strlen(s) + 1) == (size_t) 1) {
		return Q_OK;
	}
	
	char snew[slen];     /* stores new string            */
	snew[0] = '\0';      /* with a null-terminating char */

	int i;
	
	/* start by removing excess whitepace between words and leading whitespace */
	for (i = 0; (ch = s[i]) != '\0'; i++) {
		if ((isspace((int) ch) != 0)) {
			if ((!prev_isspace) && (i != 0)) {
				snew[snew_index++] = ch;
			}
			prev_isspace = true;
		} else {
			snew[snew_index++] = ch;
			prev_isspace = false;
		}
	}

	/* remove trailing whitespace and add null-terminating character */
	if (prev_isspace) {
		snew[snew_index - 1] = '\0';
	}	else {
		snew[snew_index] = '\0';
	}

	/* copy the results into s */
	strcpy(s, snew);

	return returnval;
}


/**
 * Set the cursor style.
 * @param[in] curs_style: cursor style to use.
 */
int
curs_style_set(CursStyle_t curs_style) {
	
	switch (curs_style) {
	case CURS_STYLE_BLINKING_BLOCK:
		if (printf(CURS_STYLE_STRING_BLINKING_BLOCK) < 0) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}	
		break;
	case CURS_STYLE_STEADY_BLOCK:
		if (printf(CURS_STYLE_STRING_STEADY_BLOCK) < 0) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		break;
	case CURS_STYLE_BLINKING_UL:
		if (printf(CURS_STYLE_STRING_BLINKING_UL) < 0) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		break;
	case CURS_STYLE_STEADY_UL:
		if (printf(CURS_STYLE_STRING_STEADY_UL) < 0) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		break;
	case CURS_STYLE_BLINKING_BAR:
		if (printf(CURS_STYLE_STRING_BLINKING_BAR) < 0) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		break;
	case CURS_STYLE_STEADY_BAR:
		if (printf(CURS_STYLE_STRING_STEADY_BAR) < 0) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}
		break;
	default:
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}
	if (fflush(stdout) != 0) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
	}
	return Q_OK;
}


/**
 * Find the coordinates at the center of two coordinates.
 * @param[in] y: y coordinate to find the center of.
 * @param[in] x: x coordinate to find the center of.
 * @param[in] y_offset: amount of offset for @p y.
 * @param[in] x_offset: amount of offset for @p x.
 * @param[out] y_center: center of @p y.
 * @param[out] x_center: center of @p x.
 */
void
io_centerof(int y, int x, int y_offset, int x_offset, int *y_center, int *x_center) {
	*y_center = (y - y_offset) / 2;
	*x_center = (x - x_offset) / 2;
	return;
}
