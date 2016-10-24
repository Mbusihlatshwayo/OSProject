#ifndef CONSTS
#define CONSTS

/**************************************************************************** 
 *
 * This header file contains utility constants & macro definitions.
 * 
 ****************************************************************************/

/* maximum process ///*/
#define MAXPROC 20

/* Hardware & software constants */
#define PAGESIZE		4096	/* page size in bytes */
#define WORDLEN			4		/* word size in bytes */
#define PTEMAGICNO		0x2A


#define ROMPAGESTART	0x20000000	 /* ROM Reserved Page */

/*Clock values*/
#define pseudoSec 100000000 /*time put into LDIT*/
#define smallPseudoSec 100000 /*time put into LDIR for initial.c*/
#define timeSlice 5000 	 /*time set for timers*/

/* SYSCALL values*/
#define CREATEPROCESS 1
#define TERMINATEPROCESS 2
#define VERHOGEN 3
#define PASSEREN 4
#define SPECTRAPVEC 5
#define GETCPUTIME 6
#define WAITCLOCK 7 
#define WAITIO 8


/* timer, timescale, TOD-LO and other bus regs */
#define RAMBASEADDR	0x10000000
#define TODLOADDR	0x1000001C
#define INTERVALTMR	0x10000020	
#define TIMESCALEADDR	0x10000024

/* reserved areas*/
#define ADDRESS1 0x10000000
#define NEWSYSCALL 0x200003D4
#define NEWTRAP 0x200002BC
#define NEWTLB 0x200001A4
#define NEWINTERRUPT 0x2000008C
#define OLDSYSCALL 0x20000348
#define OLDTRAP 0x20000230
#define OLDTLB 	0x200001A4
#define OLDINTERRUPT 0x20000000

/*semaphore number for devices*/
#define DEVICELISTNUM 8
#define DEVICENUM 6


/* utility constants */
#define	TRUE		1
#define	FALSE		0
#define ON              1
#define OFF             0
#define HIDDEN		static
#define EOS		'\0'

#define NULL ((void *)0xFFFFFFFF)


/* vectors number and type */
#define VECTSNUM	4

#define TLBTRAP		0
#define PROGTRAP	1
#define SYSTRAP		2

#define TRAPTYPES	3

/* device interrupts */
#define DISKINT		3
#define TAPEINT 	4
#define NETWINT 	5
#define PRNTINT 	6
#define TERMINT		7

#define DEVREGLEN	4	/* device register field length in bytes & regs per dev */
#define DEVREGSIZE	16 	/* device register size in bytes */

/* device register field number for non-terminal devices */
#define STATUS		0
#define COMMAND		1
#define DATA0		2
#define DATA1		3

/* device register field number for terminal devices */
#define RECVSTATUS      0
#define RECVCOMMAND     1
#define TRANSTATUS      2
#define TRANCOMMAND     3


/* device common STATUS codes */
#define UNINSTALLED	0
#define READY		1
#define BUSY		3

/* device common COMMAND codes */
#define RESET		0
#define ACK		1

/* operations */
#define	MIN(A,B)	((A) < (B) ? A : B)
#define MAX(A,B)	((A) < (B) ? B : A)
#define	ALIGNED(A)	(((unsigned)A & 0x3) == 0)

/* Useful operations */
#define STCK(T) ((T) = ((* ((cpu_t *) TODLOADDR)) / (* ((cpu_t *) TIMESCALEADDR))))
#define LDIT(T)	((* ((cpu_t *) INTERVALTMR)) = (T) * (* ((cpu_t *) TIMESCALEADDR))) 

/* hexadecimal values for IE, KU, BEV, and VM (for Status)*/
#define ALLOFF 0x00000000	/* everything is off bit */
#define IEc 0x00000001		/* global interrupt enable bit */
#define KUc 0x00000002		/* The “current” kernel-mode user-mode control bit.*/

#define IEp 0x00000004		/* the “previous” settings of the Status.IEc */
#define KUp 0x00000008		/* the “previous” settings of the Status.KUc*/
#define IEo 0x00000010		/* the “previous” settings of the Status.IEp*/
#define KUo 0x00000020		/* the “previous” settings of the Status.KUp*/

#define IM  0x0000ff00		/* Interrupt Mask. An 8-bit mask that enables/disables external interrupts.*/

#define BEV 0x00400000		/* The Bootstrap Exception Vector. This bit determines the starting address for the exception vectors.*/

#define VMc 0x01000000		/* The “current” VM on/off flag bit. Status.VMc=0 indicates that virtual memory translation is currently off.*/
#define VMp 0x02000000		/* the “previous” setting of the Status.VMc bit*/
#define VMo 0x04000000		/* the “previous” setting of the Status.VMp bit*/

#define TE  0x08000000		/* the processor Local Timer enable bit. A 1-bit mask that enables disables the processor’s Local Timer.*/

/*-------------------------------------------------------------------*/
#endif
