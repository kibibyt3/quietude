/*
 * Game-wide definitions. Depends on stdint.h.
 */



/* Return codes */
#define Q_OK  0
#define Q_ERROR -1

/* 
 * Type for holding arbitrary amounts of data to pass between modules. Meant to
 * be used in a pointer; it should never be called in isolation prior to being
 * cast to a proper type.
 */
typedef void Qdata_t;

/* Type for holding Qdata and its size */
typedef struct Qdatameta_t {
	QData_t *datap;
	size_t   size; 
} Qdatameta_t;

Qdatameta_t *qdatameta_create(Qdata_t
