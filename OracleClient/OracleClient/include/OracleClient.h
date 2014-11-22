#pragma once
#include <stdexcept>
#include <occi.h>
#include <iostream>
#include <string>
#include <sstream>
#include <list>
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
		
	public:
		OracleClient()
		{
			mEnv = Environment::createEnvironment(Environment::OBJECT);
			mConn=NULL;
			mStat=NULL;
		}
		virtual ~OracleClient()
		{
			Environment::terminateEnvironment(mEnv);
		}
		virtual void Disconnect();
		virtual Environment* GetEnvironment() const;
		virtual TRANSACTION_RESULT_TYPE Connect(string aUser, string aPwd, string aDb, unsigned int aCacheSize=100000);
		virtual TRANSACTION_RESULT_TYPE CreateTable(string aTableName);
		virtual TRANSACTION_RESULT_TYPE InsertData(string aTableName, PObject* aObj);
		virtual TRANSACTION_RESULT_TYPE QueryData(string aSqlStatement, list<PObject*> aObj, int aCount);
		virtual TRANSACTION_RESULT_TYPE ExecuteSql(string aSqlStatement);
	};
}