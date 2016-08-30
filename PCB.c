/* PCB.c */

#include "const.h"
#include "types.h"
#include "PCB.e"

void debugA(int a) {
	int i;
	i=0;
}

/* module global variables */ 
// COME BACK HERE !!!!!!
HIDDEN pcb_t pcbFree_h; // pointer to head of the free list


// the process queue module:
// a set of functions used to implement queues of process
void freePcb(pcb_t *p){
	// function: freePcb
	// purpose: return the process block pointed to by p into the procFree list
	// the list pointed to by pcbFree_h is updated

	
}
