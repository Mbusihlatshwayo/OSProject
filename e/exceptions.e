#ifndef EXCEPTIONS
#define EXCEPTIONS

/************************** EXCEPTIONS.E ******************************
*
*  Description here
*
*  Written by Mbusi & Gracie
*/

#include "../h/types.h"

extern int syscallHandler();
extern int tlbHandler();
extern int programTrapHandler();
extern int exceptionHandler();
extern void createProcess(state_t *statep);
extern void terminateProcess();
extern void verhogen(int *semaddr);
extern void passeren(int *semaddr);
extern void specTrapVec(int type, state_t *oldP, state_t *newP);
extern void getCPUTime();
extern void waitForClock();
extern void waitForIO(int intlNo, int dnum, int waitForTermRead);

/***************************************************************/

#endif
