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


/**
 * @defgroup IOChoiceFromSelection I/O Choice from Selection
 * Group @ref io_choice_from_selection.
 * Namely associated references and defines.
 * @{
 */
/** Prompt the user to make a choice from multiple options. */
extern int io_choice_from_selection(int optc, const char **optv,
		const char *title);
/** Height of the menu. */
#define IO_CHOICE_MENU_HEIGHT 30
/** Width of the menu. */
#define IO_CHOICE_MENU_WIDTH  60
/** Key to move up. */
#define IO_CHOICE_MENU_ICH_MOVE_UP 'w'
/** Alternate to #IO_CHOICE_MENU_ICH_MOVE_UP. */
#define IO_CHOICE_MENU_ICH_MOVE_UP_ALT KEY_UP
/** Key to move down. */
#define IO_CHOICE_MENU_ICH_MOVE_DOWN 's'
/** Alternate to #IO_CHOICE_MENU_ICH_MOVE_DOWN. */
#define IO_CHOICE_MENU_ICH_MOVE_DOWN_ALT KEY_DOWN
/** Key to confirm choice. */
#define IO_CHOICE_MENU_ICH_CONFIRM_CHOICE '\n'
/** @} */


/** Find the coordinates at the center of two coordinates.  */
/*@unused@*/
extern void io_centerof(int, int, int, int, /*@out@*/int *, /*@out@*/int *);

/** Change cursor style.                                    */
/*@unused@*/
extern int curs_style_set(CursStyle_t);

/** Trim redundant whitespace in a string.                  */
extern int io_whitespace_trim(/*@partial@*/char *);
