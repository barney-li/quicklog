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
#include <stdexcept>
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
		Environment* mEnv;
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
		}
		__declspec(dllexport) virtual ~OracleClient()
		{
			try
			{
				int lErrCode = 0;
				string lErrMsg;
				Commit(lErrCode, lErrMsg);
				Disconnect();
				if(mEnv != NULL)
				{
					Environment::terminateEnvironment(mEnv);
					mEnv=NULL;
				}
			}
			catch(SQLException ex)
			{
				throw new SQLException(ex);
			}
			catch(...)
			{
				throw new std::exception("unknown exception in ~OracleClient()");
			}
		}
		__declspec(dllexport) virtual void Disconnect();
		__declspec(dllexport) virtual Environment* GetEnvironment() const;
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE Connect(string aUser, string aPwd, string aDb, unsigned long long aCacheSize, int& aErrCode, string& aErrMsg);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE CreateTableFromType(string aTableName, string aType, int& aErrCode, string& aErrMsg);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE InsertData(string aTableName, PObject* aObj, int& aErrCode, string& aErrMsg);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE QueryData(string aTableName, string aConstrain, unsigned int aRequiredSize, list<PObject*>& aObj, size_t& aCount, int& aErrCode, string& aErrMsg);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE ExecuteSql(string aSqlStatement, int& aErrCode, string& aErrMsg);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE TryCommit(int& aErrCode, string& aErrMsg);
		__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE Commit(int& aErrCode, string& aErrMsg);
		//__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE ConnectionStatus() = 0;
		//__declspec(dllexport) virtual TRANSACTION_RESULT_TYPE Reconnect() = 0;
	};
}