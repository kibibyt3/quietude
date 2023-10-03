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
	
} QwalkObject_t;

/**
 * A playable area in the qwalk module.
 */
typedef struct QwalkField_t {
		
} QwalkField_t;

/** Initialize the qwalk module     */
extern int              qwalk_init(Qdatameta_t*);

/** Safely exit the qwalk module    */
extern int              qwalk_end(void);

/** Pass a tick in the qwalk module */
extern ModeSwitchData_t qwalk_tick(void);
