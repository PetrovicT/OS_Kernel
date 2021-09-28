#ifndef PCB_H
#define PCB_H

#include "define.h"
#include "thread.h"
#include "list.h"

extern List<PCB*> allPCBs; // from PCB.cpp
extern PCB *running; // from OS.cpp
extern PCB *mainPCB; // from OS.cpp

class PCB {
public:

	// constructors
	PCB(StackSize _stackSize, Time _timeSlice, Thread* _thread = nullptr);
	PCB(); // for idlePCB
	virtual ~PCB();

    enum ThreadState {
        NEW,
        READY,
        RUNNING,
        BLOCKED, 
        TERMINATED,
    };

    // getters
    unsigned getId() const { return myID; }
    Thread* getThread() const { return myThread; }
    ThreadState getThreadState() const { return threadState; }
    StackSize getStackSize() const { return stackSize; }
    Time getTimeSlice() const { return timeSlice; }
    static PCB* getPCBById(ID id);

    void start();
    void waitToComplete();

protected: 
    Thread* myThread;
    PCB* parent;
    List<PCB*> waitingForMe;

    unsigned *stack;
    StackSize stackSize; // not full size of the stack, full stack size is stackSize * sizeof(unsigned)
    volatile unsigned ss;
    volatile unsigned sp;
    volatile unsigned bp;

    Time timeSlice;
    ThreadState threadState;
    ID myID;
    static ID currentID;
    unsigned myLOCK;

    bool unblockedBySignal; // not by time
    bool isIdle;

    void initializeStack(pFunction fp);
    static void runWrapper();

private:  
    friend class Timer;
    friend class Thread;
    friend class OS;
    friend class KernelSem;
    friend class KernelEv;
    friend class List;
};

#endif
