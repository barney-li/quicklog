// OracleClient.cpp : Defines the exported functions for the DLL application.
//

#include <stdafx.h>
#include <OracleClient.h>
using namespace DatabaseUtilities;
void OracleClient::Disconnect()
{
	if(mConn != NULL && mStat != NULL)
	{
		mConn->terminateStatement(mStat);
		mStat=NULL;
	}
	if(mConn != NULL && mEnv != NULL)
	{
		mEnv->terminateConnection(mConn);
		mConn=NULL;
	}
}
Environment* OracleClient::GetEnvironment() const
{
	return mEnv;
}
TRANSACTION_RESULT_TYPE OracleClient::Connect(string aUser, string aPwd, string aDb, unsigned long long aCacheSize, int& aErrCode, string& aErrMsg)
{
	try
	{
		mConn = mEnv->createConnection(aUser, aPwd, aDb);
		mStat = mConn->createStatement();
		mCacheSize = aCacheSize;
		mSyncSize = aCacheSize>>1;
		return TRANS_NO_ERROR;
	}
	catch(SQLException ex)
	{
		aErrCode = ex.getErrorCode();
		aErrMsg = ex.getMessage();
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		aErrCode = -1;
		aErrMsg = "unknown exception";
		return UNKNOWN_EXCEPTION; 
	}
}

TRANSACTION_RESULT_TYPE OracleClient::CreateTableFromType(string aTableName, string aType, int& aErrCode, string& aErrMsg)
{
	try
	{
		boost::lock_guard<boost::mutex> lLockGuard(mOpMutex);
		mStat->executeUpdate("CREATE TABLE "+ aTableName + " OF " + aType);
		mCacheUsed++;
		return TRANS_NO_ERROR;
	}
	catch(SQLException ex)
	{
		aErrCode = ex.getErrorCode();
		aErrMsg = ex.getMessage();
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		aErrCode = -1;
		aErrMsg = "unknown exception";
		return UNKNOWN_EXCEPTION; 
	}
}

TRANSACTION_RESULT_TYPE OracleClient::ExecuteSql(string aSqlStatement, int& aErrCode, string& aErrMsg)
{
	try
	{
		boost::lock_guard<boost::mutex> lLockGuard(mOpMutex);
		mStat->execute(aSqlStatement);
		mCacheUsed++;
		return TRANS_NO_ERROR;
	}
	catch(SQLException ex)
	{
		aErrCode = ex.getErrorCode();
		aErrMsg = ex.getMessage();
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		aErrCode = -1;
		aErrMsg = "unknown exception";
		return UNKNOWN_EXCEPTION; 
	}
}

TRANSACTION_RESULT_TYPE OracleClient::InsertData(string aTableName, PObject* aObj, int& aErrCode, string& aErrMsg)
{
	try
	{
		boost::lock_guard<boost::mutex> lLockGuard(mOpMutex);
		mStat->setSQL("INSERT INTO " + aTableName + " VALUES (:1)");
		mStat->setObject(1, aObj);
		mStat->executeUpdate();
		mCacheUsed++;
	}
	catch(SQLException ex)
	{
		aErrCode = ex.getErrorCode();
		aErrMsg = ex.getMessage();
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		aErrCode = -1;
		aErrMsg = "unknown exception";
		return UNKNOWN_EXCEPTION; 
	}
	return TryCommit(aErrCode, aErrMsg);
}
TRANSACTION_RESULT_TYPE OracleClient::QueryData(string aTableName, string aConstrain, unsigned int aRequiredSize, list<PObject*>& aObj, size_t& aCount, int& aErrCode, string& aErrMsg)
{
	try
	{
		unsigned int lColIndex=1;
		boost::lock_guard<boost::mutex> lLockGuard(mOpMutex);
		oracle::occi::ResultSet* lResultSet;
		if(aConstrain.empty())
		{
			lResultSet = mStat->executeQuery("SELECT * FROM "+aTableName);
		}
		else
		{
			lResultSet = mStat->executeQuery("SELECT * FROM "+aTableName+" "+aConstrain); 
		}
		while( (lResultSet->next() != ResultSet::END_OF_FETCH) && (lColIndex<=aRequiredSize) )
		{
			aObj.push_back(lResultSet->getObject(lColIndex++));
		}
		aCount = aObj.size();
		return TRANS_NO_ERROR;
	}
	catch(SQLException ex)
	{
		aErrCode = ex.getErrorCode();
		aErrMsg = ex.getMessage();
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		aErrCode = -1;
		aErrMsg = "unknown exception";
		return UNKNOWN_EXCEPTION; 
	}
}
TRANSACTION_RESULT_TYPE OracleClient::Commit(int& aErrCode, string& aErrMsg)
{
	try
	{
		boost::lock_guard<boost::mutex> lLockGuard(mOpMutex);
		if(mCacheUsed>0)
		{
			mConn->commit();
			mCacheUsed = 0;
		}
		return TRANS_NO_ERROR;
	}
	catch(SQLException ex)
	{
		aErrCode = ex.getErrorCode();
		aErrMsg = ex.getMessage();
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		aErrCode = -1;
		aErrMsg = "unknown exception";
		return UNKNOWN_EXCEPTION; 
	}
}

TRANSACTION_RESULT_TYPE OracleClient::TryCommit(int& aErrCode, string& aErrMsg)
{
	try
	{
		if(mCacheUsed > mSyncSize)
		{
			Commit(aErrCode, aErrMsg);
		}// if the used cache size is bigger than sync size, signal the auto commit thread
		return TRANS_NO_ERROR;
	}
	catch(SQLException ex)
	{
		aErrCode = ex.getErrorCode();
		aErrMsg = ex.getMessage();
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		aErrCode = -1;
		aErrMsg = "unknown exception";
		return UNKNOWN_EXCEPTION; 
	}
}
