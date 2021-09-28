#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

// you must choose HUGE memory model 
// add APLICAT.LIB to the project

class PCB;

class Scheduler{
public:
	static void put (PCB *);
	static PCB* get ();
};

#endif