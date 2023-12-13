/**
 * @file item.c
 * Program file for the item module.
 * Responsible for handling items and their attributes.
 */



/**********
 * WEAPONS
 **********/
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
	{ITEM_ID_WEAPON_SHORTSWORD, 
		"short sword", ITEM_DESCRIPTION_WEAPON_SHORTSWORD,
		3, 1, 0.7, 0.2},
/*  ^  ^  ^    ^
 *  |  |  |    '-  armour_penetration
 *  |  |  '------  hit_chance
 *  |  '---------  max_range
 *  '------------  damage
 */

	{ITEM_ID_WEAPON_BASTARDSWORD,
		"bastard sword", ITEM_DESCRIPTION_WEAPON_BASTARDSWORD,
		4, 1, 0.6, 0.3},

	{ITEM_ID_WEAPON_ZWEIHANDER,
		"zweihander", ITEM_DESCRIPTION_WEAPON_ZWEIHANDER,
		5, 2, 0.5, 0.4}
}



/*********
 * ARMOUR
 *********/

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
	{ITEM_ID_ARMOUR_CLOAK,
		"cloak", ITEM_DESCRIPTION_ARMOUR_CLOAK,
		0, 0.0, 0.2},
	{ITEM_ID_ARMOUR_HAUBERK,
		"hauberk", ITEM_DESCRIPTION_ARMOUR_HAUBERK,
		1, 0.05, 0.02},
	{ITEM_ID_ARMOUR_KNIGHT,
		"knight's armour", ITEM_DESCRIPTION_ARMOUR_KNIGHT,
		3, 0.3, 0.05}
}



/********
 * BOOKS
 ********/

#define ITEM_DESCRIPTION_BOOK_ILOVEYOU \
	"A book whose cover reads: \"I LOVE YOU\", written in immaculate gothic " \
	"font. The cover is deep blood-red. Is it bleeding?"

#define ITEM_DESCRIPTION_BOOK_A \
	"A capital letter 'A' adorns the front cover. The spine is marred and " \
	"curls awkwardly. It's still warm..."

#define ITEM_DESCRIPTION_BOOK_BABEL \
	""
