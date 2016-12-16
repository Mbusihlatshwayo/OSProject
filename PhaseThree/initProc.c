/*****************************  InitProc.c  *************************************** 

 * This module implements test and all the U-proc initialization routines. 
 * It exports the VM-IO support level's global variables.
 * 
 * Helper functions: 
 * 		setStateAreas-Init the U-Procs three new processor state areas
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

/*semaphore's for mutual exclusion/schronization*/
int masterSem = 0;
int swapSem = 1; 
int diskSem = 1;

/*Arrays for u-procs, mutual exclusion sema4s, and swappool pages*/
Tproc_t procs[MAXPAGEPROCS];
swap_t swapTables[SWAPPAGES];
int sema4array[DEVICELISTNUM * DEVICENUM];

/*KsegOS page table*/
pteOS_t KSegOS;

/*******************Main Function***********************/
/*Initialize the VM-support level global vars including the u-procs.*/
void test()
{
	/*Local Vars*/
	state_t processState;
	segTbl_t* segTbl;
	
	/*Init sema4 array to 1  for mutual exclusion*/
	for (int i = 0; i < (DEVICELISTNUM*DEVICENUM); i++) {
		sema4array[i] = 1; 
	}
	
	/*P on the process sema4s*/
	for(int i = 0; i < MAXPAGEPROCS; i++)
	{
		SYSCALL(PASSEREN, (int)&masterSem,0,0);
	}
	
	/*Initialize swap pool */
	for(int i = 0; i < SWAPPAGES; i++){ 
		swapTables[i].sw_asid = -1; 
		swapTables[i].segNo = 0;
		swapTables[i].pageNo = 0;
		swapTables[i].sw_pte = NULL;
	}
	
	/*Init ksegos*/INT
	KSegOS.header = (MAGICNUMBER << 24) | KUSEGOSSIZE;
	for(int i=0; i < KUSEGOSSIZE  ;i++){
		KSegOS.pteTable[i].pte_entryHI = (SEGOSADDR + i) << 12;
		KSegOS.pteTable[i].pte_entryLO = ((SEGOSADDR + i) << 12)| DIRTYON | GLOBALON | VALIDON;
		
	 }
	
	/*Big loop for init u-procs (page 50 blue book)*/
	for(int i = 1; i < MAXPAGEPROCS +1; i++) {
		
		/*Init the U-Procs KUseg2 PTE*/
		procs[i-1].Tp_pte.header = (MAGICNUMBER << 24) | KUSEGOSSIZE;
		for(int j = 0; j < KUSEGPTESIZE; j++)
		{
			procs[i-1].Tp_pte.pteTable[j].pte_entryHI = (SEGTWOADDR + j) << 12; 
			procs[i-1].Tp_pte.pteTable[j].pte_entryLO = ALLOFF | DIRTYON;
		}
		
		procs[i-1].Tp_pte.pteTable[KUSEGPTESIZE].pte_entryHI = LASTADDR << 12; /*Last entry in the table has different entry hi*/
		
		/*Init the u-proc's private sema4*/
		procs[i-1].Tp_sem = 0;
		
		/*Init the U-proc's segment table*/
		segTbl = (segTbl_t *) STARTADDR + (i * 24); /*multiply by segtable width*/
		segTbl->ksegOS = &KSegOS;
		segTbl->kUseg2 = &(procs[i-1].Tp_pte);		
		
		processState.s_asid = i << 6;	/*Assign the U-Proc a unique ASID*/
		processState.s_pc = (memaddr) midwife;
		processState.s_t9 = (memaddr) midwife;
		processState.s_status = ALLOFF | TE | IM; /*Interrupts enabled, user-mode off, timer on*/
		processState.s_sp = TAPEBUFFERSTART - (SYSSTACK + (2*(i-1)*PAGESIZE));/*set to the SYSCALL stack*/
			
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
	
	/*set up the state for process for "step 2" of u-proc initilization*/
	state_t * newArea = setStateAreas();
	
	/*Perform the three SYS5 operations*/
	for(int i =0; i < TRAPTYPES; i++)
	{
		SYSCALL(SPECTRAPVEC, i, (int)&(procs[asid-1].Told_trap[i]), (int)newArea);
	}
		
	/*Read the contents of tape device (asid-1) onto backing store (disk 0) until end*/
	device_t *tape = (device_t*) TAPEDEV + ((asid-1) * DEVREGSIZE);
	device_t *disk = (device_t*) DISKDEV;
	int buffer = BUFFERSTART + ((asid-1) * PAGESIZE);
	
	int pageNum = 0; /*Do the while loop write for each page*/
	
	while((tape->d_data1 != EOT) && ( tape-> d_data1 !=EOF){
		tape->d_data0 = buffer; /*Specify the starting physical address for a DMA read operation*/
		
		/*Read the current block up to the next EOB/EOT marker and copy it to RAM starting at data 0 addr (buffer)*/
		tape->d_command = READBLK;
		
		/*Wait for data read*/
		status = SYSCALL(WAITIO, TAPEINT, asid-1, 0);
		
		/*Check status. If not ready, kill it*/
		if(status != READY)
		{
			SYSCALL(TERMINATE,0,0,0);
		}
		
		/*block for disk 0*/
		SYSCALL(PASSEREN, (int)&diskSem,0,0);
		
		/*Question: Find correct cylinder based on asid*/
		/*Question: Command correct?*/
		int command = (asid) << 8;
		disk->d_command = command;
		
		/*Question: Write on disk? (5.4 Yellowbook)*/
		
		/*Wait for data write*/
		status = SYSCALL(WAITIO, DISKINT, 0, 0);
		
		/*Check status. If not ready, kill it*/
		if(status != READY)
		{
			SYSCALL(TERMINATE,0,0,0);
		}
		
		/*unblock sema4*/
		SYSCALL(VERHOGEN, (int)&diskSem,0,0);
		
		pageNum = pageNum  + 1;
	}
	
	/*Set up process state*/
	state_t processState;
	processState.s_t9 = (memaddr) PAGE52ADDR;
	processState.s_pc = (memaddr) PAGE52ADDR;
	processState.s_sp = LASTSEG2PG;
	processState.s_asid = getENTRYHI();
	

	procState.s_status = ALLOFF | TE | VMc | IM;
	LDST(&processState);
	
}

state_t setStateAreas()
{	
	/*Question: Is it okay for this to be a helper function? Does the sequence from blue book matter*/
	/*Init the U-proc's three (pgmTrap, TLB, and SYS/Bp) new processor state areas*/
	state_t * newArea;
	
	for(int j = 0; j < TRAPTYPES; j++)
	{
		newArea = &(procs[i-1].Tnew_trap[j]);
		newArea->s_asid = getENTRYHI();
		newArea->s_status = ALLOFF | IM | TE | VMc; /*Interrupts on, Timer on, Virtual Memory on*/
		
		/*Based on the Traptype, set the stack pointer & pc*/
		if(j == TLBTRAP)
		{
			newArea->s_pc = (memaddr) pager;
			newArea->s_t9 = (memaddr) pager;
			newArea->s_sp = (TAPEBUFFSTART - (2 * (asid-1) * PAGESIZE));
		}
		
		else if (j == PROGTRAP)
		{
			newArea->s_pc = (memaddr) handleSyscall;
			newArea->s_t9 = (memaddr) handleSyscall;
			newArea->s_sp = (TAPEBUFFSTART - ((2 * (asid-1) * PAGESIZE) + (PAGESIZE * j)));
		}
		
		else
		{
			newArea->s_pc = (memaddr) handleSyscall;
			newArea->s_t9 = (memaddr) handleSyscall;
			newArea->s_sp  = (TAPEBUFFSTART - ((2 * (asid-1) * PAGESIZE) + (PAGESIZE * j)));
		}
		
	}
	
	return &newArea
}
