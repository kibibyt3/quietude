/**
 * @file dialogue.h
 * Header file for the dialogue module.
 */



/**
 * Specific member or submember within a #DialogueTree_t to parse for.
 */
typedef enum DialogueParseMode_t {

	/** Parse for @ref DialogueTree_t.title. */
	DIALOGUE_PARSE_MODE_TREE_TITLE = Q_ENUM_VALUE_START,
	/** Parse for @ref DialogueBranch_t.header. */
	DIALOGUE_PARSE_MODE_BRANCH_HEADER,
	/** Parse for @ref DialogueBranch_t.message. */
	DIALOGUE_PARSE_MODE_BRANCH_MESSAGE,
	/** Parse for @ref DialogueObject_t.response. */
	DIALOGUE_PARSE_MODE_OBJECT_RESPONSE,
	/** Parse for @ref DialogueObject_t.commands. */
	DIALOGUE_PARSE_MODE_OBJECT_COMMAND,
	/** Parse for @ref DialogueObject_t.args. */
	DIALOGUE_PARSE_MODE_OBJECT_ARG,

	DIALOGUE_PARSE_MODE_COUNT = DIALOGUE_PARSE_MODE_OBJECT_ARG

} DialogueParseMode_t;

/**
 * @defgroup ParseChars Dialogue Parse Characters
 * Characters that have a special meaning for QDL (Q dialogue) file parsing.
 * @{
 */
/** Beginning character for a @ref DialogueTree_t.title. */
#define DIALOGUE_PARSE_CHAR_TREE_TITLE_BEG '['
/** Ending character for a @ref DialogueTree_t.title. */
#define DIALOGUE_PARSE_CHAR_TREE_TITLE_END ']'
/** Beginning character for one of the @ref DialogueTree_t.branches. */
#define DIALOGUE_PARSE_CHAR_BRANCH_BEG '{'
/** Ending character for one of the @ref DialogueTree_t.branches. */
#define DIALOGUE_PARSE_CHAR_BRANCH_END '}'
/** Beginning character for @ref DialogueObject_t.commands. */
#define DIALOGUE_PARSE_CHAR_OBJECT_COMMANDS_BEG '('
/** Ending character for @ref DialogueObject_t.commands. */
#define DIALOGUE_PARSE_CHAR_OBJECT_COMMANDS_END ')'
/** Character to separate a command from its arg. */
#define DIALOGUE_PARSE_CHAR_COMMAND_ARG_SEPARATOR ' '
/** Delimiter for @ref DialogueObject_t.commands. */
#define DIALOGUE_PARSE_CHAR_COMMAND_DELIMITER ';'
/** Beginning/ending character for a string. */
#define DIALOGUE_PARSE_CHAR_STRING '"'
/** @} */

/**
 * A specific command executed (sometimes in concordance with an argument)
 * before or after standard execution of a parent #DialogueObject_t.
 */
typedef enum DialogueCommand_t {
	
	/** Goto a given @ref DialogueBranch_t.header. */
	DIALOGUE_COMMAND_GOTO = Q_ENUM_VALUE_START, 
	
	/** Turn the NPC into a given #QobjType_t. */
	DIALOGUE_COMMAND_BECOME,
	
	/** Exit the #DialogueTree_t. */
	DIALOGUE_COMMAND_EXIT,

	/** Total number of possible values for a #DialogueCommand_t. */
	DIALOGUE_COMMAND_COUNT = DIALOGUE_COMMAND_EXIT

} DialogueCommand_t;

/** For conversion to and from #DIALOGUE_COMMAND_GOTO. */
#define DIALOGUE_STRING_COMMAND_GOTO "goto"
/** For conversion to and from #DIALOGUE_COMMAND_BECOME. */
#define DIALOGUE_STRING_COMMAND_BECOME "become"
/** For conversion to and from #DIALOGUE_COMMAND_EXIT. */
#define DIALOGUE_STRING_COMMAND_EXIT "exit"


/**
 * A specific line of dialogue the player can choose to say to an NPC.
 */
typedef struct DialogueObject_t {

	char *response; /**< Line for the player to say. */
	
	/**
	 * Commands conditionally executed on, before, or after a #DialogueObject_t. 
	 */
	DialogueCommand_t *commands;

	/**
	 * Arguments for each #DialogueCommand_t.
	 * Indexed in lockstep with @ref DialogueObject_t.commands.
	 */
	char **args;

	/** Total number of pointers in @ref DialogueObject_t.commands. */
	size_t sz;

} DialogueObject_t;


/**
 * A branch in a #DialogueTree_t.
 */
typedef struct DialogueBranch_t {

	/*
	 * TODO: implement it such that the header can merely be an int; that is to
	 * say, dynamically generate an int for each header upon parsing for the
	 * branch to be addressed by.
	 */
	char *header; /**< Mnemonic to refer to the specific branch. */
	
	/** 
	 * Line of dialogue spoken by the parent NPC.
	 * Spefically spoken when the #DialogueBranch_t is encountered.
	 */
	char *message;

	/**
	 * Collection of each #DialogueObject_t that resides in the #DialogueBranch_t.
	 */
	DialogueObject_t **objects;

	/** Total number of pointers in @ref DialogueBranch_t.objects. */
	size_t sz;

} DialogueBranch_t;


/**
 * An entire tree of dialogue for an NPC.
 */
typedef struct DialogueTree_t {

	/** Current @ref DialogueBranch_t.header we're using. */
	char *header_active;

	/** Title of #DialogueTree_t. */
	char *title; 

	/** Every #DialogueBranch_t in the #DialogueTree_t. */
	DialogueBranch_t **branches;

	/** Total number of pointers in @ref DialogueTree_t.branches. */
	size_t sz;

} DialogueTree_t;




extern int dialogue_logic_init(const char *)/*@modifies internalState@*/;
