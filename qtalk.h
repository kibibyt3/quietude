/**
 * @file qtalk.h
 * Header file for qtalk logic and i/o modules.
 */

/** Initialize qtalk     */
extern int               qtalk_init(Qdatameta_t *);

/** Safely exit qtalk    */
extern int               qtalk_end(void);

/** Pass a tick in qtalk */
extern ModeSwitchData_t *qtalk_tick(void);
