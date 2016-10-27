/*****************************  Exceptions.c  *************************************** 
 * This module handles Syscall/Bp Exceptions (1-8), Pgm Trap Exceptions, and
 * TLB Exceptions. 
 * 
 ****************************************************************************/

#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"
#include "../e/initial.e"
#include "../e/asl.e"
#include "../e/scheduler.e"
#include "../e/exceptions.e"
#include "../e/interrupts.e"
#include "/usr/local/include/umps2/umps/libumps.e"

/****************Module global variables****************/

cpu_t endTOD;

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
		
	/*If the current process does not have a value for newTLB, kill it*/	
	if(currentProcess->p_newTLB == NULL) { 
		
		terminateProcess(); 
		
		currentProcess = NULL;
		
		scheduler();
		
	/*else "pass it up"*/	
	} else {
		
		moveState(oldTLB, currentProcess->p_oldTLB);
		
		moveState(currentProcess->p_newTLB, &(currentProcess->p_s)); 
		
		loadState(&currentProcess->p_s);
		
	}
	
	return 0;
	
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
	
	/*If the current process does not have a value for newPGM, kill it*/
	if(currentProcess->p_newPGM == NULL) { 
		
		terminateProcess(); 
		
		currentProcess = NULL;
		
		scheduler();
		
	/* else, "pass it up"*/
	} else {
		
		moveState(oldProgram, currentProcess->p_oldPGM);
		
		moveState(currentProcess->p_newPGM, &(currentProcess->p_s)); 
		
		loadState(&currentProcess->p_s);
		
	}
	
	return 0;
	
}

/*****************Syscall Exception Handling*****************/

/* Occurs: When a Syscall or Breakpoint assembler instuction is executed.
 * 
 * Executes some instruction based on the value of 1-8 found in a[0] */
int syscallHandler(){
	
	/*local vars*/
	int kernelMode; /*Hold boolean value of if in kernel/user mode*/
	
	
	/*context switch!*/	
	moveState(oldSys, &(currentProcess->p_s));
	
	/*move on from interrupt (so groundhog day won't happen)*/
	currentProcess->p_s.s_pc = (currentProcess->p_s.s_pc)+4;
	
	/*set kernelMode*/
	kernelMode = (oldSys->s_status & KUp);
	
	/*If syscall is 9 or greater, kill it or pass up*/
	if (oldSys->s_a0 >= 9) {
		
		/*If the current process does not have a value for newTLB, kill it*/	
		if(currentProcess->p_newSYS == NULL) { 
			
			terminateProcess(); 
			
			currentProcess = NULL;
			
			scheduler();
			
		/*else "pass it up"*/	
		} else {
			
			moveState(oldSys, currentProcess->p_oldSYS);
			
			moveState(currentProcess->p_newSYS, &(currentProcess->p_s)); 
			
			loadState(&currentProcess->p_s);
			
		}
		
	} 
	/* check if we are in kernel mode and the syscall is from 1-8 */
	if (oldSys->s_a0 < 9 && kernelMode != 0) { /*Question: If it freaks out here, try "==" instead of !="*/
		switch (oldSys->s_a0){
			case CREATEPROCESS: 		
				createProcess((state_t *) oldSys->s_a1);  
			
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
	
	return 0;
	
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
	loadState(&(currentProcess->p_s));

}

/*Syscall 2 causes the executing process to cease to exist (POOF). In addition, recursively, all progeny of this
 * process are terminated as well.*/
void terminateProcess()
{
	/*call SYS2 recursively in order to get rid all children*/
	while(!emptyChild(currentProcess)){
		
		currentProcess = removeChild(currentProcess);
		terminateProcess();
	}
	
	/*check if current process has been annihilated*/
	if (!currentProcess) {
		
		outChild(currentProcess);
		
		currentProcess = NULL;

	}
	
	/*Check if the currentProcess is blocked*/
	else if (currentProcess->p_semAdd != NULL)
	{
		outBlocked(currentProcess);
		
		/*if the semaAdd is not the clock, subtract a softBlk*/
		if((currentProcess->p_semAdd) > &(deviceList[0][0])){
			
			softBlockCount = softBlockCount -1;
		}
		
		/*The semAdd is the clock*/
		else{
			*(currentProcess->p_semAdd) = *(currentProcess->p_semAdd + 1);
			
		}
		
		
	}
	
	/*else the currentProcess is on the readyQueue so call outProcQ*/
	else{
		outProcQ(&(readyQueue), currentProcess);
		
	}
	
	freePcb(currentProcess); /*put it on the free PCB list beacuse it is now been officially killed*/
	processCount = processCount - 1; /*One less process to worry about. */
	
	

}

/*Syscall 3 (or "V Operation) causes the sema4 from the address given to be signaled and incremented*/
void verhogen(int *semaddr) {
	
	pcb_t *p;
	
	*(semaddr) = *(semaddr)+1; /* Increment the int pointed ay by currentProcess->p_s.s_a1 */
	
	p = removeBlocked(semaddr);

	/*If p exists based on the address given, put it on the readyQueue*/
	if (p != NULL) {
					
		insertProcQ(&readyQueue, p);
	}

	oldSys->s_pc = oldSys->s_pc + 4;
	loadState(oldSys);				/*Non-blocking call*/
	
}

/*Syscall 4 (or "P Operation) causes the sema4 from the address given to be told to wait and decremented*/
void passeren(int *semaddr) {
	
	*(semaddr) = *(semaddr)-1; /* Decrement the int pointed ay by currentProcess->p_s.s_a1 */

	/*If the semaddr is less than 0, block the currentProcess & prep for context switch*/
	if (*(semaddr) < 0) {
		
		/*Store time and change the time it took to process*/
		STCK(endTOD);
		currentProcess->p_CPUTime = (currentProcess->p_CPUTime) + (endTOD-startTOD);
		
		/*Block process*/
		moveState(oldSys, &(currentProcess->p_s));	
		currentProcess->p_semAdd = semaddr;	
		insertBlocked(semaddr, currentProcess);
		scheduler();	
	}
	
	oldSys->s_pc = oldSys->s_pc + 4;
	loadState(oldSys);
	
	
}

/* Syscall 5 causes the nucleus to store the processor state at the time of the exception in that area pointed
 * to by the address in a2, and loads the new processor state from the area pointed to by the address given in a3.
 * This syscall can only be called at most once for each of the three excpetion types(TLB, PGM, and Syscall). 
 * Any request that that calls this more than that shall be executed (Sys2 style).*/
void specTrapVec(int type, state_t *oldP, state_t *newP) {
	
	if(currentProcess->p_newTLB != NULL || currentProcess->p_newPGM != NULL || currentProcess->p_newSYS) {
		
		terminateProcess();
		scheduler();
		
	}
	
	/*type indicates which of the 6 state ptrs should be changed*/
	else {
		
		/*If a2 is a 0, it indicates a TLB type*/
		if(type == 0)
		{				
			currentProcess->p_oldTLB = oldP;
			currentProcess->p_newTLB = newP;
		}
		
		/*If a2 is a 1, it indicates a PGM type*/
		else if(type == 1)
		{
			currentProcess->p_oldPGM = oldP;
			currentProcess->p_newPGM = newP;
		}

		/*If a2 is 2, it indicates a Syscall type*/
		else
		{
			currentProcess->p_oldSYS = oldP;
			currentProcess->p_newSYS = newP;
		}

		loadState(&(currentProcess->p_s));
	}
			
}

/*Syscall 6 causes the procesor time (in microseconds) used by the requesting process to be placed/returned to the caller's v0. 
 * This means that the nucleaus must record in the PCB the amount of processor time used by each process*/
void getCPUTime(){
	
	/* place the processor time in microseconds in the v0 reg */
	currentProcess->p_s.s_v0 = currentProcess->p_CPUTime;

	/* load the state of the process to continue */ 
	loadState(&(currentProcess->p_s));
}

/*Syscall 7 performs a P operation on the pseudo-clock timer sema4. This sema4 is V'd every 100 milliseconds automatically by the nucleus*/
void waitForClock(){
	
	passeren(&clockTimer);
	
	softBlockCount = softBlockCount+1;

	loadState(&(currentProcess->p_s));

}

/*Syscall 8 performs a P operation on the I/O device sema4 indicated by the values in a1, a2, and optionally a3*/
void waitForIO(int intlNo, int dnum, int waitForTermRead){
	/*Check if the line number is a terminal based on the constant*/
	if(intlNo == TERMINT)
	{
		/*If we don't want to wait to read it as a ternimal, set the terminal line number.*/
		if(!waitForTermRead)
		{
			intlNo = intlNo - 2;
			currentProcess->p_s.s_v0 = deviceStatusList[intlNo - 2][dnum]; /*set the status word for a terminal*/
		}
	}
	
	else
	{
		intlNo = intlNo - 3;
		currentProcess->p_s.s_v0 = deviceStatusList[intlNo - 3][dnum]; /*set the status word*/
		
	}
	
	/*Perform a P operation on the correct sema4*/
	deviceList[intlNo][dnum] = (deviceList[intlNo][dnum])-1;
	
	if(deviceList[intlNo][dnum] < 0)
	{
			
		insertBlocked(&(deviceList[intlNo][dnum]), currentProcess);
		
		/*Store time and change the time it took to process*/
		STCK(endTOD);
		currentProcess->p_CPUTime = (currentProcess->p_CPUTime) + (endTOD-startTOD);
		
		
		currentProcess = NULL;
		softBlockCount = softBlockCount - 1;
		
		scheduler();	/*Blocking call*/
	}
	
	oldSys->s_pc = oldSys->s_pc + 4;
	loadState(oldSys);
	
}

