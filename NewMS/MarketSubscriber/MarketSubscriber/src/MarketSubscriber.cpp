// MarketSubscriber.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <MarketProcess.h>
#include <TradeProcess.h>
#include <ConfigReader.h>
#include <boost\thread.hpp>
#include <boost\progress.hpp>
#include <CommonLog.h>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <MarketDataType.h>
#include <MarketDataTypeMap.h>
#include <NonBlockDatabase.h>
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace std;
using namespace DatabaseUtilities;
TRANSACTION_RESULT_TYPE CreateType(OracleClient* aClient, int& aErrCode, string& aErrMsg)
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
TRANSACTION_RESULT_TYPE CreateTable(OracleClient* aClient, string aTableName, int& aErrCode, string& aErrMsg)
{
	return aClient->CreateTableFromType(aTableName, "market_data_type", aErrCode, aErrMsg);
}
void MarketDataCallback(CThostFtdcDepthMarketDataField* marketData)
{
	cout<<" ["<<marketData->InstrumentID<<"-"<<marketData->LastPrice<<"] ";//no endl at the end to increase the efficiency
}

int _tmain(int argc, _TCHAR* argv[])
{
	boost::progress_display progressDisp(100);
	for(int i=0;i<100;i++)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		progressDisp+=1;
	}

	CommonLog logger;
	logger.LogThisFast("[INFO]: market subscriber started...");
	logger.Sync();

	TradeProcess* tradeObj = null;
	MarketProcess marketObj;
	ConfigReader config;

	marketObj.SetDataPushHook(MarketDataCallback);
	char* brokerIdConfig = new char[20];
	char* investorIdConfig = new char[20];
	char* passwordConfig = new char[20];

	config.ReadList(&brokerIdConfig, "BrokerID", ";");
	config.ReadList(&investorIdConfig, "InvestorID", ";");
	config.ReadList(&passwordConfig, "Password", ";");

	bool lInstrumentListReady = false;
	int lExceptionCount = 0;
	/* initialize trade agent */
	do
	{
		try
		{
			if(tradeObj)
			{
				delete tradeObj;
			}
			tradeObj = new TradeProcess();
			strcpy(tradeObj->basicTradeProcessData.brokerId, brokerIdConfig);
			strcpy(tradeObj->basicTradeProcessData.investorId, investorIdConfig);
			strcpy(tradeObj->basicTradeProcessData.investorPassword, passwordConfig);
			tradeObj->basicTradeProcessData.numFrontAddress = config.ReadTradeFrontAddr(tradeObj->basicTradeProcessData.frontAddress);
			tradeObj->InitializeProcess();
			int lTryInitCount = 0;
			while( lTryInitCount<100 )
			{
				lTryInitCount++;
				boost::this_thread::sleep(boost::posix_time::seconds(1));
				if(tradeObj->InitializeFinished())
				{
					logger.LogThisFast("[INFO]: trade process initialization finished");
					break;
				}
			}
			if(tradeObj->InitializeFinished())
			{
				tradeObj->ReqQryInstrument();
				int lTryQryListCount = 0;
				while(lTryQryListCount<10)
				{
					lTryQryListCount++;
					boost::this_thread::sleep(boost::posix_time::seconds(10));
					if(tradeObj->InstrumentListReady())
					{
						logger.LogThisFast("[INFO]: instrument list ready");
						lInstrumentListReady = true;
						break;
					}
				}
			}
		}
		catch(std::exception ex)
		{
			lExceptionCount++;
			stringstream tempStream;
			tempStream.str("");
			tempStream<<"[EXCEPTION]: exception in main(), error message: "<<ex.what();
			cout<<tempStream.str()<<endl;
			logger.LogThisFast(tempStream.str());
			logger.Sync();
			if(lExceptionCount>10000)
			{
				cout<<"[ERROR]: too many exceptions, system halt"<<endl;
				logger.LogThisFast((string)"[EXCEPTION]: too many exceptions, system halt");
				logger.Sync();
				return -1;
			}
		}
		catch(...)
		{
			lExceptionCount++;
			stringstream tempStream;
			tempStream.str("");
			tempStream<<"[EXCEPTION]: exception in main(), error message: unknown";
			cout<<tempStream.str()<<endl;
			logger.LogThisFast(tempStream.str());
			logger.Sync();
			if(lExceptionCount>10000)
			{
				cout<<"[ERROR]: too many exceptions, system halt"<<endl;
				logger.LogThisFast((string)"[EXCEPTION]: too many exceptions, system halt");
				logger.Sync();
				return -1;
			}
		}
	}
	while(lInstrumentListReady == false);

	/* initialize market agent */
	strcpy(marketObj.broker, brokerIdConfig);
	strcpy(marketObj.investor, investorIdConfig);
	strcpy(marketObj.pwd, passwordConfig);
	marketObj.numFrontAddress = config.ReadMarketFrontAddr(marketObj.frontAddress);
	marketObj.SetInstrumentList(tradeObj->GetInstrumentList());
	marketObj.StartMarketProcess();

	delete brokerIdConfig;
	delete investorIdConfig;
	delete passwordConfig;
	for(;;)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}
	return 0;
}


