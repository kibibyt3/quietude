/**
 * @file devel_walk.h
 * Header file for devel_walk.
 */



/**
 * Type for commands sent in the I/O section of devel_walk.
 */
typedef enum DevelWalkCmd_t {
	
	DEVEL_WALK_CMD_INIT = Q_ENUM_VALUE_START,
	DEVEL_WALK_CMD_CURSOR_MOVE_UP,
	DEVEL_WALK_CMD_CURSOR_MOVE_DOWN,
	DEVEL_WALK_CMD_CURSOR_MOVE_LEFT,
	DEVEL_WALK_CMD_CURSOR_MOVE_RIGHT,
	
	DEVEL_WALK_CMD_COPY,
	DEVEL_WALK_CMD_PASTE,
	DEVEL_WALK_CMD_EDIT,
	
	DEVEL_WALK_CMD_SAVE,
	DEVEL_WALK_CMD_EXIT,
	
	DEVEL_WALK_CMD_COUNT = DEVEL_WALK_CMD_EXIT

} DevelWalkCmd_t;



/** Initialize devel_walkio.                              */
extern int devel_walkio_init(void);

/** Terminate devel_walkio.                               */
extern int devel_walkio_end(void);



/** Take input from the user.                             */
extern DevelWalkCmd_t devel_walkio_in(void);

/** Output the state to the screen.                       */
extern int devel_walkio_out(/*@keep@*/QwalkArea_t *);

/** Modify a #QwalkArea_t according to a #DevelWalkCmd_t. */
extern int devel_walkl_tick(/*@keep@*/QwalkArea_t *, DevelWalkCmd_t);



/** Load a #QwalkArea_t from storage.                     */
extern /*@only@*//*@null@*/QwalkArea_t *devel_walk_area_load(char *)/*@*/;

/** Initialize a default #QwalkArea_t.                    */
extern /*@only@*//*@null@*/QwalkArea_t *devel_walk_area_default_init(void)/*@*/;
