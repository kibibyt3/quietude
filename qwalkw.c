/*
 * Program file for the wrapper section of the qwalk module.
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "qdefs.h"

#include "qwalk.h"



static IOTile_t *tiles;
static bool     is_init = false;

int
qwalk_init(QDatameta_t*) {
	assert(is_init == false);
	is_init = true;
	return Q_OK;
}

int
qwalk_end(QDatameta_t*) {
	assert(is_init == true);
	is_init = false;
	return Q_OK;
}

ModeSwitchData_t
qwalk_tick() {
	;
}
