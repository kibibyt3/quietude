/**
 * @file qutils.c
 * Program file for qutils.
 * General utilities for Q.
 */



#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "qdefs.h"
#include "qerror.h"
#include "qutils.h"



/**
 * Calculate the absolute value of the distance between two sets of coordinates.
 * @param[in] starty: starting y-coord (i.e. the minuend y-coord).
 * @param[in] startx: starting x-coord (i.e. the minuend x-coord).
 * @param[in] endy: ending y-coord (i.e. the subtrahend y-coord).
 * @param[in] endx: ending x-coord (i.e. the subtrahend x-coord).
 * @return requested distance or #Q_ERRORCODE_INT if any of the arguments are
 * too large.
 */
int
qutils_distance_calculate(int starty, int startx, int endy, int endx) {

	static int int_predelta_max = INT_MAX / 2;
	int int_delta_max = ((int) (sqrt((double) INT_MAX))) / 2;

	if ((abs(starty) > int_predelta_max) || (abs(startx) > int_predelta_max)
			|| (abs(endy) > int_predelta_max) || (abs(endx) > int_predelta_max)) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERRORCODE_INT;
	}

	/* try to avoid using the pythagorean theorem */
	if (starty == endy) {
		return abs(startx - endx);
	} else if (startx == endx) {
		return abs(starty - endy);
	}

	int deltax, deltay;

	deltax = abs(startx - endx);
	deltay = abs(starty - endy);

	if ((deltax > int_delta_max) || (deltay > int_delta_max)) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERRORCODE_INT;
	}

	return (int) sqrt((double) (deltax * deltax) + (deltay * deltay));
}


/**
 * Create a randomized string of characters.
 * @param[in] alphabet: alphabet of characters to pull from.
 * @param[in] len: length of the string to generate.
 * @return randomized string of characters from @p alphabet of length @p len,
 * or `NULL` on error.
 */
char *
qutils_nobias_randstring(const char *alphabet, int len) {
	char *s;

	if ((s = calloc((size_t) len + (size_t) 1, sizeof(*s))) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		return NULL;
	}

	int alphabet_len = (int) strlen(alphabet);
	for (int i = 0; i < len; i++) {
		s[i] = alphabet[qutils_nobias_rand(alphabet_len - 1)];
	}

	s[len] = '\0';

	return s;
}


/**
 * Generate a pseudo-random number.
 * Specifically avoids some bias introduced by `rand()`.
 * @param[in] i: upper bound of result.
 * @return pseudo-random number between 0 and @p, inclusive.
 */
int
qutils_nobias_rand(int i) {
	int rawval;

	int rand_max_remainder = RAND_MAX % (i + 1);

	do {
		rawval = rand();
	} while (rawval > (RAND_MAX - rand_max_remainder));

	return rawval % (i + 1);
}


/**
 * Seed the qutils_rand class of functions.
 * This should only be done once.
 */
void
qutils_nobias_srand() {
	srand((unsigned) time(0));
}


/**
 * Check if two given `double` values are equal.
 * Uses #QUTILS_DOUBLES_AREEQUAL_TOLERANCE as a tolerance for comparison.
 * @param [in] lhd: left-hand `double` to compare.
 * @param [in] rhd: right-hand `double` to compare.
 * @return `false` if @p lhd and @p rhd compare unequal, and `true` otherwise.
 */
bool
qutils_doubles_areequal(double lhd, double rhd) {
	if (fabs(lhd - rhd) > QUTILS_DOUBLES_AREEQUAL_TOLERANCE) {
		return false;
	}
	return true;
}
