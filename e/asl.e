#ifndef ASL
#define ASL

/************************** ASL.E ******************************
*
*  The externals declaration file for the Active Semaphore List
*    Module.
*
*  Written by Mikeyg
*/

#include "../h/types.h"

extern int insertBlocked (int *semAdd, pcb_PTR p);
extern pcb_PTR removeBlocked (int *semAdd);
extern pcb_PTR outBlocked (pcb_PTR p);
extern pcb_PTR headBlocked (int *semAdd);
extern void initASL ();
semd_t *getActiveSem(int *semAdd);
semd_t *getFreeSemd();
semd_t *addSema(semd_t *sema, int *semAdd);
semd_t *removeSema(semd_t *sema);
void addToFreeList(semd_t *semToAdd);

/***************************************************************/

#endif
