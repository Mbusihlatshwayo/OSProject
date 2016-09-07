///////////////////////* PCB.c *///////////////////////
//Add purpose of module here. 

#include "../h/const.h"
#include "../h/types.h"
#include "../e/PCB.e"

void debugA(int a) {
	int i;
	i=0;
}

//////////////////Module global variables//////////////////

//HIDDEN pcb_t *p, pcbFree_h; // pointer to head of the free singlely-linked list
//HIDDEN pcb_PTR
HIDDEN pcb_t *pcbList_tp; //list tail pointer
//pcb_PTR pcbList_h;

 
 
//////////////////Process Queue Maintenance//////////////////
//Queue is double circularly linked list


//Initialize a variable to be a tail pointer to a process queue 
pcb_t *mkEmptyProcQ (){
	return(NULL);
}

//Return T if tp is empty, else return F
int emptyProcQ (pcb_t *tp){
	return (tp == NULL); 
}

//Insert the ProcBlk pointed to by p into the process queue.
void insertProQ (pcb_t **tp, pcb_t *p) {

	// if the procq is empty
	if(emptyProcQ(*tp)){
		// the next/previous nodes for the current node, is also the current node
		p->p_next = p; 
		p->p_previous = p;
		
		// the first node is also the tail pointer
		(*tp) = p;
	}
	// if procq has at least one PCB
	else{
		// initialize pointers for the new node to be inserted
		p->p_next = (*tp)->p_next; // the new nodes next value is set to the first element 
		p->p_next->p_previous = p; // the previous value of the first node is set to the new node
		p->p_previous = (*tp);
		// initialize the new tail pointer
		(*tp)->p_next = p;
		(*tp) = p;
	} 

}

//Remove the head element from process queue and return a pointer to that removed element.
 pcb_t *removeProcQ(pcb_t **tp) { 
	 
	 pcb_t *headPCB; //temp var for headNode
	 
	// if the procQ is empty 
	if(emptyProcQ(*tp)){
		return(NULL);
	}
	//if the ProcQ only has 1 node
	else if((*tp)->p_next == *tp){
		pcb_t *headPCB = *tp;
		*tp = mkEmptyProcQ(); //update tp to be removed
		return(headPCB);
	}
	//if the ProcQ has 2+ nodes
	else{
		
		headPCB = (*tp)->p_next; //set value of node going to be removed and returned.
		(*tp)->p_next = (headPCB->p_next); //set tp's next value as the new head of the list. Question: Can I use headPCB to see
		(headPCB->p_next)->p_previous = *tp; //set previous of new head to be the tail pointer	
		
		return(headPCB);
		
	}
 }
 
//Remove the ProcBlk pointed to by p from the process queue. Update queue based on p's location in queue (ie fix next and previous stuff)
pcb_t *outProcQ(pcb_t **tp, pcb_t *p){
	
	pcb_t *removedNode; //temp var for node being removed
	pcb_t *loopNode; //temp var for loopNode
	
	// if the procq is empty
	if(emptyProcQ(*tp)){	
		return(NULL);
	}
	
	//if procq has only 1 node
	else if ((*tp)->p_next == *tp){
		if(*tp == p){
			removedNode = *tp;
			mkEmptyProcQ(*tp);
			return removedNode;
		}
		else{
			return(NULL);
		}
	}
	
	//if procq has 2+ nodes
	else{
		
		//If p is the tail pointer
		if(*tp == p){
			
			removedNode = (*tp); //set value of node going to be removed and returned.
			
			(*tp) = (removedNode->p_previous);
			(*tp)->p_previous = (removedNode->p_previous); 
			((*tp)->p_next)->p_previous = *tp;
			
			return(removedNode);
		}
		
		//If p is the head pointer
		else if (p == (*tp)->p_next){
			return removeProcQ(*tp);
			
		}
		
		//Else p is the middle 
		else{
			
			loopNode = ((*tp)->p_next);
			
			while(loopNode != *tp)
			{
				
				if(loopNode == p)
				{
					
					(p->p_next)->p_previous = (p->p_previous);
					(p->p_previous)->p_next = (p->p_next);
					
					return p;
				}
				
				loopNode = loopNode->p_next;
			}
			
			return(NULL);			
			
		}
	}
 
}
 
 //Return first ProcBlk from process queue.
pcb_t *headProcQ(pcb_t *tp){
 
	if(!emptyProcQ(tp))
	{
		return (tp->p_next);
	}
	
	return  NULL;
 
}


//////////////////Allocation & Deallocation of ProBlk's//////////////////

//Insert the element pointed to by p onto the pcbFree list
void freePcb(pcb_t *p){
	insertProQ(&pcbList_tp, p);

}

 
//Initialize the pcbFree list to contain all the elements of the static array of MAXPROC ProbBlk's. This is called only once 
void initPcbs() {

	static pcb_t procTable[MAXPROC];

	pcbList_tp = mkEmptyProcQ();
	for(int i = 0; i < MAXPROC; i++){
		freePcb(&procTable[i]);
	}
 
}

 //Return Null if PCBFree list is empty. Otherwise, remove an element from PCBfree list, provide initial values for all fields (Null/0) and return a pointer to removed element.
pcb_t *allocPcb(){
	
	pcb_t *removedElement;
	
	removedElement = removeProcQ(&pcbList_tp);
	
	if(emptyProcQ(pcbList_tp)){
		return(NULL);
	}
	else
	{

		removedElement->p_next = NULL; 
		removedElement->p_previous = NULL;
		removedElement->p_prnt = NULL;
		removedElement->p_child = NULL;
		removedElement->p_sib = NULL;
		removedElement->p_semAdd = NULL;
		
		return removedElement;
	}

}


//////////////////Process Tree Maintenance//////////////////
//double linearly linked list

//Return T if the ProcBlk pointed to by p has no children. Return F otherwise.
 int emptyChild(pcb_t *p){
  
 if(p->p_child == NULL){
	 return 1;
 } else {
	 return 0;
 }
 
}

//Make the ProcBlk pointed to by p a child of the ProcBlk pointed to by prnt
void insertChild(pcb_t *prnt, pcb_t *p){
 
//Purpose: Parent now has another child
//Treated as a stack (use p_sib for this)
	p->p_prnt = prnt;
	if (emptyChild(prnt)) {
		p->p_sib = NULL;
	} else {
		pcb_t *temp;
		temp = prnt->p_child;
		prnt->p_child = p;
		p->p_sib = temp;
	}
}

//Make the first child of the ProBlk pointed to by p no longer a child of p. Return NULL if there were no children of p. Otherwise return removed child
pcb_t *removeChild(pcb_t *p){
 
 
 
}
 
//Make the child of the ProcBlk pointed to by p no longer a child of p. Return NULL is p has no parent. Otherwise return p.
pcb_t *outChild(pcb_t *p){
 
 
 
}
