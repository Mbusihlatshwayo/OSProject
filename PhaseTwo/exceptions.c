/*****************************  Exceptions.c  *************************************** 
 * Description here
 * 
 * 
 * 
 ****************************************************************************/

#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"
#include "../e/asl.e"

int tlbHandler(){
	
}

int syscallHandler(){
	
	/* context switch!! wake up here */
	state_t *oldState = (state_t *) OLDSYSCALL;
	state_t *oldProgram = (state_t *) OLDTRAP;
	
	int kernelMode;
	kernelMode = (oldState->s_status & KUp) >> 0x3;
	
	if (oldState->s_a0 >= 9) {
		
		/* pass up or die! */
		
	} 
	/* check if we are in kernel mode and the syscall is from 1-8 */
	else if (oldState->s_a0 < 9 && kernelMode != 0) {
		switch (oldState->s_a0)
			case: 1
			
			break;
			
			case: 2
			
			break;
			
			case: 3
			
			break;
			
			case: 4
			
			break;
			
			case: 5
			
			break;
			
			case: 6
			
			break;
			
			case: 7
			
			break;
			
			case: 8
			
			break;
			
			default:
			
			break;
		
	}
	/* if the syscall was 1-8 but we are also in user mode*/ 
	else {
		
		/* privilaged instruction */
		moveState(oldState, oldProgram);
		
		/* set the cause register to be a privileged instruction*/
		oldProgram->s_cause = 10;
		
		programTrapHandler();
		
	}
	
}

int programTrapHandler(){
	
}
