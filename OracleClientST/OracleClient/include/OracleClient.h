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
		boost::atomic<unsigned long long> mCacheUsed;
		unsigned long long mCacheSize;
		unsigned long long mSyncSize;
		//string mUser;
		//string mPwd;
		//string mDb;

	public:
		__declspec(dllexport) OracleClient()
		{
			mEnv = Environment::createEnvironment(Environment::OBJECT);
			logger = new Log("./Log/", "OracleClientRunTimeLog.log", 1024, true, 100);
		}
		__declspec(dllexport) virtual ~OracleClient()
		{
			try
			{
				Commit();
				Disconnect();
				if(mEnv != NULL)
				{
					Environment::terminateEnvironment(mEnv);
					mEnv=NULL;
				}
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
		__declspec(dllexport) virtual void Disconnect();
		__declspec(dllexport) virtual Environment* GetEnvironment() const;
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE Connect(string aUser, string aPwd, string aDb, unsigned long long aCacheSize=100000);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE CreateTableFromType(string aTableName, string aType);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE InsertData(string aTableName, PObject* aObj);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE QueryData(string aTableName, string aConstrain, unsigned int aRequiredSize, list<PObject*>& aObj, size_t& aCount);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE ExecuteSql(string aSqlStatement);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE TryCommit();
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE Commit();
		//__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE ConnectionStatus() = 0;
		//__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE Reconnect() = 0;
	};
}