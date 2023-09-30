/*
 * qwalk.h
 * qwalk module. Depends on qdefs.h and stdint.h.
 */

#define QWALK_AREA_SIZE_Y 25
#define QWALK_AREA_SIZE_X 50
#define QWALK_AREA_SIZE (QWALK_AREA_SIZE_Y * QWALK_AREA_SIZE_X)

typedef struct QwalkObject {
	
} QwalkObject;

typedef struct QwalkArea {
		
} QwalkArea;

extern int              qwalk_init(QDatameta_t);
extern int              qwalk_end(QDatameta_t);
extern ModeSwitchData_t qwalk_tick(void);
