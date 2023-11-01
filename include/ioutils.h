/**
 * @file ioutils.h
 * Header file for ioutils.
 */

/** Find the coordinates at the center of two coordinates. */
/*@unused@*/
extern void io_centerof(int, int, int, int, int *, int *);

/** Trim redundant whitespace in a string.                 */
extern int io_whitespace_trim(/*@partial@*/char *);
