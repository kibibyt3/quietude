/*
 * qattr.h
 * Header file for the attribute module. Attributes can be used to store 
 * arbitrary data tied to a key. These keys must be defined in the enum 
 * declaration. Depends on qdefs.h.
 */



/*
 * Type for holding attribute keys. This avoids needing to use char pointers or
 * char arrays which would consume excessive memory and result in needlessly
 * brief key names.
 */
typedef enum QattrKey_t {

	/* GENERAL ATTRIBUTE KEYS         */
	QATTR_KEY_NAME = 1,
	QATTR_KEY_DESCRIPTION,

	/* QWALK-EXCLUSIVE ATTRIBUTE KEYS */

	/* QTALK-EXCLUSIVE ATTRIBUTE KEYS */

	/* QCLI-EXLUSIVE ATTRIBUTE KEYS   */

	/* QSAIL_EXCLUSIVE ATTRIBUTE KEYS */

	/* INTERNAL ATTRIBUTE KEYS        */
	QATTR_KEY_COUNT = Q_ATTR_KEY_DESCRIPTION /* MUST DEFINE VIA THE FINAL ENUM */
	
} QattrKey_t;

/*
 * Type for holding attribute pairs.
 */
typedef struct Qattr_t {
	QattrKey_t    key;
	Qdatameta_t*  valuep;
} Qattr_t;

/*
 * Type for holding a collection of attributes (e.g. for tying to an object).
 */
typedef struct QattrList_t {
	size_t   count;
	Qattr_t *attrp;
	size_t   index_ok; /* For holding the earliest index of attrp that isn't yet in use */
} QattrL ist_t;



/* Returns a newly-created attr list of a given size */
extern QattrList_t* qattr_list_create(int);

/* Frees a given qattr_list from memory */
extern int qattr_list_destroy(QattrList_t*);

/* Returns the value associated with the given key in the given AttrList */
extern Qdatameta_t* qattr_list_value_get(QattrList_t*, QattrKey_t);

/* Sets an AttrKey/Datameta pair in the given AttrList */
extern int qattr_list_attr_set(QattrList_t*, QattrKey_t, Qdatameta_t*);
