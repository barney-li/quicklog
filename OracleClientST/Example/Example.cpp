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

