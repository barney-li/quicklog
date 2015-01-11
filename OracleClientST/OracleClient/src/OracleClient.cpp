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

		///交易日				TThostFtdcDateType	TradingDay;
		lSqlStatement += " trading_day varchar2(9),";	
				
		///合约代码				TThostFtdcInstrumentIDType	InstrumentID;
		lSqlStatement += " instrument_id varchar2(11),";

		///交易所代码			TThostFtdcExchangeIDType	ExchangeID;
		lSqlStatement += " exchange_id varchar2(9),";

		///合约在交易所的代码	TThostFtdcExchangeInstIDType	ExchangeInstID;
		lSqlStatement += " exchange_instrument_id varchar2(31),";

		///最新价				TThostFtdcPriceType	LastPrice;
		lSqlStatement += " last_price number,";				

		///上次结算价			TThostFtdcPriceType	PreSettlementPrice;
		lSqlStatement += " pre_settlement_price number,";

		///昨收盘				TThostFtdcPriceType	PreClosePrice;
		lSqlStatement += " pre_close_price number,";

		///昨持仓量				TThostFtdcLargeVolumeType	PreOpenInterest;
		lSqlStatement += " pre_open_interest number,";

		///今开盘				TThostFtdcPriceType	OpenPrice;
		lSqlStatement += " open_price number,";

		///最高价				TThostFtdcPriceType	HighestPrice;
		lSqlStatement += " highest_price number,";

		///最低价				TThostFtdcPriceType	LowestPrice;
		lSqlStatement += " lowest_price number,";

		///数量					TThostFtdcVolumeType	Volume;
		lSqlStatement += " volume number,";

		///成交金额				TThostFtdcMoneyType	Turnover;
		lSqlStatement += " turnover number,";

		///持仓量				TThostFtdcLargeVolumeType	OpenInterest;
		lSqlStatement += " open_interest number,";

		///今收盘				TThostFtdcPriceType	ClosePrice;
		lSqlStatement += " close_price number,";

		///本次结算价			TThostFtdcPriceType	SettlementPrice;
		lSqlStatement += " settlement_price number,";

		///涨停板价				TThostFtdcPriceType	UpperLimitPrice;
		lSqlStatement += " upper_limit_price number,";

		///跌停板价				TThostFtdcPriceType	LowerLimitPrice;
		lSqlStatement += " lower_limit_price number,";

		///昨虚实度				TThostFtdcRatioType	PreDelta;
		lSqlStatement += " pre_delta number,";

		///今虚实度				TThostFtdcRatioType	CurrDelta;
		lSqlStatement += " curr_delta number,";

		///最后修改时间			TThostFtdcTimeType	UpdateTime;
		lSqlStatement += " update_time varchar2(9),";

		///最后修改毫秒			TThostFtdcMillisecType	UpdateMillisec;
		lSqlStatement += " update_millisec number,";

		///申买价一				TThostFtdcPriceType	BidPrice1;
		lSqlStatement += " bid_price_1 number,";

		///申买量一				TThostFtdcVolumeType	BidVolume1;
		lSqlStatement += " bid_volume_1 number,";

		///申卖价一				TThostFtdcPriceType	AskPrice1;
		lSqlStatement += " ask_price_1 number,";

		///申卖量一				TThostFtdcVolumeType	AskVolume1;
		lSqlStatement += " ask_volume_1 number,";

		///申买价二				TThostFtdcPriceType	BidPrice2;
		lSqlStatement += " bid_price_2 number,";

		///申买量二				TThostFtdcVolumeType	BidVolume2;
		lSqlStatement += " bid_volume_2 number,";

		///申卖价二				TThostFtdcPriceType	AskPrice2;
		lSqlStatement += " ask_price_2 number,";

		///申卖量二				TThostFtdcVolumeType	AskVolume2;
		lSqlStatement += " ask_volume_2 number,";

		///申买价三				TThostFtdcPriceType	BidPrice3;
		lSqlStatement += " bid_price_3 number,";

		///申买量三				TThostFtdcVolumeType	BidVolume3;
		lSqlStatement += " bid_volume_3 number,";

		///申卖价三				TThostFtdcPriceType	AskPrice3;
		lSqlStatement += " ask_price_3 number,";

		///申卖量三				TThostFtdcVolumeType	AskVolume3;
		lSqlStatement += " ask_volume_3 number,";

		///申买价四				TThostFtdcPriceType	BidPrice4;
		lSqlStatement += " bid_price_4 number,";

		///申买量四				TThostFtdcVolumeType	BidVolume4;
		lSqlStatement += " bid_volume_4 number,";

		///申卖价四				TThostFtdcPriceType	AskPrice4;
		lSqlStatement += " ask_price_4 number,";

		///申卖量四				TThostFtdcVolumeType	AskVolume4;
		lSqlStatement += " ask_volume_4 number,";

		///申买价五				TThostFtdcPriceType	BidPrice5;
		lSqlStatement += " bid_price_5 number,";

		///申买量五				TThostFtdcVolumeType	BidVolume5;
		lSqlStatement += " bid_volume_5 number,";

		///申卖价五				TThostFtdcPriceType	AskPrice5;
		lSqlStatement += " ask_price_5 number,";

		///申卖量五				TThostFtdcVolumeType	AskVolume5;
		lSqlStatement += " ask_volume_5 number,";

		///当日均价				TThostFtdcPriceType	AveragePrice;
		lSqlStatement += " average_price number,";

		///业务日期				TThostFtdcDateType	ActionDay;
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

		///交易日				TThostFtdcDateType	TradingDay;
		mStat->setString(++lDataIndex,(string)aData->TradingDay);
				
		///合约代码				TThostFtdcInstrumentIDType	InstrumentID;
		lSqlStatement += " instrument_id varchar2(11),";

		///交易所代码			TThostFtdcExchangeIDType	ExchangeID;
		lSqlStatement += " exchange_id varchar2(9),";

		///合约在交易所的代码	TThostFtdcExchangeInstIDType	ExchangeInstID;
		lSqlStatement += " exchange_instrument_id varchar2(31),";

		///最新价				TThostFtdcPriceType	LastPrice;
		lSqlStatement += " last_price number,";				

		///上次结算价			TThostFtdcPriceType	PreSettlementPrice;
		lSqlStatement += " pre_settlement_price number,";

		///昨收盘				TThostFtdcPriceType	PreClosePrice;
		lSqlStatement += " pre_close_price number,";

		///昨持仓量				TThostFtdcLargeVolumeType	PreOpenInterest;
		lSqlStatement += " pre_open_interest number,";

		///今开盘				TThostFtdcPriceType	OpenPrice;
		lSqlStatement += " open_price number,";

		///最高价				TThostFtdcPriceType	HighestPrice;
		lSqlStatement += " highest_price number,";

		///最低价				TThostFtdcPriceType	LowestPrice;
		lSqlStatement += " lowest_price number,";

		///数量					TThostFtdcVolumeType	Volume;
		lSqlStatement += " volume number,";

		///成交金额				TThostFtdcMoneyType	Turnover;
		lSqlStatement += " turnover number,";

		///持仓量				TThostFtdcLargeVolumeType	OpenInterest;
		lSqlStatement += " open_interest number,";

		///今收盘				TThostFtdcPriceType	ClosePrice;
		lSqlStatement += " close_price number,";

		///本次结算价			TThostFtdcPriceType	SettlementPrice;
		lSqlStatement += " settlement_price number,";

		///涨停板价				TThostFtdcPriceType	UpperLimitPrice;
		lSqlStatement += " upper_limit_price number,";

		///跌停板价				TThostFtdcPriceType	LowerLimitPrice;
		lSqlStatement += " lower_limit_price number,";

		///昨虚实度				TThostFtdcRatioType	PreDelta;
		lSqlStatement += " pre_delta number,";

		///今虚实度				TThostFtdcRatioType	CurrDelta;
		lSqlStatement += " curr_delta number,";

		///最后修改时间			TThostFtdcTimeType	UpdateTime;
		lSqlStatement += " update_time varchar2(9),";

		///最后修改毫秒			TThostFtdcMillisecType	UpdateMillisec;
		lSqlStatement += " update_millisec number,";

		///申买价一				TThostFtdcPriceType	BidPrice1;
		lSqlStatement += " bid_price_1 number,";

		///申买量一				TThostFtdcVolumeType	BidVolume1;
		lSqlStatement += " bid_volume_1 number,";

		///申卖价一				TThostFtdcPriceType	AskPrice1;
		lSqlStatement += " ask_price_1 number,";

		///申卖量一				TThostFtdcVolumeType	AskVolume1;
		lSqlStatement += " ask_volume_1 number,";

		///申买价二				TThostFtdcPriceType	BidPrice2;
		lSqlStatement += " bid_price_2 number,";

		///申买量二				TThostFtdcVolumeType	BidVolume2;
		lSqlStatement += " bid_volume_2 number,";

		///申卖价二				TThostFtdcPriceType	AskPrice2;
		lSqlStatement += " ask_price_2 number,";

		///申卖量二				TThostFtdcVolumeType	AskVolume2;
		lSqlStatement += " ask_volume_2 number,";

		///申买价三				TThostFtdcPriceType	BidPrice3;
		lSqlStatement += " bid_price_3 number,";

		///申买量三				TThostFtdcVolumeType	BidVolume3;
		lSqlStatement += " bid_volume_3 number,";

		///申卖价三				TThostFtdcPriceType	AskPrice3;
		lSqlStatement += " ask_price_3 number,";

		///申卖量三				TThostFtdcVolumeType	AskVolume3;
		lSqlStatement += " ask_volume_3 number,";

		///申买价四				TThostFtdcPriceType	BidPrice4;
		lSqlStatement += " bid_price_4 number,";

		///申买量四				TThostFtdcVolumeType	BidVolume4;
		lSqlStatement += " bid_volume_4 number,";

		///申卖价四				TThostFtdcPriceType	AskPrice4;
		lSqlStatement += " ask_price_4 number,";

		///申卖量四				TThostFtdcVolumeType	AskVolume4;
		lSqlStatement += " ask_volume_4 number,";

		///申买价五				TThostFtdcPriceType	BidPrice5;
		lSqlStatement += " bid_price_5 number,";

		///申买量五				TThostFtdcVolumeType	BidVolume5;
		lSqlStatement += " bid_volume_5 number,";

		///申卖价五				TThostFtdcPriceType	AskPrice5;
		lSqlStatement += " ask_price_5 number,";

		///申卖量五				TThostFtdcVolumeType	AskVolume5;
		lSqlStatement += " ask_volume_5 number,";

		///当日均价				TThostFtdcPriceType	AveragePrice;
		lSqlStatement += " average_price number,";

		///业务日期				TThostFtdcDateType	ActionDay;
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
