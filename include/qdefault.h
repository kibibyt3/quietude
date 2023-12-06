/**
 * @file qdefault.h
 * Header file for qdefault.
 * Depends on @ref qdefs.h, qwalk.h, and qattr.h.
 */


/**
 * Dictionary of a default qwalk initialization.
 */
typedef struct QdefaultQwalkObject_t {
	QobjType_t type;
	/*@observer@*/const char *name;
	/*@observer@*/const char *description_brief;
	/*@observer@*/const char *description_long;
	bool canmove;
} QdefaultQwalkObject_t;

/** Number of members in a #QdefaultQwalkObject_t. */
#define QDEFAULT_QWALK_OBJECT_MEMBERC 5



extern int qdefault_qwalk_layer_object_replace(/*@reldef@*/QwalkLayer_t
		*layer, int index, QobjType_t default_type)/*@modifies layer@*/;

extern int qdefault_qwalk_layer_object_incomplete(
		/*@reldef@*/QwalkLayer_t *layer, int index, QobjType_t
		default_type)/*@modifies layer@*/;

extern int qdefault_qwalk_layer_object(/*@reldef@*/QwalkLayer_t *layer,
		int index, QobjType_t default_type)
	/*@modifies layer@*/;
