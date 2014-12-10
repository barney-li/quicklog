#pragma once
#include <Log.h>
using namespace std;
using namespace Utilities;
class CommonLog : public Log
{
private:
	static vector<CommonLog*> CommonLog::mLoggerList;
	static boost::thread* CommonLog::mAutoSyncThread;
	static bool CommonLog::mDestroyAutoSyncThread;
public:
	static void AutoSync(int aPeriod)
	{
		vector<CommonLog*>::iterator lObjIter;
		while(!mDestroyAutoSyncThread)
		{
			for(lObjIter = mLoggerList.begin(); lObjIter!=mLoggerList.end(); lObjIter++)
			{
				(*lObjIter)->TrySync();//要加括号，否则->的优先级在先
			}
			boost::this_thread::sleep(boost::posix_time::seconds(aPeriod));
		}
	}
	CommonLog(string aLogName = "log.log"):Log("data/Log/", aLogName, 32)
	{
		mLoggerList.push_back(this);
		const int lAutoSyncPeriod = 60;
		if(mAutoSyncThread == NULL)
		{
			mDestroyAutoSyncThread = false;
			mAutoSyncThread = new boost::thread(AutoSync, lAutoSyncPeriod);
		}	
	}
	void Release()
	{
		mDestroyAutoSyncThread = true;
		mAutoSyncThread = NULL;
	}
};