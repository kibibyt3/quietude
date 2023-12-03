/**
 * @file qdefault.c
 * Program file for qdefault.
 * Responsible for default initializations (particularly with regard to qattr).
 */



static QdefaultQwalkObject_t default_qwalk_tree = {
	QOBJ_TYPE_TREE, /* type */
	"tree",         /* name */
	"A tall, longing, whispering tree.", /* brief description */
	"A horribly tall tree with fingers curling and winding between opportune"
		" gaps in the Glass and the air.", /* long description */
	false /* can move */
};

