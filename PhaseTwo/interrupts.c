/*****************************  Interrupts.c  *************************************** 
 * Occurs: When either a previously initiated I/O request completes or when either a processor Local
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

#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"
#include "../e/asl.e"

int interruptHandler(){
	
}
