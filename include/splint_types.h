/**
 * @file splint_types.h
 * Defines types primarily for splint to make sense out of.
 */

/** Type to stand in for `/\*@only@*\/ int *`. */
typedef /*@only@*/int *OnlyIntp_t;

/** Type to stand in for `/\*@partial@*\/ int *`. */
typedef /*@partial@*/int *PartialIntp_t;
/** Type to stand in for `/\*@partial@*\/ OnlyIntp_t *`. */
typedef /*@partial@*/PartialIntp_t *PartialPartialIntpp_t;

/** Type to stand in for `/\*@only*\/ char *`. */
typedef /*@only@*/char *OnlyCharp_t;
/** Type to stand in for `/\*@observer*\/ const char *`. */
typedef /*@observer@*/const char *ObserverConstCharp_t;
