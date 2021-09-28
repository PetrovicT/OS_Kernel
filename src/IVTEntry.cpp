#include <dos.h>
#include "IVTEntry.h"
#include "kEvent.h"

IVTEntry* IVTEntry::IVT[IVTsize] = {nullptr};

IVTEntry::IVTEntry(IVTNo _IVTnumEntry, pInterrupt _pNewInterrupt) : IVTnumEntry(_IVTnumEntry), pNewInterrupt (_pNewInterrupt) {
	LOCK
		this->pOldInterrupt;
		this->myKernelEvent = nullptr;
		IVT[_IVTnumEntry] = this;
	UNLOCK
}


IVTEntry::~IVTEntry(){
#ifndef BCC_BLOCK_IGNORE
	DISABLE_INTERRUPT;
	this->clearEvent();
	ALLOW_INTERRUPT;
#endif
}


void IVTEntry::signal(){
	if (this->myKernelEvent != nullptr) this->myKernelEvent->signal();
}


void IVTEntry::clearEvent(){
#ifndef BCC_BLOCK_IGNORE
	DISABLE_INTERRUPT;
		setvect(this->IVTnumEntry, pOldInterrupt);
	this->myKernelEvent = nullptr;
	IVT[this->IVTnumEntry] = nullptr;
	ALLOW_INTERRUPT;
#endif
}


void IVTEntry::setEvent(KernelEv* myEvent){
	this->myKernelEvent = myEvent;
#ifndef BCC_BLOCK_IGNORE
	DISABLE_INTERRUPT;
		pOldInterrupt = getvect(this->IVTnumEntry);
		setvect(this->IVTnumEntry, pNewInterrupt);
	ALLOW_INTERRUPT;
#endif
}


IVTEntry* IVTEntry::getIVTEntry(IVTNo _ivtno){
	return IVT[_ivtno];
}

void IVTEntry::callOldRoutine(){
	pOldInterrupt();
}

;
