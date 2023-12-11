/**
 * @file player.h
 * Header file for the player module.
 * Depends on @ref qdefs.h.
 */



typedef struct Agent_t {
	char *name;
	int hp;
	int level;
	Qflags_t status_flags;
} Agent_t;

typedef struct Player_t {
	Agent_t agent;
	Qflags_t progress_flags;
} Player_t;

typedef struct AgentMain_t {
	Agent_t agent;
	Qflags_t tree_flags;
}
