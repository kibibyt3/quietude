/**
 * @file qwalk.h
 * qwalk module header file. Depends on qdefs.h and stdint.h.
 */


#define QWALK_AREA_SIZE_Y 25  /**< Y dimension for the qwalk field */
#define QWALK_AREA_SIZE_X 50  /**< X dimension for the qwalk field */

/** Area of the qwalk field */
#define QWALK_AREA_SIZE (QWALK_AREA_SIZE_Y * QWALK_AREA_SIZE_X)

/**
 * An object in a #QwalkField_t.
 */
typedef struct QwalkObject_t {
	int coord_y;                  /**< X coord of object */
	int coord_x;                  /**< Y coord of object */
	QattrList_t *attr_list;       /**< #QattrList_t pointer for attributes of object */
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
extern int               qwalk_init(Qdatameta_t *);

/** Safely exit the qwalk module             */
extern int               qwalk_end(void);

/** Pass a tick in the qwalk module          */
extern ModeSwitchData_t *qwalk_tick(void);




/** Get the y coordinate of a #QwalkObject_t */
extern int               qwalk_object_coord_y_get(QwalkObject_t*);

/** Get the x coordinate of a #QwalkObject_t */
extern int               qwalk_object_coord_x_get(QwalkObject_t*);

/** Get the #QattrList_t of a #QwalkObject_t */
extern QattrList_t      *qwalk_object_attr_list_get(QwalkObject_t*);
