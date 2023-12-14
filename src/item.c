/**
 * @file item.c
 * Program file for the item module.
 * Responsible for handling items and their attributes.
 */



/**********
 * WEAPONS
 **********/

/**
 * @defgroup ItemReferenceWeapons Item Reference Weapons
 * All values associated with weapon item references.
 * @{
 */

/* i'm using a xeiphos as reference */
#define ITEM_DESCRIPTION_WEAPON_SHORTSWORD \
	"A short blade; you take care not to point it at yourself; you can detect " \
	"in its chipped edges a mild distaste for you and precious few qualms with " \
	"the act of murder."

#define ITEM_DESCRIPTION_WEAPON_BASTARDSWORD \
	"A long blade; its grip is corroded with the sweat of the countless who " \
	"wielded it before you. Tap the blade against something firm to yield its " \
	"high-pitched, unpracticed warble."

#define ITEM_DESCRIPTION_WEAPON_ZWEIHANDER \
	"A great zweihander; its length is so great as to brush the bellies of the " \
	"swollen clouds above. It wields you. You are its instrument; you are its " \
	"extension. It is your fear congealed into weaponform."

#define WEAPONSC 3
static const ItemReferenceWeapon_t weapons[WEAPONSC] = {
	{ITEM_ID_WEAPON_SHORTSWORD, ITEM_EQUIP_SLOT_HANDS,
		"short sword", ITEM_DESCRIPTION_WEAPON_SHORTSWORD,
		3, 1, 0.7, 0.2},
/*  ^  ^  ^    ^
 *  |  |  |    '-  armour_penetration
 *  |  |  '------  hit_chance
 *  |  '---------  max_range
 *  '------------  damage
 */

	{ITEM_ID_WEAPON_BASTARDSWORD, ITEM_EQUIP_SLOT_HANDS,
		"bastard sword", ITEM_DESCRIPTION_WEAPON_BASTARDSWORD,
		4, 1, 0.6, 0.3},

	{ITEM_ID_WEAPON_ZWEIHANDER, ITEM_EQUIP_SLOT_HANDS,
		"zweihander", ITEM_DESCRIPTION_WEAPON_ZWEIHANDER,
		5, 2, 0.5, 0.4}
}

/** @} */


/*********
 * ARMOUR
 *********/

/**
 * @defgroup ItemReferenceArmour Item Reference Armour
 * All values associated with armour item references.
 * @{
 */

#define ITEM_DESCRIPTION_ARMOUR_CLOAK \
	"A coat of thick fur pulled from the body of a long-dead creature. You " \
	"think you can still smell the blood and the sticky, greasy, thick meat "
	"from the night (day?) of its death."

#define ITEM_DESCRIPTION_ARMOUR_HAUBERK \
	"A shirt of chainmail; the manifold metal rings all cling tight to each " \
	"other like a pile of hot, blood-pumping bodies. The dizzying lattice " \
	"plays tricks on your eyes."

#define ITEM_DESCRIPTION_ARMOUR_KNIGHT \
	"A complete set of knight's armour. To don it makes you feel so utterly " \
	"trapped. A claustrophobia of the greatest strength grips your heart with " \
	"black, withered fingers. You can taste the carbon dioxide building. You " \
	"take shallower breaths to compensate. It doesn't help."

#define ARMOURC 3
static const ItemReferenceArmour_t armour[ARMOURC] = {
	{ITEM_ID_ARMOUR_CLOAK, ITEM_EQUIP_SLOT_BODY,
		"cloak", ITEM_DESCRIPTION_ARMOUR_CLOAK,
		0, 0.0, 0.2},
/*  ^  ^    ^
 *  |  |    '-  warmth
 *  |  '------  encumberance
 *  '---------  defense
 */

	{ITEM_ID_ARMOUR_HAUBERK, ITEM_EQUIP_SLOT_BODY,
		"hauberk", ITEM_DESCRIPTION_ARMOUR_HAUBERK,
		1, 0.05, 0.02},
	{ITEM_ID_ARMOUR_KNIGHT, ITEM_EQUIP_SLOT_BODY,
		"knight's armour", ITEM_DESCRIPTION_ARMOUR_KNIGHT,
		3, 0.3, 0.05}
}

/** @} */


/********
 * BOOKS
 ********/

/**
 * @defgroup ItemReferenceBooks Item Reference Books
 * All values associated with book item references.
 * @{
 */
#define ITEM_DESCRIPTION_BOOK_ILOVEYOU \
	"A book whose cover reads: \"I LOVE YOU\", written in immaculate gothic " \
	"font. The cover is deep blood-red. Is it bleeding?"

#define ITEM_DESCRIPTION_BOOK_A \
	"A capital letter 'A' adorns the front cover. The spine is marred and " \
	"curls awkwardly. It's still warm..."

#define ITEM_DESCRIPTION_BOOK_BABEL \
	"A book with a rough, fuzzy cover. Stitched into it is the title: " \
	"oeooii hmepizlzz."

#define BOOKC 3
static const ItemReferenceBook_t books[BOOKC] = {
	{ITEM_ID_BOOK_ILOVEYOU, ITEM_EQUIP_SLOT_HANDS,
		"book: ILOVEYOU", ITEM_DESCRIPTION_BOOK_ILOVEYOU,
		"iloveyou.txt"},
	{ITEM_ID_BOOK_A, ITEM_EQUIP_SLOT_HANDS,
		"book: AAAAAAAA", ITEM_DESCRIPTION_BOOK_A,
		"a.txt"},
	{ITEM_ID_BOOK_BABEL, ITEM_EQUIP_SLOT_HANDS,
		"book: oeooii hmepizlzz", ITEM_DESCRIPTION_BOOK_BABEL,
		"babel.txt"}
};

/** @} */




/**
 * Determine if an #ItemID_t refers to a weapon.
 * @param[in] id: #ItemID_t in question.
 * @return `true` if it is a weapon, false if otherwise.
 */
bool
item_isweapon(ItemID_t id) {
	if ((id >= ITEM_ID_WEAPON_MIN) && (id <= ITEM_ID_WEAPON_MAX)) {
		return true;
	}
	return false;
}


/**
 * Determine if an #ItemID_t refers to armour.
 * @param[in] id: #ItemID_t in question.
 * @return `true` if it is armour, false if otherwise.
 */
bool
item_isarmour(ItemID_t id) {
	if ((id >= ITEM_ID_ARMOUR_MIN) && (id <= ITEM_ID_ARMOUR_MAX)) {
		return true;
	}
	return false;
}


/**
 * Determine if an #ItemID_t refers to a book.
 * @param[in] id: #ItemID_t in question.
 * @return `true` if it is a book, false if otherwise.
 */
bool
item_isarmour(ItemID_t id) {
	if ((id >= ITEM_ID_BOOK_MIN) && (id <= ITEM_ID_BOOK_MAX)) {
		return true;
	}
	return false;
}


/**
 * Determine the #ItemType_t that corresponds to an #ItemID_t.
 * @param[in] id: #ItemID_t in question.
 * @return corresponding #ItemType_t or #Q_ERRORCODE_ENUM on error.
 */
ItemType_t
item_gettype(ItemID_t id) {
	if ((id >= ITEM_ID_WEAPON_MIN) && (id <= ITEM_ID_WEAPON_MAX)) {
		return ITEM_TYPE_WEAPON;
	} else if ((id >= ITEM_ID_ARMOUR_MIN) && (id <= ITEM_ID_ARMOUR_MAX)) {
		return ITEM_TYPE_ARMOUR;
	} else if ((id >= ITEM_ID_BOOK_MIN) && (id <= ITEM_ID_BOOK_MAX)) {
		return ITEM_TYPE_BOOK;
	} else {
		return (ItemType_t) Q_ERRORCODE_ENUM;
	}
}
