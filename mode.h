/*
 * Interface for controlling modes. Depends on qdefs.h and stdint.h.
 */

#define MODE_DEFAULT = MODE_T_WALK

typedef enum Mode_t {
	MODE_T_INIT = 1,
	MODE_T_EXIT,
	MODE_T_WALK,
	MODE_T_TALK,
	MODE_T_CLI,
	MODE_T_SAIL,
	MODE_T_COUNT = MODE_T_SAIL; /* MUST BE CHANGED IF MODE_T_SAIL IS NOT THE LAST ENUM CONSTANT! */
} Mode_t;

typedef struct ModeSwitchData_t {
	Mode_t       mode;
	Qdatameta_t* datameta;
} ModeSwitchData_t;

extern int              mode_init(void);
extern int              mode_exit(void);

extern int              mode_switch(ModeSwitchData_t, Mode_t);
extern ModeSwitchData_t mode_tick(Mode_t);
