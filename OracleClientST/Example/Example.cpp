// Example.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Log.h>
#include <OracleClient.h>
#include "MarketDataType.h"
#include "MarketDataTypeMap.h"
#include <boost\date_time\posix_time\posix_time.hpp>
using namespace Utilities;
using namespace DatabaseUtilities;
TRANSACTION_RESULT_TYPE CreateTypeTest(OracleClient* aClient)
{
	string lSqlStatement = "CREATE TYPE market_data_type AS OBJECT(";

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

	return aClient->ExecuteSql(lSqlStatement);
}
TRANSACTION_RESULT_TYPE CreateTableTest(OracleClient* aClient)
{
	return aClient->CreateTableFromType("ag1412", "market_data_type");
}
int _tmain(int argc, _TCHAR* argv[])
{
	boost::posix_time::ptime startTime;
	boost::posix_time::ptime endTime;
	boost::posix_time::time_duration duration;

	Log* logger = new Log("./log/", "oracle client example log.log", 1024, true, 1);	
	logger->LogThisAdvance("oracle client example started", 
							LOG_INFO, 
							LOG_STDIO_FILESYSTEM);
	OracleClient* lClient = new OracleClient();
	try
	{
		oracle::occi::Environment* lEnv = lClient->GetEnvironment();
		MarketDataTypeMap(lEnv);
		if(lClient->Init("c##barney", "Lml19870310", "//192.168.183.128:1521/barneydb", 10000) == TRANS_NO_ERROR)
		{
			logger->LogThisAdvance("database connected", LOG_INFO);
		}
		else
		{
			logger->LogThisAdvance("database not connected", LOG_INFO);
		}
		
		if(CreateTypeTest(lClient) == TRANS_NO_ERROR)
		{
			logger->LogThisAdvance("create type successed", LOG_INFO);
		}
		else
		{
			logger->LogThisAdvance("create type failed", LOG_INFO);
		}

		if(CreateTableTest(lClient) == TRANS_NO_ERROR)
		{
			logger->LogThisAdvance("create table successed", LOG_INFO);
		}
		else
		{
			logger->LogThisAdvance("create table failed", LOG_INFO);
		}
		
		double lLastPrice = 1000.1;
		long long lCount = 0;
		MarketDataType* lMarketData = new MarketDataType();
		oracle::occi::Timestamp lTimeStamp;
		startTime = boost::posix_time::microsec_clock::local_time();
		for(lCount=0; lCount<10000LL; lCount++)
		{
			lMarketData->setdata_type_version(1.0);
			lTimeStamp.fromText("2014-11-26 15:58:59.789000", "yyyy-mm-dd hh24:mi:ss.ff", "", lEnv);
			lMarketData->settime_stamp(lTimeStamp);
			lMarketData->settrading_day("20141126");
			lMarketData->setlast_price(lLastPrice);
			lLastPrice = lLastPrice+1.0;
			lClient->InsertData("ag1412", lMarketData);
		}
		endTime = boost::posix_time::microsec_clock::local_time();
		duration = endTime-startTime;
		cout<<"10000 times insert without commit takes "<<duration.total_milliseconds()<<" ms"<<endl;
		lClient->Commit();
		endTime = boost::posix_time::microsec_clock::local_time();
		duration = endTime-startTime;
		cout<<"10000 times insert with commit takes "<<duration.total_milliseconds()<<" ms"<<endl;
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

