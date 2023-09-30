/*
 * Game-wide definitions. Depends on stdint.h.
 */

/* Return codes */
#define Q_SUCCESS  0
#define Q_FAILURE -1

/* Type for holding arbitrary amounts of data to pass between modules */
typedef void QData;

/* Type for holding QData and its size */
typedef struct QDatameta_t{
	QData *data;
	size_t size; 
} QData_t;
