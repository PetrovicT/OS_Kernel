#include "semaphor.h"
#include "define.h"
#include "kSem.h"


Semaphore::Semaphore(int _init){
	LOCK
		this->myImpl = new KernelSem(this, _init);
	UNLOCK
}


Semaphore::~Semaphore(){
	LOCK
		if (myImpl == nullptr) {
			UNLOCK
			return;
		}
		delete myImpl;
		myImpl = nullptr;
	UNLOCK
}


int Semaphore::wait(Time maxTimeToWait){
	if (myImpl==nullptr) return -1;
	else return myImpl->wait(maxTimeToWait);
}


void Semaphore::signal(){
	if (myImpl==nullptr) return;
	else myImpl->signal();
	return;
}


int Semaphore::val() const{
	if (myImpl==nullptr) return -1;
	else return myImpl->val();
}
