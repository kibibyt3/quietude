/**
 * @file qcli.h
 * Logic and i/o module interface for qcli.
 */



/** Initialize qcli     */
extern int               qcli_init(const Qdatameta_t *);

/** Safely exit qcli    */
extern int               qcli_end(void);

/** Pass a tick in qcli */
extern ModeSwitchData_t *qcli_tick(void);
