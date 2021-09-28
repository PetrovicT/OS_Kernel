#include "idlePCB.h"

IdlePCB::IdlePCB() : PCB() {
	this->myThread = nullptr;
	this->parent = nullptr;
	this->isIdle = true;
	this->timeSlice = 1;
	this->myLOCK = 0;
	this->stackSize = 256 / sizeof(unsigned);
    initializeStack(run);
}

void IdlePCB::run(){
    while(true) {}
}
