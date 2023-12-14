/**
 * @file item.h
 * Header file for the item module.
 * Depends on @ref qdefs.h.
 */



typedef enum ItemID_t {

	ITEM_ID_WEAPON_MIN = Q_ENUM_VALUE_START,
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



typedef enum ItemEquipSlot_t {
	ITEM_EQUIP_SLOT_HANDS = Q_ENUM_VALUE_START,
	ITEM_EQUIP_SLOT_BODY,

	ITEM_EQUIP_SLOT_COUNT = ITEM_EQUIP_SLOT_BODY
} ItemEquipSlot_t;



typedef enum ItemType_t {
	ITEM_TYPE_WEAPON = Q_ENUM_VALUE_START,
	ITEM_TYPE_ARMOUR,
	ITEM_TYPE_BOOK,
	ITEM_TYPE_COUNT = ITEM_TYPE_BOOK
} ItemType_t;



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



extern bool item_isweapon(ItemID_t id);
extern bool item_isarmour(ItemID_t id);
extern bool item_isbook(ItemID_t id);

extern ItemType_t item_gettype(ItemID_t id);
