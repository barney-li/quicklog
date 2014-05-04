#pragma once
#include <stdafx.h>
#include <stdio.h>
#include <TradeProcess.h>
#include <MarketProcess.h>
#include <boost\thread.hpp>
#include <boost\progress.hpp>
#include <ConfigReader.h>
#include <Hook.h>
#include <boost/unordered_map.hpp>
#include <boost/circular_buffer.hpp>
#include <cmath>
#include <Log.h>
#include <strstream>
#include <PASAux.h>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <Timer.h>
#include <BollingerBands.h>
#include <vector>
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost;
using namespace std;
using namespace boost::asio;
using namespace Finicial;
#define STRATEGY_BUFFER_SIZE 4096UL
#define PRICE_UPPER_LIM 100000UL
namespace Pas
{
	// strategy init error type
	typedef enum InitErrorType
	{
		ALL_GOOD = 0,
		CONFIG_ERROR,
		ALLOC_ERROR,
		OTHER_ERROR

	};
class PrimeryAndSecondary : public TradeProcess, public Hook,public Timer
{
private:
	// initialize state
	InitErrorType initStatus;
	// config reader obj
	ConfigReader config;
	// market process obj
	MarketProcess marketObj;
	// Close and far instrument name
	string primInst;
	string scndInst;
	// Close and far instrument data buffer and index
	vector <BasicMarketData> primDataBuf;
	vector <BasicMarketData> scndDataBuf;
	long long primBufIndex;
	long long scndBufIndex;
	// log
	Log logger;
	//由于行情数据中的成交量是累加值，因此需要减去上次成交量来获得成交量增量
	TThostFtdcVolumeType primLastVolume;
	TThostFtdcVolumeType scndLastVolume;
	// 策略运行所需的参数
	StrategyParameter stgArg;
	// temp string stream, used for storing messages
	stringstream tempStream;
	// BollingerBand object, used to generate Bollinger Band
	BollingerBands mBoll;
	BollingerBandData mBollData;
public:
	// constructor
	PrimeryAndSecondary(void)
	{
		
	}
private:
	// store market data into local buffer
	bool BufferData(CThostFtdcDepthMarketDataField* pDepthMarketData)
	{
		if (VerifyMarketData(*pDepthMarketData))
		{
			//recognize instrument id and put data into where they should go
			if(strncmp(pDepthMarketData->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.size()) == 0)
			{
				strncpy(primDataBuf[primBufIndex].instrumentId, pDepthMarketData->InstrumentID, sizeof(primDataBuf[primBufIndex].instrumentId));
				primDataBuf[primBufIndex].askPrice = pDepthMarketData->AskPrice1;
				primDataBuf[primBufIndex].askVolume = pDepthMarketData->AskVolume1;
				primDataBuf[primBufIndex].bidPrice = pDepthMarketData->BidPrice1;
				primDataBuf[primBufIndex].bidVolume = pDepthMarketData->BidVolume1;
				primDataBuf[primBufIndex].lastPrice = pDepthMarketData->LastPrice;
				primDataBuf[primBufIndex].volume = pDepthMarketData->Volume;
				primDataBuf[primBufIndex].trueVolume = pDepthMarketData->Volume - primLastVolume;
				primLastVolume = pDepthMarketData->Volume;
				primDataBuf[primBufIndex].upperLimit = pDepthMarketData->UpperLimitPrice;
				primDataBuf[primBufIndex].lowerLimit = pDepthMarketData->LowerLimitPrice;
				strncpy(primDataBuf[primBufIndex].updateTime, pDepthMarketData->UpdateTime, sizeof(primDataBuf[primBufIndex].updateTime));
				primDataBuf[primBufIndex].updateMillisec = pDepthMarketData->UpdateMillisec;
				primDataBuf[primBufIndex].localTime = boost::posix_time::microsec_clock::local_time();
				// don't forget to increase the index
				primBufIndex++;
				// increase vector size when it's almost full
				if(primDataBuf.size() - primBufIndex < 100)
				{
					primDataBuf.resize(primDataBuf.size()+STRATEGY_BUFFER_SIZE);
				}
			}
			else if(strncmp(pDepthMarketData->InstrumentID, stgArg.primaryInst.c_str(), stgArg.secondaryInst.size()) == 0)
			{
				strncpy(scndDataBuf[scndBufIndex].instrumentId, pDepthMarketData->InstrumentID, sizeof(scndDataBuf[scndBufIndex].instrumentId));
				scndDataBuf[scndBufIndex].askPrice = pDepthMarketData->AskPrice1;
				scndDataBuf[scndBufIndex].askVolume = pDepthMarketData->AskVolume1;
				scndDataBuf[scndBufIndex].bidPrice = pDepthMarketData->BidPrice1;
				scndDataBuf[scndBufIndex].bidVolume = pDepthMarketData->BidVolume1;
				scndDataBuf[scndBufIndex].lastPrice = pDepthMarketData->LastPrice;
				scndDataBuf[scndBufIndex].volume = pDepthMarketData->Volume;
				scndDataBuf[scndBufIndex].trueVolume = pDepthMarketData->Volume - scndLastVolume;
				scndLastVolume = pDepthMarketData->Volume;
				scndDataBuf[scndBufIndex].upperLimit = pDepthMarketData->UpperLimitPrice;
				scndDataBuf[scndBufIndex].lowerLimit = pDepthMarketData->LowerLimitPrice;
				strncpy(scndDataBuf[scndBufIndex].updateTime, pDepthMarketData->UpdateTime, sizeof(scndDataBuf[scndBufIndex].updateTime));
				scndDataBuf[scndBufIndex].updateMillisec = pDepthMarketData->UpdateMillisec;
				scndDataBuf[scndBufIndex].localTime = boost::posix_time::microsec_clock::local_time();
				// don't forget to increase the index
				scndBufIndex++;
				// increase vector size when it's almost full
				if(scndDataBuf.size() - scndBufIndex < 100)
				{
					scndDataBuf.resize(scndDataBuf.size()+STRATEGY_BUFFER_SIZE);
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
		return true;
	}
	// verify market data, any illegal data will lead to an false return
	bool VerifyMarketData(CThostFtdcDepthMarketDataField const & pData)
	{
		if(null == pData.InstrumentID) return false;
		if(pData.LastPrice>stgArg.ceilingPrice || pData.LastPrice<stgArg.floorPrice) return false;
		if(pData.AskPrice1>stgArg.ceilingPrice || pData.AskPrice1<stgArg.floorPrice) return false;
		if(pData.BidPrice1>stgArg.ceilingPrice || pData.BidPrice1<stgArg.floorPrice) return false;
		if(pData.UpperLimitPrice>stgArg.ceilingPrice || pData.UpperLimitPrice<stgArg.floorPrice) return false;
		if(pData.LowerLimitPrice>stgArg.ceilingPrice || pData.LowerLimitPrice<stgArg.floorPrice) return false;
		return true;
	}

	// check if it is good time to open
	void OpenJudge(CThostFtdcDepthMarketDataField* pDepthMarketData)
	{
		
	}

	// strategy should be initiated by market data
	virtual void HookOnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
	{
		if(BufferData(pDepthMarketData))
		{
			//calculate Boll Band
			mBoll.CalcBoll(pDepthMarketData->LastPrice, stgArg.bollPeriod, stgArg.bollAmp);
			OpenJudge(pDepthMarketData);
		}
	}
	
	// read configuration, set strategy argument, etc.
	void InitOtherCrap()
	{
		initStatus = ALL_GOOD;
		primDataBuf.resize(STRATEGY_BUFFER_SIZE);
		scndDataBuf.resize(STRATEGY_BUFFER_SIZE);
		primBufIndex = 0;
		scndBufIndex = 0;
		memset(&stgArg, 0, sizeof(stgArg));
		// read strategy arguments from configuration file
		if(config.ReadString(stgArg.primaryInst, "PrimaryInstrument") !=0 )
		{
			cout<<"[ERROR]: Can't find symble \"PrimaryInstrument\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"PrimaryInstrument\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadString(stgArg.secondaryInst, "SecondaryInstrument") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"SecondaryInstrument\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"SecondaryInstrument\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.bollPeriod, "BollPeriod") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"BollPeriod\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"BollPeriod\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.bollAmp, "BollAmp") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"BollAmp\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"BollAmp\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.openShares, "OpenShares") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"OpenShares\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"OpenShares\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.maxOpenTime, "MaxOpenTime") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"MaxOpenTime\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"MaxOpenTime\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadDouble(stgArg.floatToleration, "FloatToleration") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"FloatToleration\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"FloatToleration\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadDouble(stgArg.ceilingPrice, "CeilingPrice") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"CeilingPrice\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"CeilingPrice\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadDouble(stgArg.floorPrice, "FloorPrice") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"FloorPrice\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"FloorPrice\" in config file");
			initStatus = CONFIG_ERROR;
		}

		if(ALL_GOOD == initStatus)
		{
			cout<<"all arguments ready"<<endl;
		}
		else
		{
			cout<<"invalid argument(s), strategy will not be loaded"<<endl;
		}
	}

	void InitTradeProcess()
	{
		char* tempBroker = new char[20];
		char* tempInvestor = new char[20];
		char* tempPassword = new char[20];
		config.ReadList(&tempBroker, "BrokerID", ";");
		strcpy(basicTradeProcessData.brokerId, tempBroker);
		config.ReadList(&tempInvestor, "InvestorID", ";");
		strcpy(basicTradeProcessData.investorId, tempInvestor);
		config.ReadList(&tempPassword, "Password", ";");
		strcpy(basicTradeProcessData.investorPassword, tempPassword);
		basicTradeProcessData.numFrontAddress = config.ReadTradeFrontAddr(basicTradeProcessData.frontAddress);
		InitializeProcess();
		while(InitializeFinished() != true)
		{
			boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
		}
		delete tempBroker;
		delete tempInvestor;
		delete tempPassword;
	}
	void InitMarketProcess()
	{
		char *tempConfig = new char[20];
		config.ReadList(&tempConfig, "BrokerID", ";");
		strcpy(marketObj.broker, tempConfig);
		config.ReadList(&tempConfig, "InvestorID", ";");
		strcpy(marketObj.investor, tempConfig);
		config.ReadList(&tempConfig, "Password", ";");
		strcpy(marketObj.pwd, tempConfig);
		marketObj.numFrontAddress = config.ReadMarketFrontAddr(marketObj.frontAddress);
		marketObj.numInstrument = config.ReadInstrumentID(marketObj.instrumentList);
		// only register this callback when initStatus == ALL_GOOD
		if(ALL_GOOD == initStatus)
		{
			marketObj.SetHook(this); //Register this obj for market data call back
		}
		marketObj.StartMarketProcess();
		delete tempConfig;
	}
	// overload the timer callback of Class Timer
	void TimerCallback(const system::error_code& errorCode)
	{
		if(errorCode)
		{
			cout<<"Trade On Time"<<endl;
		}
		else
		{
			StateMachine(MAX_EVENT);
		}
	}
public:
	void StartStrategy(void)
	{
		BollingerBandData lBollData;
		
		Finicial::BollingerBands lBoll;
		for(int i=1; i<100000; i++)
		{
			lBoll.CalcBoll(i,10, 2);
			lBollData = lBoll.GetBoll(0);
		}
		InitOtherCrap();
		InitMarketProcess();
		InitTradeProcess();
	}
};
}