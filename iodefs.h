/*
 * iodefs.h
 * Header file for definitions that involve i/o genenerally, i.e. independent of
 * modes.
 */

/*
 * Type for characters that belong to tiles.
 */
typedef int IOCh_t;

/*
 * Type for holding a tile. Contains y and x coords and the io character to be
 * displayed. Intentionally abstracted from the logic half of Q.
 */
typedef struct IOTile_t {
	int    y;
	int    x;
	IOCh_t ch;
} IOTile_t;
