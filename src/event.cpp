#include "define.h"
#include "event.h"

class KernelEv;

Event::Event(IVTNo ivtNo) {
	LOCK
		this->myImpl = new KernelEv(this, ivtNo);
	UNLOCK
}

Event::~Event() {
	LOCK
		delete myImpl;
		myImpl = 0;
	UNLOCK
}

void Event::wait() {
	if (myImpl == nullptr) return;
    myImpl->wait();
}

void Event::signal() {
	if (myImpl == nullptr) return;
    myImpl->signal();
}
