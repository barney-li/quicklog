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


