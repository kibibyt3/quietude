/** 
 * @file qattr.h
 * Header file for the attribute module. Attributes can be used to store 
 * arbitrary data tied to a key. These keys must be defined in the enum 
 * declaration. Depends on qdefs.h.
 */



/*************************************************** 
 * DEFAULT QWALK QattrKey_t VALUES                 
 ***************************************************/

/* DEFAULT QattrKey_t VALUES FOR THE EARTH LAYER   */

/** Default value for #QATTR_KEY_QOBJECT_TYPE in #QWALK_LAYER_TYPE_EARTH. */
#define QATTR_KEY_QOBJECT_TYPE_QWALK_DEFAULT_EARTH \
QOBJ_TYPE_GRASS

/** Default value for #QATTR_KEY_NAME in #QWALK_LAYER_TYPE_EARTH. */
#define QATTR_KEY_NAME_DEFAULT_QWALK_DEFAULT_EARTH \
"grass"

/** Default value for #QATTR_KEY_DESCRIPTION_BRIEF in #QWALK_LAYER_TYPE_EARTH. */
#define QATTR_KEY_DESCRIPTION_BRIEF_QWALK_DEFAULT_EARTH \
"A patch of grass"

/** Default value for #QATTR_KEY_DESCRIPTION_LONG in #QWALK_LAYER_TYPE_EARTH. */
#define QATTR_KEY_DESCRIPTION_LONG_QWALK_DEFAULT_EARTH \
"Long limbs and threads of a varying greenbrown grasp longingly from the " \
"earth. You wonder if they march toward you."

/** Default value for #QATTR_KEY_CANMOVE in #QWALK_LAYER_TYPE_EARTH. */
#define QATTR_KEY_CANMOVE_QWALK_DEFAULT_EARTH \
false;



/* DEFAULT QattrKey_t VALUES FOR THE FLOATER LAYER */

/** Default value for #QATTR_KEY_QOBJECT_TYPE in #QWALK_LAYER_TYPE_FLOATER. */
#define QATTR_KEY_QOBJECT_TYPE_QWALK_DEFAULT_FLOATER \
QOBJ_TYPE_VOID

/** Default value for #QATTR_KEY_NAME in #QWALK_LAYER_TYPE_FLOATER. */
#define QATTR_KEY_NAME_DEFAULT_QWALK_DEFAULT_FLOATER \
"void"

/** Default value for #QATTR_KEY_DESCRIPTION_LONG in #QWALK_LAYER_TYPE_FLOATER. */
#define QATTR_KEY_DESCRIPTION_BRIEF_QWALK_DEFAULT_FLOATER \
"An empty space"

/** Default value for #QATTR_KEY_DESCRIPTION_BRIEF in #QWALK_LAYER_TYPE_FLOATER. */
#define QATTR_KEY_DESCRIPTION_LONG_QWALK_DEFAULT_FLOATER \
"The wind here whirls in a mundane miniature vortex. You know this. You can " \
"smell it."

/** Default value for #QATTR_KEY_CANMOVE in #QWALK_LAYER_TYPE_FLOATER. */
#define QATTR_KEY_CANMOVE_QWALK_DEFAULT_FLOATER \
true;


/* DEFAULT QattrKey_t TYPE VALUES ON ERROR */

/** Default value for a string #QattrKey_t (used on error.) */
#define QATTR_KEY_STRING_DEFAULT "DEFAULT"




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
	 * @ingroup GeneralKeys
	 * #QobjType_t of parent.
	 * This field <i>must</i> exist in any given #QattrList_t.
	 */ 
	QATTR_KEY_QOBJECT_TYPE = Q_ENUM_VALUE_START,
	
	/**
	 * @ingroup GeneralKeys
	 * Name of object.
	 */
	QATTR_KEY_NAME,                      
	
	/**
	 * @ingroup GeneralKeys
	 * Brief description of object.
	 */
	QATTR_KEY_DESCRIPTION_BRIEF,

	/**
	 * @ingroup GeneralKeys
	 * Long description of object.
	 */
	QATTR_KEY_DESCRIPTION_LONG,	
	

	/**
	 * @defgroup QwalkKeys Qwalk-Exclusive Attribute Keys
	 * A #QattrKey_t whose usage is exclusive to qwalk.
	 */
	/* QWALK-EXCLUSIVE ATTRIBUTE KEYS */ 
	
	/**
	 * @ingroup QwalkKeys 
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
	 * @ingroup InternalKeys
	 * Key for an empty (i.e. @c NULL) attribute.
	 */
	QATTR_KEY_EMPTY,                     
	
	/**
	 * @ingroup InternalKeys
	 * Test key for debugging.
	 */
	QATTR_KEY_DEBUG,                     
	
	/**
	 * @ingroup InternalKeys
	 * Number of allowed values for #QattrKey_t.
	 * Must be defined via the second-to-last enum.
	 */
	QATTR_KEY_COUNT = QATTR_KEY_DEBUG
	
} QattrKey_t;


/**
 * @defgroup AttrKeyStrings Qattr_t Key Strings
 * For use in converting #QattrKey_t values to string (@c char *) values.
 */

/**
 * @ingroup AttrKeyStrings
 * String for #QATTR_KEY_QOBJECT_TYPE.
 */
#define QATTR_STRING_KEY_QOBJECT_TYPE      "object type"
/**
 * @ingroup AttrKeyStrings
 * String for #QATTR_KEY_NAME.
 */
#define QATTR_STRING_KEY_NAME              "name"
/**
 * @ingroup AttrKeyStrings
 * String for #QATTR_KEY_DESCRIPTION_LONG.
 */
#define QATTR_STRING_KEY_DESCRIPTION_BRIEF "brief description"
/**
 * @ingroup AttrKeyStrings
 * String for #QATTR_KEY_DESCRIPTION_LONG.
 */
#define QATTR_STRING_KEY_DESCRIPTION_LONG  "long description"
/**
 * @ingroup AttrKeyStrings
 * String for #QATTR_KEY_CANMOVE.
 */
#define QATTR_STRING_KEY_CANMOVE           "can move"
/**
 * @ingroup AttrKeyStrings
 * String for #QATTR_KEY_EMPTY.
 */
#define QATTR_STRING_KEY_EMPTY             "empty"
/**
 * @ingroup AttrKeyStrings
 * String for #QATTR_KEY_DEBUG.
 */
#define QATTR_STRING_KEY_DEBUG             "debug"
/**
 * @ingroup AttrKeyStrings
 * String for an unrecognized #QattrKey_t.
 */
#define QATTR_STRING_KEY_UNRECOGNIZED      "KEY NOT RECOGNIZED!"


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
	size_t   count; /**< The number of #Qattr_t in the list. */
	/*@only@*/Qattr_t *attrp; /**< The actual collection of #Qattr_t structs. */
	size_t   index_ok; /**< The earliest index of attrp that isn't yet in use. */
} QattrList_t;

/** Splint type for a #QattrList_t with the `/\*@only*\/` annotation. */
typedef /*@only@*/QattrList_t *OnlyQattrListp_t;



/** Return a newly-created attr list of a given size.                    */
extern /*@null@*//*@partial@*/QattrList_t *qattr_list_create(size_t);

/** Clone a #QattrList_t.                                                */
/*@unused@*//*@null@*/
extern QattrList_t *qattr_list_clone(const QattrList_t *);

/** Resize a #QattrList_t.                                               */

/** Free a given #QattrList_t from memory.                               */
extern int qattr_list_destroy(/*@only@*/QattrList_t *);

/** Resize a #QattrList_t.                                               */
/*@only@*//*@null@*/
extern QattrList_t *qattr_list_resize(/*@only@*/QattrList_t *qattr_list,
		int dilation_addend)/*@modifies qattr_list@*/;

/** Write a #QattrList_t to storage.                                     */
extern int qattr_list_write(const QattrList_t *);

/** Read a #QattrList_t from storage.                                    */
extern /*@null@*//*@only@*/QattrList_t *qattr_list_read(void);

/** Return the value associated with a key in a #QattrList_t.             */
extern /*@null@*//*@observer@*/Qdatameta_t *qattr_list_value_get(
		/*@returned@*/const QattrList_t *, QattrKey_t)/*@*/;

/** Convert a #QattrKey_t to an index in a #QattrList_t.                  */
extern int qattr_list_key_to_index(
		const QattrList_t *attr_list, QattrKey_t attr_key)/*@*/;

/** Return @ref QattrList_t.count.                                        */
/*@unused@*/extern size_t qattr_list_count_get(const QattrList_t *)/*@*/;

/** Return @ref QattrList_t.index_ok.                                     */
/*@unused@*/extern size_t qattr_list_index_ok_get(const QattrList_t *)/*@*/;

/** Return @ref Qattr_t.key from a #QattrList_t as addressed by an index. */
/*@unused@*/extern QattrKey_t qattr_list_attr_key_get(const QattrList_t *, int)/*@*/;

/** Trade the places of two #QattrKey_t members of a #QattrList_t.        */
extern void qattr_list_attrs_swap(QattrList_t *attr_list,
		size_t mover_index, size_t movend_index)/*@modifies attr_list@*/;

/** Set a #QattrKey_t/#Qdatameta_t pair in the given #QattrList_t.        */
extern int qattr_list_attr_set(QattrList_t *, QattrKey_t, /*@only@*/Qdatameta_t *);

/** Delete a #Qattr_t in the given #QattrList_t.                          */
extern int qattr_list_attr_delete(OnlyQattrListp_t *attr_listp,
		QattrKey_t key)/*@modifies attr_listp@*/;

/** Modify the value of an existing #QattrKey_t.                          */
/*@unused@*/extern int qattr_list_attr_modify(QattrList_t *attr_list,
		QattrKey_t attr_key, /*@only@*/Qdatameta_t *datameta)
	/*@modifies attr_list@*/;

/** Convert a @ref Qattr_t.valuep to an `char *`.                         */
/*@observer@*//*@unused@*/extern char *qattr_value_to_string(const QattrList_t *, QattrKey_t)/*@*/;

/** Convert a #QattrKey_t to a @c char *.                                 */
/*@observer@*//*@unused@*/extern char *qattr_key_to_string(QattrKey_t)/*@*/;

/** Convert a `char *` to a #QattrKey_t.                                  */
/*@unused@*/extern QattrKey_t qattr_string_to_key(const char *)/*@*/;
