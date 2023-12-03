/**
 * @file qdefault.c
 * Program file for qdefault.
 * Responsible for default initializations (particularly with regard to qattr).
 */



/** Default for #QOBJ_TYPE_TREE. */
static QdefaultQwalkObject_t *default_qwalk_tree = {
	QOBJ_TYPE_TREE, /* type */
	"tree",         /* name */
	"A tall, longing, whispering tree.", /* brief description */
	"A horribly tall tree with fingers curling and winding between opportune"
		" gaps in the Glass and the air.", /* long description */
	false /* can move */
};

/** Collection of all defaults for qwalk. */
static QdefaultQwalkObject_t *default_qwalk_objects[] = {
	&default_qwalk_tree
}

/** Number of elements in #default_qwalk_objects. */
#define DEFAULT_QWALK_OBJECTSC 1;



/**
 * Update an object in a #QwalkLayer_t to a default.
 * Assumes that the object was already previously defined.
 * @param[in] layer: #QwalkLayer_t to operate on.
 * @param[in] index: index in @p layer of object to update.
 * @param[in] default_type: the #QobjType_t to initialize to.
 * @return #Q_OK or #Q_ERROR.
 */
int
qdefault_qwalk_layer_object(QwalkLayer_t *layer, int index, 
		QobjType_t default_type) {

	for (int i = 0; i < DEFAULT_QWALK_OBJECTSC; i++) {
		;
	}
	return Q_OK;
}
