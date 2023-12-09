/**
 * @file player.h
 * Header file for the player module.
 * Depends on @ref qdefs.h.
 */



typedef struct Player_t {
	char *name;
	Qflags_t flags;
	int hp;
	int level;
} Player_t;
