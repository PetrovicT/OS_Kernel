#ifndef IVTENTRY_H
#define IVTENTRY_H

#include "define.h"
#include "kEvent.h"

typedef unsigned char IVTNo;
#define IVTsize 256
class KernelEv;


#define PREPAREENTRY(IVTnumEntry, callOld)  								\
																			\
void interrupt interruptEvent##IVTnumEntry(...);          					\
IVTEntry newEntry##IVTnumEntry(IVTnumEntry, interruptEvent##IVTnumEntry);	\
																			\
void interrupt interruptEvent##IVTnumEntry(...){							\
	newEntry##IVTnumEntry.signal();											\
	if (callOld == 1) {														\
		newEntry##IVTnumEntry.pOldInterrupt();								\
	}																		\
	dispatch();																\
}


class IVTEntry{
public:
	IVTEntry(IVTNo _IVTnumEntry, pInterrupt pNewInterrupt);
	~IVTEntry();

	static IVTEntry* IVT[IVTsize];
	static IVTEntry* getIVTEntry(IVTNo _ivtno);
	void signal();
	void callOldRoutine();
	void setEvent(KernelEv* myEvent);
	void clearEvent();

	pInterrupt pNewInterrupt;
	pInterrupt pOldInterrupt;
	KernelEv* myKernelEvent;
protected:

private:
	IVTNo IVTnumEntry;
};

#endif
