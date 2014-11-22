// OracleClient.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <OracleClient.h>
using namespace DatabaseUtilities;
void OracleClient::Disconnect()
{
	if(mStat != NULL)
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
TRANSACTION_RESULT_TYPE OracleClient::Connect(string aUser, string aPwd, string aDb, unsigned int aCacheSize)
{
	try
	{
		mConn = mEnv->createConnection(aUser, aPwd, aDb);
		mConn->setStmtCacheSize(aCacheSize);
		mStat = mConn->createStatement();
		return TRANS_NO_ERROR;
	}
	catch(SQLException ex)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in CreateConnection(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
		cout<<tempStream.str()<<endl;
		return SQL_EXCEPTION;
	}
	catch(...)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in CreateConnection(), error message: unknown";
		cout<<tempStream.str()<<endl;
		return UNKNOWN_EXCEPTION; 
	}
}

TRANSACTION_RESULT_TYPE OracleClient::CreateTable(string aTableName)
{
	;
}
