/**
 * @file splint_types.h
 * Defines types primarily for splint to make sense out of.
 */

/** Type to stand in for `/\*@only@*\/ int *`. */
typedef /*@only@*/int *OnlyIntp_t;
/** Type to stand in for `/\*@only@*\/ OnlyIntp_t *`. */
typedef /*@only@*/OnlyIntp_t *OnlyOnlyIntpp_t;

/** Type to stand in for `/\*@partial@*\/ int *`. */
typedef /*@partial@*/int *PartialIntp_t;
/** Type to stand in for `/\*@partial@*\/ OnlyIntp_t *`. */
typedef /*@partial@*/PartialIntp_t *PartialPartialIntpp_t;
