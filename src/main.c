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
 * @return 0 on success and 1 on error.
 */
int main(int argc, char** argv) {
	
	/* Initializations */
	assert(mode_init() == Q_OK);

	ModeSwitchData_t  mode_switch_data_next = {MODE_T_INIT, NULL};
	ModeSwitchData_t *mode_switch_data_nextptr = &mode_switch_data_next;
	Mode_t mode_curr = MODE_T_INIT;

	while (mode_switch_data_next.mode != MODE_EXIT) {
		mode_switch(mode_switch_data_next);
		mode_curr = mode_switch_data_next->mode;
		while (mode_switch_data_next->mode == mode_curr) {
			mode_tick(mode_switch_data_next);
		}
	}
}
