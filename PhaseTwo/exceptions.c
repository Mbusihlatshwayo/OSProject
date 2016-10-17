/*****************************  Exceptions.c  *************************************** 
 * Description here
 * 
 * 
 * 
 ****************************************************************************/

#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"
#include "../e/initial.e"
#include "../e/asl.e"
#include "/usr/local/include/umps2/umps/libumps.e"

int tlbHandler(){
	
}

int syscallHandler(){
	
	/* context switch!! wake up here */
	state_t *oldState;
	state_t *oldProgram;
	oldState = (state_t *) OLDSYSCALL;
	oldProgram = (state_t *) OLDTRAP;
	
	int kernelMode;
	kernelMode = (oldState->s_status & KUp) >> 0x3;
	
	if (oldState->s_a0 >= 9) {
		
		/* pass up or die! */
		
	} 
	/* check if we are in kernel mode and the syscall is from 1-8 */
	else if (oldState->s_a0 < 9 && kernelMode != 0) {
		switch (oldState->s_a0){
			case 1:
		
			break;
			
			case 2:
			
			break;
			
			case 3:
			
			break;
			
			case 4:
			
			break;
			
			case 5:
			
			break;
			
			case 6:
			
			break;
			
			case 7:
			
			break;
			
			case 8:
			
			break;
			
			default:
			
			break;
		}
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
	
	if(currentProcess->p_newPGM == NULL) {
		
		currentProcess.terminateProcess();
		
		currentProcess = NULL;
		
		scheduler();
		
	} else {
		
		moveState(oldProgram, currentProcess->p_oldPGM);
		
		moveState(currentProcess->p_newPGM, currentProcess->p_s);
		
		LDST(&currentProcess->p_s);
		
	}
	
	
}

void terminateProcess()
{

	if (!currentProcess) {
		return;
	}
	
	outChild(currentProcess);

	currentProcess = NULL;
}

void createProcess(state_t *processState) {
	
	/* create and allocate a new pcb */
	pcb_t *newProcess;
	newProcess = allocPcb();
	
	if (newProcess == NULL) {
	/* set the v0 register to -1 we had an error*/
	currentProcess->p_s.s_v0 = -1;	
	}
	
	/* we were successful in creating a new proc 
	 * use the new state 
	 * */
	moveState(processState, &(newProcess->p_s));
	
	processCount++;
	insertChild(currentProcess, newProcess);
	insertProcQ(&readyQueue, newProcess);
	
	/* set the v0 register to 0 it was successful*/
	currentProcess->p_s.s_v0 = 0;	
	/* calls load state with the current process state */ 
	LDST(&(currentProcess->p_s));

}

void getCPUTime() {
	
	/* place the processor time in microseconds in the v0 reg */
	currentProcess->p_s.s_v0 = currentProcess->p_CPUTime;

	/* load the state of the process to continue */ 
	LDST(&(currentProcess->p_s));
}

void verhogen(int *semaddr) {

	if (currentProcess->p_s.s_a1 <= 0) {
		
		p = removeBlocked(&(currentProcess->p_s.s_a1));
		insertProcQ(&readyQueue, p);
		/* check if these should be inside the if statement??*/
		oldState->s_pc = oldState->s_pc + 4;
		LDST(oldState);
	}
	
}

void passeren(int *semaddr) {
	
	if(oldState->p_s.s_a1 < 0) {
		moveState(oldState, p);
		insertBlocked(oldState->p_s.s_a1, currentProcess);
		scheduler();
		
	} else {
	
		oldState->s_pc = oldState->s_pc + 4;
		LDST(oldState);
	
	}
	
}

void exception(int type, state_t *oldP, state_t *newP) {
	
	if(currentProcess->p_newTLB != NULL && currentProcess->p_newPGM != NULL && currentProcess->p_newSys) {
		
		currentProcess.terminateProcess();
		scheduler();
		
	}
	else {
		/*6 fields as ptrs to states (2 for each flag)*/
		memaddr *oldTLB;
		memaddr *newTLB;
		currentProcess->p_oldTLB = oldP->p_s.s_a1;
		currentProcess->p_newTLB = newP->p_s.s_a1;
		
		memaddr *oldPGM;
		memaddr *newPGM;
		currentProcess->p_oldPGM = oldP->p_s.s_a2;
		currentProcess->p_newPGM = newP>p_s.s_a2;
		
		memaddr *oldSys;
		memaddr *newSys;
		currentProcess->p_oldSys = oldP->p_s.s_a3;
		currentProcess->p_newSys = newP->p_s.s_a3;
		
		LDST(&(currentProc->p_s));
	}
			
}

/*SYS 8*/
void waitForClock(){
	
	clockTimer = clockTimer - 1;

	if(clockTimer< 0){

		insertBlocked(&(clockTimer),currentProcess);
		
		STCK(endTOD);

		currentProcess->p_CPUTime = currentProcess->p_CPUTime + (endTOD - startTOD);

		softBlockCount = softBlockCountt + 1;

		currentProcess = NULL;

		scheduler();

	}

	LDST(&(currentProc->p_s));

}
