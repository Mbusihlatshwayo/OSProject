#ifndef PCB
#define PCB


/**************************************************************************** 
 *
 * PCB.E
 * Header file for PCB.c
 * Note: Found in Video 4
 *
 ****************************************************************************/

#include "../h/types.h"


extern void freePcb (pcb_t *p);
extern pcb_t *allocPcb();
extern void initPcbs();

extern pcb_t *mkEmptyProcQ();
extern int emptyProcQ (pcb_t *p);
extern void insertProcQ (pcb_t **tp, pcb_t *p);
extern pcb_t *removeProQ (pcb_t **tp);
extern pcb_t *outProcQ (pcb_t **tp, pcb_t *p);
extern pcb_t *headProcQ (pcb_t *tp);

extern int emptyChild (pcb_t *p);
extern void insertChild (pcb_t *prnt, pcb_t *p);
extern pcb_t *removeChild (pcb_t *p);
extern pcb_t *outChild (pcb_t *p);






#endif