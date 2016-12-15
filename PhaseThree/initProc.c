/*****************************  Exceptions.c  *************************************** 

 * this module implements test() and all the U-proc initialization routines. 
 * It exports the VM-IO support level's global variables
 * 
 * Helper functions: 
 * 		
 * 

 ****************************************************************************/

#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"
#include "../e/initProc.e"
#include "../e/initial.e"
#include "../e/asl.e"
#include "../e/scheduler.e"
#include "../e/exceptions.e"
#include "../e/interrupts.e"
#include "/usr/local/include/umps2/umps/libumps.e"


/****************Module global variables****************/

int masterSem = 0;
int swapSem = 1; 
int diskSem0 = 1;

Tproc_t procs[MAXPAGEPROCS];
swap_t swapTables[SWAPPAGES];
int sema4array[DEVICELISTNUM * DEVICENUM];

pteOS_t KSegOS;

/*******************Main Functions***********************/
void test()
{
	/*Local Vars*/
	state_t processState;
	segTbl_t* segTbl;
	
	/*Init sema4 array*/
	for (int i = 0; i < (DEVICELISTNUM*DEVICENUM); i++) {
		sema4array[i] = 1; 
	}
	
	/*P on the process sema4s*/
	for(int i = 0; i < MAXPAGEPROCS; i++)
	{
		SYSCALL(PASSEREN, (int)&masterSem,0,0);
	}
	
	/*initialize swap pool */
	for(int i = 0; i < SWAPPAGES; i++){ 
		swapTables[i].sw_asid = -1; 
		swapTables[i].sw_pte = NULL;
	}
	
	/*Init ksegos*/
	KSegOS.header = (MAGICNUMBER << 24) | KUSEGOSSIZE;
	for(int i=0; i < KUSEGOSSIZE  ;i++){
		KSegOS.pteTable[i].pte_entryHI = (SEGOSADDR + i) << 12;
		KSegOS.pteTable[i].pte_entryLO = ((SEGOSADDR + i) << 12)| DIRTYON | GLOBALON | VALIDON;
		
	 }
	
	/* big loop for init u-procs (page 50 blue book)*/
	for(int i = 1; i < MAXPAGEPROCS +1; i++) {
		
		/*Assign the U-Proc a unique ASID*/
		processState.s_asid = i << 6;
		
		/*Init the U-Procs KUseg2 PTE*/
		procs[i-1].Tp_pte.header = 1;
		for(int j = 0; j < KUSEGPTESIZE; j++)
		{
			procs[i-1].Tp_pte.pteTable[j].pte_entryHI = (SEGTWOADDR + j) << 12; 
			procs[i-1].Tp_pte.pteTable[j].pte_entryLO = ALLOFF | DIRTYON;
		}
		
		procs[i-1].Tp_pte.pteTable[KUSEGPTESIZE].pte_entryHI = LASTADDR << 12; /*Last entry in the table has different entry hi*/
		
		/*Init the u-proc's private sema4*/
		procs[i-1].Tp_sem = 0;
		
		/*Init the U=proc's segment table*/
		segTbl = (segTbl_t *) STARTADDR + (i * 24); /*multiply by segtable width*/
		segTbl->ksegOS = &KSegOS;
		segTbl->kUseg2 = &(procs[i].Tp_pte);
		
		/*Init the U-proc's three (pgmTrap, TLB, and SYS/Bp) new processor state areas*/
		/*Question: This goes here?? Blue book steps..*/
		state_t * newArea;
		
		for(int j = 0; j < TRAPTYPES; j++)
		{
			newArea = &(procs[i-1].Tnew_trap[j]);
			newArea->s_asid = getENTRYHI();
			newArea->s_status = ALLOFF | IM | TE | VMc; /*Interrupts on, Timer on, Virtual Memory on*/
			
			/*Based on the Traptype, set the stack pointer & pc*/
			if(j == TLBTRAP)
			{
				newArea->s_pc = (memaddr) "respective support lvl";
				newArea->s_t9 = (memaddr) "respective support lvl";
				newArea->s_sp  = MATHMATH;
			}
			
			else if (j == PROGTRAP)
			{
				newArea->s_pc = (memaddr) "respective support lvl";
				newArea->s_t9 = (memaddr) "respective support lvl";
				newArea->s_sp  = MATHMATH;
			}
			
			else
			{
				newArea->s_pc = (memaddr) "respective support lvl";
				newArea->s_t9 = (memaddr) "respective support lvl";
				newArea->s_sp  = MATHMATH;
			}
			
		}
		
		
		/*set up the state for process for "step 2" of u-proc initilization*/
		processState.s_pc = (memaddr) midwife;
		processState.s_t9 = (memaddr) midwife;
		processState.s_status = ALLOFF | TE | IM; /*Interrupts enabled, user-mode off, timer on*/
		processState.s_sp = (ROMPAGESTART + 0x40); /*Question: Come back here..*/
		
		/*Create the process (SYS 1)*/
		SYSCALL(CREATEPROCESS,(int)&processState,0,0);
	}
	
	/* call sys 2 the end! */
	SYSCALL(TERMINATEPROCESS, 0,0,0);
}

/*Initialize u-proc by calling 3 SYS5's, reading in u-proc's .text and .data from the tape, and preparing for launch of u-proc*/
void midwife(){
	
	/*Who am I?*/
	int asid = getENTRYHI();
	asid = asid & MASKBIT >> 6; /*Mask what we dont need in register*/
	
	/*Perform the three SYS5 operations*/
	for(int i =0; i < TRAPTYPES; i++)
	{
		SYSCALL(SPECTRAPVEC, i, (int)&(procs[asid-1].Told_trap[i]), (int)&(procs[asid-1].Tnew_trap[i]);
	}
	
	/*Device registers in phase 2 terminal 0*/
	
	/*Read the contents of tape device (asid-1) onto backing store (disk 0) until end*/
	/*Question: what is value of end of tape and end of file? p.40*/
	device_t *tape = (device_t*) 
	device_t *disk = (device_t*)
	device_t *buffer = (device_t*)
	while((tape->d_data1 != EOT) && ( tape-> d_data1 !=EOF){
		
	}
	
	/*Set up process state*/
	state_t processState;
	processState.s_t9 = (memaddr) 0x8000.00B0;
	processState.s_pc = (memaddr) 0x8000.00B0;
	processState.s_asid = getENTRYHI();
	

	procState.s_status = ALLOFF | TE | VMc | IM;
	LDST(&processState);

	
	
	/*header for kuseg2 is magic num(42) | 31*/

	
}
