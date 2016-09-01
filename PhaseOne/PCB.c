///////////////////////* PCB.c *///////////////////////

#include "../h/const.h"
#include "../h/types.h"
#include "../e/PCB.e"

void debugA(int a) {
	int i;
	i=0;
}

/* Module global variables */ 
//HIDDEN pcb_t *p, pcbFree_h; // pointer to head of the free singlely-linked list
//HIDDEN pcb_PTR
HIDDEN pcb_t *freePcb_tp;


//Allocation & Deallocation of ProBlk's
void freePcb(pcb_t *p){
	// uses InsertProQ
	

}

 /**************************************************************************** 
 *
 * pcb_t *allocPcb(){
 * 
 * Uses removeProQ
 * 
 * }
 * 
 ****************************************************************************/
 
void initPcbs() {

//Uses MkEmptyProcQ OR pcbFree_h = NULL;
//This is in video 5 

	//int count = 20;

	static pcb_t procTable[20];
	
	for(int i=0; i< MAXPROC; i++)
	{
		freePcb(&(procTable[i]));
	}
	


 
}
 
 
 
 
 
 
 
 //Process Queue Maintenance
 
pcb_t *mkEmptyProcQ (){
	
	return(NULL);
}


/****************************************************************************/
int emptyProcQ (pcb_t *tp){
	return (tp == NULL); 
}
 /****************************************************************************/


/****************************************************************************/
 
 void insertProQ (pcb_t **tp, pcb_t *p) {

	// if the procq is empty
	if(emptyProcQ(*tp)){
		(*tp) = p;
		p->p_next = p; //This can't be null because it will throw and error for the next conditional
		p->p_previous = p;
	}
	// if procq has at least one PCB
	else{
		p->p_next = (*tp)->p_next; 
		(*tp)->p_next = p;
		(p->p_next)->p_previous = p;
		p->p_previous = (*tp);
		(*tp) = p;
	}


}
  
 /****************************************************************************/

 
 /**************************************************************************** 
 *
 * pcb_t *removeProcQ(pcb_t **tp {
 * 
 * Uses emptyProcQ
 * This has 3 cases and is an dequeue
 * 
 * }
 * 
 ****************************************************************************/
 
 
  /**************************************************************************** 
 *
 * pcb_t *outProcQ(pcb_t **tp, pcb_t *p){
 * 
 * Uses EmptyProcQ and RemoveProcQ
 * 
 * }
 * 
 ****************************************************************************/
 
pcb_t *headProcQ(pcb_t *tp){
 
	if(!emptyProcQ(tp))
	{
		return (tp->p_next);
	}
	
	return  NULL;
 
}




//Process Tree Maintenance
 /**************************************************************************** 
 *
 * int emptyChild(pcb_t *p){
 * 
 * Returns T/F
 * 
 * }
 * 
 ****************************************************************************/
 
  /**************************************************************************** 
 *
 * void insertChild(pcb_t *prnt, pcb_t *p){
 * 
 * Purpose: Parent now has another child
 * Treated as a stack (use p_sib for this)
 * 
 * }
 * 
 ****************************************************************************/
 
  /**************************************************************************** 
 *
 * pcb_t *removeChild(pcb_t *p){
 * 
 * 
 * 
 * }
 * 
 ****************************************************************************/
 
  /**************************************************************************** 
 *
 * pcb_t *outChild(pcb_t *p){
 * 
 * 
 * 
 * }
 * 
 ****************************************************************************/
