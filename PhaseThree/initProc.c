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
#include "../e/initial.e"
#include "../e/asl.e"
#include "../e/scheduler.e"
#include "../e/exceptions.e"
#include "../e/interrupts.e"
#include "/usr/local/include/umps2/umps/libumps.e"


/****************Module global variables****************/

int masterSem = 0;
int swapSem = 1; 
pte_t pageTables[MAXPAGEPROCS];
swap_t swapTables[SWAPPAGES];
pteOS_t KSegOS;

void test()
{
	/*Init ksegos*/
	/*KSegOS.header = Something or other*/
	for(int i=0; i < KSEGOSPTESIZE ;i++){
		KSegOS.pteTable[i].pte_entryHI = (0X2000 + i) << 6;
		/*KSegOS.pteTable[i].pte_entryLO = (0X2000 + i) | DirtyBit | Global | Valid; */
		
	 }
	 
	/*initialize KSeg2*/ 
	for (int i =0; i < MAXPAGEPROCS; i++){
		/* init header here */ 
		pageTables[i].pteTable.pte_entryHI = ASID;
		pageTables[i].pteTable.pte_entryLO = (0X2000 + i) << 12;
	}
	
	/*initialize swap pool */
	for (int i = 0; i < SWAPPAGES; i++){ 
		swapTables[i].sw_asid = -1; /* all init to one? */
		swapTables[i].sw_segNo = 0X8000 << 6; /* shift left six? */ 
		swapTables[i].sw_pageNo = i;
		swapTables[i].sw_pte = /* come back optional?*/ 
	}
	
	/*Init sema4 array*/
	for (int i = 0; i < 48; i++) {
		sema4array[i] = 1; 
	}
	
	/* big loop goes here */
	for(int i = 1; i < MAXPAGEPROCS; i++) {
		/*pageTables[i].header = somethingsomething*/
		
		for(int j = 0; j < KUSEGPTESIZE; j++)
		{
			pageTables[i].pteTable[j].pte_entryHI = ASID;
			pageTables[i].pteTable[j].pte_entryLO = (0X2000 + i) << 12;
		}
		
		/*Last entry in the table is werid..something with entryHI?*/
		
		/*seg table init*/
		/*segTbl_t* segTbl = (segTbl_t) ADDRESS*/
		segTbl->ksegOS = &KSegOS;
		segTbl->kUseg2 = &(pageTables[i].pteTable);
		
		/*set up the state 
		state_t processState;
		* 
		*/
		
		SYSCALL(CREATEPROCESS,0,0,0);
	}
	
	/*P on the device sema4s*/
	for(int i = 0; i < MAXPAGEPROCS; i++)
	{
		SYSCALL(PASSEREN, (int) &sema4array[i],0,0);
	}
	
	/* call sys 2 the end! */
	SYSCALL(TERMINATEPROCESS, 0,0,0);
}


void midwife(){

	
}
