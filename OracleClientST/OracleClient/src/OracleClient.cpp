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

TRANSACTION_RESULT_TYPE OracleClient::CreateMarketDataTable(string aTableName, int& aErrCode, string& aErrMsg)
{
	try
	{
		boost::lock_guard<boost::mutex> lLockGuard(mOpMutex);
		string lSqlStatement = "CREATE TABLE "+ aTableName +"(";

		lSqlStatement += " data_type_version number,";

		lSqlStatement += " time_stamp timestamp,";

		///������				TThostFtdcDateType	TradingDay;
		lSqlStatement += " trading_day varchar2(9),";	
				
		///��Լ����				TThostFtdcInstrumentIDType	InstrumentID;
		lSqlStatement += " instrument_id varchar2(11),";

		///����������			TThostFtdcExchangeIDType	ExchangeID;
		lSqlStatement += " exchange_id varchar2(9),";

		///��Լ�ڽ������Ĵ���	TThostFtdcExchangeInstIDType	ExchangeInstID;
		lSqlStatement += " exchange_instrument_id varchar2(31),";

		///���¼�				TThostFtdcPriceType	LastPrice;
		lSqlStatement += " last_price number,";				

		///�ϴν����			TThostFtdcPriceType	PreSettlementPrice;
		lSqlStatement += " pre_settlement_price number,";

		///������				TThostFtdcPriceType	PreClosePrice;
		lSqlStatement += " pre_close_price number,";

		///��ֲ���				TThostFtdcLargeVolumeType	PreOpenInterest;
		lSqlStatement += " pre_open_interest number,";

		///����				TThostFtdcPriceType	OpenPrice;
		lSqlStatement += " open_price number,";

		///��߼�				TThostFtdcPriceType	HighestPrice;
		lSqlStatement += " highest_price number,";

		///��ͼ�				TThostFtdcPriceType	LowestPrice;
		lSqlStatement += " lowest_price number,";

		///����					TThostFtdcVolumeType	Volume;
		lSqlStatement += " volume number,";

		///�ɽ����				TThostFtdcMoneyType	Turnover;
		lSqlStatement += " turnover number,";

		///�ֲ���				TThostFtdcLargeVolumeType	OpenInterest;
		lSqlStatement += " open_interest number,";

		///������				TThostFtdcPriceType	ClosePrice;
		lSqlStatement += " close_price number,";

		///���ν����			TThostFtdcPriceType	SettlementPrice;
		lSqlStatement += " settlement_price number,";

		///��ͣ���				TThostFtdcPriceType	UpperLimitPrice;
		lSqlStatement += " upper_limit_price number,";

		///��ͣ���				TThostFtdcPriceType	LowerLimitPrice;
		lSqlStatement += " lower_limit_price number,";

		///����ʵ��				TThostFtdcRatioType	PreDelta;
		lSqlStatement += " pre_delta number,";

		///����ʵ��				TThostFtdcRatioType	CurrDelta;
		lSqlStatement += " curr_delta number,";

		///����޸�ʱ��			TThostFtdcTimeType	UpdateTime;
		lSqlStatement += " update_time varchar2(9),";

		///����޸ĺ���			TThostFtdcMillisecType	UpdateMillisec;
		lSqlStatement += " update_millisec number,";

		///�����һ				TThostFtdcPriceType	BidPrice1;
		lSqlStatement += " bid_price_1 number,";

		///������һ				TThostFtdcVolumeType	BidVolume1;
		lSqlStatement += " bid_volume_1 number,";

		///������һ				TThostFtdcPriceType	AskPrice1;
		lSqlStatement += " ask_price_1 number,";

		///������һ				TThostFtdcVolumeType	AskVolume1;
		lSqlStatement += " ask_volume_1 number,";

		///����۶�				TThostFtdcPriceType	BidPrice2;
		lSqlStatement += " bid_price_2 number,";

		///��������				TThostFtdcVolumeType	BidVolume2;
		lSqlStatement += " bid_volume_2 number,";

		///�����۶�				TThostFtdcPriceType	AskPrice2;
		lSqlStatement += " ask_price_2 number,";

		///��������				TThostFtdcVolumeType	AskVolume2;
		lSqlStatement += " ask_volume_2 number,";

		///�������				TThostFtdcPriceType	BidPrice3;
		lSqlStatement += " bid_price_3 number,";

		///��������				TThostFtdcVolumeType	BidVolume3;
		lSqlStatement += " bid_volume_3 number,";

		///��������				TThostFtdcPriceType	AskPrice3;
		lSqlStatement += " ask_price_3 number,";

		///��������				TThostFtdcVolumeType	AskVolume3;
		lSqlStatement += " ask_volume_3 number,";

		///�������				TThostFtdcPriceType	BidPrice4;
		lSqlStatement += " bid_price_4 number,";

		///��������				TThostFtdcVolumeType	BidVolume4;
		lSqlStatement += " bid_volume_4 number,";

		///��������				TThostFtdcPriceType	AskPrice4;
		lSqlStatement += " ask_price_4 number,";

		///��������				TThostFtdcVolumeType	AskVolume4;
		lSqlStatement += " ask_volume_4 number,";

		///�������				TThostFtdcPriceType	BidPrice5;
		lSqlStatement += " bid_price_5 number,";

		///��������				TThostFtdcVolumeType	BidVolume5;
		lSqlStatement += " bid_volume_5 number,";

		///��������				TThostFtdcPriceType	AskPrice5;
		lSqlStatement += " ask_price_5 number,";

		///��������				TThostFtdcVolumeType	AskVolume5;
		lSqlStatement += " ask_volume_5 number,";

		///���վ���				TThostFtdcPriceType	AveragePrice;
		lSqlStatement += " average_price number,";

		///ҵ������				TThostFtdcDateType	ActionDay;
		lSqlStatement += " action_day varchar2(9)";

		lSqlStatement += ")";
		mStat->executeUpdate(lSqlStatement);
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

TRANSACTION_RESULT_TYPE OracleClient::InsertMarketData(string aTableName, CThostFtdcDepthMarketDataField* aData, oracle::occi::Timestamp aTimeStamp, int& aErrCode, string& aErrMsg)
{
	try
	{
		boost::lock_guard<boost::mutex> lLockGuard(mOpMutex);
		int lDataIndex = 0;
		oracle::occi::Timestamp lTimeStamp;
		mStat->setSQL("INSERT INTO " + aTableName + " VALUES (:1)");

		mStat->setNumber(++lDataIndex,1.0);

		mStat->setTimestamp(++lDataIndex,aTimeStamp);

		///������				TThostFtdcDateType	TradingDay;
		mStat->setString(++lDataIndex,(string)aData->TradingDay);
				
		///��Լ����				TThostFtdcInstrumentIDType	InstrumentID;
		lSqlStatement += " instrument_id varchar2(11),";

		///����������			TThostFtdcExchangeIDType	ExchangeID;
		lSqlStatement += " exchange_id varchar2(9),";

		///��Լ�ڽ������Ĵ���	TThostFtdcExchangeInstIDType	ExchangeInstID;
		lSqlStatement += " exchange_instrument_id varchar2(31),";

		///���¼�				TThostFtdcPriceType	LastPrice;
		lSqlStatement += " last_price number,";				

		///�ϴν����			TThostFtdcPriceType	PreSettlementPrice;
		lSqlStatement += " pre_settlement_price number,";

		///������				TThostFtdcPriceType	PreClosePrice;
		lSqlStatement += " pre_close_price number,";

		///��ֲ���				TThostFtdcLargeVolumeType	PreOpenInterest;
		lSqlStatement += " pre_open_interest number,";

		///����				TThostFtdcPriceType	OpenPrice;
		lSqlStatement += " open_price number,";

		///��߼�				TThostFtdcPriceType	HighestPrice;
		lSqlStatement += " highest_price number,";

		///��ͼ�				TThostFtdcPriceType	LowestPrice;
		lSqlStatement += " lowest_price number,";

		///����					TThostFtdcVolumeType	Volume;
		lSqlStatement += " volume number,";

		///�ɽ����				TThostFtdcMoneyType	Turnover;
		lSqlStatement += " turnover number,";

		///�ֲ���				TThostFtdcLargeVolumeType	OpenInterest;
		lSqlStatement += " open_interest number,";

		///������				TThostFtdcPriceType	ClosePrice;
		lSqlStatement += " close_price number,";

		///���ν����			TThostFtdcPriceType	SettlementPrice;
		lSqlStatement += " settlement_price number,";

		///��ͣ���				TThostFtdcPriceType	UpperLimitPrice;
		lSqlStatement += " upper_limit_price number,";

		///��ͣ���				TThostFtdcPriceType	LowerLimitPrice;
		lSqlStatement += " lower_limit_price number,";

		///����ʵ��				TThostFtdcRatioType	PreDelta;
		lSqlStatement += " pre_delta number,";

		///����ʵ��				TThostFtdcRatioType	CurrDelta;
		lSqlStatement += " curr_delta number,";

		///����޸�ʱ��			TThostFtdcTimeType	UpdateTime;
		lSqlStatement += " update_time varchar2(9),";

		///����޸ĺ���			TThostFtdcMillisecType	UpdateMillisec;
		lSqlStatement += " update_millisec number,";

		///�����һ				TThostFtdcPriceType	BidPrice1;
		lSqlStatement += " bid_price_1 number,";

		///������һ				TThostFtdcVolumeType	BidVolume1;
		lSqlStatement += " bid_volume_1 number,";

		///������һ				TThostFtdcPriceType	AskPrice1;
		lSqlStatement += " ask_price_1 number,";

		///������һ				TThostFtdcVolumeType	AskVolume1;
		lSqlStatement += " ask_volume_1 number,";

		///����۶�				TThostFtdcPriceType	BidPrice2;
		lSqlStatement += " bid_price_2 number,";

		///��������				TThostFtdcVolumeType	BidVolume2;
		lSqlStatement += " bid_volume_2 number,";

		///�����۶�				TThostFtdcPriceType	AskPrice2;
		lSqlStatement += " ask_price_2 number,";

		///��������				TThostFtdcVolumeType	AskVolume2;
		lSqlStatement += " ask_volume_2 number,";

		///�������				TThostFtdcPriceType	BidPrice3;
		lSqlStatement += " bid_price_3 number,";

		///��������				TThostFtdcVolumeType	BidVolume3;
		lSqlStatement += " bid_volume_3 number,";

		///��������				TThostFtdcPriceType	AskPrice3;
		lSqlStatement += " ask_price_3 number,";

		///��������				TThostFtdcVolumeType	AskVolume3;
		lSqlStatement += " ask_volume_3 number,";

		///�������				TThostFtdcPriceType	BidPrice4;
		lSqlStatement += " bid_price_4 number,";

		///��������				TThostFtdcVolumeType	BidVolume4;
		lSqlStatement += " bid_volume_4 number,";

		///��������				TThostFtdcPriceType	AskPrice4;
		lSqlStatement += " ask_price_4 number,";

		///��������				TThostFtdcVolumeType	AskVolume4;
		lSqlStatement += " ask_volume_4 number,";

		///�������				TThostFtdcPriceType	BidPrice5;
		lSqlStatement += " bid_price_5 number,";

		///��������				TThostFtdcVolumeType	BidVolume5;
		lSqlStatement += " bid_volume_5 number,";

		///��������				TThostFtdcPriceType	AskPrice5;
		lSqlStatement += " ask_price_5 number,";

		///��������				TThostFtdcVolumeType	AskVolume5;
		lSqlStatement += " ask_volume_5 number,";

		///���վ���				TThostFtdcPriceType	AveragePrice;
		lSqlStatement += " average_price number,";

		///ҵ������				TThostFtdcDateType	ActionDay;
		lSqlStatement += " action_day varchar2(9)";
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
