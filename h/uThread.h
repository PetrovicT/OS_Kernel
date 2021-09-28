#ifndef _UTHREAD_H
#define _UTHREAD_H

#include "thread.h"
#include "define.h"
#include <iostream.h>

class UserThread : public Thread {
public:

    UserThread(int argcx, char** argvx);
    virtual ~UserThread();

    virtual void run();
    int getRetVal();

private:
    int argc;
    char** argv;
    int returnValue;
};

#endif
