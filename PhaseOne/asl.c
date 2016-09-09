/*****************************  ASL.c  *************************************** 
 * This module manages the queue of active semaphore descriptors by implementing:
 * 
 * The maintenance of a single sorted list of active semaphore descriptors
 * 		each of which supports a queue of ProcBlks
 * 
 ****************************************************************************/

#include "../h/const.h"
#include "../h/types.h"
#include "../e/PCB.e"

//////////////////Module global variables//////////////////

HIDDEN semd_t *semd_h; //head pointer to ASL (single, linear w/ dummy headNode)
HIDDEN semd_t *semdFree_h; //head pointer to list holding unused semaphore descriptors (singe, linear)
int freeProcCount = 20; //The max number of ASL's available 


//////////////////Helper Functions////////////////////////

/*Looks through ASL list for sema4 at semAdd. */
semd_t *getActiveSem(int *semAdd){
	int i = 0;	
	semd_t *loopSema;
	
	loopSema = semd_h;
	
	//if the list is empty, return NULL
	if(semd_h->s_next == NULL)
	{
		return NULL;
	}
	
	//else loop through list
	while(loopSema->s_next != NULL)
	{
		//if next sema's semAdd is the address we were looking for, return that sema4
		if((loopSema->s_next)->s_semAdd == semAdd) 
		{
			return loopSema->s_next;
		}
		
		loopSema = (loopSema->s_next);
	}
	
	return NULL; //sema4 at passed in semAdd doesn't exist

}

//Return a free sema4 from the free list if available. Return null otherwise
semd_t *getFreeSemd(){
	
	semd_t *freeSemd;
	
	//check if free sema4 list is empty
	if(semdFree_h->s_next == NULL)
	{
		return NULL;
	}
	
	
	freeSemd = semdFree_h->s_next;
	
	(semdFree_h->s_next) = (freeSemd)->s_next; //the header of the free list now points old head's next sema4
	
	return freeSemd;
	
	
}

//Find where passed sema4 goes based on semAdd and update ASL list. Return newly added sema4
semd_t *addSema(semd_t *sema, int *semAdd)
{
	int i = 0;	
	int found = 0; //var to keep track if new sema found place before the end of the list was reached
	semd_t *loopSema;
	
	//If the ASL list is empty, add sema
	if(semd_h->s_next == NULL)
	{
		semd_h->s_next = sema; //head points to sema
		sema->s_next = NULL;	//sema is aloooonnneeee on list :(
		
	}
	
	//else the ASL list has multiple semas so find where new sema belongs and update list ranking
	else
	{		
		//If the first node has an address greater than the new one, add the new one before it and update head pointer
		if((semd_h->s_next)->s_semAdd > semAdd)
		{
			sema->s_next = semd_h->s_next;	//sema points to larger friend that was previously the first sema
			semd_h->s_next = sema; //head points to new sema
			
		}
		
		//else do a loop and find where semAdd of new sema is less than the next
		else 
		{
	
			loopSema = semd_h;
				
				
			while(loopSema->s_next !=NULL)
			{
				if((loopSema->s_next)->s_semAdd > semAdd)
				{
					sema->s_next = loopSema->s_next;
					loopSema->s_next = sema;
					
					found = 1;
				}
				
				i++;
			}
			
			//if the loop got to the end and new sema's address is still biggest, add to the end
			if(found == 0) 
			{
				loopSema->s_next = sema;
				sema->s_next = NULL;
			}
		}
		
	}
	
	sema->s_semAdd = semAdd; //set new semas address
	sema->s_procQ = mkEmptyProcQ(); //clear sema's procQ
	
	return sema;
}


//////////////////ASL Functions////////////////////////

//Initialize the semdFree list
void initASL(){

	HIDDEN semd_t semdTable[MAXPROC +1]; //plus one is for dummy node at head	
	HIDDEN semd_t dummyNode;
	
	int i = 0;
	
	while(i<MAXPROC)
	{
		semdTable[i].s_next = &semdTable[i+1]; //link sema4's together
		i++;
	}

	semdTable[(MAXPROC)].s_next = NULL; // the last element has nothing after it so set it to NULL
	semdFree_h = &(semdTable[0]); //set the head of the free list
	
	//Make sure dummy node is empty and the head is set to the dummy node.
	dummyNode.s_next = NULL;
	dummyNode.s_semAdd = 0;
	dummyNode.s_procQ = mkEmptyProcQ();
	semd_h = &dummyNode;
	
}


/* Return a pointer to the ProcBlk that is at the head of the process queue associated with the semaphore semAdd. Return NULL
if semAdd is not found on the ASL or if the process queue associated with semAdd is empty. */
pcb_t *headBlocked(int *semAdd){
	pcb_t *headBlock;
	semd_t *returnedSema;
	
	returnedSema = getActiveSem(semAdd); //Check if sema4 at semAdd exists
	
	if(returnedSema == NULL)
	{
		return NULL;
	}
	
	headBlock = returnedSema->s_procQ;
	return headBlock;

}

/* Insert the ProcBlk pointed to by p at the tail of the process queue
 associated with the semaphore whose physical address is semAdd and 
 set the semaphore address of p to semAdd. If the semaphore is currently not 
 active (i.e. there is no descriptor for it in the ASL), allo- cate a new descriptor
  from the semdFree list, insert it in the ASL (atthe appropriate position), initialize 
  all of the fields (i.e. set s semAdd to semAdd, and s procq to mkEmptyProcQ()), and proceed as above. If a new
   semaphore descriptor needs to be allocated and the semdFree list is empty, 
   return TRUE. In all other cases return FALSE. */

int insertBlocked(int *semAdd, pcb_t *p){
	semd_t *newSema;
	
	newSema = getActiveSem(semAdd); //check if sema4 exists at semAdd
	
	//If there isn't sema4 at the semAdd, make one at the address and add p.
	if(newSema == NULL)
	{
		newSema = getFreeSemd(); //Grab a free sema from free list
		
		//If there isn't any free sema's return true
		if(newSema == NULL)
		{
			return TRUE;
		}
		
		//else, add newSema to ASL list and update list.	
		newSema = addSema(newSema, semAdd);
		
	}
	
	insertProQ(&(newSema->s_procQ), p); //insert p at the address within the procQ
	p->p_semAdd = semAdd;

	return FALSE;
}

/* Search the ASL for a descriptor of this semaphore. If none is
found, return NULL; otherwise, remove the ﬁrst (i.e. head) ProcBlk
from the process queue of the found semaphore descriptor and return a pointer to it. If the process queue for this semaphore becomes
empty (emptyProcQ(s procq) is TRUE), remove the semaphore
descriptor from the ASL and return it to the semdFree list. */

pcb_t *removeBlocked(int *semAdd){
	
	semd_t *semAddress;
	semAddress = getActiveSem(semAdd);
	
	if (semAddress == NULL) {
		return NULL;
	} else {
		pcb_t *process;
		process = removeProcQ(&(semAddress->s_procQ));
		
		if(emptyProcQ(semAddress->s_procQ)) {
			// removeActive
			// add the free list
		}
		
		return process;
	}
}

void addToFreeList(semd_t *semToAdd) {
	freeProcCount = freeProcCount + 1;
	semToAdd->s_next = semdFree_h->s_next;
	semdFree_h->s_next = semToAdd;
}

/* Remove the ProcBlk pointed to by p from the process queue associated with p’s semaphore (p→ p semAdd) on the ASL. If ProcBlk
pointed to by p does not appear in the process queue associated with
p’s semaphore, which is an error condition, return NULL; otherwise,
return p. */

pcb_t *outBlocked(pcb_t *p){
	semd_t *semAddress;
	semAddress = getActiveSem(p->p_semAdd);
	
	if (semAddress == NULL) {
		return NULL;
	} else {
		pcb_t *process;
		process = outProcQ(&(semAddress->s_procQ), p);
		
		if (emptyProcQ(semAddress->s_procQ)) {
			semAddress = removeActive(p->p_semAdd);
			addToFreeList(semAddress);
		}
		
		return process;
	}
		
}
