#include <iostream.h>
#include <stdio.h>
#include "OS.h"
#include "define.h"
#include <dos.h>
#include <stddef.h>
#include "uThread.h"

int main(int argc, char* argv[]){

    cout << endl << "Initialize OS" << endl;
	#ifndef BCC_BLOCK_IGNORE
	DISABLE_INTERRUPT
	#endif
	OS::initialize();
    UserThread *userThr = new UserThread(argc, argv);
	#ifndef BCC_BLOCK_IGNORE
	ALLOW_INTERRUPT;
	#endif

	userThr->start();
    userThr->waitToComplete();
    int retCode = userThr->getRetVal();

    OS::restore();
    cout << endl << "Finish of userMain() with the return code: " << retCode << endl;
    cout << "Restoring OS - end" << endl;

    delete userThr;
    return 0;
}
