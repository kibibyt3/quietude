/**
 * @file qwalkio.c
 * Program file for the I/O section of the qwalk module.
 */



#include <ncurses.h>

#include "qdefs.h"
#include "qerror.h"

#include "qattr.h"
#include "mode.h"
#include "qwalk.h"



/** Character for #QWALK_COMMAND_MOVE_NORTH */
#define QWALK_CH_MOVE_NORTH 'w'
/** Character for #QWALK_COMMAND_MOVE_EAST  */
#define QWALK_CH_MOVE_EAST  'd'
/** Character for #QWALK_COMMAND_MOVE_SOUTH */
#define QWALK_CH_MOVE_SOUTH 's'
/** Character for #QWALK_COMMAND_MOVE_WEST  */
#define QWALK_CH_MOVE_WEST  'a'
/** Character for #QWALK_COMMAND_WAIT       */
#define QWALK_CH_WAIT       '.'



/** qwalk's IO window. */
/*@null@*/static WINDOW *win = NULL;



static QwalkCommand_t qwalk_input_to_command(int)/*@*/;



/**
 * Initialize the IO module of qwalk.
 * @param[out] win: @c WINDOW to set as the qwalk window
 * @return #Q_OK or #Q_ERROR.
 */
int
qwalk_io_init(WINDOW *argwin) {
	if (argwin == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	win = argwin;
	return Q_OK;
}


/**
 * Pass the subtick step of getting player input.
 * @return #QwalkCommand_t associated with player input.
 */
QwalkCommand_t
qwalk_input_subtick() {
	if (win == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return (QwalkCommand_t) Q_ERRORCODE_ENUM;
	}
	int ch;
	QwalkCommand_t cmd;
	ch = wgetch(win);
	cmd = qwalk_input_to_command(ch);
	if (cmd == (QwalkCommand_t) Q_ERRORCODE_ENUM) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return (QwalkCommand_t) Q_ERRORCODE_ENUM;
	}
	return cmd;
}


/**
 * Pass the subtick step of outputting the game state.
 * @return #Q_OK or #Q_ERROR.
 */
/* TODO: create */
int
qwalk_output_subtick() {
	if (win == NULL) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERROR;
	}

	/* ... */

	return Q_OK;
}


/**
 * Convert a raw player input to a #QwalkCommand_t.
 * @param[in] ch: raw input character from e.g. getch().
 * @return #QwalkCommand_t associated with @p ch.
 */
QwalkCommand_t
qwalk_input_to_command(int ch) {
	switch (ch) {
	case QWALK_CH_MOVE_NORTH:
		return QWALK_COMMAND_MOVE_NORTH;
	case QWALK_CH_MOVE_EAST:
		return QWALK_COMMAND_MOVE_EAST;
	case QWALK_CH_MOVE_SOUTH:
		return QWALK_COMMAND_MOVE_SOUTH;
	case QWALK_CH_MOVE_WEST:
		return QWALK_COMMAND_MOVE_WEST;
	case QWALK_CH_WAIT:
		return QWALK_COMMAND_WAIT;
	default:

		return (QwalkCommand_t) Q_ERRORCODE_ENUM;
	}
}
