/**
 * @file ioutils.h
 * Header file for ioutils.
 */



/**
 * Type to refer to cursor styles.
 */
typedef enum CursStyle_t {
	CURS_STYLE_BLINKING_BLOCK = Q_ENUM_VALUE_START, /**< Blinking block. */
	CURS_STYLE_STEADY_BLOCK, /**< Steady block.       */
	CURS_STYLE_BLINKING_UL,  /**< Blinking underline. */
	CURS_STYLE_STEADY_UL,    /**< Steady underline.   */
	CURS_STYLE_BLINKING_BAR, /**< Blinking bar.       */
	CURS_STYLE_STEADY_BAR,   /**< Steady bar.         */

	/** Number of possible values for a #CursStyle_t. */
	CURS_STYLE_COUNT = CURS_STYLE_STEADY_BAR

} CursStyle_t;



/** Find the coordinates at the center of two coordinates. */
/*@unused@*/
extern void io_centerof(int, int, int, int, /*@out@*/int *, /*@out@*/int *);

/** Change cursor style.                                   */
/*@unused@*/
extern int curs_style_set(CursStyle_t);

/** Trim redundant whitespace in a string.                 */
extern int io_whitespace_trim(/*@partial@*/char *);
