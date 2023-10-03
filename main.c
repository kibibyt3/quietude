/**
 * @file main.c
 * Main file for Q.
 */

#include "qdefs.h"

#include "mode.h"

/**
 * Main function of Q.
 * @param[in] argc: argument count
 * @param[in] argv: argument vector
 */
int main(int argc, char** argv) {
	
	/* Initializations */
	assert(mode_init() == Q_OK);

	Mode_t            mode_curr             = MODE_T_INIT;
	ModeSwitchData_t *mode_switch_data_next;

	while (mode_switch_data_next.mode != MODE_EXIT) {
		mode_switch(mode_switch_data_next, mode_curr);
		mode_curr = (*mode_switch_data_next).mode;
		do {
			mode_switch_data_next = mode_tick(mode_curr);
		} while ((*mode_switch_data_next).mode == mode_curr);
	}
}
