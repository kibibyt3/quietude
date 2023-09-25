int main(int argc, char** argv) {
	Mode mode_curr = MODE_INIT;
	Mode mode_next = MODE_DEFAULT;
	while (mode != MODE_EXIT){
		mode_switch(mode_next, mode_curr);
		mode_curr = mode_next;
		while ((mode_next = mode_tick(modecurr)) == mode_curr){
			;
		}
	}
	mode_end(mode);
}
