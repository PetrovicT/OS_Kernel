#ifndef OS_H
#define OS_H

#include <iostream.h>
#include "list.h"
#include <dos.h>
#include "idlePCB.h"
#include "timerR.h"

class OS {
public:
    static void initialize();
    static void restore();
    static void cleanMemory();

private:
	OS();
	~OS();
};

#endif
