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
#include <boost/bind.hpp>
#include <map>
#include <cmath>
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace std;
//class MarketSubscriber
CommonLog logger;
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
			cout<<'.';
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
	logger.LogThisAdvance("market subscriber started...", LOG_INFO);
	logger.Sync();

	int lErrCode = 0;
	string lErrMsg = "";
	//OracleClient* lClient = new OracleClient();
	try
	{
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
	catch (...)
	{
		std::cout<<"exception in main()"<<std::endl;
	}
	//delete lClient;
	return 0;
}