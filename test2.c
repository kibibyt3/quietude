#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>


#include "qdefs.h"

int main(int argc, char** argv) {
	char *cp;
	int  *ip;
	int r;

	cp = calloc(5, sizeof(*cp));
	ip = calloc(5, sizeof(*ip));

	cp[0] = 'c';
	cp[1] = 'h';
	cp[2] = 'a';
	cp[3] = 'r';
	cp[4] = '\0';

	r = qutil_charptointp(cp, ip, 5);
	assert(r == Q_OK);

	printf("%c%c%c%c%c", ip[0], ip[1], ip[2], ip[3], ip[4]);
	return 0;
}
