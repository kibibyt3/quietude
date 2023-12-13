/**
 * @file qutils.c
 * Program file for qutils.
 * General utilities for Q.
 */



#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include <limits.h>
#include <time.h>
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
 * Seed @ref qutils_nobias_rand().
 * This should only be done once.
 */
void
qutils_nobias_srand() {
	srand((unsigned) time(0));
}
