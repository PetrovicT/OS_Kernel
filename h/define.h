#ifndef DEFINE_H_
#define DEFINE_H_
#include <dos.h>
#include <stddef.h>

// from file thread.h
typedef unsigned long   StackSize;
typedef unsigned int    Time; // time, x 55ms
typedef int             ID;

#define nullptr 0

// boolean
#ifndef BCC_BLOCK_IGNORE
typedef int     bool;
#define true    1
#define false   0
#endif

#define timerEntry 0x08
#define userEntry 0x60
#define IVT_SIZE 256

typedef void interrupt (*pInterrupt)(...); // pointer to interrupt routine
typedef void (*pFunction)(); // function wrapper

#ifndef BCC_BLOCK_IGNORE
#define DISABLE_INTERRUPT   asm { pushf; cli; };
#define ALLOW_INTERRUPT     asm { popf; };
#endif

extern volatile unsigned LOCK_VALUE;
extern volatile unsigned TimeLeft;
#define LOCK        ++LOCK_VALUE;
#define UNLOCK      --LOCK_VALUE; if(LOCK_VALUE == 0 && (running->getTimeSlice() != 0) && (TimeLeft == 0)) { dispatch(); }

void dispatch();
// registers are 16b

#endif
