#include "qdefs.h"

#include "mode.h"

int main(int argc, char** argv) {
	
	/* Initializations */
	assert(mode_init() == Q_SUCCESS);

	Mode_t           mode_curr             =  MODE_T_INIT;
	ModeSwitchData_t mode_switch_data_next = {MODE_DEFAULT, NULL};

	while (mode_switch_data_next.mode != MODE_EXIT) {
		mode_switch(mode_switch_data_next, mode_curr);
		mode_curr = mode_switch_data_next.mode;
		do {
			mode_switch_data_next = mode_tick(mode_curr);
		} while (mode_switch_data_next.mode == mode_curr);
	}
}
