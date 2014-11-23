// OracleClient.cpp : Defines the exported functions for the DLL application.
//

#include <stdafx.h>
#include <OracleClient.h>
using namespace DatabaseUtilities;
void OracleClient::Disconnect()
{
	if(mConnPkg[0].mStat != NULL)
	{
		mConnPkg[0].mConn->terminateStatement(mConnPkg[0].mStat);
		mConnPkg[0].mStat=NULL;
	}
	if(mConnPkg[0].mConn != NULL)
	{
		mEnv->terminateConnection(mConnPkg[0].mConn);
		mConnPkg[0].mConn=NULL;
	}
	if(mConnPkg[1].mStat != NULL)
	{
		mConnPkg[1].mConn->terminateStatement(mConnPkg[1].mStat);
		mConnPkg[1].mStat=NULL;
	}
	if(mConnPkg[1].mConn != NULL)
	{
		mEnv->terminateConnection(mConnPkg[1].mConn);
		mConnPkg[1].mConn=NULL;
	}
}
Environment* OracleClient::GetEnvironment() const
{
	return mEnv;
}
unsigned int OracleClient::GetActConnIdx()
{
	boost::lock_guard<boost::mutex> lLockGuard(mActivedConnMutex);
	return mActivedConn;
}
unsigned int OracleClient::GetDactConnIdx()
{
	boost::lock_guard<boost::mutex> lLockGuard(mActivedConnMutex);
	return mActivedConn^1;
}
void OracleClient::SwitchActivedConnection()
{
	boost::lock_guard<boost::mutex> lLockGuard(mActivedConnMutex);
	if(0 == mActivedConn)
	{
		mActivedConn = 1;
	}
	else
	{
		mActivedConn = 0;
	}
}

TRANSACTION_RESULT_TYPE OracleClient::Connect(string aUser, string aPwd, string aDb, unsigned int aCacheSize)
{
	try
	{
		mConnPkg[0].mConn = mEnv->createConnection(aUser, aPwd, aDb);
		mConnPkg[0].mConn->setStmtCacheSize(aCacheSize);
		mConnPkg[0].mStat = mConnPkg[0].mConn->createStatement();
		mConnPkg[0].mCacheSize = aCacheSize;
		mConnPkg[0].mSyncSize = aCacheSize>>1;
		mConnPkg[1].mConn = mEnv->createConnection(aUser, aPwd, aDb);
		mConnPkg[1].mConn->setStmtCacheSize(aCacheSize);
		mConnPkg[1].mStat = mConnPkg[1].mConn->createStatement();
		mConnPkg[1].mCacheSize = aCacheSize;
		mConnPkg[1].mSyncSize = aCacheSize>>1;
		logger->LogThisAdvance("oracle client connected", LOG_INFO);
		return TRANS_NO_ERROR;
	}
	catch(SQLException ex)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in CreateConnection(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in CreateConnection(), error message: unknown";
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return UNKNOWN_EXCEPTION; 
	}
}

TRANSACTION_RESULT_TYPE OracleClient::CreateTableFromType(string aTableName, string aType)
{
	try
	{
		unsigned int lConnIndex = GetActConnIdx();
		boost::lock_guard<boost::mutex> lLockGuard(mConnPkg[lConnIndex].mMutex);
		mConnPkg[lConnIndex].mStat->executeUpdate("CREATE TABLE "+ aTableName + " OF " + aType);
		mConnPkg[lConnIndex].mCacheUsed++;
		return TRANS_NO_ERROR;
	}
	catch(SQLException ex)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in ExecuteSql(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in ExecuteSql(), error message: unknown";
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return UNKNOWN_EXCEPTION; 
	}
}

TRANSACTION_RESULT_TYPE OracleClient::ExecuteSql(string aSqlStatement)
{
	try
	{
		unsigned int lConnIndex = GetActConnIdx();
		boost::lock_guard<boost::mutex> lLockGuard(mConnPkg[lConnIndex].mMutex);
		mConnPkg[lConnIndex].mStat->execute(aSqlStatement);
		mConnPkg[lConnIndex].mCacheUsed++;
		return TRANS_NO_ERROR;
	}
	catch(SQLException ex)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in ExecuteSql(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in ExecuteSql(), error message: unknown";
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return UNKNOWN_EXCEPTION; 
	}
}

TRANSACTION_RESULT_TYPE OracleClient::InsertData(string aTableName, PObject* aObj)
{
	try
	{
		unsigned int lConnIndex = GetActConnIdx();
		boost::lock_guard<boost::mutex> lLockGuard(mConnPkg[lConnIndex].mMutex);
		mConnPkg[lConnIndex].mStat->setSQL("INSERT INTO " + aTableName + " VALUES (:1)");
		mConnPkg[lConnIndex].mStat->setObject(1, aObj);
		mConnPkg[lConnIndex].mStat->executeUpdate();
		mConnPkg[lConnIndex].mCacheUsed++;
		return TRANS_NO_ERROR;
	}
	catch(SQLException ex)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in InsertData(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in InsertData(), error message: unknown";
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return UNKNOWN_EXCEPTION; 
	}
}
TRANSACTION_RESULT_TYPE OracleClient::QueryData(string aTableName, string aConstrain, unsigned int aRequiredSize, list<PObject*>& aObj, unsigned int& aCount)
{
	try
	{
		int lColIndex=1;
		unsigned int lConnIndex = GetActConnIdx();
		boost::lock_guard<boost::mutex> lLockGuard(mConnPkg[lConnIndex].mMutex);
		oracle::occi::ResultSet* lResultSet;
		if(aConstrain.empty())
		{
			lResultSet = mConnPkg[lConnIndex].mStat->executeQuery("SELECT * FROM "+aTableName);
		}
		else
		{
			lResultSet = mConnPkg[lConnIndex].mStat->executeQuery("SELECT * FROM "+aTableName+" "+aConstrain); 
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
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in QueryData(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in QueryData(), error message: unknown";
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return UNKNOWN_EXCEPTION; 
	}
}
TRANSACTION_RESULT_TYPE OracleClient::Commit()
{
	try
	{
		unsigned int lConnIndex = GetDactConnIdx();
		boost::lock_guard<boost::mutex> lLockGuard(mConnPkg[lConnIndex].mMutex);
		if(mConnPkg[lConnIndex].mCacheUsed > 0)
		{
			mConnPkg[lConnIndex].mConn->commit();
		}
		return TRANS_NO_ERROR;
	}
	catch(SQLException ex)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in Commit(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in Commit(), error message: unknown";
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return UNKNOWN_EXCEPTION; 
	}
}

TRANSACTION_RESULT_TYPE OracleClient::TryCommit()
{
	try
	{
		unsigned int lConnIndex = GetActConnIdx();
		boost::lock_guard<boost::mutex> lLockGuard(mConnPkg[lConnIndex].mMutex);
		if(mConnPkg[lConnIndex].mCacheUsed > mConnPkg[lConnIndex].mSyncSize)
		{
			SwitchActivedConnection();
			/* signal the auto commit thread */
			mCommitThreadCV.notify_one();
		}// if the used cache size is bigger than sync size, signal the auto commit thread
		if(mConnPkg[lConnIndex].mCacheUsed >= mConnPkg[lConnIndex].mCacheSize)
		{
			SwitchActivedConnection();
			mConnPkg[lConnIndex].mConn->commit();
		}// if the used cache size is no less than total cache size, commit no matter what
		return TRANS_NO_ERROR;
	}
	catch(SQLException ex)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in TryCommit(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in TryCommit(), error message: unknown";
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return UNKNOWN_EXCEPTION; 
	}
}

void OracleClient::CommitTask()
{
	boost::unique_lock<boost::mutex> lCommitTaskLock(mCommitThreadMutex);
	while(mDestroyCommitThread == false)
	{
		// wait for 10 seconds before time out and commit, or wait for waking up by from external
		mCommitThreadCV.wait_for(lCommitTaskLock, boost::chrono::seconds(10));
		Commit();
	}
}