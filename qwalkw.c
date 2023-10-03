/**
 * Program file for the wrapper section of the qwalk module.
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "qdefs.h"

#include "qwalk.h"



static IOTile_t *tiles; /**< Pointer to all #IOTile_t to be displayed on-screen */  

static bool     is_init = false; /**< Whether the qwalk module is initialized   */

/**
 * Initialize the qwalk module.
 * @param[in] datameta: pointer to the #Qdatameta_t sent by the previous mode.
 * @return #Q_OK or #Q_ERROR
 */ 
int
qwalk_init(Qdatameta_t* datameta) {
	assert(is_init == false);
	is_init = true;
	return Q_OK;
}

/**
 * Safely exit the qwalk module.
 * @return #Q_OK or #Q_ERROR
 */ 
int
qwalk_end() {
	assert(is_init == true);
	is_init = false;
	return Q_OK;
}

/**
 * Pass a tick in qwalk.
 * @return switch data for determining and executing the mode for the next tick.
 */
ModeSwitchData_t
qwalk_tick() {
	;
}
