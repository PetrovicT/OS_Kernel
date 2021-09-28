#include "OS.h"
#include "kSem.h"
#include "PCB.h"
#include "list.h"

PCB *mainPCB = nullptr;
PCB *running = nullptr;
PCB *idlePCB = nullptr;

void OS::initialize(){
    mainPCB = new PCB(0, defaultTimeSlice); // no need for stack
    mainPCB->threadState = PCB::RUNNING;
    mainPCB->parent = nullptr;
    running = mainPCB;

    idlePCB = new IdlePCB();    
    Timer::inic();
}

void OS::restore(){
    Timer::restore();
    OS::cleanMemory();
}

void OS::cleanMemory(){
	delete mainPCB;
	delete idlePCB;
}
