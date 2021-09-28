#include "define.h"
#include "kSem.h"
#include "PCB.h"
#include "timerR.h"

volatile unsigned tss;
volatile unsigned tsp;
volatile unsigned tbp;

extern PCB *running;
extern PCB* idlePCB;

volatile bool requestedContextSwitch = false;
volatile unsigned LOCK_VALUE = 0;
volatile Time TimeLeft = defaultTimeSlice;

extern void tick(); // system knows about time passing
pInterrupt Timer::oldTimerInterrupt = 0;

// inicializing new timer interrupt
void Timer::inic(){
#ifndef BCC_BLOCK_IGNORE
	DISABLE_INTERRUPT
	   Timer::oldTimerInterrupt = getvect(timerEntry);
	   setvect(userEntry, Timer::oldTimerInterrupt);
	   setvect(timerEntry, Timer::timer);
	ALLOW_INTERRUPT
#endif
}

// restoring old timer interrupt
void Timer::restore(){
#ifndef BCC_BLOCK_IGNORE
	DISABLE_INTERRUPT
	   setvect(timerEntry, Timer::oldTimerInterrupt);
	ALLOW_INTERRUPT
#endif
}


void interrupt Timer::timer(...){	// interrupt routine
	// it is automatically pushed psw, pc and 8 registers on stack

	bool threadHasUnlimitedTime;
	if (running->getTimeSlice() == 0) threadHasUnlimitedTime = true;
		else threadHasUnlimitedTime = false;

	if (!requestedContextSwitch)
	{
		if(threadHasUnlimitedTime == false && TimeLeft > 0)
			TimeLeft--;

		#ifndef BCC_BLOCK_IGNORE
		asm int userEntry  // call real old timer
		#endif

		tick();
		KernelSem::allSemaphoresUpdateTime();
	}

	if ((threadHasUnlimitedTime == false && TimeLeft == 0 && LOCK_VALUE == 0) || requestedContextSwitch) {

		// save previous context
		#ifndef BCC_BLOCK_IGNORE
		asm {
			// asembler doesnt know the adress of running->sp so we need a global variable tsp, tss and tbp
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}
		#endif
		running->sp = tsp;
		running->ss = tss;
		running->bp = tbp;
		running->myLOCK = LOCK_VALUE;

		if (running->isIdle == false){
			if (running->threadState == PCB::READY || running->threadState == PCB::RUNNING)
			{
				running->threadState = PCB::READY;
				Scheduler::put((PCB*) running);
			}
		}

		running = Scheduler::get();
		// there are no threads ready
		 if (running->threadState != PCB::READY || running == nullptr){   running = idlePCB;  }
		 else {
			 running->threadState = PCB::RUNNING;
		 }

		// restoring old context
		TimeLeft = running->timeSlice;
		LOCK_VALUE = running->myLOCK;
		tsp = running->sp;
		tss = running->ss;
		tbp = running->bp;
		#ifndef BCC_BLOCK_IGNORE
		asm {
			mov sp, tsp
			mov ss, tss
			mov bp, tbp
		}
		#endif

		requestedContextSwitch = false;
		}
	}
