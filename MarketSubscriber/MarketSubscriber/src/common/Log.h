#pragma once
#include <string>
#include <fstream>
#include <boost/thread.hpp>
using namespace std;
//#define FAST_SYSTEM
class Log
{
private:
	fstream logFileHandler;
	string logDir;
	string logName;
	int syncSize;
	//int autoSyncPeriod;
	string bufferNo1;
	string bufferNo2;
	int bufferIndex;
	//boost::thread* autoSyncThread;
	//bool endAutoSyncThread;
public:
	Log(string aLogDir, string aLogName, int aSyncSize)
	{
		logDir = aLogDir;
		logName = aLogName;
		syncSize = aSyncSize;
		Init();
	}
	virtual ~Log(void)
	{
		// end auto sync thread by setting the loop condition
		//endAutoSyncThread = true;
		// sync all the buffered data
		Sync();
		if(logFileHandler.is_open())
		{
			logFileHandler.close();
		}
	}
	// write message into file immediately
	void LogThis(const char* message, bool enter = true);
	// write message into file immediately without a time stamp
	void LogThisNoTimeStamp(const char* message, bool enter = true);
	// write message into buffer and return, it will be synced to disk by a background task
	void LogThisFast(string message, bool enter = true);
	// write message into buffer and return without a time stamp, it will be synced to disk by a background task
	void LogThisFastNoTimeStamp(string message, bool enter = true);
	// sync the buffer to disk immediately
	void Sync(void);
	// sync the buffer to disk if necessary
	void TrySync(void);
	// set log file name
	void SetLogFile(string aDir, string aFileName);
private:
	// monitor the buffer in the background, if there are any data need to be synced, it will sync them.
	//static void AutoSync(Log* logger);
	void Init(void)
	{
#ifndef FAST_SYSTEM
		bufferNo1.reserve(syncSize*2);
		bufferNo2.reserve(syncSize*2);
		bufferIndex = 1;
		//endAutoSyncThread = false;
		//autoSyncThread = new boost::thread(AutoSync, this);
#endif
	}
};