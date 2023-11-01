/**
 * @file io_utils.c
 * General I/O utilities.
 */



#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "ioutils.h"



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
