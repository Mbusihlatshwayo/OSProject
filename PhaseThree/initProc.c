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
int diskSem = 1;
Tproc_t procs[MAXPAGEPROCS];
swap_t swapTables[SWAPPAGES];
int sema4array[DEVICELISTNUM * DEVICENUM];
pteOS_t KSegOS;

void test()
{
	/*Local Vars*/
	state_t processState;
	segTbl_t* segTbl;
	
	/*Init ksegos*/
	/*KSegOS.header = Something or other*/ /*Question: what is this?*/
	for(int i=0; i < KUSEGOSSIZE  ;i++){
		KSegOS.pteTable[i].pte_entryHI = (0X2000 + i) << 12;
		KSegOS.pteTable[i].pte_entryLO = ((0X2000 + i) << 12)| DIRTYON | GLOBALON | VALIDON;
		
	 }
	
	/*initialize swap pool */
	for(int i = 0; i < SWAPPAGES; i++){ 
		swapTables[i].sw_asid = -1; 
		swapTables[i].sw_pte = NULL;
	}
	
	/*Init sema4 array*/
	for (int i = 0; i < (DEVICELISTNUM*DEVICENUM); i++) {
		sema4array[i] = 1; 
	}
	
	/* big loop for init processes */
	for(int i = 1; i < MAXPAGEPROCS; i++) {
		procs[i-1].Tp_pte.header = 1;
		
		for(int j = 0; j < KUSEGPTESIZE; j++)
		{
			procs[i-1].Tp_pte.pteTable[j].pte_entryHI = (0x8000 + j) << 12; /*Question: shifted correctly?*/
			procs[i-1].Tp_pte.pteTable[j].pte_entryLO = ALLOFF | DIRTYON;
		}
		
		/*Las entry in the table has different entry hi*/
		procs[i-1].Tp_pte.pteTable[KUSEGPTESIZE-1].pte_entryHI = 0xBFFFF << 12; /*Question: shifted correctly?*/
		
		/*seg table init*/
		segTbl = (segTbl_t *) STARTADDR; /*Question: What is segment table width?*/
		segTbl->ksegOS = &KSegOS;
		segTbl->kUseg2 = &(procs[i-1].Tp_pte);
		
		/*set up the state for process*/
		processState.s_pc = (memaddr) midwife;
		processState.s_t9 = (memaddr) midwife;
		processState.s_status = ALLOFF | IEc | TE | IM;
		processState.s_asid = i << 6;
		processState.s_sp = 0; /*Question: What is this??*/
		

		
		SYSCALL(CREATEPROCESS,(int)&processState,0,0);
	}
	
	/*P on the device sema4s*/
	for(int i = 0; i < MAXPAGEPROCS; i++)
	{
		SYSCALL(PASSEREN, (int)&masterSem,0,0);
	}
	
	
	/* call sys 2 the end! */
	SYSCALL(TERMINATEPROCESS, 0,0,0);
}


void midwife(){

	
}
