#pragma once
#include "Log.h"
#include <list>
#include <boost/thread.hpp>
class InstrumentLog : public Log
{
private:
	static list<InstrumentLog*> mLoggerList;
	static boost::thread* mAutoSyncThread;
	static void AutoSync(void)
	{
		list<InstrumentLog*>::iterator lObjIter;
		for(lObjIter = mLoggerList.begin(); lObjIter!=mLoggerList.end(); lObjIter++)
		{
			*lObjIter
		}
	}
public:
	InstrumentLog(string aInstrumentName):Log("./data/MarketData/", aInstrumentName+".dat", 32, 10)
	{
		mLoggerList.push_front(this);
		if(mAutoSyncThread == NULL)
		{
			
		}
	}
};