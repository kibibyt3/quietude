/**
 * @file qdefault.h
 * Header file for qdefault.
 */


/**
 * Dictionary of a default qwalk initialization.
 */
typedef struct QdefaultQwalkObject_t {
	QobjType_t type;
	char *name;
	char *description_brief;
	char *description_long;
	bool canmove;
} QdefaultQwalkObject_t;



extern qdefault_qwalk(QwalkLayer_t *layer, int index, const char *default_type)
	/*@modifies layer@*/;
