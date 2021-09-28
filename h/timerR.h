#ifndef TIMERR_H
#define TIMERR_H

#include "define.h"
#include "SCHEDULE.H"
#include <dos.h>
#include <iostream.h>       
#include "idlePCB.h"


class Timer{
public:
	static void inic(); // initialize new timer interrupt
    static void restore(); // restore old timer interrupt

    static pInterrupt oldTimerInterrupt;
    static void interrupt timer(...);

private:
    Timer() {}
	~Timer() {}
};

#endif
