#pragma once
#include "Log.h"
#include <list>
#include <boost/thread.hpp>
class InstrumentLog : public Log
{
private:
	static list<InstrumentLog*> mLoggerList;
	static boost::thread* mAutoSyncThread;
	static bool mDestroyAutoSyncThread;
public:
	static void AutoSync(int aPeriod)
	{
		list<InstrumentLog*>::iterator lObjIter;
		while(!mDestroyAutoSyncThread)
		{
			for(lObjIter = mLoggerList.begin(); lObjIter!=mLoggerList.end(); lObjIter++)
			{
				(*lObjIter)->TrySync();//要加括号，否则->的优先级在先
			}
			boost::this_thread::sleep(boost::posix_time::seconds(aPeriod));
		}
	}
public:
	InstrumentLog(string aInstrumentName):Log("./data/MarketData/", aInstrumentName+".dat", 32)
	{
		mLoggerList.push_front(this);
		const int lAutoSyncPeriod = 10;
		if(mAutoSyncThread == NULL)
		{
			mDestroyAutoSyncThread = false;
			mAutoSyncThread = new boost::thread(AutoSync, lAutoSyncPeriod);
		}
	}
	~InstrumentLog()
	{
	}
	void Release()
	{
		mDestroyAutoSyncThread = true;
		mAutoSyncThread = NULL;
	}
};
