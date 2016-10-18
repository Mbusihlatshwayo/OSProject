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

/****************Module global variables****************/

cpu_t endTOD;
cpu_t startTOD;

state_t *oldSys = (state_t *) OLDSYSCALL;
state_t *oldProgram = (state_t *) OLDTRAP;
state_t *oldTLB = (state_t *) OLDTLB;


/*****************TLB Exception Handling*****************/

/* Occurs: When MPS2 fails to translate a virtual address into its corresponding physical address.
 * 
 * Executes one of two actions based on if current process has performed a SYS5: 
 * 1) If process has not performed SYS5 - currentProcess and its progeny are "killed" (SYS2)
 * 2) If the process has performed SYS5 - "pass up" the processor state from TLB OLD AREA to the one
 * 		recorded in currentProcess and the processor new TLB state recorded in currentProcess becomes 
 * 		the current processor state.*/
int tlbHandler(){
		
	if(currentProcess->p_newTLB == NULL) { /*Question: Is this the right check?*/
		
		terminateProcess(); /*Question: Is it okay to call Sys2 here?*/
		
		currentProcess = NULL;
		
		scheduler();
		
	} else {
		
		moveState(oldTLB, currentProcess->p_oldTLB);
		
		moveState(currentProcess->p_newTLB, &(currentProcess->p_s)); /*Question: Will this do the moving of new TLB state to current proc state?*/
		
		LDST(&currentProcess->p_s);
		
	}
	
}

/*****************Pgm Trap Exception Handling******************/

/* Occurs: When the executing process attempts to perform some illegal or undefined action
 * 
 * Executes one of two actions based on if current process has performed a SYS5: 
 * 1) If process has not performed SYS5 - currentProcess and its progeny are "killed" (SYS2)
 * 2) If the process has performed SYS5 - "pass up" the processor state from PGM OLD AREA to the one
 * 		recorded in currentProcess and the processor new PGM state recorded in currentProcess becomes 
 * 		the current processor state.*/
int programTrapHandler(){
	
	if(currentProcess->p_newPGM == NULL) { /*Question: Is this the right check?*/
		
		terminateProcess(); /*Question: Is it okay to call Sys2 here?*/
		
		currentProcess = NULL;
		
		scheduler();
		
	} else {
		
		moveState(oldProgram, currentProcess->p_oldPGM);
		
		moveState(currentProcess->p_newPGM, &(currentProcess->p_s)); /*Question: Will this do the moving of new PGM state to current proc state?*/
		
		LDST(&currentProcess->p_s);
		
	}
	
	
}

/*****************Interrupt Exception Handling******************/

/* Occurs: When either a previously initiated I/O request completes or when either a processor Local
 * 		Timer/Interval Timer makes a 0x0000.0000 -> 0xFFFF.FFFF transition
 * 
 * Execute actions:
 * 1) Acknowledge the outstanding interrupt by writing acknowledge command code or a new command in the 
 * 		interrupting device's  device register.
 * 2) Perform V operation on the sema4 associated with the interrupting (sub)device and pseudo-clock sema4.
 * 3) If SYS8was requested prior to the handling of this interrupt, store the interrupting (sub)device's 
 * 		status word in the newly unblocked process'es v0. Alternatively if SYS8 has not been called, 
 * 		store off the word until SYS8 is eventually called.*/
int exceptionHandler(){
	/* In notes for 10/17 and vids */
	
}

/*****************Syscall Exception Handling*****************/

/* Occurs: When a Syscall or Breakpoint assembler instuction is executed.
 * 
 * Executes some instruction based on the value of 1-8 found in a[0] */
int syscallHandler(){
	
	/*context switch!*/	
	moveState(oldSys, &(currentProcess->p_s));
	currentProcess->p_s.s_pc = (currentProcess->p_s.s_pc)+4;
	
	/*set kernelMode*/
	int kernelMode = (oldSys->s_status & KUp); /*Question: this is how we see is KUp is on correct?*/
	
	if (oldSys->s_a0 >= 9) {
		
		/*pass up or die: This part is in p.23 in blue book (or can we just call SYS2?)*/
		
	} 
	/* check if we are in kernel mode and the syscall is from 1-8 */
	else if (oldSys->s_a0 < 9 && kernelMode != 0) {
		switch (oldSys->s_a0){
			case CREATEPROCESS: 		
				createProcess((state_t *) oldSys->s_a1);  /*Question: Conflicting types warnings on these function calls??*/
			
			break;
			
			case TERMINATEPROCESS:
				terminateProcess();
				scheduler();
			
			break;
			
			case VERHOGEN:
				verhogen((int *) oldSys->s_a1);
			
			break;
			
			case PASSEREN:
				passeren((int *) oldSys->s_a1);
			
			break;
			
			case SPECTRAPVEC:
				specTrapVec((int) oldSys->s_a1, (state_t *) oldSys->s_a2, (state_t *) oldSys->s_a3);
			
			break;
			
			case GETCPUTIME:
				getCPUTime();
			
			break;
			
			case WAITCLOCK:
				waitForClock();
			
			break;
			
			case WAITIO:
				waitForIO((int) oldSys->s_a1, (int) oldSys->s_a2, (int) oldSys->s_a3);
			
			break;
			
		}
	}
	/* if the syscall was 1-8 but we are also in user mode*/ 
	else {
		
		/* privilaged instruction */
		moveState(oldSys, oldProgram);
		
		/* set the cause register to be a privileged instruction*/
		oldProgram->s_cause = 10;
		
		programTrapHandler();
		
	}
	
}

/*Syscall 1 causes a new process (progeny) of the caller to be created. If the new process cannot be created
 *(no free PCBs for example), the error code -1 is placed within the caller's v0. Otherwise, 0 is placed in v0 */
void createProcess(state_t *statep) { 		
	
	/* create and allocate a new pcb */
	pcb_t *newProcess;
	newProcess = allocPcb();
	
	/*check if process cannot be created*/
	if (newProcess == NULL) {
		/* set the v0 register to -1 we had an error*/
		currentProcess->p_s.s_v0 = -1;	
	}
	else{
	
		/* we were successful in creating a new proc so move to the new state */
		moveState(statep, &(newProcess->p_s));
		
		processCount = processCount+1;
		insertChild(currentProcess, newProcess);
		insertProcQ(&readyQueue, newProcess);
		
		/* set the v0 register to 0 it was successful*/
		currentProcess->p_s.s_v0 = 0;	
	}
		
	/* calls load state with the current process state */ 
	LDST(&(currentProcess->p_s));

}

/*Syscall 2 causes the executing process to cease to exist (POOF). In addition, recursively, all progeny of this
 * process are terminated as well.*/
void terminateProcess()
{
	/*check if current process has been annihilated*/
	if (!currentProcess) {
		return;
	}
	
	outChild(currentProcess);

	currentProcess = NULL;
}

/*Syscall 3 (or "V Operation) causes the sema4 from the address given to be signaled and incremented*/
void verhogen(int *semaddr) {
	
	/*This is in 10/5 notes*/

	if (currentProcess->p_s.s_a1 <= 0) { /*Question: in notes, this was OldSys but this actually has to be a process to use p_s right?*/
		
		pcb_t *p;
		p = removeBlocked(semaddr); 	/*Question: is "p" currentProcess or a new PCB?*/
		insertProcQ(&readyQueue, p);
	}
	/* Question: should be inside the if statement?*/
	oldSys->s_pc = oldSys->s_pc + 4;
	LDST(oldSys);
	
}

/*Syscall 4 (or "P Operation) causes the sema4 from the address given to be told to wait and decremented*/
void passeren(int *semaddr) {
	
	if(currentProcess->p_s.s_a1 < 0) { /*Question: same as line 228*/

		moveState(oldSys, &(currentProcess->p_s)); 			/*Question: same as line 231*/
		insertBlocked(&(currentProcess->p_s.s_a1), currentProcess); /*Question: same as line 228*/
		scheduler();
		
	} else {
	
		oldSys->s_pc = oldSys->s_pc + 4;
		LDST(oldSys);
	
	}
	
}

/* Syscall 5 causes the nucleus to store the processor state at the time of teh exception in that area pointed
 * to by the address in a2, and loads the new processor state from the area pointed to by the address given in a3.
 * This syscall can only be called at most once for each of the three excpetion types(TLB, PGM, and Syscall). 
 * Any request that that calls this more than that shall be executed (Sys2 style).*/
void specTrapVec(int type, state_t *oldP, state_t *newP) {
	
	if(currentProcess->p_newTLB != NULL || currentProcess->p_newPGM != NULL || currentProcess->p_newSys) {
		
		terminateProcess();
		scheduler();
		
	}
	else {
		/*6 fields as ptrs to states (2 for each flag)*/
		
		/*Question: How can we get the flags from the params if they are state_t's? Only PCB'a have p_s.
		currentProcess->p_oldTLB = oldP->p_s.s_a1;
		currentProcess->p_newTLB = newP->p_s.s_a1;
		
		currentProcess->p_oldPGM = oldP->p_s.s_a2;
		currentProcess->p_newPGM = newP>p_s.s_a2;
		
		currentProcess->p_oldSys = oldP->p_s.s_a3;
		currentProcess->p_newSys = newP->p_s.s_a3;*/
		
		LDST(&(currentProcess->p_s));
	}
			
}

/*Syscall 6 causes the procesor time (in microseconds) used by the requesting process to be placed/returned to the caller's v0. 
 * This means that the nucleaus must record in the PCB the amount of processor time used by each process*/
void getCPUTime() {
	
	/* place the processor time in microseconds in the v0 reg */
	currentProcess->p_s.s_v0 = currentProcess->p_CPUTime;

	/* load the state of the process to continue */ 
	LDST(&(currentProcess->p_s));
}

/*Syscall 7 performs a P operation on the pseudo-clock timer sema4. This sema4 is V'd every 100 milliseconds automatically by the nucleus*/
void waitForClock(){
	
	clockTimer = clockTimer - 1;

	if(clockTimer< 0){

		insertBlocked(&(clockTimer),currentProcess);
		
		STCK(endTOD);

		currentProcess->p_CPUTime = currentProcess->p_CPUTime + (endTOD - startTOD);

		softBlockCount = softBlockCount + 1;

		currentProcess = NULL;

		scheduler();

	}

	LDST(&(currentProcess->p_s));

}

/*Syscall 8 performs a P operation on the I/O device sema4 indicated by the values in a1, a2, and optionally a3*/
void waitForIO(int intlNo, int dnum, int waitForTermRead)
{
	/* This is in notes for 10/14/16... I think?*/
	
}
