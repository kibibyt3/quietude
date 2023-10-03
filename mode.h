/**
 * @file mode.h
 * Interface for controlling modes.
 * Functions and typedefs for modes, which conceptually partition 
 * the five main facets of q from each other. Depends on qdefs.h and stdint.h.
 */

#define MODE_DEFAULT = MODE_T_WALK /**< The mode the game begins in */

/** Type for the various modes the game can play in. */
typedef enum Mode_t {
	MODE_T_INIT = 1,    /**< Initialization mode */
	MODE_T_EXIT,        /**< Exit mode */
	MODE_T_WALK,
	MODE_T_TALK,
	MODE_T_CLI,
	MODE_T_SAIL,
	MODE_T_COUNT = MODE_T_SAIL; /**< MUST BE CHANGED IF MODE_T_SAIL IS NOT THE LAST ENUM CONSTANT! */
} Mode_t;

/**
 * Type for data passed between modes. 
 * Mode name and data to deal with return type weirdness when modes need to be
 * changed and data exchanged between modes simultaneously.
 */
typedef struct ModeSwitchData_t {
	Mode_t       mode; /**< mode to switch to */
	Qdatameta_t* datameta; /**< datameta to pass to the next mode, if applicable */
} ModeSwitchData_t;

/** Initialize mode module.         */
extern int              mode_init(void);

/** Exit mode module.               */
extern int              mode_exit(void);

/** Switch to a different mode      */
extern int              mode_switch(ModeSwitchData_t, Mode_t);

/** Pass a tick in the current mode */
extern ModeSwitchData_t mode_tick(Mode_t);
