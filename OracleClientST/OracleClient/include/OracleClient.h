#pragma once
//#include <stdexcept>
#include <occi.h>
#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <Log.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <vector>
#include "MarketDataType.h"
#include "MarketDataTypeMap.h"
using namespace Utilities;
using namespace oracle::occi;
using namespace std;
namespace DatabaseUtilities
{
	enum TRANSACTION_RESULT_TYPE
	{
		TRANS_NO_ERROR,
		CAN_NOT_REACH_DATABASE,
		SQL_EXCEPTION,
		QUERY_LIST_FULL,
		UNKNOWN_EXCEPTION
	};
	struct MarketData
	{
		MarketData(string aTableName, MarketDataType* aPayload)
		{
			mTableName = aTableName;
			//memcpy(&mPayload, aPayload, sizeof(mPayload));
			mPayload = *aPayload;
		}
		MarketData()
		{
			;
		}
		string mTableName;
		MarketDataType mPayload;
	};
	class OracleClient
	{
	private:
		// OCCI environment
		Environment* mEnv;
		// logger
		Log* logger;
		Connection* mConn;
		Statement* mStat;
		boost::mutex mOpMutex;
		unsigned long long mCacheUsed;
		unsigned long long mCacheSize;
		unsigned long long mSyncSize;
		// double buffer
		vector<MarketData> mBuffer[2];
		// indicate the actived buffer index
		boost::atomic<unsigned int> mActivedBuffer;
		// commit thread
		boost::thread* mCommitThread;
		// destroy commit thread flag
		boost::atomic<bool> mDestroyCommitThread;
		// to synchronize commit thread
		boost::condition_variable mCommitThreadCV;
		// to protect commit thread
		boost::mutex mCommitThreadMutex;
		// max commit period
		unsigned long long mMaxCommitPeriod;
		// ID for login
		string mUser;
		// password for login
		string mPwd;
		// database address
		string mDb;
		// initialize statue
		TRANSACTION_RESULT_TYPE mInitStat;
		// commit accomplish indicator
		boost::atomic<bool> mCommitFinished;
		// commit thread
		__declspec(dllexport) void CommitTask();
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE Init();
		__declspec(dllexport) virtual void Release();

	public:
		__declspec(dllexport) OracleClient(string aUser, string aPwd, string aDb, unsigned long long aCacheSize=100000, unsigned long long aMaxCommitPeriod=10)
		{
			mUser = aUser;
			mPwd = aPwd;
			mDb = aDb;
			mCacheSize = aCacheSize;
			mMaxCommitPeriod = aMaxCommitPeriod;
			mDestroyCommitThread = false;
			mInitStat = UNKNOWN_EXCEPTION;
			mCommitThread = new boost::thread(boost::bind(&OracleClient::CommitTask, this));
			boost::this_thread::yield();
		}
		__declspec(dllexport) virtual ~OracleClient()
		{
			try
			{
				mDestroyCommitThread = true;
				mCommitThreadCV.notify_one();// make sure the commit thread pass the wait
				mCommitThread->join();// wait for the thread to finish
			}
			catch(SQLException ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in ~OracleClient(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
				cout<<tempStream.str()<<endl;
				logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
			}
			catch(...)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in ~OracleClient(), error message: unknown";
				cout<<tempStream.str()<<endl;
				logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
			}
			if(logger != NULL)
			{
				delete logger;
				logger = NULL;
			}
		}
		__declspec(dllexport) virtual Environment* GetEnvironment() const;
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE CreateTableFromType(string aTableName, string aType);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE InsertData(string aTableName, MarketDataType* aObj);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE QueryData(string aTableName, string aConstrain, unsigned int aRequiredSize, list<PObject*>& aObj, size_t& aCount);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE ExecuteSql(string aSqlStatement);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE TryCommit();
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE Commit();
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE GetInitStatus();
	};
}