/**
 * @file io_utils.c
 * General I/O utilities.
 */



#include <ncurses.h>

#include "ioutils.h"



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
