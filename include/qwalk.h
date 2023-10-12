/**
 * @file qwalk.h
 * qwalk module header file. Depends on qdefs.h and stdint.h.
 */


#define QWALK_AREA_SIZE_Y 25  /**< Y dimension for the qwalk field */
#define QWALK_AREA_SIZE_X 50  /**< X dimension for the qwalk field */

/** Area of the qwalk field */
#define QWALK_AREA_SIZE (QWALK_AREA_SIZE_Y * QWALK_AREA_SIZE_X)



/**
 * Commands to be processed by the logic of the game.
 * The entire walk module needs to be know these commands; this is because the
 * I/O submodule translates raw input into commands.
 */
typedef enum QwalkCommand_t {
	QWALK_COMMAND_MOVE_NORTH = Q_ENUM_VALUE_START, /**< Move north */
	QWALK_COMMAND_MOVE_EAST,                       /**< Move east  */
	QWALK_COMMAND_MOVE_SOUTH,                      /**< Move south */
	QWALK_COMMAND_MOVE_WEST,                       /**< Move west  */
	/** Pass a tick without taking an action*/
	QWALK_COMMAND_WAIT,                            

	/**
	 * Number of possible commands.
	 * Must be equal to the final enum constant.
	 */
	QWALK_COMMAND_COUNT = QWALK_COMMAND_WAIT
} QwalkCommand_t;



/**
 * An object in a #QwalkField_t.
 */
typedef struct QwalkObject_t {
	int coord_y;                  /**< X coord of object */
	int coord_x;                  /**< Y coord of object */
	/** #QattrList_t pointer for object attributes */
	QattrList_t *attr_list;
} QwalkObject_t;

/**
 * A playable area in the qwalk module.
 */
typedef struct QwalkField_t {
	/** Pointer to the collection of #QwalkObject_t present on the field */
	QwalkObject_t *objects;

	/*
	 * For future implementations when there will be more than one field vvv
	   		QwalkMetaCoord_t meta_coord_x;
	   		QwalkMetaCoord_t meta_coord_y;
	*/

} QwalkField_t;




/** Initialize the qwalk module              */
extern           int               qwalk_init(const Qdatameta_t *);

/** Safely exit the qwalk module             */
extern           int               qwalk_end(void);

/** Pass a tick in the qwalk module          */
extern /*@null@*/ModeSwitchData_t *qwalk_tick(void);



/** Execute the subtick step of executing the game logic */
extern /*@null@*/ModeSwitchData_t *qwalk_logic_subtick(QwalkField_t *, QwalkCommand_t);


/** Execute the subtick step of taking an input */
extern           QwalkCommand_t    qwalk_input_subtick();

/** Execute the subtick step of updating the screen */
extern           int               qwalk_output_subtick();



/** Get the y coordinate of a #QwalkObject_t */
extern int               qwalk_object_coord_y_get(const QwalkObject_t*);

/** Get the x coordinate of a #QwalkObject_t */
extern int               qwalk_object_coord_x_get(const QwalkObject_t*);

/** Get the #QattrList_t of a #QwalkObject_t */
extern QattrList_t      *qwalk_object_attr_list_get(const QwalkObject_t*);
