/*****************************  Initial.c  *************************************** 
 * This module implements main() which initializes the nuclues global variables,
 * The device areas, and the sema4 list.
 * 
 * Helper functions: 
 * 		moveState: copies process state before context switch
 * 
 ****************************************************************************/

#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"
#include "../e/asl.e"
#include "../e/exceptions.e"
#include "../e/interrupts.e"
#include "../e/scheduler.e"
#include "/usr/local/include/umps2/umps/libumps.e"

/****************Module global variables****************/
int processCount; 										/*The count of the number of processes in the system*/
int softBlockCount;										/*The number of processes in the system currently blocked and waiting for an interrupt*/
pcb_t * currentProcess;									/*A ptr to a PCB that reps the current executing process*/
pcb_t *readyQueue;										/*A tail ptr to a queue pf PCBs repping proccesses that are ready & waiting for execution turn*/
int deviceList[DEVICELISTNUM][DEVICENUM];				/*List of nucleus maintained sema4s*/
unsigned int deviceStatusList[DEVICELISTNUM][DEVICENUM]; /*List of statuses of the nucleus maintained sema4s*/
int clockTimer;											/*The clock sema4*/
cpu_t startTOD;											/*The TOD clock start*/



int main()
{

	 initPcbs();
	 initASL();
	 
	 processCount = 0;
	 softBlockCount =0;
	 currentProcess = NULL;
	 readyQueue = mkEmptyProcQ();
	 
	 
	 /* declare and initialize device area*/
	 devregarea_t *deviceArea;
	 deviceArea = (devregarea_t*) ADDRESS1;
	
	 /*init RAMPTOP*/
	 unsigned int RAMTOP;
	 RAMTOP = (deviceArea->rambase) + (deviceArea->ramsize);
	 
	  
	 /*populate 4 new areas in low memory (Syscall/break new, program trap new, TLB mang. new, interrupt new)
	 * set the stack pointer(last page of physcial mem). This is the same for all 4
	 *  set the status (all 4 is the same): VM off, interrupts masked, supervisor mode ON
	 * set the PC to address of correct function. This is different for all 4.
	 */
	  state_t *newState;
	  
	  /* declare and initialize SYSCALL new area*/
	  newState = (state_t *) NEWSYSCALL;
	  /*Question: Do we need to call STST(newState) here?*/
	  newState->s_pc = (memaddr) syscallHandler();
	  newState->s_t9 = (memaddr) syscallHandler();
	  newState->s_sp = RAMTOP;
	  newState->s_status = ALLOFF;
	  
	  /* declare and initialize Program Trap new area */
	  newState = (state_t *) NEWTRAP;
	  /*Question: Do we need a moveState here?*/
	  newState->s_pc = (memaddr) programTrapHandler();
	  newState->s_t9 = (memaddr) programTrapHandler();
	  newState->s_sp = RAMTOP;
	  newState->s_status = ALLOFF;
	  
	  /* declare and initialize Memory Management new area */
	  newState = (state_t *) NEWTLB;
	  /*Question: Do we need a moveState here?*/
	  newState->s_pc = (memaddr) tlbHandler();
	  newState->s_t9 = (memaddr) tlbHandler();
	  newState->s_sp = RAMTOP;
	  newState->s_status = ALLOFF;
	  
	  /* declare and initialize Interrupt new area */
	  newState = (state_t *) NEWINTERRUPT;
	  /*Question: Do we need a moveState here?*/
	  newState->s_pc = (memaddr) interruptHandler();
	  newState->s_t9 = (memaddr) interruptHandler();
	  newState->s_sp = RAMTOP;
	  newState->s_status = ALLOFF;
	
	/*Init all nucleaus maintained sema4s to 0. There is one sema4 for each external device plus one for clocktimer*/
	  
	  for(int i=0; i<DEVICELISTNUM; i++)
	  {
		  for(int j=0; j<DEVICENUM; j++)
		  {
			  deviceList[i][j] = 0;
		  }
	  }
	  
	  /*Init clock stuff*/
	  clockTimer = 0; 		
	  setTIMER(timeSlice);	
	  LDIT(smallPseudoSec);
	  
	  
	  /*Init first process for Ready Queue. 
	   * 	Status: Interrupts on, VM off, Local Timer on, kernel-mode on
	   * 	SP: RAMTOP-FRAMESIZE
	   * 	PC: test() (from p2test)
	   */ 
	  pcb_t *p = allocPcb();
	  
	  processCount++;
	  (p->p_s).s_pc = (memaddr) test(); /*Question: Do we need to move p2test from Mikey's dir?*/
	  (p->p_s).s_t9 = (memaddr) test();
	  (p->p_s).s_sp = RAMTOP - PAGESIZE; 
	  (p->p_s).s_status = ALLOFF | TE | IEc | KUc; /* Interrupts enabled, vm off, local timer enabled, kernal-mode on*/
	  
	  

	  insertProcQ(&readyQueue, p);
	  scheduler();
}

/*copy before context switch to after*/
void moveState(state_t *previous, state_t *current ) {
	
	current->s_asid = previous->s_asid;
	current->s_cause = previous->s_cause;
	current->s_status = previous->s_status;
	current->s_pc = previous->s_pc;
	
	/*Question: Do we need a while loop here?*/
	for (int i = 0; i <= STATEREGNUM; i++) {
		current->s_reg[i] = previous->s_reg[i];
	}
	
}
