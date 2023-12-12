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
	ITEM_ID_WEAPON_MAX = ITEM_ID_ZWEIHANDER,


	ITEM_ID_ARMOUR_MIN,
	ITEM_ID_ARMOUR_CLOAK = ITEM_ID_ARMOUR_MIN,
	ITEM_ID_ARMOUR_KNIGHT,
	ITEM_ID_ARMOUR_HAUBERK,
	ITEM_ID_ARMOUR_MAX

	ITEM_ID_BOOK,

	ITEM_ID_COUNT = ITEM_ID_WEAPON_MAX;

} ItemID_t;



typedef enum ItemType_t {
	ITEM_TYPE_WEAPON = Q_ENUM_VALUE_START,
	ITEM_TYPE_ARMOUR,
	ITEM_TYPE_BOOK,
	ITEM_TYPE_COUNT = ITEM_TYPE_BOOK
} ItemType_t;



typedef struct ItemReferenceWeapon_t {
	ItemID_t id;
	const char *name;
	const char *description;
	int damage;
	int max_range;
	float hit_chance;
	float armour_penetration;
} ItemReferenceWeapon_t;

typedef struct ItemReferenceArmour_t {
	ItemID_t id;
	const char *name;
	const char *description;
	int defense;
	int encumberance;
} ItemReferenceArmour_t;

typedef struct ItemReferenceBook_t {
	ItemID_t id;
	const char *name;
	const char *description;
	const char *book_filename;
} ItemReferenceBook_t;
