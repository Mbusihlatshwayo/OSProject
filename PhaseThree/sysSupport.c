/* this module implements the VM-IO support level sys/BP and pgmTrap exception handlers */

/* NEED WRITETERMINAL WRITEPRINTER */

#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"
#include "../e/asl.e"
#include "../e/scheduler.e"
#include "../e/initial.e"
#include "../e/interrupts.e"
#include "../e/exceptions.e"
#include "/usr/local/include/umps2/umps/libumps.e"


/* globals */

state_t *oldSys = (state_t *) OLDSYSCALL;	/*Old Syscall state*/


void handleSyscall(){
	
	
	switch (oldSys->s_a0){
			

				case WRITETERMINAL: 		

					writeTerminal(char *virtAddr, int len); 

				break;	

				case WRITEPRINTER:

					writePrinter(char *virtAddr, int len);

				break;
				
				case TERMINATE:
				
					terminate();
				
				break;
				
	}

}

void writeTerminal(char *virtAddr, int len){
	
	char string[];
	state_t *proc;
	
	/*Who am I?*/
	int asid = getENTRYHI();
	asid = asid & MASKBIT >> 6; /*Mask what we dont need in register*/
	
	proc = getCaller(asid, SYSTRAP);
	string = proc->s_a1;
	
	/* get the device register for the terminal we will write to */
	
	/* handle terminal stuff here */
	
	proc->s_v0 = 0;
	LDST(proc);

	
	int leng, status;

	if(len < 0 || len > 128) {
		
		terminate();
		
	}


	for (leng = 0; str[leng] != '\0'; leng++);
	
	status = SYSCALL (device, (int)str, leng, 0);
	
	if (status < 0) {
		status = SYSCALL (device, (int)s, 26, 0);
		SYSCALL (TERMINATE, 0, 0, 0);
	}
	
}

void writePrinter(){
	
	char string[];
	state_t *proc;
	
	/*Who am I?*/
	int asid = getENTRYHI();
	asid = asid & MASKBIT >> 6; /*Mask what we dont need in register*/
	
	proc = getCaller(asid, SYSTRAP);
	string = proc->s_a1;
	
	/* get the device register for the terminal we will write to */
	
	/* handle terminal stuff here this is slightly diff from terminal above */
	
	proc->s_v0 = 0;
	LDST(proc);

	
	int leng, status;

	if(len < 0 || len > 128) {
		
		terminate();
		
	}


	for (leng = 0; str[leng] != '\0'; leng++);
	
	status = SYSCALL (device, (int)str, leng, 0);
	
	if (status < 0) {
		status = SYSCALL (device, (int)s, 26, 0);
		SYSCALL (TERMINATE, 0, 0, 0);
	}
	
}
void terminate(){
	
	int asid = getENTRYHI();
	asid = asid & MASKBIT >> 6;
	state_t *proc;
	proc = getCaller(asid, SYSTRAP);
	
	SYSCALL(PASSEREN, (int)&swapSem, 0, 0);
	TLBCLR();
	SYSCALL(VERHOGEN,(int)&swapSem, 0, 0);
	
	SYSCALL(TERMINATEPROCESS,0,0,0);
	
}
