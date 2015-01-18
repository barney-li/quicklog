#pragma once
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
#include <boost/bind.hpp>
#include <map>
#include <cmath>
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace std;
using namespace DatabaseUtilities;
//class MarketSubscriber
NonBlockDatabase* lNonBlockClient;
CommonLog logger;
string gTableName;
#define TableSpaceCount 15
string tableSpaceMap[TableSpaceCount][4]=\
{
	{"marketdata2012","'C:\\tablespace\\marketdata2012.dbf'","part2012","'2013-01-01'"},
	{"marketdata2013","'C:\\tablespace\\marketdata2013.dbf'","part2013","'2014-01-01'"},
	{"marketdata2014","'C:\\tablespace\\marketdata2014.dbf'","part2014","'2015-01-01'"},
	{"marketdata201501","'C:\\tablespace\\marketdata201501.dbf'","part201501","'2015-02-01'"},
	{"marketdata201502","'C:\\tablespace\\marketdata201502.dbf'","part201502","'2015-03-01'"},
	{"marketdata201503","'C:\\tablespace\\marketdata201503.dbf'","part201503","'2015-04-01'"},
	{"marketdata201504","'C:\\tablespace\\marketdata201504.dbf'","part201504","'2015-05-01'"},
	{"marketdata201505","'C:\\tablespace\\marketdata201505.dbf'","part201505","'2015-06-01'"},
	{"marketdata201506","'C:\\tablespace\\marketdata201506.dbf'","part201506","'2015-07-01'"},
	{"marketdata201507","'C:\\tablespace\\marketdata201507.dbf'","part201507","'2015-08-01'"},
	{"marketdata201508","'C:\\tablespace\\marketdata201508.dbf'","part201508","'2015-09-01'"},
	{"marketdata201509","'C:\\tablespace\\marketdata201509.dbf'","part201509","'2015-10-01'"},
	{"marketdata201510","'C:\\tablespace\\marketdata201510.dbf'","part201510","'2015-11-01'"},
	{"marketdata201511","'C:\\tablespace\\marketdata201511.dbf'","part201511","'2015-12-01'"},
	{"marketdata201512","'C:\\tablespace\\marketdata201512.dbf'","part201512","'2016-01-01'"}
};
struct DataFeature
{
	DataFeature()
	{
		mUpdateTime = "";
		mUpdateMS = 0;
		mLastAskVol = 0;
		mLastBidVol = 0;
		mVolume = 0;
		mLastAskPrice = 0;
		mLastBidPrice = 0;
		mOpenInterest = 0;
	}
	DataFeature(string aUpdateTime,int aUpdateMS, int aLastAskVol, int aLastBidVol, int aVolume, double aLastAskPrice, double aLastBidPrice, double aOpenInterest)
	{
		mUpdateTime = aUpdateTime;
		mUpdateMS = aUpdateMS;
		mLastAskVol = aLastAskVol;
		mLastBidVol = aLastBidVol;
		mVolume = aVolume;
		mLastAskPrice = aLastAskPrice;
		mLastBidPrice = aLastBidPrice;
		mOpenInterest = aOpenInterest;
	}
	bool operator == (const DataFeature other) const
	{
		if(mUpdateTime == other.mUpdateTime &&\
			mUpdateMS == other.mUpdateMS &&\
			mLastAskVol == other.mLastAskVol &&\
			mLastBidVol == other.mLastBidVol &&\
			mVolume == other.mVolume &&\
			(abs(mLastAskPrice - other.mLastAskPrice)<0.0000001) &&\
			(abs(mLastBidPrice - other.mLastBidPrice)<0.0000001) &&\
			(abs(mOpenInterest - other.mOpenInterest)<0.0000001))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	string mUpdateTime;
	int mUpdateMS;
	int mLastAskVol;
	int mLastBidVol;
	int mVolume;
	double mLastAskPrice;
	double mLastBidPrice;
	double mOpenInterest;
};
map<string,DataFeature> gLastData;
TRANSACTION_RESULT_TYPE CreateType(OracleClient* aClient, int& aErrCode, string& aErrMsg)
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
	aErrMsg = "";
	return aClient->ExecuteSql(lSqlStatement, aErrCode, aErrMsg);
}
TRANSACTION_RESULT_TYPE CreateTableSpaces(OracleClient* aClient, int& aErrCode, string& aErrMsg)
{
	TRANSACTION_RESULT_TYPE lReturnCode = TRANS_NO_ERROR;
	string lTotalErrMsg = "";
	string lSqlStatement = "";
	for(int i=0;i<TableSpaceCount;i++)
	{
		lSqlStatement = "create tablespace "+tableSpaceMap[i][0]+" datafile "+tableSpaceMap[i][1]+" size 128M autoextend on next 128M";
		lReturnCode = aClient->ExecuteSql(lSqlStatement, aErrCode, aErrMsg);
		lTotalErrMsg += aErrMsg;
	}
	aErrMsg = lTotalErrMsg;
	return lReturnCode;
}
TRANSACTION_RESULT_TYPE CreateTable(OracleClient* aClient, string aTableName, int& aErrCode, string& aErrMsg)
{
	string lSqlStatement = "create table " + aTableName + " of market_data_type partition by range(time_stamp)\n (\n";
	string lTotalErrMsg = "";
	for(int i=0;i<TableSpaceCount-1;i++)
	{
		lSqlStatement += "partition "+tableSpaceMap[i][2]+" values less than (to_timestamp("+tableSpaceMap[i][3]+",'YYYY-MM-DD')) tablespace "+tableSpaceMap[i][0]+" storage(initial 128M next 128M minextents 1 maxextents unlimited),\n";
	}
	lSqlStatement += "partition "+tableSpaceMap[TableSpaceCount-1][2]+" values less than (to_timestamp("+tableSpaceMap[TableSpaceCount-1][3]+",'YYYY-MM-DD')) tablespace "+tableSpaceMap[TableSpaceCount-1][0]+" storage(initial 128M next 128M minextents 1 maxextents unlimited)\n)";
	return aClient->ExecuteSql(lSqlStatement, aErrCode, aErrMsg);
}
bool IsRedundent(CThostFtdcDepthMarketDataField* marketData)
{
	DataFeature lData = DataFeature((string)marketData->UpdateTime, marketData->UpdateMillisec, marketData->AskVolume1, marketData->BidVolume1, marketData->Volume, marketData->AskPrice1, marketData->BidPrice1, marketData->OpenInterest);
	if(gLastData[(string)marketData->InstrumentID] == lData)
	{
		return true;
	}//redundent
	else
	{
		gLastData[(string)marketData->InstrumentID] = lData;
		return false;
	}//not redundent
}
void MarketDataCallback(CThostFtdcDepthMarketDataField* marketData)
{
	static string lErrMsg;
	try
	{
		if(IsRedundent(marketData) == false)
		{
			if(lNonBlockClient->InsertData(gTableName, marketData, lErrMsg) != NonBlockDatabase::NONBLOCK_NO_ERROR)
			{
				logger.LogThisAdvance(lErrMsg, LOG_ERROR);
			}
			else
			{
				cout<<'.';
			}
		}
	}
	catch(...)
	{
		stringstream tempStream;
		tempStream.str("");
		tempStream<<"exception in MarketDataCallback(), error message: unknown";
		logger.LogThisAdvance(tempStream.str(), LOG_ERROR);
	}
	//cout<<" ["<<marketData->InstrumentID<<"-"<<marketData->LastPrice<<"] ";//no endl at the end to increase the efficiency
}
int StartMarketSubscriber()
{
	ConfigReader config;
	string lId;
	string lPwd;
	string lDb;
	int lNonBlockBuffer = 0;
	logger.LogThisAdvance("market subscriber started...", LOG_INFO);
	logger.Sync();

	int lErrCode = 0;
	string lErrMsg = "";
	//OracleClient* lClient = new OracleClient();
	try
	{
		config.ReadString(lId, "DatabaseUser");
		config.ReadString(lPwd, "DatabasePwd");
		config.ReadString(lDb, "DatabaseAddress");
		config.ReadInteger(lNonBlockBuffer, "NonBlockBufferSize");
		config.ReadString(gTableName, "TableName");

		logger.LogThisAdvance("******************************************", LOG_INFO);
		logger.LogThisAdvance("* Database Client initialization started *", LOG_INFO);
		logger.LogThisAdvance("******************************************", LOG_INFO);
		
		cout<<"ID: "<<lId<<endl;
		cout<<"Password: "<<lPwd<<endl;
		cout<<"Database: "<<lDb<<endl;
		cout<<"Cache Size: "<<lNonBlockBuffer<<endl;
		cout<<"Table Name: "<<gTableName<<endl;
		//if(lClient->Connect(lId, lPwd, lDb, 10000, lErrCode, lErrMsg) == TRANS_NO_ERROR)
		//{
		//	logger.LogThisAdvance("database connected", LOG_INFO);
		//}
		//else
		//{
		//	logger.LogThisAdvance("database cannot be connected, error message: " + lErrMsg, LOG_INFO);
		//}
		//
		//if(CreateType(lClient, lErrCode, lErrMsg) == TRANS_NO_ERROR)
		//{
		//	logger.LogThisAdvance("create type successed", LOG_INFO, LOG_STDIO);
		//}
		//else if(lErrCode == 955)
		//{
		//	logger.LogThisAdvance("type has already been created before", LOG_INFO, LOG_STDIO);
		//}
		//else
		//{
		//	logger.LogThisAdvance("create type failed, error message: "+lErrMsg, LOG_ERROR);
		//}

		//if(CreateTableSpaces(lClient, lErrCode, lErrMsg) == TRANS_NO_ERROR)
		//{
		//	logger.LogThisAdvance("create table spaces successed", LOG_INFO, LOG_STDIO);
		//}
		//else if(lErrCode == 955)
		//{
		//	logger.LogThisAdvance("table spaces have already been created before", LOG_INFO, LOG_STDIO);
		//}
		//else
		//{
		//	logger.LogThisAdvance("create table spaces failed, error message: "+lErrMsg, LOG_ERROR);
		//};

		//if(gTableName.length()>0)
		//{
		//	if(CreateTable(lClient,gTableName, lErrCode, lErrMsg) == TRANS_NO_ERROR)
		//	{
		//		logger.LogThisAdvance("create table " + gTableName + " successed", LOG_INFO, LOG_STDIO);
		//	}
		//	else if(lErrCode == 955)
		//	{
		//		logger.LogThisAdvance("table " + gTableName + " has already been created before", LOG_INFO, LOG_STDIO);
		//	}
		//	else
		//	{
		//		logger.LogThisAdvance("create table " + gTableName + " failed, error message: "+lErrMsg, LOG_ERROR);
		//	};
		//}

		lNonBlockClient = new NonBlockDatabase(lId, lPwd, lDb, lNonBlockBuffer, 1);
		while(lNonBlockClient->InitFinished() == false)
		{
			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}

		logger.LogThisAdvance("*******************************************", LOG_INFO);
		logger.LogThisAdvance("* Database Client initialization finished *", LOG_INFO);
		logger.LogThisAdvance("*******************************************", LOG_INFO);
	
		boost::progress_display progressDisp(100);
		for(int i=0;i<100;i++)
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			progressDisp+=1;
		}
		TradeProcess* tradeObj = null;
		MarketProcess marketObj;
		

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
						logger.LogThisAdvance("trade process initialization finished", LOG_INFO);
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
							logger.LogThisAdvance("instrument list ready", LOG_INFO);
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
				tempStream<<"exception in main(), error message: "<<ex.what();
				logger.LogThisAdvance(tempStream.str(), LOG_ERROR);
				logger.Sync();
				if(lExceptionCount>10000)
				{
					logger.LogThisAdvance((string)"too many exceptions, system halt", LOG_ERROR);
					logger.Sync();
					return -1;
				}
			}
			catch(...)
			{
				lExceptionCount++;
				stringstream tempStream;
				tempStream.str("");
				tempStream<<"exception in main(), error message: unknown";
				logger.LogThisAdvance(tempStream.str(), LOG_ERROR);
				logger.Sync();
				if(lExceptionCount>10000)
				{
					logger.LogThisAdvance((string)"too many exceptions, system halt", LOG_ERROR);
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
	delete lNonBlockClient;
	//delete lClient;
	return 0;
}