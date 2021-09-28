#ifndef EXPPCB_H_
#define EXPPCB_H_

#include "PCB.h"

class expandedPCB {
public:
	expandedPCB(PCB* _myPCB, Time _maxTimeToWait) : myPCB(_myPCB), maxTimeToWait(_maxTimeToWait) {}

private:
	PCB* myPCB;
	Time maxTimeToWait;

	friend class PCB;
	friend class KernelSem;
};

#endif
