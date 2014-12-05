#pragma once
#include <stdexcept>
#include <occi.h>
#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <Log.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
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
	struct ConnectionPackage
	{
		ConnectionPackage(Connection* aConn, Statement* aStat, boost::mutex* aMutex)
		{
			mConn = aConn;
			mStat = aStat;
			mCacheSize = 0;
			mCacheUsed = 0;
			mSyncSize = 0;
		}
		ConnectionPackage()
		{
			mConn = NULL;
			mStat = NULL;
			mCacheSize = 0;
			mCacheUsed = 0;
			mSyncSize = 0;
		}
		~ConnectionPackage()
		{
			mConn = NULL;
			mStat = NULL;
			mCacheSize = 0;
			mCacheUsed = 0;
			mSyncSize = 0;
		}
		Connection* mConn;
		Statement* mStat;
		boost::mutex mMutex;
		unsigned long long mCacheUsed;
		unsigned long long mCacheSize;
		unsigned long long mSyncSize;
	};
	class OracleClient
	{
	private:
		// OCCI environment
		Environment* mEnv;
		// represent the current actived connection
		ConnectionPackage mConnPkg[2];
		// indicate the actived connection index in ConnectionPackage
		unsigned int mActivedConn;
		// logger
		Log* logger;
		// to protect mActivedConn 
		boost::mutex mActivedConnMutex;
		// to synchronize commit thread
		boost::condition_variable mCommitThreadCV;
		// to protect commit thread
		boost::mutex mCommitThreadMutex;
		// commit thread
		boost::thread* mCommitThread;
		// destroy commit thread flag
		boost::atomic<bool> mDestroyCommitThread;
		// max commit period
		unsigned long long mMaxCommitPeriod;
		// to protect commit operation
		boost::mutex mCommitMutex;

		// get actived connection index
		__declspec(dllexport) unsigned int GetActConnIdx();
		// get deactived connection index
		__declspec(dllexport) unsigned int GetDactConnIdx();
		// switch actived connection to another
		__declspec(dllexport) void SwitchActivedConnection();
		// commit thread
		__declspec(dllexport) void CommitTask();

	public:
		__declspec(dllexport) OracleClient()
		{
			mEnv = Environment::createEnvironment(Environment::OBJECT);
			mActivedConn = 0;
			mDestroyCommitThread = false;
			mMaxCommitPeriod = 0;
			mCommitThread = new boost::thread(boost::bind(&OracleClient::CommitTask, this));
			logger = new Log("./Log/", "OracleClientRunTimeLog.log", 1024, true, 100);

		}
		__declspec(dllexport) virtual ~OracleClient()
		{
			mDestroyCommitThread = true;
			mCommitThreadCV.notify_one();// make sure the commit thread pass the wait
			mCommitThread->join();// wait for the thread to finish
			Disconnect();// OCCI will commit when terminating connection
			if(mEnv != NULL)
			{
				Environment::terminateEnvironment(mEnv);
				mEnv=NULL;
			}
			if(logger != NULL)
			{
				delete logger;
				logger = NULL;
			}
		}
		__declspec(dllexport) virtual Environment* GetEnvironment() const;
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE Connect(string aUser, string aPwd, string aDb, unsigned long long aCacheSize=100000, unsigned long long aMaxCommitPeriod=10);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE Disconnect();
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE CreateTableFromType(string aTableName, string aType);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE InsertData(string aTableName, PObject* aObj);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE QueryData(string aTableName, string aConstrain, unsigned int aRequiredSize, list<PObject*>& aObj, size_t& aCount);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE ExecuteSql(string aSqlStatement);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE TryCommit();
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE Commit();
	};
}