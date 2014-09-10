// TradeAgentVS.cpp : 
// 使用posix_time库的时候貌似要定义下边这玩意，要不然构造ptime时会有异常
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
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace std;

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

	TradeProcess tradeObj;
	MarketProcess marketObj;
	ConfigReader config;

	marketObj.SetDataPushHook(MarketDataCallback);
	char* brokerIdConfig = new char[20];
	char* investorIdConfig = new char[20];
	char* passwordConfig = new char[20];

	config.ReadList(&brokerIdConfig, "BrokerID", ";");
	config.ReadList(&investorIdConfig, "InvestorID", ";");
	config.ReadList(&passwordConfig, "Password", ";");
	/* initialize trade agent */
	strcpy(tradeObj.basicTradeProcessData.brokerId, brokerIdConfig);
	strcpy(tradeObj.basicTradeProcessData.investorId, investorIdConfig);
	strcpy(tradeObj.basicTradeProcessData.investorPassword, passwordConfig);
	tradeObj.basicTradeProcessData.numFrontAddress = config.ReadTradeFrontAddr(tradeObj.basicTradeProcessData.frontAddress);
	tradeObj.InitializeProcess();
	while(!tradeObj.InitializeFinished())
	{
		boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));	
	}
	tradeObj.ReqQryInstrument();
	
	while(!tradeObj.InstrumentListReady())
	{
		boost::this_thread::sleep(boost::posix_time::seconds(1));
	}
	/* initialize market agent */
	strcpy(marketObj.broker, brokerIdConfig);
	strcpy(marketObj.investor, investorIdConfig);
	strcpy(marketObj.pwd, passwordConfig);
	marketObj.numFrontAddress = config.ReadMarketFrontAddr(marketObj.frontAddress);
	marketObj.SetInstrumentList(tradeObj.GetInstrumentList());
	//marketObj.numInstrument = config.ReadInstrumentID(marketObj.instrumentList);
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


