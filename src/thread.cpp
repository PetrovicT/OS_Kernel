#include "thread.h"
#include "define.h"
#include "PCB.h"
#include "timerR.h"

extern PCB* running;
#ifndef BCC_BLOCK_IGNORE
extern bool requestedContextSwitch;
#endif

// constructor
Thread::Thread(StackSize _stackSize, Time _timeSlice) {
	LOCK
		myPCB = new PCB(_stackSize, _timeSlice, this);
	UNLOCK
}


// destructor
Thread::~Thread(){
	this->waitToComplete();
    LOCK
        if(myPCB != nullptr) {
			if(myPCB->stack != nullptr) delete[] myPCB->stack; //if needed
			myPCB->stack = nullptr; //if needed
			delete myPCB; // free memory
        }
        myPCB = nullptr; // do not point to deallocated memory
    UNLOCK
}


void Thread::start(){
    if(myPCB == nullptr) return;
	 LOCK
	 	 myPCB->start();
	 UNLOCK
}


void Thread::waitToComplete() {
    if(myPCB == nullptr) return;
	LOCK
		myPCB->waitToComplete();
	UNLOCK
}


// getters
ID Thread::getId(){
    if(myPCB == nullptr) return -1;
    else return myPCB->getId();
}


ID Thread::getRunningId(){
    if(running == nullptr) return -1;
    else return running->getId();
}


Thread* Thread::getThreadById(ID _id)
{
	PCB* _pcb=PCB::getPCBById(_id);
	if (_pcb == nullptr) return nullptr;
	else return _pcb->getThread();
}


void dispatch(){ // synchronized context change
#ifndef BCC_BLOCK_IGNORE
	DISABLE_INTERRUPT // we must disable interrupts manually, hardware does that for us only in interrupt routines (I=0)
		requestedContextSwitch = true;
		asm int timerEntry
	ALLOW_INTERRUPT // I=1
#endif
}

;
