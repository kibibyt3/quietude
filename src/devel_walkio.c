/**
 * @file devel_walkio.c
 * I/O submodule for devel_walk.
 * Does not handle internal logic; takes in and output what it's told to.
 */



#include <stdlib.h>
#include <stddef.h>
#include <locale.h>
#include <ncurses.h>
#include <form.h>
#include <cdk.h>

#include "qdefs.h"
#include "iodefs.h"
#include "ioutils.h"
#include "qerror.h"

#include "mode.h"
#include "qattr.h"
#include "qwalk.h"
#include "devel_walk.h"




/** Width of window created by @ref devel_walkio_string_input_choice().      */
#define DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_HEIGHT 40
/** Height of window created by @ref devel_walkio_string_input_choice().     */
#define DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_WIDTH 90
/** Width of window created by @ref devel_walkio_string_input_raw().         */
#define DEVEL_WALKIO_STRING_INPUT_RAW_WIN_HEIGHT 25
/** Height of window created by @ref devel_walkio_string_input_raw().        */
#define DEVEL_WALKIO_STRING_INPUT_RAW_WIN_WIDTH 120

/** Title of area_win when operating on #QWALK_LAYER_TYPE_EARTH.             */
#define DEVEL_WALKIO_AREA_TITLE_LAYER_EARTH   "EARTH LAYER"
/** Title of area_win when operating on #QWALK_LAYER_TYPE_FLOATER.           */
#define DEVEL_WALKIO_AREA_TITLE_LAYER_FLOATER "FLOATER LAYER"

/** Title of the window created by @ref devel_walkio_string_input_raw().     */
#define DEVEL_WALKIO_STRING_INPUT_RAW_WIN_TITLE \
	"INPUT REQUESTED"


/**
 * Header message of window created by @ref devel_walkio_string_input_choice(). 
 */
#define DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_HEADER \
	"Select one of the following... (Arrow keys to navigate)"
/**
 * Prompt message of window created by @ref devel_walkio_string_input_choice().
 */
#define DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_PROMPT "Choice: "



/**
 * @addtogroup DevelWalkInputChars devel_walk Input Characters
 * Characters meant to be input by the user in devel_walk.
 * @{
 */

/**
 * Input character for #DEVEL_WALK_CMD_CURSOR_MOVE_NORTH.
 */
#define DEVEL_WALK_ICH_CURSOR_MOVE_NORTH      'w'

/** Alternate to #DEVEL_WALK_ICH_CURSOR_MOVE_NORTH. */
#define DEVEL_WALK_ICH_CURSOR_MOVE_NORTH_ALT KEY_UP

/**
 * Input character for #DEVEL_WALK_CMD_CURSOR_MOVE_SOUTH.
 */
#define DEVEL_WALK_ICH_CURSOR_MOVE_SOUTH      's'

/** Alternate to #DEVEL_WALK_ICH_CURSOR_MOVE_SOUTH. */
#define DEVEL_WALK_ICH_CURSOR_MOVE_SOUTH_ALT KEY_DOWN

/**
 * Input character for #DEVEL_WALK_CMD_CURSOR_MOVE_EAST.
 */
#define DEVEL_WALK_ICH_CURSOR_MOVE_EAST       'd'

/** Alternate to #DEVEL_WALK_ICH_CURSOR_MOVE_EAST. */
#define DEVEL_WALK_ICH_CURSOR_MOVE_EAST_ALT KEY_RIGHT

/**
 * Input character for #DEVEL_WALK_CMD_CURSOR_MOVE_WEST.
 */
#define DEVEL_WALK_ICH_CURSOR_MOVE_WEST       'a'

/** Alternate to #DEVEL_WALK_ICH_CURSOR_MOVE_WEST. */
#define DEVEL_WALK_ICH_CURSOR_MOVE_WEST_ALT KEY_LEFT

/**
 * Input character for #DEVEL_WALK_CMD_CURSOR_TOGGLE_ALTITUDE.
 */
#define DEVEL_WALK_ICH_CURSOR_TOGGLE_ALTITUDE 'z'

/**
 * Input character for #DEVEL_WALK_CMD_COPY.
 */
#define DEVEL_WALK_ICH_COPY                   'c'

/**
 * Input character for #DEVEL_WALK_CMD_PASTE.
 */
#define DEVEL_WALK_ICH_PASTE                  'v'

/**
 * Input character for #DEVEL_WALK_CMD_ATTR_DELETE.
 */
#define DEVEL_WALK_ICH_ATTR_DELETE            'r'

/**
 * Input character for #DEVEL_WALK_CMD_ATTR_INSERT.
 */
#define DEVEL_WALK_ICH_ATTR_INSERT            'i'

/**
 * Input character for #DEVEL_WALK_CMD_EDIT.
 */
#define DEVEL_WALK_ICH_EDIT                   'e'

/**
 * Input character for #DEVEL_WALK_CMD_SAVE.
 */
#define DEVEL_WALK_ICH_SAVE                   'x'

/**
 * Input character for #DEVEL_WALK_CMD_EXIT.
 */
#define DEVEL_WALK_ICH_EXIT                   'q'

/** @} */



/** The window that outputs the #QwalkArea_t.          */
/*@null@*/static WINDOW *area_win = NULL;
/** The window that outputs the relevant #QattrList_t. */
/*@null@*/static WINDOW *info_win = NULL;
/** The window to draw the border for #area_win.       */
/*@null@*/static WINDOW *area_border_win = NULL;
/** The window to draw the border for #info_win.       */
/*@null@*/static WINDOW *info_border_win = NULL;

/** 
 * Stores string from user. 
 */
static char devel_walkio_userstring[DEVEL_WALKIO_USERSTRING_LENGTH_MAX] = 
/*@i1@*/DEVEL_WALKIO_USERSTRING_DEFAULT;

/**
 * Stores int from user.
 */
static int devel_walkio_userint = Q_ERROR;

/**
 * Array of all possible values in @ref ObjTypeStrings.
 */
/*@observer@*/static char *qobj_selectable_string_types[] = {
	QOBJ_STRING_TYPE_PLAYER,
	QOBJ_STRING_TYPE_GRASS,
	QOBJ_STRING_TYPE_TREE,
	QOBJ_STRING_TYPE_VOID};

/**
 * Number of possible values in @ref ObjTypeStrings.
 */
#define QOBJ_SELECTABLE_STRING_TYPE_COUNT 4


/**
 * Array of all possible values for a user-facing #QattrKey_t.
 */
/*@observer@*/static const QattrKey_t qattr_selectable_keys[] = {
	QATTR_KEY_QOBJECT_TYPE,
	QATTR_KEY_NAME,
	QATTR_KEY_DESCRIPTION_BRIEF,
	QATTR_KEY_DESCRIPTION_LONG,
	QATTR_KEY_CANMOVE
};

/**
 * Number of elements in #qattr_selectable_keys.
 */
#define QATTR_SELECTABLE_KEYS_COUNT 5


/**
 * Array of all possible values for a @c bool.
 */
/*@observer@*/static char *selectable_bool_types[] = {
	BOOL_STRING_FALSE,
	BOOL_STRING_TRUE};

/**
 * Number of possible values for a @c bool.
 */
#define SELECTABLE_BOOL_TYPES_COUNT 2



/**
 * Mode for @ref devel_walkio_info_out() to operate in.
 */
typedef enum DevelWalkIOInfoOutMode_t {
	
	/** Mode to merely display the relevant #QattrList_t. */
	DEVEL_WALKIO_INFO_OUT_MODE_DISPLAY = Q_ENUM_VALUE_START,

	/** 
	 * Mode to have the user select a specific #QattrKey_t from the #QattrList_t.
	 */
	DEVEL_WALKIO_INFO_OUT_MODE_CHOICE,

	/** Total number of possible values for a #DevelWalkIOInfoOutMode_t. */
	DEVEL_WALKIO_INFO_OUT_MODE_COUNT = DEVEL_WALKIO_INFO_OUT_MODE_CHOICE

} DevelWalkIOInfoOutMode_t;



/** for splint's benefit */
/*@external@*/
extern int wattr_on(WINDOW *, attr_t, /*@null@*/void *);



static int devel_walkio_area_out(const QwalkArea_t *, const int *); 
static int devel_walkio_info_out(const QwalkArea_t *, const int *, DevelWalkIOInfoOutMode_t);
static int devel_walkio_string_input_raw(const char *);
static int devel_walkio_string_input_choice(QattrKey_t);
static DevelWalkCmd_t devel_walkio_input_to_command(int);



/**
 * Initialize devel_walkio.
 */
int
devel_walkio_init() {
	int returnval = Q_OK;

	/*
	 * ensure #DEVEL_WALKIO_USERSTRING_LENGTH_MAX can hold the entire
	 * @ref devel_walkio_string_input_raw() window.
	 */
	if (DEVEL_WALKIO_USERSTRING_LENGTH_MAX <
			(DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_HEIGHT *
			 DEVEL_WALKIO_STRING_INPUT_CHOICE_WIN_WIDTH)) {
		Q_ERRORFOUND(QERROR_BADDEFINE);
	}
	if (setlocale(LC_ALL, "") == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
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
	if (curs_set(1) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	return returnval;
}


/**
 * Set the devel_walkio @c WINDOW vars.
 * @param[in] area_argwin: window to output the #QwalkArea_t.
 * @param[in] info_argwin: window to output the relevant #QattrList_t.
 * @param[in] area_border_argwin: window to draw the border for @p area_argwin.
 * @param[in] info_border_argwin: window to draw the border for @p info_argwin.
 * return #Q_OK or #Q_ERROR.
 */
int
devel_walkio_wins_init(WINDOW *area_argwin, WINDOW *area_border_argwin,
		WINDOW *info_argwin, WINDOW *info_border_argwin) {
	if ((area_argwin == NULL) || (info_argwin == NULL) ||
			(area_border_argwin == NULL) || (info_border_argwin == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	if ((area_win != NULL) || (info_win != NULL) ||
			(area_border_win != NULL) || (info_border_win != NULL)) {
		Q_ERRORFOUND(QERROR_NONNULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	area_win = area_argwin;
	info_win = info_argwin;
	area_border_win = area_border_argwin;
	info_border_win = info_border_argwin;
	if ((keypad(area_win, true) == ERR) || (keypad(info_win, true) == ERR)) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}
	return Q_OK;
}


/**
 * Terminate devel_walkio.
 */
int
devel_walkio_end() {

	if (curs_style_set(CURS_STYLE_BLINKING_BLOCK) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}
	
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
	int r;
	DevelWalkCmd_t cmd;
	QattrList_t *attr_list;
	QattrKey_t key;
	char *init_str;
	CursStyle_t curs_style = CURS_STYLE_BLINKING_BLOCK;
	
	if ((area_win == NULL) || (info_win == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
	}
	
	/* change cursor style depending on current layer. */
	if (curs_loc[2] == 0) {
		curs_style = CURS_STYLE_BLINKING_UL;
	} else {
		curs_style = CURS_STYLE_BLINKING_BLOCK;
	}
	if (curs_style_set(curs_style) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
	}

	/* get input and clear message */
	ch = wgetch(area_win);
	if (devel_walkio_message_print(NULL) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return (DevelWalkCmd_t) Q_ERROR;
	}
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

		if (curs_set(0) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
		}
		attr_list = devel_walkl_loc_attr_list_get(walk_area, curs_loc);
		if (attr_list == NULL) {
			Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
			return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
		}
		if (devel_walkio_info_out(walk_area, curs_loc, DEVEL_WALKIO_INFO_OUT_MODE_CHOICE) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
		}
		key = qattr_list_attr_key_get(attr_list, devel_walkio_userint);
		
		/* handle keys with specific possible input values */
		if ((key == QATTR_KEY_QOBJECT_TYPE) || (key == QATTR_KEY_CANMOVE)) {
			if ((r = devel_walkio_string_input_choice(key)) == Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
			} else if (r == Q_ERROR_NOCHANGE) {
				/* tell logic not to alter anything if user exited w/o saving */
				cmd = DEVEL_WALK_CMD_WAIT;
			}
			if (curs_set(1) == ERR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
			}
		} else if ((key == QATTR_KEY_NAME) || (key == QATTR_KEY_DESCRIPTION_BRIEF)
				|| (key == QATTR_KEY_DESCRIPTION_LONG)) {
			/* handle keys that take arbitrary string input */
			
			if (curs_set(1) == ERR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
			}
			if ((init_str = qattr_value_to_string(attr_list, key)) == NULL) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
			}

			if ((r = devel_walkio_string_input_raw(init_str)) == Q_ERROR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
			} else if (r == Q_ERROR_NOCHANGE) {
				/* tell logic not to alter anything if user exited w/o saving */
				cmd = DEVEL_WALK_CMD_WAIT;
			}
		}

	} else if (cmd == DEVEL_WALK_CMD_ATTR_DELETE) {
		/* deal with DEVEL_WALK_CMD_ATTR_DELETE */
		
		if (curs_set(0) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
		}
		if (devel_walkio_info_out(
					walk_area, curs_loc, DEVEL_WALKIO_INFO_OUT_MODE_CHOICE) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
		}
		if (curs_set(1) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
		}
	} else if (cmd == DEVEL_WALK_CMD_ATTR_INSERT) {
		/* deal with the command to insert attributes */
		const char *qattr_selectable_keys_strings[QATTR_SELECTABLE_KEYS_COUNT];
		int choice;
		for (int i = 0; i < QATTR_SELECTABLE_KEYS_COUNT; i++) {
			qattr_selectable_keys_strings[i] =
				qattr_key_to_string(qattr_selectable_keys[i]);
		}
		choice = io_choice_from_selection(QATTR_SELECTABLE_KEYS_COUNT,
				qattr_selectable_keys_strings, DEVEL_WALKIO_CHOICE_MENU_TITLE);
		if (choice == Q_ERRORCODE_INT) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			return (DevelWalkCmd_t) Q_ERRORCODE_ENUM;
		}
		devel_walkio_userint = choice;
	}

	return cmd;
}


/**
 * Output a #QwalkArea_t and focus object info.
 * @param[in] walk_area: relevant #QwalkArea_t.
 * @param[in] curs_loc:  y, x, and z coords of cursor.
 * @return #Q_OK or #Q_ERROR.
 */
int
devel_walkio_out(const QwalkArea_t *walk_area, const int *curs_loc) {
	int returnval = Q_OK;
	int r;
	int y_center;
	int x_center;
	char area_title[DEVEL_WALK_AREA_WIN_BORDER_COLS];
	int area_title_len;

	if ((area_win == NULL) || (info_win == NULL)
			|| (area_border_win == NULL) || (info_border_win == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERROR;
	}

	r = devel_walkio_area_out(walk_area, curs_loc);
	if (r != Q_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	
	r = devel_walkio_info_out(walk_area, curs_loc, DEVEL_WALKIO_INFO_OUT_MODE_DISPLAY);
	if (r != Q_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}


	/* print dynamic layer title in title of area_win */
	if (curs_loc[2] == 0) {
		strcpy(area_title, DEVEL_WALKIO_AREA_TITLE_LAYER_EARTH);
	} else {
		strcpy(area_title, DEVEL_WALKIO_AREA_TITLE_LAYER_FLOATER);
	}
	area_title_len = (int) strlen(area_title);
	io_centerof(0, DEVEL_WALK_AREA_WIN_BORDER_COLS, 0, area_title_len, &y_center, &x_center);
	box(area_border_win, 0, 0);
	if (mvwprintw(area_border_win, y_center, x_center, "%s", area_title) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	
	box(info_border_win, 0, 0);
	
	if (wmove(area_win, curs_loc[0], curs_loc[1]) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	

	if (wrefresh(info_border_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	if (wrefresh(area_border_win) == ERR) {
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
			
			/* if *obj_typep isn't a layer_floater void, print it to the screen */
			if ((*obj_typep != QOBJ_TYPE_VOID) || (i != 1)) {
				
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
	return returnval;
}


/**
 * Output the #QattrList_t of a specific #QwalkObj_t.
 * @param[in] walk_area: #QwalkArea_t to operate with.
 * @param[in] curs_loc:   y, x, z coords of the cursor.
 * @param[in] mode:      #DevelWalkIOInfoOutMode_t to operate in.
 * @return #Q_OK or #Q_ERROR.
 */
int
devel_walkio_info_out(const QwalkArea_t *walk_area, const int *curs_loc, DevelWalkIOInfoOutMode_t mode) {
	
	/*@observer@*/QattrList_t *attr_list;
	int returnval = Q_OK;

	/* routine validation */
	if ((mode < (DevelWalkIOInfoOutMode_t) Q_ENUM_VALUE_START) || (mode > DEVEL_WALKIO_INFO_OUT_MODE_COUNT)) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERROR;
	}
	if ((area_win == NULL) || (info_win == NULL)) {
		Q_ERRORFOUND(QERROR_MODULE_UNINITIALIZED);
		return Q_ERROR;
	}
	if ((walk_area == NULL) || (curs_loc == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}
	
	if (wclear(info_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}

	attr_list = devel_walkl_loc_attr_list_get(walk_area, curs_loc);
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
	bool isinputloop;
	int choice_highlight = 0;
	int input;

	if (mode == DEVEL_WALKIO_INFO_OUT_MODE_DISPLAY) {
		isinputloop = false;
	} else {
		isinputloop = true;
	}

	/* 
	 * FIXME: please for the love of god move some of the inner nested sections
	 * to their own functions. this is so ugly. the degree of nesting is absurd.
	 * four of these lines are nested to the fifth degree!
	 */
	do {
		/* print all key-value pairs. */
		for (int i = 0; i < (int) qattr_list_count_get(attr_list); i++) {
		
			if ((key = qattr_list_attr_key_get(attr_list, i))
					== (QattrKey_t) Q_ERRORCODE_ENUM) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				return Q_ERROR;
			}

			if (isinputloop && (choice_highlight == i)) {
				if (wattr_on(info_win, A_REVERSE, NULL) == ERR) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
					returnval = Q_ERROR;
				}
			}

			/* embolden, underline, and print key */
			if (wattr_on(info_win, A_BOLD | A_UNDERLINE, NULL) == ERR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				returnval = Q_ERROR;
			}
			if (wprintw(info_win, "%s", qattr_key_to_string(key)) == ERR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				returnval = Q_ERROR;
			}
			if (wattr_off(info_win, A_BOLD | A_UNDERLINE, NULL) == ERR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				returnval = Q_ERROR;
			}

			/* print value */
			if (wprintw(info_win, ": %s\n", qattr_value_to_string(attr_list, key))
					== ERR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				returnval = Q_ERROR;
			}
			if (isinputloop && (choice_highlight == i)) {
				if (wattr_off(info_win, A_REVERSE, NULL) == ERR) {
					Q_ERRORFOUND(QERROR_ERRORVAL);
					returnval = Q_ERROR;
				}
			}
			if (wprintw(info_win, "\n") == ERR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				returnval = Q_ERROR;
			}
		}

		/* deal with user input */
		if (isinputloop) {
			if (wrefresh(info_win) == ERR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				returnval = Q_ERROR;
			}
			input = wgetch(info_win);
			if (wclear(info_win) == ERR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				returnval = Q_ERROR;
			}
			switch (input) {
			case KEY_UP:
				if (choice_highlight > 0) {
					choice_highlight--;
				}
				break;
			case KEY_DOWN:
				if (choice_highlight < ((int) qattr_list_count_get(attr_list)) - 1) {
					choice_highlight++;
				}
				break;
			case '\n':
				devel_walkio_userint = choice_highlight;
				isinputloop = false;
				break;
			case ERR:
				Q_ERRORFOUND(QERROR_ERRORVAL);
				break;
			}
		}
	} while (isinputloop);
	return returnval;
}


/**
 * Get @ref devel_walkio_userstring.
 * @return @ref devel_walkio_userstring.
 */
char *
devel_walkio_userstring_get()
/*@globals devel_walkio_userstring@*/
{
	return devel_walkio_userstring;
}


/**
 * Get @ref devel_walkio_userint.
 * @return @ref devel_walkio_userint.
 */
int
devel_walkio_userint_get()
/*@globals devel_walkio_userint@*/
{
	return devel_walkio_userint;
}

 
/**
 * Get one of a number of specific possible values for a key.
 * Set #devel_walkio_string_input_choice to the value chosen.
 * @param[in] key: key whose values are to be searched.
 * @return #Q_ERROR if @p key isn't a valid key for an input choice selection
 * or #Q_ERROR_NOCHANGE if the user exited without saving.
 */
int
devel_walkio_string_input_choice(QattrKey_t key) {
	
	/* vars */
	WINDOW *win;
	CDKSCREEN *screen;
	CDKALPHALIST *alphalist;
	char *choice;
	int returnval = Q_OK;	
	/*@observer@*/char **options;
	int optionc;

	if ((info_win == NULL) || (area_win == NULL)) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}

	switch (key) {
	case QATTR_KEY_QOBJECT_TYPE:
		options = qobj_selectable_string_types;
	 	optionc = QOBJ_SELECTABLE_STRING_TYPE_COUNT;
		break;
	case QATTR_KEY_CANMOVE:
		options = selectable_bool_types;
		optionc = SELECTABLE_BOOL_TYPES_COUNT;
		break;
	default:
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERROR;
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
			options, optionc,
			(chtype) ' ', 	/* filler character */
			(chtype) 0,   	/* highlight        */
			(boolean) true,	/* box?             */
			false);       	/* shadow?          */
	
	
	if (alphalist == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		strcpy(devel_walkio_userstring, Q_ERRORCODE_CHARSTRING);
		returnval = Q_ERROR;
		
	} else {

		/* check if the user exited via ESC */
		if ((choice = activateCDKAlphalist(alphalist, NULL)) == NULL) {
			returnval = Q_ERROR_NOCHANGE;
			strcpy(devel_walkio_userstring, Q_ERRORCODE_CHARSTRING);
		} else {
			strcpy(devel_walkio_userstring, choice);
		}
		
		/* free memory */
		destroyCDKAlphalist(alphalist);
	}
	/* free memory */
	destroyCDKScreen(screen);
	
	if (delwin(win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;	
	}

	if (wrefresh(info_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}
	if (wrefresh(area_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
	}
	/*@i3@*/return returnval;
}


/**
 * Open a window to get raw string input from the user.
 * @param[in] str_init: initial string to place in the window.
 * @return #Q_OK, #Q_ERROR, or #Q_ERROR_NOCHANGE if the user did not save
 * changes.
 */
int
devel_walkio_string_input_raw(const char *str_init) {
	FIELD *fields[2];
	FORM  *form;
	WINDOW *border_win;
	WINDOW *form_win;
	int rows = 0, columns = 0;
	int y_dummy = 0, x_dummy = 0;
	/*@partial@*/int *y_center = &y_dummy, *x_center = &x_dummy;
	int ch;
	int returnval = Q_OK;
	int r;
	
	fields[0] = new_field(
			DEVEL_WALKIO_STRING_INPUT_RAW_WIN_HEIGHT,
			DEVEL_WALKIO_STRING_INPUT_RAW_WIN_WIDTH,
			0,
			0,
			0,
			0);
	if (fields[0] == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	fields[1] = NULL;
	if ((form = new_form(fields)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}
	
	if (field_opts_off(fields[0], (Field_Options) O_BLANK) != E_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}

	if (scale_form(form, &rows, &columns) != E_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}

	io_centerof(LINES, COLS, rows + 2, columns + 2, y_center, x_center);
	
	/* create the input window at the center of the screen with a border and title */
	if ((border_win = newwin(rows + 2, columns + 2, *y_center, *x_center)) == NULL) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}
	box(border_win, 0, 0);
	io_centerof(0, columns + 2,
			0, (int) strlen(DEVEL_WALKIO_STRING_INPUT_RAW_WIN_TITLE),
			y_center, x_center);

	if (mvwprintw(border_win, *y_center, *x_center,
				DEVEL_WALKIO_STRING_INPUT_RAW_WIN_TITLE) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}

	/* initializations */	
	if (set_form_win(form, border_win) != E_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}
	if ((form_win = derwin(border_win, rows, columns, 1, 1)) == NULL){
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}
	if (set_form_sub(form, form_win) != E_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}
	if (keypad(form_win, true) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}
	if (post_form(form) != E_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}
	

	
	
	/*
	 * initialize form manually to avoid word-wrap issues if str_init isn't NULL 
	 */
	if (str_init != NULL) {
		for (int i = 0; str_init[i] != '\0'; i++) {
			if ((r = form_driver(form, (int) str_init[i])) != E_OK) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				switch (r) {
				case E_BAD_ARGUMENT:
					fprintf(stderr, "bad arg\n");
					break;
				case E_BAD_STATE:
					fprintf(stderr, "bad state\n");
					break;
				case E_NOT_POSTED:
					fprintf(stderr, "not posted\n");
					break;
				case E_INVALID_FIELD:
					fprintf(stderr, "invalid field\n");
					break;
				case E_NOT_CONNECTED:
					fprintf(stderr, "not connected\n");
					break;
				case E_REQUEST_DENIED:
					fprintf(stderr, "request denied\n");
					break;
				case E_SYSTEM_ERROR:
					fprintf(stderr, "system error\n");
					break;
				case E_UNKNOWN_COMMAND:
					fprintf(stderr, "system error\n");
					break;
				}
				returnval = Q_ERROR;
			}
		}
	}

	if (form_driver(form, REQ_VALIDATION) != E_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}

	if (wrefresh(border_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}
	if (wrefresh(form_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		abort();
	}



	/*******************
	 * MAIN LOGIC LOOP
	 ******************/

	bool isnosave = false;
	/* confirm input */
	while ((!isnosave) && 
			((ch = wgetch(form_win)) != (int) DEVEL_WALKIO_STRING_INPUT_RAW_KEY_ENTRY_CONFIRM)){
		
		switch (ch) {

		/* backspace */
		case DEVEL_WALKIO_STRING_INPUT_RAW_KEY_DEL_PREV:
			r = form_driver(form, REQ_DEL_PREV);
			break;
		
		/* delete */
		case DEVEL_WALKIO_STRING_INPUT_RAW_KEY_DEL_CHAR:
			r = form_driver(form, REQ_DEL_CHAR);
			break;
		
		/* delete all */
		case DEVEL_WALKIO_STRING_INPUT_RAW_KEY_CLR_FIELD:
			r = form_driver(form, REQ_CLR_FIELD);
			break;

		/* arrow right */
		case DEVEL_WALKIO_STRING_INPUT_RAW_KEY_NEXT_CHAR:
			r = form_driver(form, REQ_NEXT_CHAR);
			break;
		
		/* arrow left */
		case DEVEL_WALKIO_STRING_INPUT_RAW_KEY_PREV_CHAR:
			r = form_driver(form, REQ_PREV_CHAR);
			break;
		
		/* arrow up */
		case DEVEL_WALKIO_STRING_INPUT_RAW_KEY_UP_CHAR:
			r = form_driver(form, REQ_UP_CHAR);
			break;
		
		/* arrow down */
		case DEVEL_WALKIO_STRING_INPUT_RAW_KEY_DOWN_CHAR:
			r = form_driver(form, REQ_DOWN_CHAR);
			break;
		
		/* goto beginning */
		case DEVEL_WALKIO_STRING_INPUT_RAW_KEY_BEG_FIELD:
			r = form_driver(form, REQ_BEG_FIELD);
			break;

		/* goto end */
		case DEVEL_WALKIO_STRING_INPUT_RAW_KEY_END_FIELD:
			r = form_driver(form, REQ_END_FIELD);
			break;

		/* exit without saving */
		case DEVEL_WALKIO_STRING_INPUT_RAW_KEY_EXIT:
			isnosave = true;
			break;

		/* enter character */
		default:
			r = form_driver(form, ch);
			break;
		}

		/* error-checking */
		IF_STRINGIFY(r == E_BAD_ARGUMENT);
		IF_STRINGIFY(r == E_BAD_STATE);
		IF_STRINGIFY(r == E_NOT_POSTED);
		IF_STRINGIFY(r == E_INVALID_FIELD);
		IF_STRINGIFY(r == E_NOT_CONNECTED);
		IF_STRINGIFY(r == E_REQUEST_DENIED);
		IF_STRINGIFY(r == E_SYSTEM_ERROR);
		IF_STRINGIFY(r == E_UNKNOWN_COMMAND);
	}

	/* only update userstring if we're meant to save it */
	if (!isnosave) {
		if (form_driver(form, REQ_VALIDATION) != E_OK) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			returnval = Q_ERROR;
		}


		/* update userstring with the unsanitized buffer */
		/*@i4@*/strcpy(devel_walkio_userstring, field_buffer(fields[0], 0));
	

		if (io_whitespace_trim(devel_walkio_userstring) != Q_OK) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			returnval = Q_ERROR;
		}
	} else {
		returnval = Q_ERROR_NOCHANGE;
	}

	/* free memory */
	if (unpost_form(form) != E_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	if (free_form(form) != E_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	if (free_field(fields[0]) != E_OK) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	if (delwin(form_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	if (delwin(border_win) == ERR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		returnval = Q_ERROR;
	}
	/*@i3@*/return returnval;
}


/**
 * Print a text string to the user and display it at the bottom of @p area_win.
 * @param[in] s: message to print. If @c NULL, clears the area to which messages
 * are printed.
 * @return #Q_OK on success or #Q_ERROR if @p s is too long for the screen or if 
 * a different error occurs.
 */
int
devel_walkio_message_print(char *s) {
	int max_y, max_x;
	int returnval = Q_OK;

	if (area_win == NULL) {
		Q_ERRORFOUND(QERROR_NULL_POINTER_UNEXPECTED);
		return Q_ERROR;
	}

	/*@i6@*/getmaxyx(area_win, max_y, max_x);
	
	/* normal, non-null behaviour */
	if (s != NULL) {
		/* make sure message will fit */
		if ((int) strlen(s) > (max_x - 1)) {
			Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
			return Q_ERROR;
		}
	
		if (mvwprintw(area_win, max_y - 1, 0, s) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			returnval = Q_ERROR;
		}
	/* @c NULL behaviour */
	} else {
		if (wmove(area_win, max_y - 1, 0) == ERR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
			returnval = Q_ERROR;
		}
		for (int i = 0; i < (max_x - 1); i++) {
			if (waddch(area_win, (chtype) ' ') == ERR) {
				Q_ERRORFOUND(QERROR_ERRORVAL);
				returnval = Q_ERROR;
			}
		}
	}

	return returnval;

}


/**
 * Convert a raw player input to a #DevelWalkCmd_t.
 * @param[in] ch: raw input character from e.g. `getch()`.
 * @return #DevelWalkCmd_t associated with @p ch.
 */
DevelWalkCmd_t
devel_walkio_input_to_command(int ch) {
	switch (ch) {

	case DEVEL_WALK_ICH_CURSOR_MOVE_NORTH_ALT:
	/*@fallthrough@*/
	case DEVEL_WALK_ICH_CURSOR_MOVE_NORTH:
		return DEVEL_WALK_CMD_CURSOR_MOVE_NORTH;

	case DEVEL_WALK_ICH_CURSOR_MOVE_SOUTH_ALT:
	/*@fallthrough@*/
	case DEVEL_WALK_ICH_CURSOR_MOVE_SOUTH:
		return DEVEL_WALK_CMD_CURSOR_MOVE_SOUTH;

	case DEVEL_WALK_ICH_CURSOR_MOVE_EAST_ALT:
	/*@fallthrough@*/
	case DEVEL_WALK_ICH_CURSOR_MOVE_EAST:
		return DEVEL_WALK_CMD_CURSOR_MOVE_EAST;

	case DEVEL_WALK_ICH_CURSOR_MOVE_WEST_ALT:
	/*@fallthrough@*/
	case DEVEL_WALK_ICH_CURSOR_MOVE_WEST:
		return DEVEL_WALK_CMD_CURSOR_MOVE_WEST;

	case DEVEL_WALK_ICH_CURSOR_TOGGLE_ALTITUDE:
		return DEVEL_WALK_CMD_CURSOR_TOGGLE_ALTITUDE;

	case DEVEL_WALK_ICH_COPY:
		return DEVEL_WALK_CMD_COPY;

	case DEVEL_WALK_ICH_PASTE:
		return DEVEL_WALK_CMD_PASTE;

	case DEVEL_WALK_ICH_ATTR_DELETE:
		return DEVEL_WALK_CMD_ATTR_DELETE;

	case DEVEL_WALK_ICH_ATTR_INSERT:
		return DEVEL_WALK_CMD_ATTR_INSERT;

	case DEVEL_WALK_ICH_EDIT:
		return DEVEL_WALK_CMD_EDIT;

	case DEVEL_WALK_ICH_SAVE:
		return DEVEL_WALK_CMD_SAVE;

	case DEVEL_WALK_ICH_EXIT:
		return DEVEL_WALK_CMD_EXIT;

	default:
		if (devel_walkio_message_print(DEVEL_WALKIO_MESSAGE_INPUT_INVALID) == Q_ERROR) {
			Q_ERRORFOUND(QERROR_ERRORVAL);
		}
		return DEVEL_WALK_CMD_WAIT;
	}
}
