/**
 * @file ioutils.h
 * Header file for ioutils.
 */

/** Find the coordinates at the center of two coordinates. */
extern void io_centerof(int, int, int, int, /*@out@*/int *, /*@out@*/int *);

/** Trim redundant whitespace in a string.                 */
int io_whitespace_trim(/*@out@*/char *);
