/**
 * @file qwalk.h
 * qwalk module header file. Depends on qdefs.h, stdint.h, and ncurses.h.
 */


#define QWALK_LAYER_SIZE_Y 25  /**< Y dimension for a #QwalkLayer_t */
#define QWALK_LAYER_SIZE_X 50  /**< X dimension for a #QwalkLayer_t */

/** Area of the qwalk field */
#define QWALK_LAYER_SIZE (QWALK_LAYER_SIZE_Y * QWALK_LAYER_SIZE_X)

/** Minimum x/y coordinate value on a #QwalkLayer_t */
#define QWALK_LAYER_COORD_MINIMUM 0

/** Total amount of #QwalkLayer_t per #QwalkArea_t. */
#define QWALK_AREA_TOTAL_LAYER_COUNT 2

/** Filename for the starting area in qwalk. */
#define QWALK_AREA_FILENAME_DEFAULT "data/walk-world/test2.dat"



/**
 * Commands to be processed by the logic of the game.
 * The entire walk module needs to be know these commands; this is because the
 * I/O submodule translates raw input into commands.
 */
typedef enum QwalkCommand_t {
	
	/** Minimum value for a move command */
	QWALK_COMMAND_MOVE_MIN = Q_ENUM_VALUE_START,
	/** Move north. */
	QWALK_COMMAND_MOVE_NORTH = QWALK_COMMAND_MOVE_MIN,
	QWALK_COMMAND_MOVE_EAST,  /**< Move east.  */
	QWALK_COMMAND_MOVE_SOUTH, /**< Move south. */
	QWALK_COMMAND_MOVE_WEST,  /**< Move west.  */
	/** Maximum value for a move command. */
	QWALK_COMMAND_MOVE_MAX = QWALK_COMMAND_MOVE_WEST,
	
	/** Pass a tick without taking an action. */
	QWALK_COMMAND_WAIT,                            

	/** Exit the game. */
	QWALK_COMMAND_EXIT,

	/**
	 * Number of possible commands.
	 * Must be equal to the final enum constant.
	 */
	QWALK_COMMAND_COUNT = QWALK_COMMAND_EXIT
} QwalkCommand_t;


/**
 * Direction type.
 */
typedef enum Qdirection_t {
	
	/** North. */
	QDIRECTION_NORTH = Q_ENUM_VALUE_START,
	
	QDIRECTION_NORTHEAST, /**< Northeast. */
	QDIRECTION_EAST,      /**< East.      */
	QDIRECTION_SOUTHEAST, /**< Southeast. */
	QDIRECTION_SOUTH,     /**< South.     */
	QDIRECTION_SOUTHWEST, /**< Southwest. */
	QDIRECTION_WEST,      /**< West.      */
	QDIRECTION_NORTHWEST, /**< Northwest. */

	/**
	 * Amount of possible values for a #Qdirection_t.
	 * Must be defined by the final @c enum constant.
	 */
	QDIRECTION_COUNT = QDIRECTION_NORTHWEST
} Qdirection_t;

/**
 * Translate north to a y coordinate multiplicand (which is then reduced to an
 * augend depending on its factor, if applicable).
 */
#define QDIRECTION_NORTH_Y_MULTIPLICAND -1

/**
 * Translate east to an x coordinate multiplicand (which is then reduced to an
 * augend depending on its factor, if applicable).
 */
#define QDIRECTION_EAST_X_MULTIPLICAND   1

/**
 * Translate south to a y coordinate multiplicand (which is then reduced to an
 * augend depending on its factor, if applicable).
 */
#define QDIRECTION_SOUTH_Y_MULTIPLICAND  1

/**
 * Translate west to a x coordinate multiplicand (which is then reduced to an
 * augend depending on its factor, if applicable).
 */
#define QDIRECTION_WEST_X_MULTIPLICAND  -1

/**
 * Default multiplier for the multiplicands in the event that it isn't otherwise
 * specified.
 */
#define QDIRECTION_MULTIPLIER_DEFAULT    1



/**
 * An object in a #QwalkLayer_t.
 */
typedef struct QwalkObj_t {
	int coord_y;                  /**< X coord of object. */
	int coord_x;                  /**< Y coord of object. */
	/** #QattrList_t pointer for object attributes. */
	QattrList_t *attr_list;
} QwalkObj_t;


/**
 * A lone z-level of a playable area in the qwalk module.
 */
typedef struct QwalkLayer_t {
	/** Pointer to the collection of #QwalkObj_t present on the field. */
	/*@only@*/QwalkObj_t *objects;
	int index_ok; /**< next available index. */
} QwalkLayer_t;


/**
 * Specific layer type.
 */
typedef enum QwalkLayerType_t {

	/** @ref QwalkArea_t.layer_earth. */
	QWALK_LAYER_TYPE_EARTH = Q_ENUM_VALUE_START, 
	
	/** @ref QwalkArea_t.layer_floater. */
	QWALK_LAYER_TYPE_FLOATER,

	/** Number of possible values for a #QwalkLayerType_t. */
	QWALK_LAYER_TYPE_COUNT = QWALK_LAYER_TYPE_FLOATER
} QwalkLayerType_t;


/**
 * A full playable area in qwalk.
 */
typedef struct QwalkArea_t {
	QwalkLayer_t *layer_earth;   /**< layer that's embedded in the earth.  */
	QwalkLayer_t *layer_floater; /**< layer that sits on top of the earth. */
	
	/*
	 * For future implementations when there will be more than one field vvv
	   		QwalkMetaCoord_t meta_coord_x;
	   		QwalkMetaCoord_t meta_coord_y;
	 *
	*/
} QwalkArea_t;




/** Initialize the qwalk module.    */
extern           int               qwalk_init(
		/*@observer@*/const char *area_filename)/*@modifies internalState@*/;

/** Safely exit the qwalk module.   */
extern           int               qwalk_end(void);

/** Pass a tick in the qwalk module.*/
extern int qwalk_tick(void);



/** Execute the subtick step of executing the game logic. */
extern int qwalk_logic_subtick(QwalkArea_t *, QwalkCommand_t);

/** Initialize the I/O module.                            */
extern           int               qwalk_io_init(WINDOW *)
	/*@modifies internalState@*/;

extern           void              qwalk_io_end(void)
	/*@modifies internalState@*/;

/** Execute the subtick step of taking an input.          */
extern           QwalkCommand_t    qwalk_input_subtick(void);

/** Execute the subtick step of updating the screen.      */
extern           int               qwalk_output_subtick(const QwalkArea_t *);



/** Create a #QwalkArea_t.                                */
extern /*@null@*//*@only@*/QwalkArea_t *qwalk_area_create(/*@returned@*//*@keep@*/QwalkLayer_t *, /*@returned@*//*@keep@*/QwalkLayer_t *);

/** Destroy a #QwalkArea_t.                               */
extern int qwalk_area_destroy(/*@only@*//*@null@*/QwalkArea_t *);

/** Write a #QwalkArea_t to storage.                      */
extern int qwalk_area_write(const QwalkArea_t *)/*@*/;

/** Read a #QwalkArea_t from storage.                     */
extern /*@null@*//*@only@*/QwalkArea_t *qwalk_area_read(void);

/** Get the layer_earth member from a #QwalkArea_t.       */
extern /*@null@*//*@observer@*/QwalkLayer_t *qwalk_area_layer_earth_get(const /*@null@*//*@returned@*/QwalkArea_t *)/*@*/;

/** Get the layer_floater member from a #QwalkArea_t.     */
extern /*@null@*//*@observer@*/QwalkLayer_t *qwalk_area_layer_floater_get(const/*@null@*//*@returned@*/QwalkArea_t *)/*@*/;



/** Create a #QwalkLayer_t.                               */
extern /*@null@*//*@partial@*/QwalkLayer_t *qwalk_layer_create(void);
 
/** Destory a #QwalkLayer_t.                              */
extern                        int           qwalk_layer_destroy(/*@only@*/QwalkLayer_t *);

/** Write a #QwalkLayer_t to storage.                     */
extern                        int           qwalk_layer_write(const QwalkLayer_t *);

/** Read a #QwalkLayer_t from storage.                    */
extern    /*@null@*//*@only@*/QwalkLayer_t *qwalk_layer_read(void);

/** Add a #QwalkObj_t * to a #QwalkLayer_t *.             */
extern                        int          qwalk_layer_object_set(/*@null@*/QwalkLayer_t *, int, int, /*@null@*//*@only@*/QattrList_t *);



/** Get the y coordinate of a #QwalkObj_t.                */
extern int               qwalk_layer_object_coord_y_get(/*@null@*/const QwalkLayer_t *, int)/*@*/;

/** Get the x coordinate of a #QwalkObj_t.                */
extern int               qwalk_layer_object_coord_x_get(/*@null@*/const QwalkLayer_t *, int)/*@*/;

/** Get the #QattrList_t of a #QwalkObj_t.                */
extern /*@null@*//*@observer@*/QattrList_t *qwalk_layer_object_attr_list_get(/*@null@*/const QwalkLayer_t *, int)/*@*/;

/** Set a #QattrKey_t with a default #Qdatameta_t oriented around qwalk.  */
extern int qwalk_attr_list_attr_set_default(QattrList_t *attr_list,
		QattrKey_t attr_key, QwalkLayerType_t layer_type);



extern int    qwalk_coords_to_index(int, int)/*@*/;

extern int   *qwalk_index_to_coords(int)/*@*/;

extern bool   qwalk_logic_coords_arevalid(int, int)/*@*/;

extern chtype qwalk_obj_type_to_chtype(QobjType_t)/*@*/;
