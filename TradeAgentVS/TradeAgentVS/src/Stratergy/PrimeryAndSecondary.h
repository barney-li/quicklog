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

//#define WIN_BOLL_ADJUST // ���ݳֲ�ʱ����ڳ���boll���

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
	bool mCloseScndOnly;//����ƽ���Ƿ��ƽ�ִ��������������ֱ������������
	time_duration mAvgInPositionDuration;// ƽ���ֲ�ʱ��
	time_duration mMaxInPositionDuration;// ���ֲ�ʱ��
	double mAvgWin;// ƽ��ӯ��
	double mMaxWin;// ���ӯ��
	double mAvgLose;// ƽ������
	double mMaxLose;// ������
	int mOrderCount;// �µ�����
	int mTradeCount;// �ɽ�����
#endif
	// ���ֵ�ʱ��
	string mOpenTime;
	// �������������еĳɽ������ۼ�ֵ�������Ҫ��ȥ�ϴγɽ�������óɽ�������
	TThostFtdcVolumeType primLastVolume;
	TThostFtdcVolumeType scndLastVolume;
	// ������������Ĳ���
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
	//��¼����һ�εĶ����������������ġ���ѯ����������
	ORDER_INDEX_TYPE lastPrimOrder;
	ORDER_INDEX_TYPE lastScndOrder;
	// ���������ȴ����ֲ������߳�ָ��
	boost::thread* mWaitPrimOpenThread;
	boost::thread* mWaitScndOpenThread;
	// ��������״̬���Ļ�����
	boost::mutex mStateMachineMutex;
	// �����������ݴ洢�Ļ�����
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
	// order index������ͬ��request��������Ӧ��Ӧ��
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
	InstrumentType mLatestInstType;//���µ��������ͣ����������Ǵ�����
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
	// �жϿ��������Ƿ��Ѿ��������㡣���۲�ͻ����㲼�ִ�ʱ����Ϊ�����������㣬
	// ����Ϊ�˱���۲�ڲ��ִ���Եʱ��������price bad�����������һ������
	// �����������ֱ���۲�����ڲ㲼�ִ����ڲŴ���price bad��
	/************************************************************************/
	void StopOpenJudge(CThostFtdcDepthMarketDataField const& pDepthMarketData);

	/************************************************************************/
	// �����ٲú����������ִ���ȴﵽ�趨ֵ�����������������Լ�ļ۲�����
	// 4�ֿ�������֮һ�����׳��¼�OPEN_PRICE_GOOD���ڿ��ִ�������Լʱ������
	// �����ᱻ�ٴ���֤��
	/************************************************************************/
	void OpenJudge(CThostFtdcDepthMarketDataField const& pDepthMarketData);

	/************************************************************************/
	// ֹӯ�жϺ��������۲���Բ�Ĳ��ִ�ʱ��ֹӯ����������
	/************************************************************************/
	bool StopWinJudge(CThostFtdcDepthMarketDataField const& pDepthMarketData);

	/************************************************************************/
	// �����¼��ӿڡ�
	/************************************************************************/
	void SetEvent(TRADE_EVENT aLatestEvent);

	/************************************************************************/
	// ͨ������ʱ�����̼��뵱ǰ�����¼ۼ��㸡ӯ��
	/************************************************************************/
	double EstimateProfit();

	/************************************************************************/
	// �ж��Ƿ��ں��������ʱ��
	/************************************************************************/
	bool IsDataTime(string aDataTime);

	/************************************************************************/
	// �ж��Ƿ��ڽ���ʱ��
	/************************************************************************/
	bool IsTradeTime(string aDataTime);

	/************************************************************************/
	// �ж��Ƿ��ڿ��Կ��ֵ�ʱ��
	/************************************************************************/
	bool IsOpenTime(string aDataTime);

	/************************************************************************/
	// �ж��Ƿ��ھ�����ֵ�ʱ��
	/************************************************************************/
	bool IsEasyGoTime(string aDataTime);

	/************************************************************************/
	// ���������Ĳ��ִ����
	/************************************************************************/
	double AdjustWinBollAmp(string aOpenTime, string aCurrentTime, double aWinBollAmp, double aAdjustVolume, double aDurationStep);
	
	/************************************************************************/
	// �жϷ�����ʱ���Ƿ��뱾��ʱ��һ��
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
	// ��ʼ��ʣ�ಿ��
	/************************************************************************/
	InitErrorType InitOtherCrap();

	/************************************************************************/
	// ��ʼ��trade process
	/************************************************************************/
	void InitTradeProcess();

	/************************************************************************/
	// ��ʼ��market process
	/************************************************************************/
	void InitMarketProcess();
	/*****************************/
public:
	
	/* below are all the thread routines */
	void WaitPrimOpen(int aId)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(stgArg.primOpenTime));
		//���û���µ�ID��������˵����ͣ���ڵ��εĲ���
		if((mStateMachine.GetState() == OPENING_PRIM_STATE) && (aId == mOpenPrimId))
		{
			logger.LogThisFast("[EVENT]: PRIM_OPEN_TIMEOUT");
			SetEvent(PRIM_OPEN_TIMEOUT);
		}
	}
	void WaitScndOpen(int aId)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(stgArg.scndOpenTime));
		//���û���µ�ID��������˵����ͣ���ڵ��εĲ���
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
	// �첽�¼����ͺ��������첽�ķ�ʽ�ӳٷ����¼�������ģ�⽻�ס�
	/************************************************************************/
	void AsyncEventPoster(TRADE_EVENT aEvent);

	/************************************************************************/
	// ������ڻز�
	/************************************************************************/
	void AsyncEventPoster(void);

	/************************************************************************/
	// �ز��У�OPENING_SCND_STATE״̬�µ��첽�¼���������
	/************************************************************************/
	void OpeningScndStateAsyncEventGenerator();

	/************************************************************************/
	// �ز��У�OPENING_PRIM_STATE״̬�µ��첽�¼���������
	/************************************************************************/
	void OpeningPrimStateAsyncEventGenerator();

	/************************************************************************/
	// �ز��У�CLOSING_BOTH_STATE״̬�µ��첽�¼���������
	/************************************************************************/
	void ClosingBothStateAsyncEventGenerator();

	/************************************************************************/
	// �ز��У�CLOSING_SCND_STATE״̬�µ��첽�¼���������
	/************************************************************************/
	void ClosingScndStateAsyncEventGenerator();

	/************************************************************************/
	// �ز��У�CLOSING_PRIM_STATE״̬�µ��첽�¼���������
	/************************************************************************/
	void ClosingPrimStateAsyncEventGenerator();

	/************************************************************************/
	// �ز��У�WAITING_PRIM_CLOSE_STATE״̬�µ��첽�¼���������
	/************************************************************************/
	void WaitingPrimCloseStateAsyncEventGenerator();

	/************************************************************************/
	// �ز��У�WAITING_SCND_CLOSE_STATE״̬�µ��첽�¼���������
	/************************************************************************/
	void WaitingScndCloseStateAsyncEventGenerator();

	/************************************************************************/
	// �ز��У�CANCELLING_SCND_STATE״̬�µ��첽�¼���������
	/************************************************************************/
	void CancellingScndStateAsyncEventGenerator();

	/************************************************************************/
	// �ز��У�CANCELLING_PRIM_STATE״̬�µ��첽�¼���������
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