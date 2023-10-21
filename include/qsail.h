/**
 * @file qsail.h
 * Header for qsail logic and i/o modules.
 */  

/** Initialize qsail     */
extern int               qsail_init(const Qdatameta_t *);

/** Safely exit qsail    */
extern int               qsail_end(void);

/** Pass a tick in qsail */
extern ModeSwitchData_t *qsail_tick(void);

