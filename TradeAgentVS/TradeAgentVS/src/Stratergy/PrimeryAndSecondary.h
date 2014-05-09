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
	// state machine object
	PasStateMachine mStateMachine;
	// trade direction
	TRADE_DIR mTradeDir;
	//记录最新一次的订单索引，用来更改、查询、撤销订单
	ORDER_INDEX_TYPE lastPrimOrder;
	ORDER_INDEX_TYPE lastScndOrder;
	// 用来阻塞等待开、平操作的线程指针
	boost::thread* mWaitPrimOpenThread;
	boost::thread* mWaitScndOpenThread;
	boost::thread* mWaitPrimCloseThread;
	boost::thread* mWaitScndCloseThread;
	// 用来保护状态机的互斥锁
	boost::mutex mStateMachineMutex;
	// 用来保护数据存储的互斥锁
	boost::mutex mBufferDataMutex;
public:
	// constructor
	PrimeryAndSecondary(void)
	{
		
	}
private:
	/*****************************/
	/* auxalary routines */
	// store market data into local buffer
	bool BufferData(CThostFtdcDepthMarketDataField* pDepthMarketData)
	{
		boost::lock_guard<boost::mutex> lLockGuard(mBufferDataMutex);
		if (VerifyMarketData(*pDepthMarketData))
		{
			//recognize instrument id and put data into where they should go
			if(strncmp(pDepthMarketData->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.size()) == 0)
			{
				// don't forget to increase the index
				primBufIndex++;
				// increase vector size when it's almost full
				if(primDataBuf.size() - primBufIndex < 100)
				{
					primDataBuf.resize(primDataBuf.size()+STRATEGY_BUFFER_SIZE);
				}
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
				
			}
			else if(strncmp(pDepthMarketData->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.size()) == 0)
			{
				// don't forget to increase the index
				scndBufIndex++;
				// increase vector size when it's almost full
				if(scndDataBuf.size() - scndBufIndex < 100)
				{
					scndDataBuf.resize(scndDataBuf.size()+STRATEGY_BUFFER_SIZE);
				}
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
		// using bid_price - ask_price to do the open timing judge, this is rougher to meet
		if( primDataBuf[primBufIndex].bidPrice - scndDataBuf[scndBufIndex].askPrice > 0 &&
			primDataBuf[primBufIndex].bidPrice - scndDataBuf[scndBufIndex].askPrice > mBoll.GetBollInt(0).mOutterUpperLine )
		{
			/* condition 1 */
			logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND1");
			SetEvent(OPEN_PRICE_GOOD);
		}
		else if( primDataBuf[primBufIndex].askPrice - scndDataBuf[scndBufIndex].bidPrice > 0 &&
			primDataBuf[primBufIndex].askPrice - scndDataBuf[scndBufIndex].bidPrice < mBoll.GetBollInt(0).mOutterLowerLine )
		{
			/* condition 2 */
			logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND2");
			SetEvent(OPEN_PRICE_GOOD);
		}
		else if( scndDataBuf[primBufIndex].bidPrice - primDataBuf[scndBufIndex].askPrice > 0 &&
			scndDataBuf[primBufIndex].bidPrice - primDataBuf[scndBufIndex].askPrice > mBoll.GetBollInt(0).mOutterUpperLine )
		{
			/* condition 3 */
			logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND3");
			SetEvent(OPEN_PRICE_GOOD);
		}
		else if( scndDataBuf[primBufIndex].askPrice - primDataBuf[scndBufIndex].bidPrice > 0 &&
			scndDataBuf[primBufIndex].askPrice - primDataBuf[scndBufIndex].bidPrice < mBoll.GetBollInt(0).mOutterLowerLine )
		{
			/* condition 4 */
			logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND4");
			SetEvent(OPEN_PRICE_GOOD);
		}
		else
		{
			// maybe this shouldn't be here
			logger.LogThisFast("[EVENT]: OPEN_PRICE_NOT_GOOD");
			SetEvent(OPEN_PRICE_BAD);
		}
	}
	/*****************************/

	/*****************************/
	/* all the action routines and state machine interface */
	void SetEvent(TRADE_EVENT aLatestEvent)
	{
		// using lock guard to call lock and unlock automatically
		boost::lock_guard<boost::mutex> lLockGuard(mStateMachineMutex);
		TRADE_STATE lLastState = mStateMachine.GetState();
		TRADE_STATE lNextState = mStateMachine.SetEvent(aLatestEvent);
		if(lNextState == lLastState)
		{
			return;
		}
		else
		{
			switch(lNextState)
			{
			case IDLE_STATE:
				/* do nothing */
				break;
			case OPENING_SCND_STATE:
				OpenScnd();
				break;
			case OPENING_PRIM_STATE:
				OpenPrim();
				break;
			case PENDING_STATE:
				/* do nothing */
				break;
			case CLOSING_BOTH_STATE:
				CloseBoth();
				break;
			case CANCELLING_SCND_STATE:
				CancelScnd();
				break;
			case CLOSING_SCND_STATE:
				CloseScnd();
				break;
			case CANCELLING_PRIM_STATE:
				CancelPrim();
				break;
			case WAITING_SCND_CLOSE_STATE:
				/* do nothing */
				break;
			case WAITING_PRIM_CLOSE_STATE:
				/* do nothing */
				break;
			default:
				break;
			}
		}
	}
	void OpenScnd()
	{
		if( primDataBuf[primBufIndex].bidPrice - scndDataBuf[scndBufIndex].askPrice > 0 &&
			primDataBuf[primBufIndex].bidPrice - scndDataBuf[scndBufIndex].askPrice > mBoll.GetBollInt(0).mOutterUpperLine )
		{
			/* condition 1 */
			logger.LogThisFast("[ACTION]: BUY_SCND");
			mTradeDir = BUY_SCND_SELL_PRIM;
			ReqOrderInsert(	scndDataBuf[scndBufIndex].instrumentId, 
							scndDataBuf[scndBufIndex].bidPrice,
							stgArg.openShares,
							THOST_FTDC_D_Buy,
							THOST_FTDC_OF_Open,
							&lastScndOrder,
							THOST_FTDC_HF_Speculation);
			mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this));
		}
		else if( primDataBuf[primBufIndex].askPrice - scndDataBuf[scndBufIndex].bidPrice > 0 &&
			primDataBuf[primBufIndex].askPrice - scndDataBuf[scndBufIndex].bidPrice < mBoll.GetBollInt(0).mOutterLowerLine )
		{
			/* condition 2 */
			logger.LogThisFast("[ACTION]: SHORT_SCND");
			mTradeDir = BUY_PRIM_SELL_SCND;
			ReqOrderInsert(	scndDataBuf[scndBufIndex].instrumentId, 
							scndDataBuf[scndBufIndex].askPrice,
							stgArg.openShares,
							THOST_FTDC_D_Sell,
							THOST_FTDC_OF_Open,
							&lastScndOrder,
							THOST_FTDC_HF_Speculation);
			mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this));
		}
		else if( scndDataBuf[primBufIndex].bidPrice - primDataBuf[scndBufIndex].askPrice > 0 &&
			scndDataBuf[primBufIndex].bidPrice - primDataBuf[scndBufIndex].askPrice > mBoll.GetBollInt(0).mOutterUpperLine )
		{
			/* condition 3 */
			logger.LogThisFast("[ACTION]: SHORT_SCND");
			mTradeDir = BUY_PRIM_SELL_SCND;
			ReqOrderInsert(	scndDataBuf[scndBufIndex].instrumentId, 
							scndDataBuf[scndBufIndex].askPrice,
							stgArg.openShares,
							THOST_FTDC_D_Sell,
							THOST_FTDC_OF_Open,
							&lastScndOrder,
							THOST_FTDC_HF_Speculation);
			mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this));
		}
		else if( scndDataBuf[primBufIndex].askPrice - primDataBuf[scndBufIndex].bidPrice > 0 &&
			scndDataBuf[primBufIndex].askPrice - primDataBuf[scndBufIndex].bidPrice < mBoll.GetBollInt(0).mOutterLowerLine )
		{
			/* condition 4 */
			logger.LogThisFast("[ACTION]: BUY_SCND");
			mTradeDir = BUY_SCND_SELL_PRIM;
			ReqOrderInsert(	scndDataBuf[scndBufIndex].instrumentId, 
							scndDataBuf[scndBufIndex].bidPrice,
							stgArg.openShares,
							THOST_FTDC_D_Buy,
							THOST_FTDC_OF_Open,
							&lastScndOrder,
							THOST_FTDC_HF_Speculation);
			mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this));
		}
		else
		{
			logger.LogThisFast("[ERROR]: wrong open scnd condition");
			cout<<"[ERROR]: wrong open scnd condition"<<endl;
		}
	}
	void OpenPrim()
	{
		if(BUY_SCND_SELL_PRIM == mTradeDir)
		{
			logger.LogThisFast("[ACTION]: SHORT_PRIM");
			ReqOrderInsert(	primDataBuf[primBufIndex].instrumentId, 
							primDataBuf[primBufIndex].lowerLimit,
							stgArg.openShares,
							THOST_FTDC_D_Sell,
							THOST_FTDC_OF_Open,
							&lastPrimOrder,
							THOST_FTDC_HF_Speculation);
			mWaitPrimOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitPrimOpen, this));
		}
		else if(BUY_PRIM_SELL_SCND == mTradeDir)
		{
			logger.LogThisFast("[ACTION]: BUY_PRIM");
			ReqOrderInsert(	primDataBuf[primBufIndex].instrumentId, 
							primDataBuf[primBufIndex].upperLimit,
							stgArg.openShares,
							THOST_FTDC_D_Buy,
							THOST_FTDC_OF_Open,
							&lastPrimOrder,
							THOST_FTDC_HF_Speculation);
			mWaitPrimOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitPrimOpen, this));
		}
		else
		{
			logger.LogThisFast("[FATAL ERROR]: wrong open prim condition");
			cout<<"[FATAL ERROR]: wrong open prim condition"<<endl;
		}
	}
	void CloseScnd()
	{
		if(BUY_SCND_SELL_PRIM == mTradeDir)
		{
			logger.LogThisFast("[ACTION]: SELL_SCND");
			ReqOrderInsert(	scndDataBuf[scndBufIndex].instrumentId, 
							scndDataBuf[scndBufIndex].lowerLimit,
							stgArg.openShares,
							THOST_FTDC_D_Sell,
							THOST_FTDC_OF_CloseToday,
							&lastScndOrder,
							THOST_FTDC_HF_Speculation);
			mWaitScndCloseThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndClose, this));
		}
		else if(BUY_PRIM_SELL_SCND == mTradeDir)
		{
			logger.LogThisFast("[ACTION]: COVER_SCND");
			ReqOrderInsert(	scndDataBuf[scndBufIndex].instrumentId, 
							scndDataBuf[scndBufIndex].upperLimit,
							stgArg.openShares,
							THOST_FTDC_D_Buy,
							THOST_FTDC_OF_CloseToday,
							&lastScndOrder,
							THOST_FTDC_HF_Speculation);
			mWaitScndCloseThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndClose, this));
		}
		else
		{
			logger.LogThisFast("[FATAL ERROR]: wrong close scnd condition");
			cout<<"[FATAL ERROR]: wrong close scnd condition"<<endl;
		}
	}
	void ClosePrim()
	{
		if(BUY_SCND_SELL_PRIM == mTradeDir)
		{
			logger.LogThisFast("[ACTION]: COVER_PRIM");
			ReqOrderInsert(	primDataBuf[primBufIndex].instrumentId, 
							primDataBuf[primBufIndex].upperLimit,
							stgArg.openShares,
							THOST_FTDC_D_Buy,
							THOST_FTDC_OF_CloseToday,
							&lastPrimOrder,
							THOST_FTDC_HF_Speculation);
			mWaitPrimCloseThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitPrimClose, this));
		}
		else if(BUY_PRIM_SELL_SCND == mTradeDir)
		{
			logger.LogThisFast("[ACTION]: SELL_SCND");
			ReqOrderInsert(	primDataBuf[primBufIndex].instrumentId, 
							primDataBuf[primBufIndex].lowerLimit,
							stgArg.openShares,
							THOST_FTDC_D_Sell,
							THOST_FTDC_OF_CloseToday,
							&lastPrimOrder,
							THOST_FTDC_HF_Speculation);
			mWaitPrimCloseThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitPrimClose, this));
		}
		else
		{
			logger.LogThisFast("[FATAL ERROR]: wrong close prim condition");
			cout<<"[FATAL ERROR]: wrong close prim condition"<<endl;
		}
	}
	void CancelScnd()
	{
		ReqOrderAction( scndDataBuf[scndBufIndex].instrumentId,
						lastScndOrder.orderRef);
	}
	void CancelPrim()
	{
		ReqOrderAction(	primDataBuf[primBufIndex].instrumentId,
						lastPrimOrder.orderRef);
	}
	void CloseBoth()
	{
		CloseScnd();
		ClosePrim();
	}
	/*****************************/
	
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

	/*****************************/
	/* below are all the callback routines*/
	// strategy should be initiated by market data
	virtual void HookOnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
	{
		if(BufferData(pDepthMarketData) 
			&& strncmp(pDepthMarketData->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
		{
			//calculate Boll Band
			mBoll.CalcBoll(abs(primDataBuf[primBufIndex].lastPrice-scndDataBuf[scndBufIndex].lastPrice), stgArg.bollPeriod, stgArg.outterBollAmp, stgArg.innerBollAmp);
			OpenJudge(pDepthMarketData);
		}
		BollingerBandData lBoll = mBoll.GetBoll(0);
		cout<<pDepthMarketData->LastPrice<<" "<<lBoll.mMidLine<<" "<<lBoll.mOutterUpperLine<<" "<<lBoll.mOutterLowerLine<<endl;
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
			//StateMachine(MAX_EVENT);
		}
	}
	///成交通知
	void OnRtnTrade(CThostFtdcTradeField* pTrade)
	{
		if((pTrade->OffsetFlag == THOST_FTDC_OF_CloseToday)||
			(pTrade->OffsetFlag == THOST_FTDC_OF_Close)||
			(pTrade->OffsetFlag == THOST_FTDC_OF_ForceClose))
		{
			if(strncmp(pTrade->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
			{
				logger.LogThisFast("[EVENT]: SCND_CLOSED");
			}
			if(strncmp(pTrade->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
			{
				logger.LogThisFast("[EVENT]: PRIM_CLOSED");
			}
		}
		if(pTrade->OffsetFlag == THOST_FTDC_OF_Open)
		{
			if(strncmp(pTrade->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
			{
				logger.LogThisFast("[EVENT]: SCND_OPENED");
				SetEvent(SCND_OPENED);
			}
			if(strncmp(pTrade->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
			{
				logger.LogThisFast("[EVENT]: PRIM_OPENED");
				SetEvent(PRIM_OPENED);
			}
		}

	}
	///报单通知
    virtual void OnRtnOrder(CThostFtdcOrderField* pOrder)
	{
		//cout<<"---> OnRtnOrder: "<<std::endl;
		//cout<<"------> Instrument ID: "<<pOrder->InstrumentID<<endl;
		//cout<<"------> Order Status: "<<pOrder->OrderStatus<<endl;
		//cout<<"------> Cancel Time: "<<pOrder->CancelTime<<endl;
		//cout<<"------> Status Message: "<<pOrder->StatusMsg<<endl;
		//cout<<"------> Order Submit Status: "<<pOrder->OrderSubmitStatus<<endl;
		if((pOrder->OrderStatus == THOST_FTDC_OST_Canceled) && (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted))
		{
			//撤单回报
			if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
			{
				logger.LogThisFast("[EVENT]: SCND_CANCELLED");
				SetEvent(SCND_CANCELLED);
			}
			if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
			{
				logger.LogThisFast("[EVENT]: PRIM_CANCELLED");
				SetEvent(PRIM_CANCELLED);
			}
			
		}
		if((pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)
			&&(pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted)
			&&(pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday))
		{
			//平仓回报
			if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
			{
				logger.LogThisFast("[EVENT]: SCND_CLOSED");
				SetEvent(SCND_CLOSED);
			}
			if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
			{
				logger.LogThisFast("[EVENT]: PRIM_CLOSED");
				SetEvent(PRIM_CLOSED);
			}
		}
		if((pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)
			&&(pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted)
			&&(pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Open))
		{
			//开仓回报
			if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
			{
				logger.LogThisFast("[EVENT]: SCND_OPENED");
				SetEvent(SCND_OPENED);
			}
			if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
			{
				logger.LogThisFast("[EVENT]: PRIM_OPENED");
				SetEvent(PRIM_OPENED);
			}
		}
	}
	/*****************************/

	/*****************************/
	/* below are all the thread routines */
	void WaitPrimOpen()
	{
		boost::this_thread::sleep_for(boost::chrono::microseconds(stgArg.primOpenTime));
		SetEvent(PRIM_OPEN_TIMEOUT);
	}
	void WaitScndOpen()
	{
		boost::this_thread::sleep_for(boost::chrono::microseconds(stgArg.scndOpenTime));
		SetEvent(SCND_OPEN_TIMEOUT);
	}
	void WaitPrimClose()
	{
		boost::this_thread::sleep_for(boost::chrono::microseconds(stgArg.primCloseTime));
		SetEvent(PRIM_CLOSE_TIMEOUT);
	}
	void WaitScndClose()
	{
		boost::this_thread::sleep_for(boost::chrono::microseconds(stgArg.primCloseTime));
		SetEvent(PRIM_CLOSE_TIMEOUT);
	}
	/*****************************/
public:
	/* strategy entry */
	void StartStrategy(void)
	{
		bool trading = true;
		string input;
		if(InitOtherCrap() == ALL_GOOD)
		{
			InitMarketProcess();
			InitTradeProcess();
		}
		while(trading)
		{
			cin>>input;
		}
	}
};
}