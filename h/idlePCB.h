#ifndef IDLEPCB_H
#define IDLEPCB_H

#include "PCB.h"

class IdlePCB : public PCB {
public:
    IdlePCB();
private:
    static void run();
};

#endif
