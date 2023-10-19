/** 
 * @file qattr.h
 * Header file for the attribute module. Attributes can be used to store 
 * arbitrary data tied to a key. These keys must be defined in the enum 
 * declaration. Depends on qdefs.h.
 */



/**
 * Type for holding attribute keys. This avoids needing to use char pointers or
 * char arrays which would consume excessive memory and result in needlessly
 * brief key names.
 */
typedef enum QattrKey_t {

	/* GENERAL ATTRIBUTE KEYS         */
	
	/** #QobjType_t object type */ 
	QATTR_KEY_QOBJECT_TYPE = Q_ENUM_VALUE_START,
	QATTR_KEY_NAME,                       /**< name of object */
	QATTR_KEY_DESCRIPTION_BRIEF,          /**< brief description of object */
	QATTR_KEY_DESCRIPTION_LONG,           /**< long description of object  */
	/* QWALK-EXCLUSIVE ATTRIBUTE KEYS */ 

	/* QTALK-EXCLUSIVE ATTRIBUTE KEYS */

	/* QCLI-EXLUSIVE ATTRIBUTE KEYS   */

	/* QSAIL_EXCLUSIVE ATTRIBUTE KEYS */

	/* INTERNAL ATTRIBUTE KEYS        */
	QATTR_KEY_EMPTY,                     /**< key for an empty (i.e. @c NULL) attribute */
	QATTR_KEY_DEBUG,                     /**< test key for debugging                    */
	/**
	 * Number of allowed values for #QattrKey_t.
	 * Must be defined via the second-to-last enum.
	 */
	QATTR_KEY_COUNT = QATTR_KEY_DEBUG
	
} QattrKey_t;

/**
 * Type for holding attribute pairs.
 */
typedef struct Qattr_t {
	QattrKey_t    key;    /**< Key to address the value by */
	/*@only@*/Qdatameta_t  *valuep; /**< Value to hold the proper data for the attribute */
} Qattr_t;

/**
 * Type for holding a collection of attributes.
 * (e.g. for tying to an object).
 */
typedef struct QattrList_t {
	size_t   count;    /**< The number of #Qattr_t in the list                */
	/*@only@*/Qattr_t *attrp;    /**< The actual collection of #Qattr_t       */
	size_t   index_ok; /**< The earliest index of attrp that isn't yet in use */
} QattrList_t;



/** Returns a newly-created attr list of a given size */
extern /*@null@*//*@partial@*/QattrList_t* qattr_list_create(size_t);

/** Frees a given #QattrList_t from memory */
extern int qattr_list_destroy(/*@only@*/QattrList_t *);

/*
** Transfers control of a #Qattr_t to the caller *
extern *@null@*Qdatameta_t *qattr_list_attr_remove(QattrList_t *, QattrKey_t);
*/

/** Returns the value associated with a key in a #QattrList_t */
extern /*@null@*//*@observer@*/Qdatameta_t *qattr_list_value_get(/*@returned@*/QattrList_t *, QattrKey_t)/*@*/;

/** Sets a #QattrKey_t/#Qdatameta_t pair in the given #QattrList_t */
extern int qattr_list_attr_set(QattrList_t*, QattrKey_t, /*@only@*/Qdatameta_t*);
