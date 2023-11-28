/**
 * @file mode.h
 * Interface for controlling modes.
 * Functions and typedefs for modes, which conceptually partition 
 * the main facets of q from each other. Depends on qdefs.h and stdint.h.
 */



#define MODE_DEFAULT MODE_T_WALK /**< The mode the game begins in */



/** Type for the various modes the game can play in. */
typedef enum Mode_t {
	
	/** Initialization mode */	
	MODE_T_INIT = Q_ENUM_VALUE_START,    
	MODE_T_EXIT,        /**< Exit mode */
	MODE_T_WALK,        /**< Walk mode */
	MODE_T_TALK,        /**< Talk mode */
	MODE_T_CLI,         /**< CLI  mode */
	MODE_T_SAIL,        /**< Sail mode */
	
	/**
	 * Number of allowed values for #Mode_t. 
	 * Must be set to equal the final enum constant.
	 */
	MODE_T_COUNT = MODE_T_SAIL
} Mode_t;

/**
 * Type for data passed between modes. 
 * Mode name and data to deal with return type weirdness when modes need to be
 * changed and data exchanged between modes simultaneously.
 */
typedef struct ModeSwitchData_t {
	Mode_t       mode;     /**< mode to switch to */
	/*@null@*/Qdatameta_t *datameta; /**< datameta to pass to the next mode, if applicable */
} ModeSwitchData_t;



extern int mode_init(void)/*@modifies internalState@*/;
extern int mode_exit(void)/*@modifies internalState@*/;
extern int mode_tick(void)/*@globals internalState@*/;
extern int mode_switch(void)/*@modifies internalState@*/;

extern int mode_buffer_switch(Mode_t mode,
		/*@only@*//*@returned@*/Qdatameta_t *datameta)
/*@modifies internalState@*//*@globals internalState@*/;

extern Mode_t mode_curr_get(void)/*@globals internalState@*/;


extern int mode_switch_data_datameta_set(
		ModeSwitchData_t *switch_data,
		/*@only@*//*@returned@*//*@null@*/Qdatameta_t *datameta)
	/*@modifies switch_data@*/;

extern int mode_switch_data_mode_set(
		ModeSwitchData_t *switch_data, Mode_t mode)/*@modifies switch_data@*/;

/*@observer@*//*@null@*/
extern Qdatameta_t *mode_switch_data_datameta_get(
		ModeSwitchData_t *switch_data)/*@*/;

extern Mode_t mode_switch_data_mode_get(ModeSwitchData_t *switch_data)/*@*/;
