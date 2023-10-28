/**
 * @file devel_walkio.c
 * I/O submodule for devel_walk.
 * Does not handle internal logic; takes in and output what it's told to.
 */



#include <stdlib.h>
#include <stddef.h>
#include <ncurses.h>
#include <cdk.h>

#include "qdefs.h"
#include "qerror.h"

#include "mode.h"
#include "qattr.h"
#include "qwalk.h"
#include "devel_walk.h"



/** Max amount of characters that can be stored in #devel_walkio_userstring. */ 
#define DEVEL_WALKIO_USERSTRING_LENGTH_MAX 1024

/** Width of window created by @ref devel_walkio_string_input_choice. */
#define DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_HEIGHT 17
/** Height of window created by @ref devel_walkio_string_input_choice. */
#define DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_WIDTH 50
/** Header message of window created by @ref devel_walkio_string_input_choice. */
#define DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_HEADER \
	"Select one of the following...\r(Arrow keys to navigate)"
/** Prompt message of window created by @ref devel_walkio_string_input_choice. */
#define DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_PROMPT "Choice: "


/**
 * @defgroup DevelWalkInputChars devel_walk Input Characters
 * Characters meant to be input by the user in devel_walk.
 */



/**
 * @ingroup DevelWalkInputChars
 * Input character for #DEVEL_WALK_CMD_CURSOR_MOVE_NORTH.
 */
#define DEVEL_WALK_ICH_CURSOR_MOVE_NORTH      'w'

/**
 * @ingroup DevelWalkInputChars
 * Input character for #DEVEL_WALK_CMD_CURSOR_MOVE_SOUTH.
 */
#define DEVEL_WALK_ICH_CURSOR_MOVE_SOUTH      's'

/**
 * @ingroup DevelWalkInputChars
 * Input character for #DEVEL_WALK_CMD_CURSOR_MOVE_EAST.
 */
#define DEVEL_WALK_ICH_CURSOR_MOVE_EAST       'd'

/**
 * @ingroup DevelWalkInputChars
 * Input character for #DEVEL_WALK_CMD_CURSOR_MOVE_WEST.
 */
#define DEVEL_WALK_ICH_CURSOR_MOVE_WEST       'a'

/**
 * @ingroup DevelWalkInputChars
 * Input character for #DEVEL_WALK_CMD_CURSOR_TOGGLE_ALTITUDE.
 */
#define DEVEL_WALK_ICH_CURSOR_TOGGLE_ALTITUDE 'z'

/**
 * @ingroup DevelWalkInputChars
 * Input character for #DEVEL_WALK_CMD_COPY.
 */
#define DEVEL_WALK_ICH_COPY                   'c'

/**
 * @ingroup DevelWalkInputChars
 * Input character for #DEVEL_WALK_CMD_PASTE.
 */
#define DEVEL_WALK_ICH_PASTE                  'v'

/**
 * @ingroup DevelWalkInputChars
 * Input character for #DEVEL_WALK_CMD_EDIT.
 */
#define DEVEL_WALK_ICH_EDIT                   'e'

/**
 * @ingroup DevelWalkInputChars
 * Input character for #DEVEL_WALK_CMD_SAVE.
 */
#define DEVEL_WALK_ICH_SAVE                   'x'

/**
 * @ingroup DevelWalkInputChars
 * Input character for #DEVEL_WALK_CMD_EXIT.
 */
#define DEVEL_WALK_ICH_EXIT                   'q'



/** The window that outputs the #QwalkArea_t.          */
/*@null@*/static WINDOW *area_win = NULL;
/** The window that outputs the relevant #QattrList_t. */
/*@null@*/static WINDOW *info_win = NULL;

/** 
 * Stores string from user. 
 */
/*@out@*/static char devel_walkio_userstring[DEVEL_WALKIO_USERSTRING_LENGTH_MAX];

/**
 * Array of all possible values in @ref QobjTypeStrings.
 */
/*@observer@*/static char *qobj_selectable_string_types[] = {
	QOBJ_STRING_TYPE_PLAYER,
	QOBJ_STRING_TYPE_GRASS,
	QOBJ_STRING_TYPE_TREE,
	QOBJ_STRING_TYPE_VOID};
/**
 * Number of possible values in @ref qobj_string_types.
 */
#define QOBJ_SELECTABLE_STRING_TYPE_COUNT 4



static int devel_walkio_area_out(const QwalkArea_t *, const int *); 
static int devel_walkio_info_out(const QwalkArea_t *, const int *);
static int devel_walkio_string_input_choice(QattrKey_t);
static DevelWalkCmd_t devel_walkio_input_to_command(int);



/**
 * Initialize devel_walkio.
 */
int
devel_walkio_init() {
	int returnval = Q_OK;

	if (initscr() == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}
	if (noecho() == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	if (cbreak() == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	return returnval;
}


/**
 * Set the devel_walkio @c WINDOW vars.
 * @param[out] area_argwin: window to output the #QwalkArea_t.
 * @param[out] info_argwin: window to output the relevant #QattrList_t.
 * return #Q_OK or #Q_ERROR.
 */
int
devel_walkio_wins_init(WINDOW *area_argwin, WINDOW *info_argwin) {
	if ((area_argwin == NULL) || (info_argwin == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if ((area_win != NULL) || (info_win != NULL)) {
		Q_ERRORFOUND(QERROR_NONNULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	area_win = area_argwin;
	info_win = info_argwin;
	return Q_OK;
}


/**
 * Terminate devel_walkio.
 */
int
devel_walkio_end() {
	if (endwin() == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}
	return Q_OK;
}


/**
 * Take input from the user.
 * Additionally, sets @ref devel_walkio_userstring where applicable 
 * for the benefit of @ref devel_walkl.c.
 * @param[in] walk_area: current #QwalkArea_t.
 * @param[in] curs_loc:  y, x, z coords of cursor.
 * @return user #DevelWalkCmd_t or #Q_ERRORCODE_ENUM.
 */
DevelWalkCmd_t
devel_walkio_in(const QwalkArea_t *walk_area, const int *curs_loc) {
	
	int ch;
	DevelWalkCmd_t cmd;
	QattrList_t *attr_list;
	int choice_index;
	QattrKey_t key;

	if ((area_win == NULL) || (info_win == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
	}
	
	ch = wgetch(area_win);
	if (ch == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
	}

	cmd = devel_walkio_input_to_command(ch);
	if (cmd == (DevelWalkCmd_t) Q_ERRORCODE_ENUM) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
	}


	/* deal with #DEVEL_WALK_CMD_EDIT */
	if (cmd == DEVEL_WALK_CMD_EDIT) {
		
		attr_list = devel_walkl_loc_attr_list_get(walk_area, curs_loc);
		if (attr_list == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
		}
		choice_index = devel_walkio_info_attr_pick(attr_list);
		key = qattr_list_attr_key_get(attr_list, choice_index);
		if (devel_walkio_string_input_choice(key) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;	
		}
	}


	return cmd;
}


/**
 * Output a #QwalkArea_t and focus object info.
 * @param[in] walk_area: relevant #QwalkArea_t.
 * @param[in] curs_loc:  y, x, and z coords of cursor.
 * @param[in] isedit:    are we editing the current square?
 * @return #Q_OK or #Q_ERROR.
 */
int
devel_walkio_out(const QwalkArea_t *walk_area, const int *curs_loc) {
	int returnval = Q_OK;
	int r;
	
	if ((area_win == NULL) || (info_win == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERROR;
	}

	r = devel_walkio_area_out(walk_area, curs_loc);
	if (r != Q_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	
	r = devel_walkio_info_out(walk_area, curs_loc);
	if (r != Q_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}

	if (wrefresh(info_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	if (wrefresh(area_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	return returnval;
}


/**
 * Output the state to the screen.
 * @param[in] walk_area: #QwalkArea_t to output to the screen.
 * @param[in] curs_loc: y, x, z coords of the cursor.
 * @return #Q_OK or #Q_ERROR.
 */
int
devel_walkio_area_out(const QwalkArea_t *walk_area, const int *curs_loc) {
	/*@observer@*/QattrList_t *layer_object_attr_list;
	/*@observer@*/Qdatameta_t *datameta_value;
	QobjType_t *obj_typep;
	chtype outch;
	int *coords;
	int r;
	int returnval = Q_OK;

	/* routine error-checking */
	if ((area_win == NULL) || (info_win == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERROR;
	}
	if ((walk_area == NULL) || (curs_loc == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if ((!(qwalk_logic_coords_arevalid(curs_loc[0], curs_loc[1])))
			|| ((curs_loc[2] != 0) && (curs_loc[2] != 1))) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}

	/*@observer@*/QwalkLayer_t *layer_earth; 
	layer_earth = qwalk_area_layer_earth_get(walk_area);
	if (layer_earth == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	
	
	/*@observer@*/QwalkLayer_t *layer_floater; 
	layer_floater = qwalk_area_layer_floater_get(walk_area);
	if (layer_floater == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	
	/*
	 * iterate through both layers and print their contents to the screen;
	 * print everything on layer_earth and then print layer_floater non-void
	 * objects
	 */
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < QWALK_LAYER_SIZE; j++) {
			if (i == 0) {
				layer_object_attr_list = qwalk_layer_object_attr_list_get(layer_earth, j);
			} else {
				layer_object_attr_list = qwalk_layer_object_attr_list_get(layer_floater, j);
			}
			if (layer_object_attr_list == NULL) {
				Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
				return Q_ERROR;
			}
	
			datameta_value = qattr_list_value_get(layer_object_attr_list, QATTR_KEY_QOBJECT_TYPE);
			if (datameta_value == NULL) {
				Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
				return Q_ERROR;
			}
	
			if (qdatameta_type_get(datameta_value) != QDATA_TYPE_QOBJECT_TYPE) {
				Q_ERRORFOUND(QERROR_QDATAMETA_TYPE_INCOMPATIBLE);
				abort();
			}
	
			obj_typep = ((QobjType_t *) (qdatameta_datap_get(datameta_value)));
			if (obj_typep == NULL) {
				Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
				return Q_ERROR;
			}
			
			/* if *obj_typep isn't an empty space, print it to the screen */
			if (*obj_typep != QOBJ_TYPE_VOID) {
				
				outch = qwalk_obj_type_to_chtype(*obj_typep);
				if (outch == (chtype) ERR) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
					abort();
				}
	
				coords = qwalk_index_to_coords(j);
				r = mvwaddch(area_win, coords[0], coords[1], outch);
				free(coords);
				if (r != OK) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
					return Q_ERROR;
				}
			}
		}
	}
	/* TODO: maybe change cursor style depending on curs_loc[2]. */
	if (wmove(area_win, curs_loc[0], curs_loc[1]) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	return returnval;
}


/**
 * Output the #QattrList_t of a specific #QwalkObj_t.
 * @param[in] walk_area: #QwalkArea_t to operate with.
 * @param[in] curs_loc:  y, x, z coords of the cursor.
 * @return #Q_OK or #Q_ERROR.
 */
int
devel_walkio_info_out(const QwalkArea_t *walk_area, const int *curs_loc) {
	
	/*@observer@*/QwalkLayer_t *layer_active;
	/*@observer@*/QattrList_t *attr_list;
	int returnval = Q_OK;
	
	/* routine error-checking */
	if ((area_win == NULL) || (info_win == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERROR;
	}
	if ((walk_area == NULL) || (curs_loc == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if ((!(qwalk_logic_coords_arevalid(curs_loc[0], curs_loc[1])))
			|| ((curs_loc[2] != 0) && (curs_loc[2] != 1))) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}

	if (wclear(info_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}

	if (curs_loc[2] == 0) {
		layer_active = qwalk_area_layer_earth_get(walk_area);
	}
	else if (curs_loc[2] == 1) {
		layer_active = qwalk_area_layer_floater_get(walk_area);
	} else {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}

	attr_list = qwalk_layer_object_attr_list_get(layer_active,
			qwalk_coords_to_index(curs_loc[0], curs_loc[1]));
	
	/* ensure attr_list is entirely filled out. */
	if (attr_list == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if (qattr_list_index_ok_get(attr_list) != qattr_list_count_get(attr_list)) {
		Q_ERRORFOUND(QERROR_STRUCT_INCOMPLETE);
		return Q_ERROR;
	}


	QattrKey_t key;
	
	/* print all key-value pairs. */
	for (int i = 0; i < (int) qattr_list_count_get(attr_list); i++) {
		
		if ((key = qattr_list_attr_key_get(attr_list, i))
				== (QattrKey_t) Q_ERRORCODE_ENUM) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return Q_ERROR;
		}

		if (wprintw(info_win, "%s : %s\n",
					qattr_key_to_string(key),
					(char *) qattr_value_to_string(attr_list, key))
				== ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			returnval = Q_ERROR;
		}
	}

	return returnval;
}


/**
 * Get @ref devel_walkio_userstring.
 * @return @ref devel_walkio_userstring.
 */
char *
devel_walkio_userstring_get() {
	return devel_walkio_userstring;
}


/**
 * Get one of a number of specific possible values for a key.
 * Set #devel_walkio_string_input_choice to the value chosen.
 * @param[in] key: key whose values are to be searched.
 * @return #Q_ERROR if @p key isn't a valid key for an input choice selection.
 */
int
devel_walkio_string_input_choice(QattrKey_t key) {
	
	/* vars */
	WINDOW *win;
	CDKSCREEN *screen;
	CDKALPHALIST *alphalist;
	/*@observer@*/char *out;
	/*@observer@*/char *list[] = {QOBJ_STRING_TYPE_PLAYER, QOBJ_STRING_TYPE_GRASS,
		QOBJ_STRING_TYPE_TREE, QOBJ_STRING_TYPE_VOID};
	int returnval = Q_OK;	
	/*@observer@*/char **options;
	int optionc;

	switch (key) {
	case QATTR_KEY_QOBJECT_TYPE:
		options = qobj_selectable_string_types;
	 	optionc = QOBJ_SELECTABLE_STRING_TYPE_COUNT;
		break;	
	}

	/* initializations */
	win = newwin(DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_HEIGHT,
			DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_WIDTH,
			(LINES - DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_HEIGHT) / 2,
			(COLS - DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_WIDTH) / 2);
	if (win == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}
	screen = initCDKScreen(win);
	alphalist = newCDKAlphalist(screen,
			0, 0, 0, 0, /* initialize alphalist such that it fills the entirety of win */
			DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_HEADER,
			DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_PROMPT,
			qobj_selectable_string_types, QOBJ_SELECTABLE_STRING_TYPE_COUNT,
			(chtype) ' ', /* filler character */
			(chtype) 0,   /* highlight        */
			true,         /* box?             */
			false);       /* shadow?          */
	
	
	if (alphalist == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	} else {
		out = activateCDKAlphalist(alphalist, NULL);
	
		/*
		 * only copy the choice into @ref devel_walkio_userstring if we exit by
		 * hitting enter
		 */
		if (alphalist->exitType == vNORMAL) {
			if (strcpy(devel_walkio_userstring, out) == NULL) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
			}
		}
		
	/* destroy everything created */
		destroyCDKAlphalist(alphalist);
	}
	destroyCDKScreen(screen);
	
	if (delwin(win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;	
	}
	/*@i3@*/return returnval;
}



/**
 * Convert a raw player input to a #DevelWalkCmd_t.
 * @param[in] ch: raw input character from e.g. `getch()`.
 * @return #DevelWalkCmd_t associated with @p ch.
 */
DevelWalkCmd_t
devel_walkio_input_to_command(int ch) {
	switch (ch) {
	
	case DEVEL_WALK_ICH_CURSOR_MOVE_NORTH:
		return DEVEL_WALK_CMD_CURSOR_MOVE_NORTH;
	
	case DEVEL_WALK_ICH_CURSOR_MOVE_SOUTH:
		return DEVEL_WALK_CMD_CURSOR_MOVE_SOUTH;
	
	case DEVEL_WALK_ICH_CURSOR_MOVE_EAST:
		return DEVEL_WALK_CMD_CURSOR_MOVE_EAST;
	
	case DEVEL_WALK_ICH_CURSOR_MOVE_WEST:
		return DEVEL_WALK_CMD_CURSOR_MOVE_WEST;
	
	case DEVEL_WALK_ICH_CURSOR_TOGGLE_ALTITUDE:
		return DEVEL_WALK_CMD_CURSOR_TOGGLE_ALTITUDE;
	
	case DEVEL_WALK_ICH_COPY:
		return DEVEL_WALK_CMD_COPY;
	
	case DEVEL_WALK_ICH_PASTE:
		return DEVEL_WALK_CMD_PASTE;
	
	case DEVEL_WALK_ICH_EDIT:
		return DEVEL_WALK_CMD_EDIT;
	
	case DEVEL_WALK_ICH_SAVE:
		return DEVEL_WALK_CMD_SAVE;
	
	case DEVEL_WALK_ICH_EXIT:
		return DEVEL_WALK_CMD_EXIT;
	
	default:
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
	}
}
