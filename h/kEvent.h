#ifndef KEVENT_H
#define KEVENT_H

#include "event.h"
#include "define.h"
#include "PCB.h"

typedef unsigned char IVTNo;
class Event;

class KernelEv {
public:
	KernelEv(Event* myEvent, IVTNo ivtNo);
	~KernelEv();

	void wait();
	void signal();

protected:

private:
	Event* myEvent;
	IVTNo myIvtNo;
	int value;
	PCB* blockedPCB;
	PCB* parentPCB;
};
#endif
