#include "uThread.h"

int userMain(int argc, char* argv[]);

UserThread::UserThread(int argcx, char** argvx) : Thread (defaultStackSize, 2){
    argc = argcx;
    argv = argvx;
    returnValue = 0;
}

UserThread::~UserThread(){
    waitToComplete();
}

void UserThread::run(){
	returnValue = userMain(argc, argv);
}

int UserThread::getRetVal(){
	return returnValue;
}
