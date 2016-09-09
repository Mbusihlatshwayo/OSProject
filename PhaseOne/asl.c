#include "../h/const.h"
#include "../h/types.h"
#include "../e/PCB.e"

HIDDEN semd_t *semd_h;
HIDDEN semd_t *semdFree_h;

int freeProcCount = 20;

void initASL(){

	HIDDEN semd_t semdTable[MAXPROC +1]; //plus one is for dummy node at head
	
	HIDDEN semd_t dummyNode;
	
	for (int i = 0; i < MAXPROC; i++) {
		semdTable[i].s_next = &semdTable[i+1]; //link them together
	}

	semdTable[(MAXPROC)].s_next = NULL; // the last element has nothing after it so set it to NULL
	semdFree_h = &(semdTable[0]); //set the head of the free list
	
	//Make sure dummy node is empty and the head is set to the dummy node.
	dummyNode.s_next = NULL;
	dummyNode.s_semAdd = 0;
	dummyNode.s_procQ = mkEmptyProcQ();
	semd_h = &dummyNode;
	
}

/*Helper Function: Looks through list for semAdd if not found allocNewASL*/
semd_t *getActiveSem(int *semAdd){
	int i = 0;	
	semd_t *loopNode;
	
	loopNode = semd_h;
	
	//if the list is empty
	if(semd_h->s_next == NULL)
	{
		return NULL;
	}
	
	while(loopNode->s_next != NULL)
	{
		//if next node address is the same sem address, return it
		if((loopNode->s_next)->s_semAdd == semAdd) 
		{
			return loopNode->s_next;
		}
		
		loopNode = (loopNode->s_next);
	}
	
	return NULL; //semadd doesn't exist

}

/* Return a pointer to the ProcBlk that is at the head of the process queue associated with the semaphore semAdd. Return NULL
if semAdd is not found on the ASL or if the process queue associated with semAdd is empty. */
pcb_t *headBlocked(int *semAdd){
	pcb_t *headBlock;
	semd_t *returnedNode;
	
	returnedNode = getActiveSem(semAdd);
	
	if(returnedNode == NULL)
	{
		return NULL;
	}
	
	headBlock = returnedNode->s_procQ;
	return headBlock;

}


//Helper function: return a free semi4 from the free list if avaliable. Return null otherwise
semd_t *getFreeSemd(){
	
	semd_t *freeSemd;
	
	//check if free semi4 list is empty
	if(semdFree_h->s_next == NULL)
	{
		return NULL;
	}
	
	
	freeSemd = semdFree_h->s_next;
	
	(semdFree_h->s_next) = (freeSemd)->s_next; //the header of the free list now points old head's next semi4
	
	return freeSemd;
	
	
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
	semd_t *newNode;
	
	newNode = getActiveSem(semAdd);
	
	//if there isn't semd at the address, make one at the address and add p.
	if(newNode == NULL)
	{
		newNode = getFreeSemd();
		
		//If there isn't any free semi's but we need one.
		if(newNode == NULL)
		{
			return TRUE;
		}
		
		semd_h->s_next = newNode; //Question: Do we need helper function?
		newNode->s_semAdd = semAdd;
		newNode->s_procQ = mkEmptyProcQ();
		
	}
	
	insertProQ(&(newNode->s_procQ)y, p); //insert at the new address p within the procQ
	p->p_semAdd = semAdd;


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
