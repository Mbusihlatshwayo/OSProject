///////////////////////* PCB.c *///////////////////////

#include "../h/const.h"
#include "../h/types.h"
#include "../e/PCB.e"

void debugA(int a) {
	int i;
	i=0;
}

/* Module global variables */ 
HIDDEN pcb_t *p, pcbFree_h; // pointer to head of the free singlely-linked list




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


/**************************************************************************** 
 *
 * int emptyProQ(pcb_t *tp){
 * 
 * 
 * 
 * }
 * 
 ****************************************************************************/


/**************************************************************************** 
 *
 *  insertProQ (pcb_t **tp, pcb_t *p) {
 * 
 * Uses emptyProcQ
 * This has 3 cases and is an enqueue
 * 
 * }
 * 
 ****************************************************************************/

 
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
