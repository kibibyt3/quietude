/*
 * Game-wide definitions. Depends on stdint.h.
 */



/* Return codes */
#define Q_OK  0
#define Q_ERROR -1

/* Type for holding arbitrary amounts of data to pass between modules */
typedef void Qdata;

/* Type for holding QDatameta and its size */
typedef struct Qdatameta_t {
	QData *datap;
	size_t size; 
} QDatameta_t;
