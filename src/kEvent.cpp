#include "kEvent.h"
#include "IVTEntry.h"
#include "schedule.h"

KernelEv::KernelEv(Event* _myEvent, IVTNo _ivtNo){
	LOCK
		this->myEvent = _myEvent;
		this->myIvtNo = _ivtNo;
		this->value = 0;
		this->blockedPCB = nullptr;
		this->parentPCB = running;
		IVTEntry::IVT[_ivtNo]->setEvent(this);
	UNLOCK
}

KernelEv::~KernelEv(){
	LOCK
		if (this->blockedPCB != nullptr)  // unblock pcb
			if (this->blockedPCB->threadState == PCB::BLOCKED) {
				this->blockedPCB->threadState = PCB::READY;
				Scheduler::put(this->blockedPCB);
			}
		IVTEntry::IVT[this->myIvtNo]->clearEvent();
	UNLOCK
}

void KernelEv::wait(){
	LOCK
		if (running == this->parentPCB){ // block pcb
			this->blockedPCB = this->parentPCB;
			this->value--;
			this->blockedPCB->threadState = PCB::BLOCKED;
			dispatch();
			UNLOCK
		} else UNLOCK
}

void KernelEv::signal(){
	LOCK
		if (this->blockedPCB == nullptr) {
			this->value = 1;
		} else {
        // unblock
			if (this->blockedPCB->threadState == PCB::BLOCKED) {
				this->blockedPCB->threadState = PCB::READY;
				Scheduler::put(this->blockedPCB);
			}
			this->blockedPCB == nullptr;
			this->value = 0;
		}
	UNLOCK
}
