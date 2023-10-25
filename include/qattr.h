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

	/**
	 * @defgroup GeneralKeys General Attribute Keys
	 * A #QattrKey_t whose usage is relevant across all modules.
	 */
	/* GENERAL ATTRIBUTE KEYS         */
	
	/**
	 * @addtogroup GeneralKeys
	 * #QobjType_t of parent.
	 * This field <i>must</i> exist in any given #QattrList_t.
	 */ 
	QATTR_KEY_QOBJECT_TYPE = Q_ENUM_VALUE_START,
	
	/**
	 * @addtogroup GeneralKeys
	 * Name of object.
	 */
	QATTR_KEY_NAME,                      
	
	/**
	 * @addtogroup GeneralKeys
	 * Brief description of object.
	 */
	QATTR_KEY_DESCRIPTION_BRIEF,

	/**
	 * @addtogroup GeneralKeys
	 * Long description of object.
	 */
	QATTR_KEY_DESCRIPTION_LONG,	
	

	/**
	 * @defgroup QwalkKeys Qwalk-Exclusive Attribute Keys
	 * A #QattrKey_t whose usage is exclusive to qwalk.
	 */
	/* QWALK-EXCLUSIVE ATTRIBUTE KEYS */ 
	
	/**
	 * @addtogroup QwalkKeys 
	 * Whether object can move.
	 */
	QATTR_KEY_CANMOVE,



	/**
	 * @defgroup QtalkKeys Qtalk-Exclusive Attribute Keys
	 * A #QattrKey_t whose usage is exclusive to qwalk.
	 */
	/* QTALK-EXCLUSIVE ATTRIBUTE KEYS */



	/**
	 * @defgroup QCLIKeys QCLI-Exclusive Attribute Keys
	 * A #QattrKey_t whose usage is exclusive to qcli.
	 */
	/* QCLI-EXLUSIVE ATTRIBUTE KEYS   */

	
	
	/**
	 * @defgroup QsailKeys Qsail-Exclusive Attribute Keys
	 * A #QattrKey_t whose usage is exclusive to qsail.
	 */
	/* QSAIL_EXCLUSIVE ATTRIBUTE KEYS */



	/**
	 * @defgroup InternalKeys Internal Attribute Keys
	 * A #QattrKey_t whose usage is for error-checking, debugging, etc.
	 */
	/* INTERNAL ATTRIBUTE KEYS        */
	
	/** 
	 * @addtogroup InternalKeys
	 * Key for an empty (i.e. @c NULL) attribute.
	 */
	QATTR_KEY_EMPTY,                     
	
	/**
	 * @addtogroup InternalKeys
	 * Test key for debugging.
	 */
	QATTR_KEY_DEBUG,                     
	
	/**
	 * @addtogroup InternalKeys
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



/** Return a newly-created attr list of a given size.                    */
extern /*@null@*//*@partial@*/QattrList_t *qattr_list_create(size_t);

/** Free a given #QattrList_t from memory.                               */
extern int qattr_list_destroy(/*@only@*/QattrList_t *);

/** Write a #QattrList_t to storage.                                     */
extern int qattr_list_write(const QattrList_t *);

/** Read a #QattrList_t from storage.                                    */
extern /*@null@*//*@only@*/QattrList_t *qattr_list_read(void);

/*
** Transfers control of a #Qattr_t to the caller *
extern *@null@*Qdatameta_t *qattr_list_attr_remove(QattrList_t *, QattrKey_t);
*/

/** Return the value associated with a key in a #QattrList_t.             */
extern /*@null@*//*@observer@*/Qdatameta_t *qattr_list_value_get(/*@returned@*/QattrList_t *, QattrKey_t)/*@*/;

/** Return @ref QattrList_t.count.                                        */
extern size_t qattr_list_count_get(const QattrList_t *)/*@*/;

/** Return @ref QattrList_t.index_ok.                                     */
extern size_t qattr_list_index_ok_get(const QattrList_t *)/*@*/;

/** Return @ref Qattr_t.key from a #QattrList_t as addressed by an index. */
extern QattrKey_t qattr_list_attr_key_get(const QattrList_t *, int)/*@*/;

/** Set a #QattrKey_t/#Qdatameta_t pair in the given #QattrList_t.        */
extern int qattr_list_attr_set(QattrList_t *, QattrKey_t, /*@only@*/Qdatameta_t *);
