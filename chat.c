#include "chat.h"

#define MESSAGE_SIZE 400
#define RESPONSE_LENGTH 400
#define DIRECTOR_LENGTH 400
#define MAX_RESPONSES 6
#define MAX_SECTIONS 100

/* Commands that deal with special behaviour called within a .brn file */
typedef enum command {
	NONE = 1; /* No special behaviour */
	EXIT;     /* Exit the conversation subroutine */
} command;

/* Section of the tree file; a specific conversational branch */
typedef struct section {
	
	int **responses; /* Possible responses from the user */
	int **directors; /* Header names associated with each response */
	int *header;     /* Name of section used by directors */
	int *message;    /* NPC opening message for section */
	int responsec;   /* Number of responses in section */

} section;

static section *sections; /* All sections of .brn file */
static int sectionc;  /* Number of sections in .brn file */


static 

