/**
 * @file item_inventory.c
 * Program file for the #ItemInventory_t interface.
 */




#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "qdefs.h"
#include "qerror.h"

#include "item.h"




ItemInventory_t *
item_inventory_create(void) {

	ItemInventory_t *inventory;

	if ((inventory = calloc((size_t) 1, sizeof(*inventory))) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		return NULL;
	}

	if ((inventory->items = calloc(
					(size_t) ITEM_INVENTORY_ITEMS_MAX_DEFAULT,
					sizeof(inventory->items)))
			== NULL) {
		Q_ERROR_SYSTEM("calloc()");
		free(inventory);
		return NULL;
	}

	for (int i = 0; i < ITEM_INVENTORY_ITEMS_MAX_DEFAULT; i++) {
		inventory->items[i] = ITEM_ID_EMPTY;
	}

	inventory->items_max = ITEM_INVENTORY_ITEMS_MAX_DEFAULT;
	inventory->index_ok = 0;

	inventory->equip_slot_hands = ITEM_INVENTORY_EQUIP_SLOT_EMPTY;
	inventory->equip_slot_body = ITEM_INVENTORY_EQUIP_SLOT_EMPTY;

	return inventory;
}


void
item_inventory_destroy(ItemInventory_t *inventory) {

	free(inventory->items);
	free(inventory);

	return;
}


int
item_inventory_write(ItemInventory_t *inventory) {
	return Q_OK;
}


ItemInventory_t *
item_inventory_read() {
	return NULL;
}


/**
 * Set an #ItemID_t to belong to an #ItemInventory_t.
 * @param[out] inventory: #ItemInventory_t in question.
 * @param[in] id: #ItemID_t to give to @p inventory.
 * @return #Q_OK or #Q_ERROR.
 */
int
item_inventory_item_set(ItemInventory_t *inventory, ItemID_t id) {

	if (inventory->index_ok >= inventory->items_max) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}

	if ((id < (ItemID_t) Q_ENUM_VALUE_START) || (id > ITEM_ID_COUNT)) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}

	inventory->items[inventory->index_ok++] = id;

	return Q_OK;
}


/**
 * Resize an #ItemInventory_t.
 * Change @ref ItemInventory_t.items_max and resize @ref ItemInventory_t.items.
 * @param[out] inventory: #ItemInventory_t to resize.
 * @param[in] items_max: value to resize @p inventory to.
 * @return #Q_OK or #Q_ERROR.
 */
int
item_inventory_items_max_set(ItemInventory_t *inventory, int items_max) {

	if (inventory->index_ok > items_max) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}

	ItemID_t *items_new;

	if ((items_new = calloc((size_t) items_max, sizeof(*items_new))) == NULL) {
		Q_ERROR_SYSTEM("calloc()");
		return Q_ERROR;
	}

	for (int i = 0; i < items_max; i++) {
		if (i < inventory->index_ok) {
			items_new[i] = inventory->items[i];
		} else {
			items_new[i] = ITEM_ID_EMPTY;
		}
	}

	free(inventory->items);

	inventory->items = items_new;
	inventory->items_max = items_max;

	return Q_OK;
}


/**
 * Set @p id from @p inventory to @p equip_slot in @p inventory.
 * @param[out] inventory: #ItemInventory_t to modify.
 * @param[in] equip_slot: #ItemEquipSlot_t the slot is addressed by.
 * @param[in] id: #ItemID_t from @p inventory to equip.
 * @return #Q_OK or #Q_ERROR.
 */
int
item_inventory_equip_slot_set(
		ItemInventory_t *inventory, ItemEquipSlot_t equip_slot, int index) {

	int *equip_slotp;

	if (index >= inventory->items_max) {
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}

	switch (equip_slot) {
	case ITEM_EQUIP_SLOT_BODY:
		equip_slotp = &(inventory->equip_slot_body);
		break;
	case ITEM_EQUIP_SLOT_HANDS:
		equip_slotp = &(inventory->equip_slot_hands);
		break;
	default:
		Q_ERRORFOUND(QERROR_PARAMETER_INVALID);
		return Q_ERROR;
	}

	if (index >= inventory->index_ok) {
		*equip_slotp = ITEM_INVENTORY_EQUIP_SLOT_EMPTY; 
		return Q_OK;
	}

	*equip_slotp = index;

	return Q_OK;
}
