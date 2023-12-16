/**
 * @file qutils.h
 * Header file for qutils.
 */



extern int qutils_distance_calculate(
		int starty, int startx, int endy, int endx)/*@*/;

/*@null@*/
extern char *qutils_nobias_randstring(const char *alphabet, int len);

extern int qutils_nobias_rand(int i);

extern void qutils_nobias_srand(void);

#define QUTILS_DOUBLES_AREEQUAL_TOLERANCE .00001
extern bool qutils_doubles_areequal(double lhd, double rhd)/*@*/;
