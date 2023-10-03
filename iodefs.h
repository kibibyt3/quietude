/**
 * @file iodefs.h
 * Header file for definitions that involve i/o genenerally, i.e. independent of
 * modes.
 */

/**
 * Type for characters that belong to tiles.
 */
typedef int IOCh_t;

/**
 * Type for holding a tile.
 * Contains y and x coords and the io character to be
 * displayed. Intentionally abstracted from the logic half of Q.
 */
typedef struct IOTile_t {
	int    y;  /**< Y coord of the tile                 */
	int    x;  /**< X coord of the tile                 */
	IOCh_t ch; /**< #IOCh_t to be displayed on the tile */
} IOTile_t;
