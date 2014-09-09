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
#include <boost/filesystem.hpp>
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
#define KEEP_BOLL
#include <queue>
#endif

//#define OPPONENT_PRICE_OPEN
#define QUEUE_PRICE_OPEN
//#define LAST_PRICE_OPEN

//#define COINTEGRATION_TEST// only for cointegration test

//#define WIN_BOLL_ADJUST // 根据持仓时间调节出场boll宽度

namespace Pas
{
	// strategy init error type
	typedef enum
	{
		ALL_GOOD = 0,
		CONFIG_ERROR,
		ALLOC_ERROR,
		OTHER_ERROR

	}InitErrorType;
	// time zone type
	struct TimeZone
	{
		double mStartTime;
		double mEndTime;
		TimeZone(double aStartTime, double aEndTime)
		{
			mStartTime = aStartTime;
			mEndTime = aEndTime;
		}
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
#ifdef BACK_TEST
	Log mProfitLog;
	Log mTestStatistics;
	int mWin;
	int mLose;
	double mTotalProfit;
	//queue<TRADE_EVENT> mEventQueue;
	string mPrimOpenTime;
	string mScndOpenTime;
	string mPrimCloseTime;
	string mScndCloseTime;
	double mPrimClosePrice;
	double mScndClosePrice;
	bool mCloseScndOnly;//本次平仓是否仅平仓次主力（主力开仓被撤销的情况）
	time_duration mAvgInPositionDuration;// 平均持仓时间
	time_duration mMaxInPositionDuration;// 最大持仓时间
	double mAvgWin;// 平均盈利
	double mMaxWin;// 最大盈利
	double mAvgLose;// 平均亏损
	double mMaxLose;// 最大亏损
	int mOrderCount;// 下单次数
	int mTradeCount;// 成交次数
#endif
	// 开仓的时间
	string mOpenTime;
	// 由于行情数据中的成交量是累加值，因此需要减去上次成交量来获得成交量增量
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
	bool mQueryCD;
	boost::thread* mCoolDownCancelPrimThread;
	boost::thread* mCoolDownCancelScndThread;
	boost::thread* mCoolDownClosePrimThread;
	boost::thread* mCoolDownCloseScndThread;
	int mOpenPrimId;
	int mOpenScndId;
	double mPrimEnterPrice;
	double mScndEnterPrice;
	TRADE_STATE mLastState;
	// order index，用来同步request请求与相应的应答
	int mPrimReqOrderId;
	int mScndReqOrderId;
	// traded shares
	int mTradedShares;
	// trade time period;
	time_period* mTradePeriod1;
	time_period* mTradePeriod2;
	time_period* mTradePeriod3;
	time_period* mTradePeriod4;
	// valid data period;
	time_period* mDataPeriod1;
	time_period* mDataPeriod2;
	time_period* mDataPeriod3;
	time_period* mDataPeriod4;
	// time period which opening is allowed
	time_period* mOpenPeriod1;
	time_period* mOpenPeriod2;
	time_period* mOpenPeriod3;
	time_period* mOpenPeriod4;
	InstrumentType mLatestInstType;//最新的数据类型，是主力还是次主力
public:
	// constructor
	PrimeryAndSecondary(void)
	{
		mBollLog.SetLogFile("./Data/Log/", "Boll.log");
#ifdef BACK_TEST
		mProfitLog.SetLogFile("./Data/Log/", "Profit.log");
		mTestStatistics.SetLogFile("./Data/Log/", "Statistics.log");
		mWin=0;
		mLose=0;
		mTotalProfit=0;
#endif
	}
	~PrimeryAndSecondary(void)
	{
#ifdef BACK_TEST
		if(!boost::filesystem::exists("./Data/Log/Statistics.log"))
		{
			mTestStatistics.LogThisNoTimeStamp("BollPeriod	OutterBollAmp	BollAmpLimit	WinBollAmp	StopLosePoint	StopWinPoint	PrimInst-ScndInst	WinRate	TotalProfit	WinCount	LoseCount");
		}
		int lWinRate = 0;
		tempStream.clear();
		tempStream.str("");
		if(mWin+mLose > 0)
		{
			lWinRate = 100*mWin/(mWin+mLose);
		}
		//tempStream<<stgArg.bollPeriod<<"	"<<stgArg.outterBollAmp<<"	"<<(int)(stgArg.bollAmpLimit/stgArg.minMove)<<"	"<<stgArg.winBollAmp<<"	"<<(int)(stgArg.stopLossPrice/stgArg.minMove)<<"	"<<(int)(stgArg.stopWinPoint/stgArg.minMove)<<"	"<<stgArg.winBollAmpAdjust<<"	"<<(int)(stgArg.durationStep)<<"	"<<stgArg.primaryInst<<"-"<<stgArg.secondaryInst<<"	"<<lWinRate<<"	"<<mTotalProfit<<"	"<<mWin<<"	"<<mLose;
		tempStream<<stgArg.bollPeriod<<"	"<<stgArg.outterBollAmp<<"	"<<(int)(stgArg.bollAmpLimit/stgArg.minMove)<<"	"<<stgArg.winBollAmp<<"	"<<(int)(stgArg.stopLossPrice/stgArg.minMove)<<"	"<<(int)(stgArg.stopWinPoint/stgArg.minMove)<<"	"<<stgArg.primaryInst<<"-"<<stgArg.secondaryInst<<"	"<<lWinRate<<"	"<<mTotalProfit<<"	"<<mWin<<"	"<<mLose;
		mTestStatistics.LogThisNoTimeStamp(tempStream.str().c_str());
		cout<<tempStream.str()<<endl;
#endif
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
	int CheckPrimOrder();
	int CheckScndOrder();
	/*****************************/
	/* auxalary routines */
	// check if this trade must be ended now
	bool StopLoseJudge(CThostFtdcDepthMarketDataField const& pDepthMarketData);

	void LogBollData();

	// store market data into local buffer
	bool BufferData(CThostFtdcDepthMarketDataField* pDepthMarketData);

	// store market data into local buffer, with instrument type specified
	void BufferData(CThostFtdcDepthMarketDataField* pDepthMarketData, InstrumentType aWhichInst);

	// verify market data, any illegal data will lead to an false return
	bool VerifyMarketData(CThostFtdcDepthMarketDataField const & pData);
	bool VerifyMarketData(BasicMarketData const & pData);
	/************************************************************************/
	// 判断开仓条件是否已经不再满足。当价差突破外层布林带时，认为开仓条件满足，
	// 但是为了避免价差处在布林带边缘时反复触发price bad的条件，因此一旦开仓
	// 条件被满足后，直到价差落回内层布林带以内才触发price bad。
	/************************************************************************/
	void StopOpenJudge(CThostFtdcDepthMarketDataField const& pDepthMarketData);

	/************************************************************************/
	// 开仓仲裁函数，若布林带宽度达到设定值，且主力与次主力合约的价差满足
	// 4种开仓条件之一，则抛出事件OPEN_PRICE_GOOD。在开仓次主力合约时，开仓
	// 条件会被再次验证。
	/************************************************************************/
	void OpenJudge(CThostFtdcDepthMarketDataField const& pDepthMarketData);

	/************************************************************************/
	// 止盈判断函数，当价差超过对侧的布林带时，止盈条件成立。
	/************************************************************************/
	bool StopWinJudge(CThostFtdcDepthMarketDataField const& pDepthMarketData);

	/************************************************************************/
	// 交易事件接口。
	/************************************************************************/
	void SetEvent(TRADE_EVENT aLatestEvent);

	/************************************************************************/
	// 通过开仓时的收盘价与当前的最新价计算浮盈。
	/************************************************************************/
	double EstimateProfit();

	/************************************************************************/
	// 判断是否处于合理的数据时间
	/************************************************************************/
	bool IsDataTime(string aDataTime);

	/************************************************************************/
	// 判断是否处于交易时间
	/************************************************************************/
	bool IsTradeTime(string aDataTime);

	/************************************************************************/
	// 判断是否处于可以开仓的时间
	/************************************************************************/
	bool IsOpenTime(string aDataTime);

	/************************************************************************/
	// 判断是否处于尽快出仓的时间
	/************************************************************************/
	bool IsEasyGoTime(string aDataTime);

	/************************************************************************/
	// 调整出场的布林带宽度
	/************************************************************************/
	double AdjustWinBollAmp(string aOpenTime, string aCurrentTime, double aWinBollAmp, double aAdjustVolume, double aDurationStep);
	
	/************************************************************************/
	// 判断服务器时间是否与本地时间一致
	/************************************************************************/
	bool IsServerTimeConsistWithLocal(string aServerDate, string aServerTime);

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

	/************************************************************************/
	// 初始化剩余部分
	/************************************************************************/
	InitErrorType InitOtherCrap();

	/************************************************************************/
	// 初始化trade process
	/************************************************************************/
	void InitTradeProcess();

	/************************************************************************/
	// 初始化market process
	/************************************************************************/
	void InitMarketProcess();
	/*****************************/
public:
	
	/* below are all the thread routines */
	void WaitPrimOpen(int aId)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(stgArg.primOpenTime));
		//如果没有新的ID产生，那说明还停留在当次的操作
		if((mStateMachine.GetState() == OPENING_PRIM_STATE) && (aId == mOpenPrimId))
		{
			logger.LogThisFast("[EVENT]: PRIM_OPEN_TIMEOUT");
			SetEvent(PRIM_OPEN_TIMEOUT);
		}
	}
	void WaitScndOpen(int aId)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(stgArg.scndOpenTime));
		//如果没有新的ID产生，那说明还停留在当次的操作
		if((mStateMachine.GetState() == OPENING_SCND_STATE) && (aId == mOpenScndId))
		{
			logger.LogThisFast("[EVENT]: SCND_OPEN_TIMEOUT");
			SetEvent(SCND_OPEN_TIMEOUT);
		}
	}
	void CoolDownCancelPrim()
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(500));
		mCancelPrimCD = true;
	}
	void CoolDownCancelScnd()
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(500));
		mCancelScndCD = true;
	}
	void CoolDownClosePrim()
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(500));
		mClosePrimCD = true;
	}
	void CoolDownCloseScnd()
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(500));
		mCloseScndCD = true;
	}
	void CoolDownQuery()
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(1500));// no less than 1000, that's the rule of CTP
		mQueryCD = true;
	}
	void PeriodicCheckPosition()
	{
		for(;;)
		{
			boost::this_thread::sleep(boost::posix_time::seconds(3));
			CheckPrimPosition();
			boost::this_thread::sleep(boost::posix_time::seconds(3));
			CheckScndPosition();
		}
	}
	/*****************************/

#ifdef SIMULATION
	/************************************************************************/
	// 异步事件发送函数，以异步的方式延迟发出事件，用于模拟交易。
	/************************************************************************/
	void AsyncEventPoster(TRADE_EVENT aEvent);

	/************************************************************************/
	// 这个用于回测
	/************************************************************************/
	void AsyncEventPoster(void);

	/************************************************************************/
	// 回测中，OPENING_SCND_STATE状态下的异步事件产生函数
	/************************************************************************/
	void OpeningScndStateAsyncEventGenerator();

	/************************************************************************/
	// 回测中，OPENING_PRIM_STATE状态下的异步事件产生函数
	/************************************************************************/
	void OpeningPrimStateAsyncEventGenerator();

	/************************************************************************/
	// 回测中，CLOSING_BOTH_STATE状态下的异步事件产生函数
	/************************************************************************/
	void ClosingBothStateAsyncEventGenerator();

	/************************************************************************/
	// 回测中，CLOSING_SCND_STATE状态下的异步事件产生函数
	/************************************************************************/
	void ClosingScndStateAsyncEventGenerator();

	/************************************************************************/
	// 回测中，CLOSING_PRIM_STATE状态下的异步事件产生函数
	/************************************************************************/
	void ClosingPrimStateAsyncEventGenerator();

	/************************************************************************/
	// 回测中，WAITING_PRIM_CLOSE_STATE状态下的异步事件产生函数
	/************************************************************************/
	void WaitingPrimCloseStateAsyncEventGenerator();

	/************************************************************************/
	// 回测中，WAITING_SCND_CLOSE_STATE状态下的异步事件产生函数
	/************************************************************************/
	void WaitingScndCloseStateAsyncEventGenerator();

	/************************************************************************/
	// 回测中，CANCELLING_SCND_STATE状态下的异步事件产生函数
	/************************************************************************/
	void CancellingScndStateAsyncEventGenerator();

	/************************************************************************/
	// 回测中，CANCELLING_PRIM_STATE状态下的异步事件产生函数
	/************************************************************************/
	void CancellingPrimStateAsyncEventGenerator();


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
			InitTradeProcess();// Initialize trade process first, if this failed, we don't need market process anyway
			InitMarketProcess();
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
			if(input == "primorder")
			{
				CheckPrimOrder();
			}
			if(input == "scndorder")
			{
				CheckScndOrder();
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
			if(input == "qryinstrument")
			{
				ReqQryInstrument();
			}
			input.clear();
		}
#endif
	}
};
}