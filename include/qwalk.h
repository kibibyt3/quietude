/**
 * @file qwalk.h
 * qwalk module header file. Depends on qdefs.h and stdint.h.
 */


#define QWALK_LAYER_SIZE_Y 25  /**< Y dimension for a #QwalkLayer_t */
#define QWALK_LAYER_SIZE_X 50  /**< X dimension for a #QwalkLayer_t */

/** Area of the qwalk field */
#define QWALK_LAYER_SIZE (QWALK_LAYER_SIZE_Y * QWALK_LAYER_SIZE_X)

/** Minimum x/y coordinate value on a #QwalkLayer_t */
#define QWALK_LAYER_COORD_MINIMUM 0



/**
 * Commands to be processed by the logic of the game.
 * The entire walk module needs to be know these commands; this is because the
 * I/O submodule translates raw input into commands.
 */
typedef enum QwalkCommand_t {
	
	/** Minimum value for a move command */
	QWALK_COMMAND_MOVE_MIN = Q_ENUM_VALUE_START,
	QWALK_COMMAND_MOVE_NORTH, /**< Move north */
	QWALK_COMMAND_MOVE_EAST,  /**< Move east  */
	QWALK_COMMAND_MOVE_SOUTH, /**< Move south */
	QWALK_COMMAND_MOVE_WEST,  /**< Move west  */
	/** Maximum value for a move command */
	QWALK_COMMAND_MOVE_MAX = QWALK_COMMAND_MOVE_WEST,
	
	/** Pass a tick without taking an action*/
	QWALK_COMMAND_WAIT,                            

	/**
	 * Number of possible commands.
	 * Must be equal to the final enum constant.
	 */
	QWALK_COMMAND_COUNT = QWALK_COMMAND_WAIT
} QwalkCommand_t;


/**
 * An object in a #QwalkLayer_t.
 */
typedef struct QwalkObj_t {
	int coord_y;                  /**< X coord of object */
	int coord_x;                  /**< Y coord of object */
	/** #QattrList_t pointer for object attributes */
	QattrList_t *attr_list;
} QwalkObj_t;

/**
 * A lone z-level of a playable area in the qwalk module.
 */
typedef struct QwalkLayer_t {
	/** Pointer to the collection of #QwalkObj_t present on the field */
	QwalkObj_t **objects;
} QwalkLayer_t;

/**
 * A full playable area in qwalk.
 */
typedef struct QwalkArea_t {
	QwalkLayer_t *layer_earth;   /**< layer that's embedded in the earth  */
	QwalkLayer_t *layer_floater; /**< layer that sits on top of the earth */
	
	/*
	 * For future implementations when there will be more than one field vvv
	   		QwalkMetaCoord_t meta_coord_x;
	   		QwalkMetaCoord_t meta_coord_y;
	 *
	*/
} QwalkArea_t;




/** Initialize the qwalk module              */
extern           int               qwalk_init(/*@only@*/Qdatameta_t *);

/** Safely exit the qwalk module             */
extern           int               qwalk_end(void);

/** Pass a tick in the qwalk module          */
extern int qwalk_tick(ModeSwitchData_t *);



/** Execute the subtick step of executing the game logic */
extern int qwalk_logic_subtick(QwalkArea_t *, QwalkCommand_t, ModeSwitchData_t *);

/** Execute the subtick step of taking an input */
extern           QwalkCommand_t    qwalk_input_subtick(void);

/** Execute the subtick step of updating the screen */
extern           int               qwalk_output_subtick(void);


/** Get the layer_earth member from a #QwalkArea_t   */
extern /*@null@*/QwalkLayer_t      *qwalk_area_layer_earth_get(const QwalkArea_t *)/*@*/;

/** Get the layer_floater member from a #QwalkArea_t */
extern /*@null@*/QwalkLayer_t      *qwalk_area_layer_floater_get(const QwalkArea_t *)/*@*/;


/** Get a specific #QwalkObj_t * from a #QwalkLayer_t */
extern /*@null@*/QwalkObj_t    *qwalk_layer_object_get(const QwalkLayer_t *, int)/*@*/;

/** Set the y coordinate of a #QwalkObj_t */
extern           int               qwalk_object_coord_y_set(QwalkObj_t *, int);

/** Set the x coordinate of a #QwalkObj_t */
extern           int               qwalk_object_coord_x_set(QwalkObj_t *, int);

/** Get the y coordinate of a #QwalkObj_t */
extern int               qwalk_object_coord_y_get(const QwalkObj_t *)/*@*/;

/** Get the x coordinate of a #QwalkObj_t */
extern int               qwalk_object_coord_x_get(const QwalkObj_t *)/*@*/;

/** Get the #QattrList_t of a #QwalkObj_t */
extern QattrList_t      *qwalk_object_attr_list_get(const QwalkObj_t *)/*@*/;
