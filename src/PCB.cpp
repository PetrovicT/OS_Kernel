#include "PCB.h"
#include "SCHEDULE.H"
#include <dos.h>
#include <iostream.h>
#include "IVTEntry.h"
#include "kEvent.h"
#include "kSem.h"


List<PCB*> allPCBs;
ID PCB::currentID = 0;

PCB::PCB(StackSize _stackSize, Time _timeSlice, Thread* _thread){
    LOCK
		this->myThread = _thread;
		this->parent = running; 
		this->timeSlice = _timeSlice;
		this->myLOCK = 0;
		this->myID = ++currentID;
		this->threadState = PCB::NEW;
		this->isIdle = false;

		// stack
		if (_stackSize > 65536) _stackSize = 65536;  // Maximum stack size is 64KB, 64KB = 2^16 B = 65636
		if (_stackSize < 256 && _stackSize != 0) _stackSize = 256;
		if (_stackSize == 0) _stackSize = 0; // for mainPCB
		this->stackSize = _stackSize / sizeof(unsigned);

		if (_thread != nullptr) { // only mainPCB does not need stack
			initializeStack(runWrapper);
		}

		allPCBs.insertBack(this);

	UNLOCK
}

#ifndef BCC_BLOCK_IGNORE
PCB::PCB(){} // for idlePCB
#endif


PCB::~PCB(){
    LOCK
        if (this->stack != nullptr) delete[] stack;
        this->stack = nullptr;
        // update list of pcbs, delete this pcb
         for(List<PCB*>::Iterator i = allPCBs.createIterHead();i != allPCBs.end(); i++){
        	 if(this == *i){ i.deleteCurrentNode(); break; }
         }
    UNLOCK
}


void PCB::initializeStack(pFunction fp){
    LOCK
        stack = new unsigned[stackSize];
    UNLOCK
    stack[stackSize - 1] = 0x0200;  // initial psw, only I=1
#ifndef BCC_BLOCK_IGNORE
    stack[stackSize - 2] = FP_SEG(fp); // CS
    stack[stackSize - 3] = FP_OFF(fp); // IP
    // from -4 to -11 are registers: ax, bx, cx, dx, es, ds, si, di
    // -12 represents bp
    ss = FP_SEG(stack + stackSize - 12);
    sp = FP_OFF(stack + stackSize - 12);
#endif
    bp = sp;
}


void PCB::runWrapper(){
    running->getThread()->run(); // wait until I finish
    LOCK

		// unblock all pcbs that are waiting on me
		if(running == nullptr) return;
			while(!running->waitingForMe.isEmpty()){
				PCB* _pcb = running->waitingForMe.getFirst();
				running->waitingForMe.deleteFront();
				if (_pcb != nullptr) {  // unblock _pcb
					if(_pcb->threadState == PCB::BLOCKED){
						_pcb->threadState = PCB::READY;
						Scheduler::put(_pcb);
					}
				} // end of unblock _pcb
			} // end of unblocking

        running->threadState = PCB::TERMINATED;
        dispatch();

    UNLOCK
}


void PCB::start(){
    LOCK
		if (this->threadState != PCB::NEW) { // thread can be started only one time
			UNLOCK
			return;
		}
        if (threadState == PCB::NEW){
        	threadState = PCB::READY;
            Scheduler::put(this);
        } 
    UNLOCK
}


// running is waiting for this
void PCB::waitToComplete(){
	LOCK
		if(this==nullptr) {
			UNLOCK
			return;
		}
		if(this->threadState == PCB::TERMINATED) {
			UNLOCK
			return;
		}
		if (this->isIdle == true){
			UNLOCK
			return;
		}
		if(this == running) {
			UNLOCK
			return;
		}
		if(this->threadState == PCB::NEW){
			UNLOCK
			return;
		}
		// all checked, can block running

		this->waitingForMe.insertBack(running);

		// block running pcb
			if (running->threadState != PCB::TERMINATED)
				running->threadState = PCB::BLOCKED;

		dispatch();
	UNLOCK
}


PCB* PCB::getPCBById(ID id){
	LOCK
		List<PCB*>::Iterator i = allPCBs.createIterHead();
		for(; i != allPCBs.end(); i++){
			if (id == (*i)->getId()) return (*i);
		}
    UNLOCK
    return nullptr;
}
