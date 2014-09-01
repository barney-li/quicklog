// TradeAgentVS.cpp : 
// 使用posix_time库的时候貌似要定义下边这玩意，要不然构造ptime时会有异常
#include "stdafx.h"
#include <iostream>
#include <MarketProcess.h>
#include <ConfigReader.h>
#include <boost\thread.hpp>
#include <boost\progress.hpp>
#include <Log.h>
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

	Log logger;
	logger.LogThisFast("[INFO]: market subscriber started...");
	logger.Sync();

	MarketProcess marketObj;
	ConfigReader config;

	marketObj.SetDataPushHook(MarketDataCallback);
	char *tempConfig = new char[20];
	config.ReadList(&tempConfig, "BrokerID", ";");
	strcpy(marketObj.broker, tempConfig);
	config.ReadList(&tempConfig, "InvestorID", ";");
	strcpy(marketObj.investor, tempConfig);
	config.ReadList(&tempConfig, "Password", ";");
	strcpy(marketObj.pwd, tempConfig);
	marketObj.numFrontAddress = config.ReadMarketFrontAddr(marketObj.frontAddress);
	marketObj.numInstrument = config.ReadInstrumentID(marketObj.instrumentList);
	marketObj.StartMarketProcess();
	delete tempConfig;
	for(;;)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}
	return 0;
}


