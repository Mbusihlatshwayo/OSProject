/*****************************  Pager.c  *************************************** 

 * This module implements the VM-I/O support level TLB exception handler; the pager 
 * 
 * Helper functions: 
 * 		
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
#include "../e/sysSupport.e"
#include "../e/pager.e"
#include "/usr/local/include/umps2/umps/libumps.e"


/****************Module global variables****************/

Tproc_t procs[MAX];
int nextPage;
state_t * causeProcess;

/*******************Main Function***********************/

void pager(){
	
	int missingPage, pageIndex, segment,frameNumber, pageNumber, removingProcID;//removing??
	
	devregarea_t* deviceData= (devregarea_t *) RAMBASEADDR;
	
	asid =  getENTRYHI();
	asid = (asid & MASKBIT) >> 6; /*Gracie : getASID is not built it*/
	
	causeProcess = &(procs[asid-1].Told_trap[SYSTRAP]); /*Gracie: getCaller is not built in*/
	
	SYSCALL(PASSEREN, (int) &swapSem, 0, 0); /*P the swap sema4*/
	
    /*Gracie: pageLocation is RAMTOP minus 3 stacks of 4k*/
    int pageLocation = ((deviceData-> rambase) + (deviceData->ramsize)) - (PAGESIZE * 3);
    
    /*Gracie: causeRegister stuff*/
    int cause = ((causeProcess->s_cause) & 0xFC) >> 2;

	/*If cause isn't tlb invalid kill the proc*/
	/*Qeustion: The TLBL and TLBS might be in the yellow book. If it's not change the name of "TLBL" etc*/
	if(cause != TLBL & cause !=TLBS)
	{
		SYSCALL(TERMINATEPROCESS, 0, 0, 0);
	}
	
	/*Gracie: missingPage stuff & pageIndex*/
	missingPage = causeProcess->s_asid & 0x3ffff00) >> 12;
	pageIndex = causeProcess->s_asid & 0x3ffff00) >> 12;
	
	/*Gracie: our const is the same as thiers because it's from yellow book*/
	if(missingPage >= KUSEGPTESIZE ) {
		pageIndex = KUSEGPTESIZE -1;
	}
	
	/*Gracie: Gotta get "frameNumber" and set new pageLocation before this if statement*/
	
	 /*It is occupied */
	 if(swapPool[frameNum].asid != -1) {

		//interrupts off 
		status = getSTATUS();
		setSTATUS(ALLOFF);
                                              
         /*Gracie: removingProcID is being swapped out (since it is occupied but we need it) so gotta get it's page table info and then kill it later*/
		removingProcID = swapPool[frameNumber].asid;
		pageNumber = swapPool[frameNumber].pageNo;
		if(pageNumber >= KUSEGPTESIZE) {
			pageNumber = KUSEGPTESIZE -1;
		}
		
		/*Gracie: Gotta invalidate the pageTable for the process we are swapping out*/
		
		setStatus(status);
		
		/*Gracie: This "diskIO" stuff is in our initProc.c file, so I'll copy the write parts over when you're done*/
	}
	
	/*Gracie: Another "diskIO"*/
}
