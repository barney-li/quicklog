// Example.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Log.h>
#include <OracleClient.h>
#include "MarketDataType.h"
#include "MarketDataTypeMap.h"
#include <boost\date_time\posix_time\posix_time.hpp>
#include "NonBlockDatabase.h"
using namespace Utilities;
using namespace DatabaseUtilities;
TRANSACTION_RESULT_TYPE CreateTypeTest(OracleClient* aClient, int& aErrCode, string& aErrMsg)
{
	string lSqlStatement = "CREATE TYPE market_data_type AS OBJECT(";

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

	return aClient->ExecuteSql(lSqlStatement, aErrCode, aErrMsg);
}
TRANSACTION_RESULT_TYPE CreateTableTest(OracleClient* aClient, int& aErrCode, string& aErrMsg)
{
	return aClient->CreateMarketDataTable("testtable",aErrCode, aErrMsg);
	//string lSql = "alter table testtable nologging";
	//return aClient->ExecuteSql(lSql, aErrCode, aErrMsg);
}
int _tmain(int argc, _TCHAR* argv[])
{
	int lErrCode = 0;
	string lErrMsg = "";
	boost::posix_time::ptime startTime;
	boost::posix_time::ptime endTime;
	boost::posix_time::time_duration duration;

	Log* logger = new Log("./log/", "oracle client example log.log", 1024, true, 1);	
	logger->LogThisAdvance("oracle client example started", 
							LOG_INFO, 
							LOG_STDIO_FILESYSTEM);
	OracleClient* lClient = new OracleClient(false);
	try
	{
		//oracle::occi::Environment* lEnv = lClient->GetEnvironment();
		
		if(lClient->Connect("c##barney", "Lml19870310", "//192.168.0.107:1521/barneydb", 10000, lErrCode, lErrMsg) == TRANS_NO_ERROR)
		{
			logger->LogThisAdvance("database connected", LOG_INFO);
		}
		else
		{
			logger->LogThisAdvance("database cannot be connected, error message: " + lErrMsg, LOG_INFO);
		}
		
		/*if(CreateTypeTest(lClient, lErrCode, lErrMsg) == TRANS_NO_ERROR)
		{
			logger->LogThisAdvance("create type successed", LOG_INFO, LOG_STDIO);
		}
		else if(lErrCode == 955)
		{
			logger->LogThisAdvance("type has already been created before", LOG_INFO, LOG_STDIO);
		}
		else
		{
			logger->LogThisAdvance("create type failed, error message: "+lErrMsg, LOG_ERROR);
		}
		MarketDataTypeMap(lEnv);*/
		if(CreateTableTest(lClient, lErrCode, lErrMsg) == TRANS_NO_ERROR)
		{
			logger->LogThisAdvance("create table successed", LOG_INFO, LOG_STDIO);
		}
		else if(lErrCode == 955)
		{
			logger->LogThisAdvance("table has already been created before", LOG_INFO, LOG_STDIO);
		}
		else
		{
			logger->LogThisAdvance("create table failed, error message: "+lErrMsg, LOG_ERROR);
		}
		
		double lLastPrice = 1000.1;
		long long lCount = 0;
		//oracle::occi::Timestamp lTimeStamp;
		
		NonBlockDatabase* lNonBlockClient = new NonBlockDatabase("c##barney", "Lml19870310", "//192.168.0.107:1521/barneydb", 10000, 1);
		while(lNonBlockClient->InitFinished() == false)
		{
			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}
		CThostFtdcDepthMarketDataField lMarketDataStruct;
		string lErrMsg;
		startTime = boost::posix_time::microsec_clock::local_time();
		memset(&lMarketDataStruct, 0, sizeof(CThostFtdcDepthMarketDataField));
		for(lCount=0; lCount<100000LL; lCount++)
		{
			memcpy(&lMarketDataStruct.InstrumentID, "ag1412", sizeof(TThostFtdcInstrumentIDType));
			memcpy(&lMarketDataStruct.TradingDay, "20141204", sizeof(TThostFtdcDateType));
			lMarketDataStruct.LastPrice = lLastPrice;
			if(lNonBlockClient->InsertData("testtable", &lMarketDataStruct, lErrMsg) == NonBlockDatabase::BUFFER_OVERFLOW)
			{
				cout<<lErrMsg<<endl;
			}
			if(lCount%1000 == 0)
			{
				cout<<lCount<<".";
				boost::this_thread::sleep(boost::posix_time::seconds(1));
			}
			lLastPrice = lLastPrice+1.0;
		}
		delete lNonBlockClient;
		endTime = boost::posix_time::microsec_clock::local_time();
		duration = endTime-startTime;
		cout<<"100000 times insert without commit takes "<<duration.total_milliseconds()<<" ms"<<endl;
		endTime = boost::posix_time::microsec_clock::local_time();
		duration = endTime-startTime;
		cout<<"100000 times insert with commit takes "<<duration.total_milliseconds()<<" ms"<<endl;
	}
	catch(SQLException ex)
	{
		std::stringstream tempStream;
		tempStream.str("");	
		tempStream<<"exception in main(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
		cout<<tempStream.str()<<endl;
		return SQL_EXCEPTION;
	}
	catch (...)
	{
		std::cout<<"exception in main()"<<std::endl;
	}
	
	delete lClient;
	delete logger;
	return 0;
}

