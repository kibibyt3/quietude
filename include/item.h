/**
 * @file item.h
 * Header file for the item module.
 * Depends on @ref qdefs.h.
 */



typedef enum ItemID_t {

	ITEM_ID_EMPTY = Q_ENUM_VALUE_START,

	ITEM_ID_WEAPON_MIN,
	ITEM_ID_WEAPON_SHORTSWORD = ITEM_ID_WEAPON_MIN,
	ITEM_ID_WEAPON_BASTARDSWORD,
	ITEM_ID_WEAPON_ZWEIHANDER,
	ITEM_ID_WEAPON_MAX = ITEM_ID_WEAPON_ZWEIHANDER,

	ITEM_ID_ARMOUR_MIN,
	ITEM_ID_ARMOUR_CLOAK = ITEM_ID_ARMOUR_MIN,
	ITEM_ID_ARMOUR_HAUBERK,
	ITEM_ID_ARMOUR_KNIGHT,
	ITEM_ID_ARMOUR_MAX,

	ITEM_ID_BOOK_MIN,
	ITEM_ID_BOOK_ILOVEYOU = ITEM_ID_BOOK_MIN,
	ITEM_ID_BOOK_A,
	ITEM_ID_BOOK_BABEL,
	ITEM_ID_BOOK_MAX = ITEM_ID_BOOK_BABEL,

	ITEM_ID_COUNT = ITEM_ID_WEAPON_MAX

} ItemID_t;



typedef enum ItemType_t {
	ITEM_TYPE_WEAPON = Q_ENUM_VALUE_START,
	ITEM_TYPE_ARMOUR,
	ITEM_TYPE_BOOK,
	ITEM_TYPE_COUNT = ITEM_TYPE_BOOK
} ItemType_t;


typedef enum ItemEquipSlot_t {
	/** Used to refer to @ref ItemInventory_t.equip_slot_hands. */
	ITEM_EQUIP_SLOT_HANDS = Q_ENUM_VALUE_START,
	/** Used to refer to @ref ItemInventory_t.equip_slot_body. */
	ITEM_EQUIP_SLOT_BODY,

	ITEM_EQUIP_SLOT_COUNT = ITEM_EQUIP_SLOT_BODY
} ItemEquipSlot_t;



typedef struct ItemReferenceWeapon_t {
	ItemID_t id;
	ItemEquipSlot_t equip_slot;
	/*@observer@*/const char *name;
	/*@observer@*/const char *description;
	int damage;
	int max_range;
	double hit_chance;
	double armour_penetration;
} ItemReferenceWeapon_t;


typedef struct ItemReferenceArmour_t {
	ItemID_t id;
	ItemEquipSlot_t equip_slot;
	/*@observer@*/const char *name;
	/*@observer@*/const char *description;
	int defense;
	double encumberance;
	double warmth;
} ItemReferenceArmour_t;


typedef struct ItemReferenceBook_t {
	ItemID_t id;
	ItemEquipSlot_t equip_slot;
	/*@observer@*/const char *name;
	/*@observer@*/const char *description;
	/*@observer@*/const char *book_filename;
} ItemReferenceBook_t;



/** Used to set an equip slot as empty. */
#define ITEM_INVENTORY_EQUIP_SLOT_EMPTY -1000
typedef struct ItemInventory_t {
	ItemID_t *items;
	int items_max;
	int index_ok;
	/**
	 * Index in @ref ItemInventory_t.items to equip to hands.
	 * Can be set to #ITEM_INVENTORY_EQUIP_SLOT_EMPTY if nothing is equipped.
	 */
	int equip_slot_hands;
	/**
	 * Index in @ref ItemInventory_t.items to equip to body.
	 * Can be set to #ITEM_INVENTORY_EQUIP_SLOT_EMPTY if nothing is equipped.
	 */
	int equip_slot_body;
} ItemInventory_t;



/**
 * @defgroup ItemInventoryInterface Item Inventory Interface
 * Interface for #ItemInventory_t.
 * @{
 */

#define ITEM_INVENTORY_ITEMS_MAX_DEFAULT 15
/*@null@*/
extern ItemInventory_t *item_inventory_create(void)/*@*/;

extern void item_inventory_destroy(/*@only@*/ItemInventory_t *inventory)
	/*@modifies inventory@*/;

extern int item_inventory_write(ItemInventory_t *inventory);
/*@null@*/
extern ItemInventory_t *item_inventory_read(void);


/**
 * @defgroup ItemInventorySetters Item Inventory Setters
 * @{
 */

extern int item_inventory_item_set(ItemInventory_t *inventory, ItemID_t id)
	/*@modifies inventory@*/;

extern int item_inventory_items_max_set(
		ItemInventory_t *inventory, int items_max)/*@modifies inventory@*/;

extern int item_inventory_equip_slot_set(
		ItemInventory_t *inventory, ItemEquipSlot_t equip_slot, int index)
	/*@modifies inventory@*/;

/** @} */


/**
 * @defgroup ItemInventoryGetters Item Inventory Getters
 * @{
 */

extern ItemID_t item_inventory_item_get(ItemInventory_t *inventory, int index)
	/*@*/;
extern int item_inventory_items_max_get(ItemInventory_t *inventory)/*@*/;
extern int item_inventory_index_ok_get(ItemInventory_t *inventory)/*@*/;
extern ItemID_t item_inventory_equip_slot_get(
		ItemInventory_t *inventory, ItemEquipSlot_t equip_slot)/*@*/;

/** @} */


/** @} */


/**
 * @defgroup ItemReferenceGetters Item Reference Getters
 * Get members from item reference arrays, as indexed by an #ItemID_t.
 * @{
 */

/**
 * @defgroup ItemReferenceGettersUniversal Universal Item Reference Getters
 * Getters for members shared among each item reference type.
 * @{
 */

extern ItemEquipSlot_t item_reference_equip_slot_get(ItemID_t id)
	/*@globals internalState@*/;

/*@observer@*//*@null@*/
extern const char     *item_reference_name_get(ItemID_t id)
	/*@globals internalState@*/;

/*@observer@*//*@null@*/
extern const char     *item_reference_description_get(ItemID_t id)
	/*@globals internalState@*/;

/** @} */

extern int item_reference_weapon_damage_get(ItemID_t id)
	/*@globals internalState@*/;
extern int item_reference_weapon_max_range_get(ItemID_t id)
	/*@globals internalState@*/;
extern double item_reference_weapon_hit_chance_get(ItemID_t id)
	/*@globals internalState@*/;
extern double item_reference_weapon_armour_penetration_get(ItemID_t id)
	/*@globals internalState@*/;

extern int item_reference_armour_defense_get(ItemID_t id)
	/*@globals internalState@*/;
extern double item_reference_armour_encumberance_get(ItemID_t id)
	/*@globals internalState@*/;
extern double item_reference_armour_warmth_get(ItemID_t id)
	/*@globals internalState@*/;

/*@observer@*//*@null@*/
extern const char *item_reference_book_filename_get(ItemID_t id)
	/*@globals internalState@*/;

/** @} */


/**
 * @defgroup ItemReferenceIndexGetters Item Reference Index Getters
 * Functions to get indexes for item references from item reference arrays.
 * @{
 */
extern int  item_reference_weapons_index_get(ItemID_t id)
	/*@globals internalState@*/;
extern int  item_reference_armour_index_get(ItemID_t id)
	/*@globals internalState@*/;
extern int  item_reference_books_index_get(ItemID_t id)
	/*@globals internalState@*/;
/** @} */

/**
 * @defgroup ItemIstypeFunctions Item Istype Functions
 * Determines if a given #ItemID_t refers to a specific #ItemType_t.
 * @{
 */
extern bool item_isweapon(ItemID_t id)/*@*/;
extern bool item_isarmour(ItemID_t id)/*@*/;
extern bool item_isbook(ItemID_t id)/*@*/;
/** @} */

extern ItemType_t item_type_get(ItemID_t id)/*@*/;
