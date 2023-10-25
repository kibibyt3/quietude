/**
 * @file devel_walk.h
 * Header file for devel_walk.
 * Depends on ncurses.h.
 */



/**
 * Type for commands sent in the I/O section of devel_walk.
 */
typedef enum DevelWalkCmd_t {
	
	DEVEL_WALK_CMD_INIT = Q_ENUM_VALUE_START,
	DEVEL_WALK_CMD_CURSOR_MOVE_NORTH,
	DEVEL_WALK_CMD_CURSOR_MOVE_SOUTH,
	DEVEL_WALK_CMD_CURSOR_MOVE_EAST,
	DEVEL_WALK_CMD_CURSOR_MOVE_WEST,
	DEVEL_WALK_CMD_CURSOR_TOGGLE_ALTITUDE,
	
	DEVEL_WALK_CMD_COPY,
	DEVEL_WALK_CMD_PASTE,
	DEVEL_WALK_CMD_EDIT,
	
	DEVEL_WALK_CMD_SAVE,
	DEVEL_WALK_CMD_EXIT,
	
	DEVEL_WALK_CMD_COUNT = DEVEL_WALK_CMD_EXIT

} DevelWalkCmd_t;



/** Initialize devel_walkio.                              */
extern int devel_walkio_init(WINDOW *, WINDOW *);

/** Terminate devel_walkio.                               */
extern int devel_walkio_end(void);



/** Take input from the user.                             */
extern DevelWalkCmd_t devel_walkio_in(void);

/** Output the a #QwalkArea_t to the screen.              */
extern int devel_walkio_out(const QwalkArea_t *, const int *);

/** Modify a #QwalkArea_t according to a #DevelWalkCmd_t. */
extern int devel_walkl_tick(QwalkArea_t *, DevelWalkCmd_t);



/** Load a #QwalkArea_t from storage.                     */
extern /*@only@*//*@null@*/QwalkArea_t *devel_walk_area_load(char *)/*@*/;

/** Initialize a default #QwalkArea_t.                    */
extern /*@only@*//*@null@*/QwalkArea_t *devel_walk_area_default_init(void)/*@*/;
