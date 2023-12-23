/**
 * @file agent.h
 * Header file for the agent module.
 * Depends on @ref qdefs.h and @ref item.h.
 */




/**
 * Type of an #Agent_t.
 */
typedef enum AgentType_t {

	AGENT_TYPE_PLAYER = Q_ENUM_VALUE_START, /**< Player type. */
	AGENT_TYPE_NPC_FRIENDLY, /**< Friendly NPC type. */
	AGENT_TYPE_NPC_ENEMY, /**< Enemy NPC type. */

	/** Number of possible values for an #AgentType_t. */
	AGENT_TYPE_COUNT = AGENT_TYPE_NPC_ENEMY

} AgentType_t;


#define AGENT_NAME_LEN_MAX	30
typedef struct Agent_t {
	AgentType_t agent_type;
	char *name;
	int hp_max;
	int hp;
	int level;
	ItemInventory_t *inventory;
	Qflags_t flags;
} Agent_t;


typedef enum AgentPresetFileSection_t {

	AGENT_PRESET_FILE_SECTION_TYPE = Q_ENUM_VALUE_START,
	AGENT_PRESET_FILE_SECTION_NAME,
	AGENT_PRESET_FILE_SECTION_HP_MAX,
	AGENT_PRESET_FILE_SECTION_LEVEL,
	AGENT_PRESET_FILE_SECTION_INVENTORY,
	AGENT_PRESET_FILE_SECTION_FLAGS

} AgentPresetFileSection_t;




/*@only@*//*@null@*/
extern Agent_t	*agent_create(const char *preset_file);
extern void			agent_destroy(/*@only@*/Agent_t *agent);


extern AgentType_t	agent_type_get(Agent_t *agent);
extern char					*agent_name_get(Agent_t *agent);
extern int					agent_hp_max_get(Agent_t *agent);
extern int					agent_hp_get(Agent_t *agent);
extern int					agent_level_get(Agent_t *agent);
extern ItemID_t			agent_inventory_item_id_get(Agent_t *agent, int index);
extern bool					agent_flags_getf(int i, Agent_t *agent);


extern int	agent_type_set(Agent_t *agent, AgentType_t agent_type);
extern int	agent_name_set(Agent_t *agent, char *name);
extern void	agent_hp_max_set(Agent_t *agent, int hp_max);
extern void	agent_hp_set(Agent_t *agent, int hp);
extern void	agent_level_set(Agent_t *agent, int level);
extern int	agent_inventory_item_id_set(Agent_t *agent, int index, ItemID_t id);
extern int	agent_flags_setf(int i, Agent_t *agent, bool b);
