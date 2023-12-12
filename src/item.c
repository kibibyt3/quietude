/**
 * @file item.c
 * Program file for the item module.
 * Responsible for handling items and their attributes.
 */


/* i'm using a xeiphos as reference */
#define ITEM_DESCRIPTION_WEAPON_SHORTSWORD \
	"A short blade; you take care not to point it at yourself; you can detect "
	"in its chipped edges a mild distaste for you and precious few qualms with "
	"the act of murder."

#define ITEM_DESCRIPTION_WEAPON_BASTARDSWORD \
	"A long blade; its grip is corroded with the sweat of the countless who "
	"wielded it before you. Tap the blade against something firm to yield its "
	"high-pitched, unpracticed warble."

#define ITEM_DESCRIPTION_WEAPON_ZWEIHANDER \
	"A great zweihander; its length is so great as to brush the bellies of the "
	"swollen clouds above. It wields you. You are its instrument; you are its "
	"extension. It is your fear congealed into weaponform."

#define WEAPONSC 3
static const ItemReferenceWeapon_t weapons[WEAPONSC] = {
	{ITEM_ID_WEAPON_SHORTSWORD, 
		"short sword", ITEM_DESCRIPTION_WEAPON_SHORTSWORD,
		3, 1, .7, .2},
/*  ^  ^   ^   ^
 *  |  |   |   '-  armour_penetration
 *  |  |   '-----  hit_chance
 *  |  '---------  max_range
 *  '------------  damage
 */

	{ITEM_ID_WEAPON_BASTARDSWORD,
		"bastard sword", ITEM_DESCRIPTION_WEAPON_BASTARDSWORD,
		4, 1, .6, .3},

	{ITEM_ID_WEAPON_ZWEIHANDER,
		"zweihander", ITEM_DESCRIPTION_WEAPON_ZWEIHANDER,
		5, 2, .5, .4}
}



#define ARMOURC 3
/* static const ItemReferenceArmour_t armour[armourc] */
