// OracleClient.cpp : Defines the exported functions for the DLL application.
//

#include <stdafx.h>
#include <OracleClient.h>
using namespace DatabaseUtilities;
TRANSACTION_RESULT_TYPE OracleClient::GetInitStatus()
{
	return mInitStat;
}
void OracleClient::Release()
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
	if(mEnv != NULL)
	{
		Environment::terminateEnvironment(mEnv);
		mEnv=NULL;
	}
	mInitStat = UNKNOWN_EXCEPTION;
}
Environment* OracleClient::GetEnvironment() const
{
	return mEnv;
}
TRANSACTION_RESULT_TYPE OracleClient::Init()
{
	try
	{
		mEnv = Environment::createEnvironment(Environment::OBJECT);
		logger = new Log("./Log/", "OracleClientRunTimeLog.log", 1024, true, 100);
		MarketDataTypeMap(mEnv);
		mConn = mEnv->createConnection(mUser, mPwd, mDb);
		mStat = mConn->createStatement();
		mCacheUsed = 0;
		mSyncSize = mCacheSize>>1;
		mActivedBuffer = 1;
		mBuffer[0].reserve(mCacheSize);
		mBuffer[1].reserve(mCacheSize);
		mBufferNo1.reserve(mCacheSize);
		mBufferNo2.reserve(mCacheSize);
		logger->LogThisAdvance("oracle client connected", LOG_INFO);
		mDestroyCommitThread = false;
		mInitStat = TRANS_NO_ERROR;
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

TRANSACTION_RESULT_TYPE OracleClient::InsertData(string aTableName, MarketDataType* aObj)
{
	try
	{
		boost::lock_guard<boost::mutex> lLockGuard(mOpMutex);
		MarketData tempObj(aTableName, aObj);
		
		if(mActivedBuffer == 1)
		{
			mBufferNo1.push_back(tempObj);
			cout<<"buffer: "<<mActivedBuffer<<"; size: "<<mBufferNo1.size()<<endl;
			if(mBufferNo1.size()>=mCacheSize && mCommitFinished)
			{
				mActivedBuffer = 2;
				mCommitThreadCV.notify_one();
				cout<<"notify commit task"<<endl;
			}
		}
		else if(mActivedBuffer == 2)
		{
			mBufferNo2.push_back(tempObj);
			cout<<"buffer: "<<mActivedBuffer<<"; size: "<<mBufferNo2.size()<<endl;
			if(mBufferNo2.size()>=mCacheSize && mCommitFinished)
			{
				mActivedBuffer = 1;
				mCommitThreadCV.notify_one();
				cout<<"notify commit task"<<endl;
			}
		}
		
		//if(mBuffer[mActivedBuffer].size()>=mCacheSize && mCommitFinished)
		//{
		//	mActivedBuffer ^= 1;
		//	mCommitThreadCV.notify_one();
		//	cout<<"notify commit task"<<endl;
		//}
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
		if(mCommitFinished)
		{
			boost::lock_guard<boost::mutex> lLockGuard(mOpMutex);
			mActivedBuffer ^= 1;
			mCommitThreadCV.notify_one();
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
		boost::lock_guard<boost::mutex> lLockGuard(mOpMutex);
		if(mBuffer[mActivedBuffer].size()>=mCacheSize && mCommitFinished)
		{
			mActivedBuffer ^= 1;
			mCommitThreadCV.notify_one();
		}
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
	Init();
	boost::unique_lock<boost::mutex> lCommitTaskLock(mCommitThreadMutex);
	while(mDestroyCommitThread == false)
	{
		try
		{
			mCommitFinished = true;
			// wait for mMaxCommitPeriod seconds before time out and commit, or wait for waking up from external
			if( 0 == mMaxCommitPeriod ) 
			{
				mCommitThreadCV.wait(lCommitTaskLock);
			}
			else 
			{
				mCommitThreadCV.wait_for(lCommitTaskLock, boost::chrono::seconds(mMaxCommitPeriod));
			}
			mCommitFinished = false;
			if(mActivedBuffer == 2)
			{
				if(mBufferNo1.size()>0)
				{
					cout<<"update buffer 1"<<endl;
					cout<<"commit"<<endl;
					mBufferNo1.clear();
				}
			}
			else if(mActivedBuffer == 1)
			{
				if(mBufferNo2.size()>0)
				{
					cout<<"update buffer 1"<<endl;
					cout<<"commit"<<endl;
					mBufferNo2.clear();
				}
			}
			//if(mBuffer[lCommitBuf].size()>0)
			//{
				//boost::posix_time::ptime startTime;
				//boost::posix_time::ptime endTime;
				//boost::posix_time::time_duration duration;
				//startTime = boost::posix_time::microsec_clock::local_time();
				//for(vector<MarketData>::iterator it = mBuffer[lCommitBuf].begin(); it!=mBuffer[lCommitBuf].end(); it++)
				//{
				//	cout<<"update buffer"<<lCommitBuf<<endl;
				//	mStat->setSQL("INSERT INTO " + it->mTableName + " VALUES (:1)");
				//	mStat->setObject(1, &it->mPayload);
				//	mStat->executeUpdate();
				//}
				//cout<<"update buffer "<<lCommitBuf<<endl;
				//cout<<"commit"<<endl;
				//mConn->commit();
				//endTime = boost::posix_time::microsec_clock::local_time();
				//duration = endTime-startTime;
				//cout<<"10000 times execute update takes "<<duration.total_milliseconds()<<" ms"<<endl;
				//mBuffer[lCommitBuf].clear();
			//}
		}
		catch(SQLException ex)
		{
			std::stringstream tempStream;
			tempStream.str("");	
			tempStream<<"exception in CommitTask(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
			cout<<tempStream.str()<<endl;
			logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		}
		catch(...)
		{
			std::stringstream tempStream;
			tempStream.str("");	
			tempStream<<"exception in CommitTask(), error message: unknown";
			cout<<tempStream.str()<<endl;
			logger->LogThisAdvance(tempStream.str(), LOG_ERROR);
		}
	}
	Release();// OCCI will commit when terminating connection
	
}