#include "kSem.h"
#include "schedule.h"
#include "expPCB.h"
#include "list.h"
#include "PCB.h"

List<KernelSem*> allKernelSems;
ID KernelSem::currentKernSemID = 0;

KernelSem::KernelSem(Semaphore* _mySemaphore, int init) : mySemaphore(_mySemaphore) {
	LOCK
		if (init > 0) this->value = init;
		else this->value = 0;

		allKernelSems.insertBack(this);
		this->myId = ++currentKernSemID;
	UNLOCK
}


KernelSem::~KernelSem(){
    LOCK
		// unblock all blocked threads on this semaphore - with limited time
		while(!blockedLimitedPCBs.isEmpty())
		{
			PCB* pcb = blockedLimitedPCBs.getFirst()->myPCB;
			if (pcb != nullptr)  // unblock pcb
			{
				if(pcb->threadState == PCB::BLOCKED)
				{
					pcb->threadState = PCB::READY;
					Scheduler::put(pcb);
				}
			}
			blockedLimitedPCBs.deleteFront();
	    }

		// unblock all blocked threads on this semaphore - with unlimited time
		while(!blockedUnlimitedPCBs.isEmpty())
		{
			PCB* pcb = blockedUnlimitedPCBs.getFirst();
			if (pcb != nullptr)  // unblock pcb
			{
				if(pcb->threadState == PCB::BLOCKED)
				{
					pcb->threadState = PCB::READY;
					Scheduler::put(pcb);
				}
			}
			blockedUnlimitedPCBs.deleteFront();
		}

		this->removeKSemFromList();
    UNLOCK
}


void KernelSem::removeKSemFromList() {
	 LOCK
		 List<KernelSem*>::Iterator i = allKernelSems.createIterHead();
		 for(; i != allKernelSems.end(); i++) {
			 if((*i) == this){
				 i.deleteCurrentNode();
				 break;
			 }
		 }
	 UNLOCK
}


void KernelSem::allSemaphoresUpdateTime(){

	if (allKernelSems.isEmpty()) return;
	LOCK
		List<KernelSem*>::Iterator i = allKernelSems.createIterHead();
		for (; i!=allKernelSems.end(); i++)
		{ // for every semaphore
			if ((*i)->blockedLimitedPCBs.isEmpty()) continue;
			//*i because it is a list of pointers on semaphores, not a list of semaphores
			(*i)->blockedLimitedPCBs.getFirst()->maxTimeToWait--;  // update only time for 1st element
			if ((*i)->blockedLimitedPCBs.getFirst()->maxTimeToWait == 0)
			{ // we need to unblock the 1st pcb and update the 2nd
				PCB* _pcb = (*i)->blockedLimitedPCBs.getFirst()->myPCB;
				(*i)->blockedLimitedPCBs.deleteFront();
				if (_pcb != nullptr)  // unblock pcb
				{
					_pcb->unblockedBySignal = false;
					if(_pcb->threadState == PCB::BLOCKED)
					{
						_pcb->threadState = PCB::READY;
						Scheduler::put(_pcb);
					}
				(*i)->value++;
				}
			}
		}
	UNLOCK
}


int KernelSem::wait(Time maxTimeToWait){
	LOCK
		if(maxTimeToWait < 0) return -5;
        --value;
		// do not block any threads
		if(value >= 0) {
			UNLOCK
			return 1;
		}

		// block running thread
		PCB* _pcb = running;
		_pcb->unblockedBySignal = false; // we need it for return value
        // block
        if (_pcb->isIdle == false)
        {
			if(_pcb->threadState != PCB::TERMINATED)
			{
				_pcb->threadState = PCB::BLOCKED;
			}
        }
        // blocking is done

        // inserting into list of threads that need to wait
        expandedPCB* expPCB = 0; // in case maxTimeToWait is not 0
		if (maxTimeToWait == 0)	{
			blockedUnlimitedPCBs.insertBack(running);
		}
		else {
			expPCB = new expandedPCB(_pcb, maxTimeToWait);
			this->insertExpandedPCB(expPCB);
		}

		dispatch();

		// previously blocked thread will continue to run on this line

		// if signal() unblocked the thread return 1, else return 0 (if the time run out)
		if (_pcb->unblockedBySignal == true) {
			_pcb->unblockedBySignal = false;
			UNLOCK
			return 1;
		}
		else {
			UNLOCK
			return 0;
		}
}

// if times to wait are 5,8,10,13,14
// we will save them as 5,3,2,3,1
void KernelSem::insertExpandedPCB(expandedPCB* _expPCB){
	LOCK
		List<expandedPCB*>::Iterator i = blockedLimitedPCBs.createIterHead();
		for (; i != blockedLimitedPCBs.end() && (_expPCB->maxTimeToWait > (*i)->maxTimeToWait); i++) {
			_expPCB->maxTimeToWait -= (*i)->maxTimeToWait;	}
		if((*i) != nullptr) (*i)->maxTimeToWait-=_expPCB->maxTimeToWait;
		i.insertBeforeArgument(_expPCB);
	UNLOCK
}


void KernelSem::signal(){
    LOCK
        (this->value)++;
    	if (this->value <= 0)
    	{ // then we need to unblock 1 pcb
    		bool onePCBUnblocked = false;

    		// first try to unblock pcb in list with unlimited time - bigger priority
    		if (!blockedUnlimitedPCBs.isEmpty())
    		{
    			PCB* _pcb = blockedUnlimitedPCBs.getFirst();
    			if (_pcb != 0)
    			{
    				blockedUnlimitedPCBs.deleteFront();
    				// unblock the _pcb
    				if (_pcb->threadState == PCB::BLOCKED) {
    					_pcb->threadState = PCB::READY;
    					Scheduler::put(_pcb);
    				}
    				_pcb->unblockedBySignal = true;
    				onePCBUnblocked = true;
    				UNLOCK
					return;
    			}

    		}

    		// try to unblock pcb in list with limited time
    		if (onePCBUnblocked == false && !blockedLimitedPCBs.isEmpty()){
    			PCB* _pcb = blockedLimitedPCBs.getFirst()->myPCB;
    			Time _maxTimeToWait = blockedLimitedPCBs.getFirst()->maxTimeToWait;
				if (_pcb != 0)
				{
					blockedLimitedPCBs.deleteFront();
					// unblock the _pcb
					if (_pcb->threadState == PCB::BLOCKED) {
						_pcb->threadState = PCB::READY;
						Scheduler::put(_pcb);
					}
					_pcb->unblockedBySignal = true;
					// update list, more precisely times in that list
					if (blockedLimitedPCBs.getFirst() != nullptr){
						blockedLimitedPCBs.getFirst()->maxTimeToWait = blockedLimitedPCBs.getFirst()->maxTimeToWait + _maxTimeToWait;
					}
				}
    		}
    	}
    UNLOCK
}


int KernelSem::val() const{
	return this->value;
}
