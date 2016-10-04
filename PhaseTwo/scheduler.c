/*****************************  Scheduler.c  *************************************** 
 * Description here
 * 
 * 
 * 
 ****************************************************************************/

#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"
#include "../e/asl.e"
#include "../e/initial.e"
#include "/usr/local/include/umps2/umps/libumps.e"

void scheduler(){
	
	if(currentProcess == NULL){
		/* if the process count is zero invoke the HALT ROM  service*/
		if(processCount == 0) {
			
			HALT();
			
		}
		/* deadlock is defined when the process count > 0 and the soft block count is zero*/ 
		else if (processCount > 0 && softBlockCount == 0) {
			
			PANIC();
			
		}
		/* if processCount > 0 and softblock count > 0 we WAIT */
		else if (processCount > 0 && softBlockCount > 0) {
			
			WAIT();
			
		}
	}
	
	/* if current process is not empty take from the ready queue*/
	currentProcess = removeProcQ(&readyQueue);
	
	/* add time */
	
	/* context switch*/
	LDST(&(currentProcess->p_s));
}
