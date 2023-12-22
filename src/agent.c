/**
 * @file agent.c
 * Module responsible for agents, which include NPCs and PCs.
 */



#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "qdefs.h"
#include "qerror.h"
#include "item.h"
#include "agent.h"




Agent_t *
agent_create(char *preset_file) {
	return NULL;
}


void
agent_destroy(/*@only@*/Agent_t *agent) {

	free(agent->name);
	free(agent->inventory);

	free(agent);

	return;
}



int
agent_type_set(Agent_t *agent, AgentType_t agent_type) {

	if ((agent_type < (AgentType_t) Q_ENUM_VALUE_START)
			|| (agent_type > AGENT_TYPE_COUNT)) {
		Q_ERRORFOUND(QERROR_ENUM_CONSTANT_INVALID);
		return Q_ERROR;
	}

	agent->agent_type = agent_type;

	return Q_OK;
}


int
agent_name_set(Agent_t *agent, char *name) {

	if (strlen(name) > (size_t) AGENT_NAME_LEN_MAX) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}

	agent->name = name;
	return Q_OK;
}


void
agent_hp_max_set(Agent_t *agent, int hp_max) {

	agent->hp_max = hp_max;
	return;
}


void
agent_hp_set(Agent_t *agent, int hp) {

	agent->hp = hp;
	return;
}


void
agent_level_set(Agent_t *agent, int level) {

	agent->level = level;
	return;
}


int
agent_inventory_item_id_set(Agent_t *agent, int index, ItemID_t id) {

	if (item_inventory_item_set(agent->inventory, index, id) == Q_ERROR) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	return Q_OK;
}


void
agent_flags_setf(int i, Agent_t *agent, bool b) {

	qflags_setf(i, agent->flags, b);
	return;
}



AgentType_t
agent_type_get(Agent_t *agent) {
	return agent->agent_type;
}


char *
agent_name_get(Agent_t *agent) {
	return agent->name;
}


int
agent_hp_max_get(Agent_t *agent) {
	return agent->hp_max;
}


int
agent_hp_get(Agent_t *agent) {
	return agent->hp;
}


int
agent_level_get(Agent_t *agent) {
	return agent->level;
}


ItemID_t
agent_inventory_item_id_get(Agent_t *agent, int index) {

	ItemID_t id;
	if ((id = item_inventory_item_get(agent->inventory, index))
			== (ItemID_t) Q_ERRORCODE_ENUM) {
		Q_ERRORFOUND(QERROR_ERRORVAL);
		return Q_ERROR;
	}

	return id;
}


bool
agent_flags_getf(int i, Agent_t *agent) {

	return qflags_getf(i, agent->flags);
}
