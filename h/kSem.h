#ifndef KSEM_H
#define KSEM_H

#include "define.h"
#include "list.h"
#include "PCB.h"
#include "semaphor.h"

class KernelSem {
public:

	KernelSem (Semaphore* mySemaphore, int init);
	virtual ~KernelSem ();

	virtual int wait(Time maxTimeToWait = 0);
	virtual void signal();
    int val() const;
    static void allSemaphoresUpdateTime();
    // update waiting time for list blockedLimitedPCBs
    // and try to wake them up if the time has passed

protected:
    ID myId;
    static ID currentKernSemID;
    friend class expandedPCB;

private:
    Semaphore* mySemaphore;
    int value;
    void removeKSemFromList();
    void insertExpandedPCB(expandedPCB* _expPCB);

    List<PCB*> blockedUnlimitedPCBs;
    List<expandedPCB*> blockedLimitedPCBs;
};

#endif
