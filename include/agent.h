/**
 * @file agent.h
 * Header file for the agent module.
 * Depends on @ref qdefs.h.
 */



/**
 * Type of an #Agent_t.
 */
typedef enum AgentType_t {

	AGENT_TYPE_PLAYER = Q_ENUM_VALUE_START, /** Player type. */
	AGENT_TYPE_NPC_FRIENDLY, /**< Friendly NPC type. */
	AGENT_TYPE_NPC_ENEMY, /**< Enemy NPC type. */

	/** Number of possible values for an #AgentType_t. */
	AGENT_TYPE_COUNT = AGENT_TYPE_NPC_ENEMY

} AgentType_t;

typedef struct Agent_t {
	AgentType_t agent_type;
	char *name;
	int hp;
	int level;
	ItemInventory_t *inventory;
	Qflags_t flags;
} Agent_t;
