#pragma once
#include <string>
#include <fstream>
#include <boost/thread.hpp>
using namespace std;
namespace Utilities
{
	enum LOG_OPS_STATUS
	{
		LOG_NO_ERROR,
		OPEN_FILE_FAILED,
		WRITE_FILE_FAILED,
		CLOSE_FILE_FAILED,
		WRITE_BUFFER_FAILED
	};
	class Log
	{
	private:
		fstream logFileHandler;
		string logDir;
		string logName;
		unsigned int syncSize;
		int autoSyncPeriod;
		string bufferNo1;
		string bufferNo2;
		int bufferIndex;
		boost::thread* autoSyncThread;
		bool endAutoSyncThread;
	public:
		__declspec(dllexport) Log(string aLogDir, string aLogName, int aSyncSize, bool aCreateAutoSyncThread=false, int aAutoSyncPeriod=10)
		{
			logDir = aLogDir;
			logName = aLogName;
			syncSize = aSyncSize;
			autoSyncPeriod = aAutoSyncPeriod;
			Init();
		}
		__declspec(dllexport) virtual ~Log(void)
		{
			// end auto sync thread by setting the loop condition
			endAutoSyncThread = true;
			// sync all the buffered data
			Sync();
			CloseLogFile();
		}
		// write message into file immediately
		__declspec(dllexport) LOG_OPS_STATUS LogThis(string message, bool enter = true);
		// write message into file immediately without a time stamp
		__declspec(dllexport) LOG_OPS_STATUS LogThisNoTimeStamp(string message, bool enter = true);
		// write message into buffer and return, it will be synced to disk by a background task
		__declspec(dllexport) LOG_OPS_STATUS LogThisFast(string message, bool enter = true);
		// write message into buffer and return without a time stamp, it will be synced to disk by a background task
		__declspec(dllexport) LOG_OPS_STATUS LogThisFastNoTimeStamp(string message, bool enter = true);
		// sync the buffer to disk immediately
		__declspec(dllexport) LOG_OPS_STATUS Sync(void);
		// sync the buffer to disk if necessary
		__declspec(dllexport) LOG_OPS_STATUS TrySync(void);
		// set log file name
		__declspec(dllexport) void SetLogFile(string aDir, string aFileName);
	
	private:
		// open log file
		__declspec(dllexport) LOG_OPS_STATUS OpenLogFile(void);
		// close log file
		__declspec(dllexport) LOG_OPS_STATUS CloseLogFile(void);
		// monitor the buffer in the background, if there are any data need to be synced, it will sync them.
		static __declspec(dllexport) void AutoSync(Log* logger);
		__declspec(dllexport) void Init(bool aCreateAutoSyncThread=false)
		{
			bufferNo1.reserve(syncSize*2);
			bufferNo2.reserve(syncSize*2);
			bufferIndex = 1;
			if(aCreateAutoSyncThread)
			{
				endAutoSyncThread = false;
				autoSyncThread = new boost::thread(AutoSync, this);
			}
		}
	};
}