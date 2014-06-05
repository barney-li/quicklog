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
#include <BollingerBand.h>
#include <vector>
#include <PasStateMachine.h>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost;
using namespace std;
using namespace boost::asio;
using namespace Finicial;
using namespace Pas;
#define STRATEGY_BUFFER_SIZE 4096UL
#define PRICE_UPPER_LIM 100000UL

#define BACK_TEST

#ifdef BACK_TEST
#define SIMULATION
#endif
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
class PrimeryAndSecondary : public TradeProcess, public Hook, public Timer
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
	//Log logger;
	Log mBollLog;
	//由于行情数据中的成交量是累加值，因此需要减去上次成交量来获得成交量增量
	TThostFtdcVolumeType primLastVolume;
	TThostFtdcVolumeType scndLastVolume;
	// 策略运行所需的参数
	StrategyParameter stgArg;
	// temp string stream, used for storing messages
	stringstream tempStream;
	// BollingerBand object, used to generate Bollinger Band
	BollingerBand mBoll;
	BollingerBandData mBollData;
	// state machine object
	PasStateMachine mStateMachine;
	// trade direction
	TRADE_DIR mTradeDir;
	// open condition
	OPEN_CONDITION mOpenCond;
	//记录最新一次的订单索引，用来更改、查询、撤销订单
	ORDER_INDEX_TYPE lastPrimOrder;
	ORDER_INDEX_TYPE lastScndOrder;
	// 用来阻塞等待开仓操作的线程指针
	boost::thread* mWaitPrimOpenThread;
	boost::thread* mWaitScndOpenThread;
	// 用来保护状态机的互斥锁
	boost::mutex mStateMachineMutex;
	// 用来保护数据存储的互斥锁
	boost::mutex mBufferDataMutex;
	int mPrimTodayLongPosition;
	int mPrimYdLongPosition;
	int mScndTodayLongPosition;
	int mScndYdLongPosition;
	int mPrimTodayShortPosition;
	int mPrimYdShortPosition;
	int mScndTodayShortPosition;
	int mScndYdShortPosition;
	bool mStart;
	boost::thread* mPeriodicCheckPositionThread;
	bool mCancelPrimCD;
	bool mCancelScndCD;
	bool mClosePrimCD;
	bool mCloseScndCD;
	bool mCheckPositionCD;
	boost::thread* mCoolDownCancelPrimThread;
	boost::thread* mCoolDownCancelScndThread;
	boost::thread* mCoolDownClosePrimThread;
	boost::thread* mCoolDownCloseScndThread;
	boost::thread* mCoolDownCheckPositionThread;
	int mOpenPrimId;
	int mOpenScndId;
	double mPrimEnterPrice;
	double mScndEnterPrice;
public:
	// constructor
	PrimeryAndSecondary(void)
	{
		mBollLog.SetLogFile("./Data/Log/", "Boll.log");
	}
private:
	void OpenScnd();
	void OpenPrim();
	void CloseScnd();
	void ClosePrim();
	void CloseBoth();
	void CancelScnd();
	void CancelPrim();
	void CheckPrimPosition();
	void CheckScndPosition();
	void CheckPrimOrder();
	void CheckScndOrder();
	/*****************************/
	/* auxalary routines */
	// check if this trade must be ended now
	bool StopLoseJudge(CThostFtdcDepthMarketDataField* pDepthMarketData);

	void LogBollData();

	// store market data into local buffer
	bool BufferData(CThostFtdcDepthMarketDataField* pDepthMarketData);

	// verify market data, any illegal data will lead to an false return
	bool VerifyMarketData(CThostFtdcDepthMarketDataField const & pData);

	/************************************************************************/
	// 判断开仓条件是否已经不再满足。当价差突破外层布林带时，认为开仓条件满足，
	// 但是为了避免价差处在布林带边缘时反复触发price bad的条件，因此一旦开仓
	// 条件被满足后，直到价差落回内层布林带以内才触发price bad。
	/************************************************************************/
	void StopOpenJudge();

	/************************************************************************/
	// 开仓仲裁函数，若布林带宽度达到设定值，且主力与次主力合约的价差满足
	// 4种开仓条件之一，则抛出事件OPEN_PRICE_GOOD。在开仓次主力合约时，开仓
	// 条件会被再次验证。
	/************************************************************************/
	void OpenJudge(CThostFtdcDepthMarketDataField* pDepthMarketData);

	/************************************************************************/
	// 止盈判断函数，当价差超过对侧的布林带时，止盈条件成立。
	/************************************************************************/
	bool StopWinJudge();

	/************************************************************************/
	// 交易事件接口。
	/************************************************************************/
	void SetEvent(TRADE_EVENT aLatestEvent);

	/************************************************************************/
	// 通过开仓时的收盘价与当前的最新价计算浮盈。
	/************************************************************************/
	double EstimateProfit();

	/*****************************/
	/* below are all the callback routines*/

public: 
	virtual void HookOnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData);
private:
	virtual void OnRtnTrade(CThostFtdcTradeField* pTrade);
	virtual void OnRtnOrder(CThostFtdcOrderField* pOrder);
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryOrder(CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);
	/*****************************/
	/* below are all the initialization routines */
	// read configuration, set strategy argument, etc.
	InitErrorType InitOtherCrap()
	{
		initStatus = ALL_GOOD;
		primDataBuf.resize(STRATEGY_BUFFER_SIZE);
		scndDataBuf.resize(STRATEGY_BUFFER_SIZE);
		primBufIndex = 0;
		scndBufIndex = 0;
		memset(&stgArg, 0, sizeof(stgArg));
		mPrimTodayLongPosition = 0;
		mPrimYdLongPosition = 0;
		mScndTodayLongPosition = 0;
		mScndYdLongPosition = 0;
		mPrimTodayShortPosition = 0;
		mPrimYdShortPosition = 0;
		mScndTodayShortPosition = 0;
		mScndYdShortPosition = 0;
		mCancelPrimCD = true;
		mCancelScndCD = true;
		mClosePrimCD = true;
		mCloseScndCD = true;
		mCheckPositionCD = true;
		mOpenPrimId = 0;
		mOpenScndId = 0;
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
		if(config.ReadInteger(stgArg.outterBollAmp, "OutterBollAmp") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"OutterBollAmp\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"OutterBollAmp\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.innerBollAmp, "InnerBollAmp") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"InnerBollAmp\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"InnerBollAmp\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.stopBollAmp, "StopBollAmp") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"StopBollAmp\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"StopBollAmp\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.winBollAmp, "WinBollAmp") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"WinBollAmp\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"WinBollAmp\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.bollAmpLimit, "BollAmpLimit") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"BollAmpLimit\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"BollAmpLimit\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadDouble(stgArg.stopLossPrice, "StopLossPrice") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"StopLossPrice\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"StopLossPrice\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.openShares, "OpenShares") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"OpenShares\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"OpenShares\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.primOpenTime, "PrimOpenTime") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"PrimOpenTime\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"PrimOpenTime\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.scndOpenTime, "ScndOpenTime") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"ScndOpenTime\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"ScndOpenTime\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.primCloseTime, "PrimCloseTime") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"PrimCloseTime\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"PrimCloseTime\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.scndCloseTime, "ScndCloseTime") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"ScndCloseTime\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"ScndCloseTime\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.primCancelTime, "PrimCancelTime") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"PrimCancelTime\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"PrimCancelTime\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.scndCancelTime, "ScndCancelTime") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"ScndCancelTime\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"ScndCancelTime\" in config file");
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
			mPeriodicCheckPositionThread = new boost::thread(boost::bind(&PrimeryAndSecondary::PeriodicCheckPosition, this));
		}
		else
		{
			cout<<"invalid argument(s), strategy will not be loaded"<<endl;
		}
		return initStatus;

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
	/*****************************/
public:
	
	/* below are all the thread routines */
	void WaitPrimOpen(int aId)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(stgArg.primOpenTime));
		//如果没有新的ID产生，那说明还停留在当次的操作
		if(aId == mOpenPrimId)
		{
			SetEvent(PRIM_OPEN_TIMEOUT);
		}
	}
	void WaitScndOpen(int aId)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(stgArg.scndOpenTime));
		//如果没有新的ID产生，那说明还停留在当次的操作
		if(aId == mOpenScndId)
		{
			SetEvent(SCND_OPEN_TIMEOUT);
		}
	}
	void CoolDownCancelPrim()
	{
		boost::this_thread::sleep(boost::posix_time::seconds(2));
		mCancelPrimCD = true;
	}
	void CoolDownCancelScnd()
	{
		boost::this_thread::sleep(boost::posix_time::seconds(2));
		mCancelScndCD = true;
	}
	void CoolDownClosePrim()
	{
		boost::this_thread::sleep(boost::posix_time::seconds(2));
		mClosePrimCD = true;
	}
	void CoolDownCloseScnd()
	{
		boost::this_thread::sleep(boost::posix_time::seconds(2));
		mCloseScndCD = true;
	}
	void CoolDownCheckPosition()
	{
		boost::this_thread::sleep(boost::posix_time::seconds(2));
		mCheckPositionCD = true;
	}
	void PeriodicCheckPosition()
	{
		for(;;)
		{
			boost::this_thread::sleep(boost::posix_time::seconds(3));
			CheckPrimPosition();
			boost::this_thread::sleep(boost::posix_time::seconds(2));
			CheckScndPosition();
		}
	}
	/*****************************/

#ifdef SIMULATION
	/************************************************************************/
	// 异步事件发送函数，以异步的方式延迟发出事件。
	/************************************************************************/
	void AsyncEventPoster(TRADE_EVENT aEvent)
	{
#ifndef BACK_TEST
		boost::this_thread::sleep(boost::posix_time::seconds(1));
#endif
		SetEvent(aEvent);
	}
#endif
public:
	/* strategy entry */
	void StartStrategy(void)
	{
		bool trading = true;
		mStart = true;
		string input;
		if(InitOtherCrap() == ALL_GOOD)
		{
#ifndef BACK_TEST
			InitMarketProcess();
			InitTradeProcess();
#endif
		}
#ifndef BACK_TEST
		while(trading)
		{
			cin>>input;
			if(input == "start")
			{
				mStart = true;
				cout<<"strategy has been started manually"<<endl;
			}
			if(input == "pause")
			{
				mStart = false;
				cout<<"strategy has been paused manually"<<endl;
			}
			if(input == "exit")
			{
				mStart = false;
				CloseBoth();
			}
			if(input == "position")
			{
				string lInstrument;
				cout<<"input instrument: "<<endl;
				cin>>lInstrument;
				ReqQryInvestorPosition(lInstrument.c_str());
			}
			if(input == "cond1")
			{
				mOpenCond = OPEN_COND1;
				SetEvent(OPEN_PRICE_GOOD);
			}
			if(input == "cond2")
			{
				mOpenCond = OPEN_COND2;
				SetEvent(OPEN_PRICE_GOOD);
			}
			if(input == "cond3")
			{
				mOpenCond = OPEN_COND3;
				SetEvent(OPEN_PRICE_GOOD);
			}
			if(input == "cond4")
			{
				mOpenCond = OPEN_COND4;
				SetEvent(OPEN_PRICE_GOOD);
			}
			if(input == "muststop")
			{
				SetEvent(MUST_STOP);
			}
			if(input == "closegood")
			{
				SetEvent(CLOSE_PRICE_GOOD);
			}
			if(input == "buy")
			{
				string lInstrument;
				cout<<"input instrument: "<<endl;
				cin>>lInstrument;
				double lPrice;
				cout<<"input price: "<<endl;
				cin>>lPrice;
				int lVolume;
				cout<<"input volume: "<<endl;
				cin>>lVolume;
				Buy(lInstrument, lPrice, lVolume, &lastPrimOrder);
			}
			if(input == "sell")
			{
				string lInstrument;
				cout<<"input instrument: "<<endl;
				cin>>lInstrument;
				double lPrice;
				cout<<"input price: "<<endl;
				cin>>lPrice;
				int lVolume;
				cout<<"input volume: "<<endl;
				cin>>lVolume;
				Sell(lInstrument, lPrice, lVolume, &lastPrimOrder);
			}
			if(input == "short")
			{
				string lInstrument;
				cout<<"input instrument: "<<endl;
				cin>>lInstrument;
				double lPrice;
				cout<<"input price: "<<endl;
				cin>>lPrice;
				int lVolume;
				cout<<"input volume: "<<endl;
				cin>>lVolume;
				SellShort(lInstrument, lPrice, lVolume, &lastPrimOrder);
			}
			if(input == "cover")
			{
				string lInstrument;
				cout<<"input instrument: "<<endl;
				cin>>lInstrument;
				double lPrice;
				cout<<"input price: "<<endl;
				cin>>lPrice;
				int lVolume;
				cout<<"input volume: "<<endl;
				cin>>lVolume;
				BuyToCover(lInstrument, lPrice, lVolume, &lastPrimOrder);
			}
			if(input == "cancel")
			{
				CancelOrder(&lastPrimOrder);
			}
			if(input == "cancelscnd")
			{
				CancelScnd();
			}
			if(input == "cancelprim")
			{
				CancelPrim();
			}
			input.clear();
		}
#endif
	}
};
}