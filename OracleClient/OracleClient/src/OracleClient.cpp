// OracleClient.cpp : Defines the exported functions for the DLL application.
//

#include <stdafx.h>
#include <OracleClient.h>
using namespace DatabaseUtilities;
void OracleClient::Disconnect()
{
	if(mConn != NULL)
	{
		mConn->terminateStatement(mStat);
		mStat=NULL;
	}
	if(mConn != NULL)
	{
		mEnv->terminateConnection(mConn);
		mConn=NULL;
	}
}
Environment* OracleClient::GetEnvironment() const
{
	return mEnv;
}
TRANSACTION_RESULT_TYPE OracleClient::Connect(string aUser, string aPwd, string aDb, unsigned long long aCacheSize)
{
	try
	{
		mConn = mEnv->createConnection(aUser, aPwd, aDb);
		mStat = mConn->createStatement();
		mCacheSize = aCacheSize;
		mSyncSize = aCacheSize>>1;
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
		boost::lock_guard<boost::mutex> lLockGuard(mOpMutex);
		mStat->executeUpdate("CREATE TABLE "+ aTableName + " OF " + aType);
		mCacheUsed++;
		return TRANS_NO_ERROR;
	}
	catch(SQLException ex)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in CreateTableFromType(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in CreateTableFromType(), error message: unknown";
		cout<<tempStream.str()<<endl;
		logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		return UNKNOWN_EXCEPTION; 
	}
}

TRANSACTION_RESULT_TYPE OracleClient::ExecuteSql(string aSqlStatement)
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
		boost::lock_guard<boost::mutex> lLockGuard(mOpMutex);
		mStat->setSQL("INSERT INTO " + aTableName + " VALUES (:1)");
		mStat->setObject(1, aObj);
		mStat->executeUpdate();
		mCacheUsed++;
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
	return TryCommit();
	//return TRANS_NO_ERROR;
}
TRANSACTION_RESULT_TYPE OracleClient::QueryData(string aTableName, string aConstrain, unsigned int aRequiredSize, list<PObject*>& aObj, size_t& aCount)
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
		boost::lock_guard<boost::mutex> lLockGuard(mOpMutex);
		mConn->commit();
		mCacheUsed = 0;
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
		if(mCacheUsed > mSyncSize)
		{
			/* signal the auto commit thread */
			Commit();
		}// if the used cache size is bigger than sync size, signal the auto commit thread
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
