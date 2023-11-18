/**
 * @file devel_walk.h
 * Header file for devel_walk.
 * Depends on ncurses.h.
 */



/** Total width of the info window. */
#define DEVEL_WALK_AREA_INFO_WIN_COLS 80

/** Total width of the window to hold the border of the info window. */
#define DEVEL_WALK_AREA_INFO_WIN_BORDER_COLS (DEVEL_WALK_AREA_INFO_WIN_COLS + 2)

/** Total width of the window to hold the border of the area window. */
#define DEVEL_WALK_AREA_WIN_BORDER_COLS (COLS - DEVEL_WALK_AREA_INFO_WIN_BORDER_COLS)


/** 
 * Max amount of characters that can be stored in #devel_walkio_userstring. 
 * Must exceed the product of #DEVEL_WALKIO_STRING_INPUT_RAW_WIN_HEIGHT and
 * DEVEL_WALKIO_STRING_INPUT_RAW_WIN_WIDTH.
 */ 
#define DEVEL_WALKIO_USERSTRING_LENGTH_MAX 5000

/** Default value for #devel_walkio_userstring.                              */
#define DEVEL_WALKIO_USERSTRING_DEFAULT "INIT"

/** Message if the user uses an incorrect input.                             */
#define DEVEL_WALKIO_MESSAGE_INPUT_INVALID \
	"Invalid input..."
/** Message if the user tries and fails to modify something.                 */
#define DEVEL_WALKIO_MESSAGE_MODIFY_ERROR \
	"An error occurred while trying to modify an object..."
/** Message if the user successfully copies something.                       */
#define DEVEL_WALKIO_MESSAGE_COPY_SUCCESS \
	"Successfully copied object!"
/** Message if the user successfully pastes something.                       */
#define DEVEL_WALKIO_MESSAGE_PASTE_SUCCESS \
	"Successfully pasted object!"
/** Message if the user tries to paste while the clipboard is empty.         */
#define DEVEL_WALKIO_MESSAGE_PASTE_ERROR \
	"Clipboard is empty!"
/** Message if the user successfully saves.                                  */
#define DEVEL_WALKIO_MESSAGE_SAVE_SUCCESS \
	"Area successfully saved to disk!"
/** Message if the user fails to save.                                       */
#define DEVEL_WALKIO_MESSAGE_SAVE_ERROR \
	"ERROR: save failed!"



/**
 * @addtogroup DevelWalkioStringInputRawKeys Raw String Input Keys
 * Keys used to control @ref devel_walkio_string_input_raw().
 * @{
 */

/** Key to press to confirm input in @ref devel_walkio_string_input_raw().   */
#define DEVEL_WALKIO_STRING_INPUT_RAW_KEY_ENTRY_CONFIRM \
	'\n'

/** Key to press to backspace in @ref devel_walkio_string_input_raw().       */
#define DEVEL_WALKIO_STRING_INPUT_RAW_KEY_DEL_PREV \
	'\b'

/** Key to press to delete in @ref devel_walkio_string_input_raw().          */
#define DEVEL_WALKIO_STRING_INPUT_RAW_KEY_DEL_CHAR \
	KEY_DC

/** Key to press to delete all in @ref devel_walkio_string_input_raw().      */
#define DEVEL_WALKIO_STRING_INPUT_RAW_KEY_CLR_FIELD \
	KEY_SDC

/** Key to press to arrow right in @ref devel_walkio_string_input_raw().     */
#define DEVEL_WALKIO_STRING_INPUT_RAW_KEY_NEXT_CHAR \
	KEY_RIGHT

/** Key to press to arrow left in @ref devel_walkio_string_input_raw().      */
#define DEVEL_WALKIO_STRING_INPUT_RAW_KEY_PREV_CHAR \
	KEY_LEFT

/** Key to press to arrow up in @ref devel_walkio_string_input_raw().        */
#define DEVEL_WALKIO_STRING_INPUT_RAW_KEY_UP_CHAR \
	KEY_UP

/** Key to press to arrow down in @ref devel_walkio_string_input_raw().      */
#define DEVEL_WALKIO_STRING_INPUT_RAW_KEY_DOWN_CHAR \
	KEY_DOWN

/** Key to press to goto beginning in @ref devel_walkio_string_input_raw().  */
#define DEVEL_WALKIO_STRING_INPUT_RAW_KEY_BEG_FIELD \
	KEY_HOME

/** Key to press to goto end in @ref devel_walkio_string_input_raw().        */
#define DEVEL_WALKIO_STRING_INPUT_RAW_KEY_END_FIELD \
	KEY_END

/** Key to exit without saving in @ref devel_walkio_string_input_raw().      */
#define DEVEL_WALKIO_STRING_INPUT_RAW_KEY_EXIT \
	KEY_F(1)

/** @} */



/**
 * Type for commands sent in the I/O section of devel_walk.
 */
typedef enum DevelWalkCmd_t {
	
	/**
	 * Initialization command.
	 * Only ever sent internally; the player definitionally cannot ever send this
	 * command to the program.
	 */
	DEVEL_WALK_CMD_INIT = Q_ENUM_VALUE_START,

	/**
	 * Pass a tick without doing anything.
	 */
	DEVEL_WALK_CMD_WAIT,



	/** Minimum value for move commands. */
	DEVEL_WALK_CMD_MOVE_MIN,

	/** Move the cursor north. */
	DEVEL_WALK_CMD_CURSOR_MOVE_NORTH = DEVEL_WALK_CMD_MOVE_MIN,
	DEVEL_WALK_CMD_CURSOR_MOVE_SOUTH,      /** Move the cursor south.        */
	DEVEL_WALK_CMD_CURSOR_MOVE_EAST,       /** Move the cursor east.         */
	DEVEL_WALK_CMD_CURSOR_MOVE_WEST,       /** Move the cursor west.         */
	DEVEL_WALK_CMD_CURSOR_TOGGLE_ALTITUDE, /** Toggle the cursor's altitude. */
	
	/** Maximum value for move commands. */
	DEVEL_WALK_CMD_MOVE_MAX = DEVEL_WALK_CMD_CURSOR_TOGGLE_ALTITUDE,



	/** Minimum value for modify commands. */
	DEVEL_WALK_CMD_MODIFY_MIN,

	/** Copy the #QattrList_t of the #QwalkObj_t the cursor is pointing at. */
	DEVEL_WALK_CMD_COPY = DEVEL_WALK_CMD_MODIFY_MIN,
	
	/** 
	 * Paste a #QattrList_t to the #QwalkObj_t the cursor is pointing at.  
	 * Namely that of the last object that #DEVEL_WALK_CMD_COPY was used on.
	 */
	DEVEL_WALK_CMD_PASTE,

	/**
	 * Delete a specific attribute from an object.
	 */
	DEVEL_WALK_CMD_ATTR_DELETE,

	/**
	 * Insert a new attribute for an object.
	 */
	DEVEL_WALK_CMD_ATTR_INSERT,

	/** Edit the #QattrList_t of the #QwalkObj_t the cursor is pointing at. */
	DEVEL_WALK_CMD_EDIT,

	/** Maximum value for modify commands. */
	DEVEL_WALK_CMD_MODIFY_MAX = DEVEL_WALK_CMD_EDIT,
	


	/** Minimum value for control commands. */
	DEVEL_WALK_CMD_CONTROL_MIN,

	/** Save the current #QwalkArea_t. */
	DEVEL_WALK_CMD_SAVE = DEVEL_WALK_CMD_CONTROL_MIN,
	
	/** Exit the program. */
	DEVEL_WALK_CMD_EXIT,

	/** Maximum value for control commands. */
	DEVEL_WALK_CMD_CONTROL_MAX = DEVEL_WALK_CMD_EXIT,
	


	/** Total number of possible values for a #DevelWalkCmd_t. */
	DEVEL_WALK_CMD_COUNT = DEVEL_WALK_CMD_CONTROL_MAX

} DevelWalkCmd_t;



/** Initialize devel_walkio.                                         */
extern int devel_walkio_init(void);

/** Initialize the devel_walkio @c WINDOW vars.                      */
extern int devel_walkio_wins_init(WINDOW *, WINDOW *, WINDOW *, WINDOW *);

/** Terminate devel_walkio.                                          */
extern int devel_walkio_end(void);

/** Print a text string to the user.                                 */
extern int devel_walkio_message_print(/*@null@*/char *);

/** Get @ref devel_walkio_userstring.                                */
/*@observer@*//*@out@*/extern char *devel_walkio_userstring_get(void)/*@globals internalState@*/;

/** Get @ref devel_walkio_userint.                                   */
extern int devel_walkio_userint_get(void)/*@globals internalState@*/;

/** Initialize @c WINDOW types for devel_walk.                       */
extern int devel_walk_wins_init(/*@out@*/WINDOW **, /*@out@*/WINDOW **, /*@out@*/WINDOW **, /*@out@*/WINDOW **);

/** Close @c WINDOW types for devel_walk.                            */
extern int devel_walk_wins_close(WINDOW **, WINDOW **, WINDOW **, WINDOW **);



/** Take input from the user.                                        */
extern DevelWalkCmd_t devel_walkio_in(const QwalkArea_t *, const int *);

/** Output the a #QwalkArea_t to the screen.                         */
extern int devel_walkio_out(const QwalkArea_t *, const int *);



/** Modify a #QwalkArea_t according to a #DevelWalkCmd_t.            */
extern int devel_walkl_tick(/*@null@*/QwalkArea_t *, /*@null@*/int *, DevelWalkCmd_t);

/** Get a #QattrList_t from a #QwalkArea_t according to coordinates. */
/*@observer@*//*@null@*/
extern QattrList_t *devel_walkl_loc_attr_list_get(const QwalkArea_t *, const int *)/*@*/;

/** Set a #QattrList_t to a #QwalkArea_t according to coordinates. */
extern int devel_walkl_loc_attr_list_set(QwalkArea_t *, const int *, /*@only@*/QattrList_t *);
