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
	//�������������еĳɽ������ۼ�ֵ�������Ҫ��ȥ�ϴγɽ�������óɽ�������
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
	int mPrimTodayPosition;
	int mPrimYdPosition;
	int mScndTodayPosition;
	int mScndYdPosition;
	char mPrimPosDir;
	char mScndPosDir;
	bool mStart;
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
	/*****************************/
	/* auxalary routines */
	// check if this trade must be ended now
	bool StopLoseJudge(CThostFtdcDepthMarketDataField* pDepthMarketData)
	{
		bool lIsClose = false;
		if(mStateMachine.GetState() == PENDING_STATE)
		{
			if(OPEN_COND1 == mOpenCond)
			{
				if(primDataBuf[primBufIndex].lastPrice - scndDataBuf[scndBufIndex].lastPrice > stgArg.stopBollAmp*mBoll.GetBoll(0).mStdDev + mBoll.GetBoll(0).mMidLine);
				{
					SetEvent(MUST_STOP);
					lIsClose = true;
				}
			}
			else if(OPEN_COND2 == mOpenCond)
			{
				if(primDataBuf[primBufIndex].lastPrice - scndDataBuf[scndBufIndex].lastPrice < (-1)*stgArg.stopBollAmp*mBoll.GetBoll(0).mStdDev + mBoll.GetBoll(0).mMidLine)
				{
					SetEvent(MUST_STOP);
					lIsClose = true;
				}
			}
			else if(OPEN_COND3 == mOpenCond)
			{
				if(scndDataBuf[scndBufIndex].lastPrice - primDataBuf[primBufIndex].lastPrice > stgArg.stopBollAmp*mBoll.GetBoll(0).mStdDev + mBoll.GetBoll(0).mMidLine)
				{
					SetEvent(MUST_STOP);
					lIsClose = true;
				}
			}
			else if(OPEN_COND4 == mOpenCond)
			{
				if(scndDataBuf[scndBufIndex].lastPrice - primDataBuf[primBufIndex].lastPrice < (-1)*stgArg.stopBollAmp*mBoll.GetBoll(0).mStdDev + mBoll.GetBoll(0).mMidLine)
				{
					SetEvent(MUST_STOP);
					lIsClose = true;
				}
			}
		}
		
		return lIsClose;
	}
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
	/************************************************************************/
	// �жϿ��������Ƿ��Ѿ��������㡣���۲�ͻ����㲼�ִ�ʱ����Ϊ�����������㣬
	// ����Ϊ�˱���۲�ڲ��ִ���Եʱ��������price bad�����������һ������
	// �����������ֱ���۲�����ڲ㲼�ִ����ڲŴ���price bad��
	/************************************************************************/
	void StopOpenJudge()
	{
		if(mStateMachine.GetState() != OPENING_SCND_STATE)
		{
			return;
		}
		else
		{
			if(OPEN_COND1 == mOpenCond)
			{
				if(primDataBuf[primBufIndex].bidPrice - scndDataBuf[scndBufIndex].askPrice < mBoll.GetBoll(0).mInnerUpperLine);
				{
					logger.LogThisFast("[EVENT]: OPEN_PRICE_NOT_GOOD");
					SetEvent(OPEN_PRICE_BAD);
				}
			}
			else if(OPEN_COND2 == mOpenCond)
			{
				if(primDataBuf[primBufIndex].askPrice - scndDataBuf[scndBufIndex].bidPrice > mBoll.GetBoll(0).mInnerLowerLine)
				{
					logger.LogThisFast("[EVENT]: OPEN_PRICE_NOT_GOOD");
					SetEvent(OPEN_PRICE_BAD);
				}
			}
			else if(OPEN_COND3 == mOpenCond)
			{
				if(scndDataBuf[scndBufIndex].bidPrice - primDataBuf[primBufIndex].askPrice < mBoll.GetBoll(0).mInnerUpperLine)
				{
					logger.LogThisFast("[EVENT]: OPEN_PRICE_NOT_GOOD");
					SetEvent(OPEN_PRICE_BAD);
				}
			}
			else if(OPEN_COND4 == mOpenCond)
			{
				if(scndDataBuf[scndBufIndex].askPrice - primDataBuf[primBufIndex].bidPrice > mBoll.GetBoll(0).mInnerLowerLine)
				{
					logger.LogThisFast("[EVENT]: OPEN_PRICE_NOT_GOOD");
					SetEvent(OPEN_PRICE_BAD);
				}
			}
			else
			{
				logger.LogThisFast("[FATAL ERROR]: ILLEGAL OPEN COND");
			}
		}
	}
	/************************************************************************/
	// �����ٲú����������ִ���ȴﵽ�趨ֵ�����������������Լ�ļ۲�����
	// 4�ֿ�������֮һ�����׳��¼�OPEN_PRICE_GOOD���ڿ��ִ�������Լʱ������
	// �����ᱻ�ٴ���֤��
	/************************************************************************/
	void OpenJudge(CThostFtdcDepthMarketDataField* pDepthMarketData)
	{
		// if the bollinger band is not wide enough, then return
		if(mBoll.GetBoll(0).mOutterUpperLine - mBoll.GetBoll(0).mOutterLowerLine < stgArg.bollAmpLimit)
		{
			return;
		}
		// using bid_price - ask_price to do the open timing judge, this is rougher to meet
		if( primDataBuf[primBufIndex].bidPrice - scndDataBuf[scndBufIndex].askPrice > 0 &&
			primDataBuf[primBufIndex].bidPrice - scndDataBuf[scndBufIndex].askPrice > mBoll.GetBoll(0).mOutterUpperLine )
		{
			/* condition 1 */
			logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND1");
			SetEvent(OPEN_PRICE_GOOD);
		}
		else if( primDataBuf[primBufIndex].askPrice - scndDataBuf[scndBufIndex].bidPrice > 0 &&
			primDataBuf[primBufIndex].askPrice - scndDataBuf[scndBufIndex].bidPrice < mBoll.GetBoll(0).mOutterLowerLine )
		{
			/* condition 2 */
			logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND2");
			SetEvent(OPEN_PRICE_GOOD);
		}
		else if( scndDataBuf[scndBufIndex].bidPrice - primDataBuf[primBufIndex].askPrice > 0 &&
			scndDataBuf[scndBufIndex].bidPrice - primDataBuf[primBufIndex].askPrice > mBoll.GetBoll(0).mOutterUpperLine )
		{
			/* condition 3 */
			logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND3");
			SetEvent(OPEN_PRICE_GOOD);
		}
		else if( scndDataBuf[scndBufIndex].askPrice - primDataBuf[primBufIndex].bidPrice > 0 &&
			scndDataBuf[scndBufIndex].askPrice - primDataBuf[primBufIndex].bidPrice < mBoll.GetBoll(0).mOutterLowerLine )
		{
			/* condition 4 */
			logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND4");
			SetEvent(OPEN_PRICE_GOOD);
		}
	}
	/************************************************************************/
	// ֹӯ�жϺ��������۲���Բ�Ĳ��ִ�ʱ��ֹӯ����������
	/************************************************************************/
	bool StopWinJudge()
	{
		bool lGoodToClose = false;
		if(mStateMachine.GetState() == PENDING_STATE)
		{
			if(OPEN_COND1 == mOpenCond)
			{
				if(primDataBuf[primBufIndex].lastPrice - scndDataBuf[scndBufIndex].lastPrice <= mBoll.GetBoll(0).mOutterLowerLine);
				{
					SetEvent(MUST_STOP);
					lGoodToClose = true;
				}
			}
			else if(OPEN_COND2 == mOpenCond)
			{
				if(primDataBuf[primBufIndex].lastPrice - scndDataBuf[scndBufIndex].lastPrice >= mBoll.GetBoll(0).mOutterUpperLine)
				{
					SetEvent(MUST_STOP);
					lGoodToClose = true;
				}
			}
			else if(OPEN_COND3 == mOpenCond)
			{
				if(scndDataBuf[scndBufIndex].lastPrice - primDataBuf[primBufIndex].lastPrice <= mBoll.GetBoll(0).mOutterLowerLine)
				{
					SetEvent(MUST_STOP);
					lGoodToClose = true;
				}
			}
			else if(OPEN_COND4 == mOpenCond)
			{
				if(scndDataBuf[scndBufIndex].lastPrice - primDataBuf[primBufIndex].lastPrice >= mBoll.GetBoll(0).mOutterUpperLine)
				{
					SetEvent(MUST_STOP);
					lGoodToClose = true;
				}
			}
		}
		return lGoodToClose;
	}
	/************************************************************************/
	// �����¼��ӿڡ�
	/************************************************************************/
	void SetEvent(TRADE_EVENT aLatestEvent)
	{
		// using lock guard to invoke lock and unlock automatically
		boost::lock_guard<boost::mutex> lLockGuard(mStateMachineMutex);
		TRADE_STATE lLastState = mStateMachine.GetState();
		TRADE_STATE lNextState = mStateMachine.SetEvent(aLatestEvent);
		
		switch(lNextState)
		{
		case IDLE_STATE:
			/* do nothing */
			break;
		case OPENING_SCND_STATE:
			// avoiding multiple open
			if(lNextState != lLastState)
			{
				OpenScnd();
			}
			ReqQryInvestorPosition(stgArg.secondaryInst.c_str());
			break;
		case OPENING_PRIM_STATE:
			// avoiding multiple open
			if(lNextState != lLastState)
			{
				OpenPrim();
				
			}
			ReqQryInvestorPosition(stgArg.primaryInst.c_str());
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
		if(config.ReadInteger(stgArg.stopBollAmp, "StopBollAmp") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"StopBollAmp\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"StopBollAmp\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.bollAmpLimit, "BollAmpLimit") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"BollAmpLimit\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"BollAmpLimit\" in config file");
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
	//void HookOnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData);
	
	void LogBollData()
	{
		BollingerBandData tempData = mBoll.GetBoll(0);
		tempStream.clear();
		tempStream.str("");
		tempStream<<primDataBuf[primBufIndex].lastPrice<<"	"<<scndDataBuf[scndBufIndex].lastPrice<<"	"<<tempData.mMidLine<<"	"<<tempData.mStdDev<<"	"<<tempData.mOutterUpperLine<<"	"<<tempData.mOutterLowerLine<<"	"<<tempData.mInnerUpperLine<<"	"<<tempData.mInnerLowerLine;
		mBollLog.LogThisFast(tempStream.str());
	}
	///�ɽ�֪ͨ
	//void OnRtnTrade(CThostFtdcTradeField* pTrade);
	///����֪ͨ
    //virtual void OnRtnOrder(CThostFtdcOrderField* pOrder);
	// ��ѯ��λ��Ӧ����
	//void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	/* below are all the thread routines */
	void WaitPrimOpen()
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(stgArg.primOpenTime));
		SetEvent(PRIM_OPEN_TIMEOUT);
	}
	void WaitScndOpen()
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(stgArg.scndOpenTime));
		SetEvent(SCND_OPEN_TIMEOUT);
	}
	/*****************************/
public:
	/* strategy entry */
	void StartStrategy(void)
	{
		bool trading = true;
		mStart = true;
		string input;
		if(InitOtherCrap() == ALL_GOOD)
		{
			InitMarketProcess();
			InitTradeProcess();
		}
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
			input.clear();
		}
	}
};
}