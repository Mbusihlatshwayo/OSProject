/*****************************  Initial.c  *************************************** 
 * Description here
 * 
 * 
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
int processCount;
int softBlockCount;
pcb_t * currentProcess;
pcb_t *readyQueue;
int deviceList[DEVICELISTNUM][DEVICENUM];
int clockTimer;



int main()
{

	 initPcbs();
	 initASL();
	 
	 processCount = 0;
	 softBlockCount =0;
	 currentProcess = NULL;
	 readyQueue = mkEmptyProcQ();
	 
	 
	 /* declare and initialize device area*/
	 devregarea_t*deviceArea;
	 deviceArea = (devregarea_t*) ADDRESS1;
	 move
	 /*init RAMPTOP*/
	 unsigned int RAMTOP;
	 RAMTOP = (deviceArea->rambase) + (deviceArea->ramsize);
	 
	  
	 /*populate 4 new areas in low memory (Syscall/break new, program trap new, TLB mang. new, interrupt new)
	 * set the stack pointer(last page of physcial mem). This is the same for all 4
	 *  set the status (all 4 is the same): VM off, interrupts masked, supervisor mode ON
	 * set the PC to address of correct function. This is different for all 4.
	 */
	  
	  /* declare and initialize SYSCALL new area*/
	  state_t *newState;
	  newState = (state_t *) NEWSYSCALL;
	  newState->s_pc = (memaddr) syscallHandler();
	  newState->s_t9 = (memaddr) syscallHandler();
	  newState->s_sp = RAMTOP;
	  newState->s_status = ALLOFF;
	  
	  /* declare and initialize Program Trap new area */
	  newState = (state_t *) NEWTRAP;
	  newState->s_pc = (memaddr) programTrapHandler();
	  newState->s_t9 = (memaddr) programTrapHandler();
	  newState->s_sp = RAMTOP;
	  newState->s_status = ALLOFF;
	  
	  /* declare and initialize Memory Management new area */
	  newState = (state_t *) NEWTLB;
	  newState->s_pc = (memaddr) tlbHandler();
	  newState->s_t9 = (memaddr) tlbHandler();
	  newState->s_sp = RAMTOP;
	  newState->s_status = ALLOFF;
	  
	  /* declare and initialize Interrupt new area */
	  newState = (state_t *) NEWINTERRUPT;
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
	  
	  clockTimer = 0;
	  
	  
	  /*Init first process for Ready Queue. 
	   * 	Status: Interrupts on, VM off, Local Timer on, kernel-mode on
	   * 	SP: RAMTOP-FRAMESIZE
	   * 	PC: test() (from p2test)
	   */
	   
	  pcb_t *p = allocPcb();
	  
	  processCount++;
	  (p->p_s).s_pc = test(); /*need to move p2test from Mikey's dir*/
	  (p->p_s).s_t9 = test();
	  (p->p_s).s_sp = RAMTOP - PAGESIZE; 
	  (p->p_s).s_status = ALLOFF | TE | IEc | KUc; /* Interrupts enabled, vm off, local timer enabled, kernal-mode on*/
	  
	  

	  insertProcQ(&readyQueue, p);
	  scheduler();
}

/*copy before context switch to after*/
void moveState(state_t *previous, state_t *current ) {
	
	previous->s_asid = current->s_asid;
	previous->s_cause = current->s_cause;
	previous->s_status = current->s_status;
	previous->s_pc = current->s_pc;
	for (int i = 0; i <= STATEREGNUM; i++) {
		previous->s_reg[i] = current->s_reg[i];
	}
	
}
