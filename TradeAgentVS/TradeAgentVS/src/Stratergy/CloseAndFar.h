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
#include <StrategyAux.h>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <Timer.h>
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost;
using namespace std;
using namespace boost::asio;
#define STRATEGY_BUFFER_SIZE 100
#define PRICE_UPPER_LIM 100000
class CloseAndFar : public TradeProcess, public Hook, public StrategyAux, public Timer
{
private:
	// config reader obj
	ConfigReader config;
	// market process obj
	MarketProcess marketObj;
	// total number of instruments that read from configuration file
	int totalInstrument;
	// keep all the instruments that read from configuration file
	char* instrumentList[MAX_INSTRUMENT];
	// container store instrumentID as key, and its serial as value
	typedef unordered_map<string, int> instrumentMapType;
	instrumentMapType instrumentMap;
	// Used to buffer market data. This will consume about 100*MAX_INSTRUMENT*STRATEGY_BUFFER_SIZE bytes in stack
	BasicMarketData dataBuf[MAX_INSTRUMENT][STRATEGY_BUFFER_SIZE];
	// used to store the index position of every instruments' data buf
	int posInBuf[MAX_INSTRUMENT];
	ORDER_INDEX_TYPE lastOrder;//记录最新一次的订单索引，用来更改、查询、撤销订单
	Log logger;//日志
	TThostFtdcVolumeType lastVolume;//由于行情数据中的成交量是累加值，因此需要减去上次成交量来获得成交量增量
	// object to structure of strategy parameter
	StrategyParameter stgArg;
	// temp string stream, used for storing messages
	stringstream tempStream;
public:
	CloseAndFar(void)
	{
		for(int i=0; i<MAX_INSTRUMENT; i++)
		{
			instrumentList[i] = new char[INSTRUMENT_SIZE];
		}
	}
	~CloseAndFar(void)
	{
		
		for(int i=0; i<MAX_INSTRUMENT; i++)
		{
			delete instrumentList[i];
		}
	}
	

	// strategy should be initiated by market data
	virtual void HookOnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
	{
		BufferData(pDepthMarketData);
		CAFStrategy(pDepthMarketData);

		std::cout<<pDepthMarketData->InstrumentID<<" last bid: "<<pDepthMarketData->BidPrice1<<std::endl;
	}
private:

	void InitOtherCrap()
	{
		initStatus = ALL_GOOD;
		memset(dataBuf, 0, sizeof(dataBuf));
		memset(posInBuf, 0, sizeof(posInBuf));
		//belows are essential strategy parameters
		if(config.ReadInteger(stgArg.minMove,"MinMove") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"MinMove\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"MinMove\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadDouble(stgArg.mainPriceDelta, "MainPriceDelta") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"MainPriceDelta\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"MainPriceDelta\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadDouble(stgArg.secPriceDiff, "SecPriceDiff") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"SecPriceDiff\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"SecPriceDiff\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.secVolumeDiff, "SecVolumeDiff") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"SecVolumeDiff\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"SecVolumeDiff\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.openCloseVolume, "OpenCloseVolume") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"OpenCloseVolume\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"OpenCloseVolume\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadInteger(stgArg.maxOpenTime, "MaxOpenTime") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"MaxOpenTime\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"MaxOpenTime\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadDouble(stgArg.openPriceConst, "OpenPriceConst") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"OpenPriceConst\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"OpenPriceConst\" in config file");
			initStatus = CONFIG_ERROR;
		}
		if(config.ReadDouble(stgArg.closePriceConst, "ClosePriceConst") != 0)
		{
			cout<<"[ERROR]: Can't find symble \"ClosePriceConst\" in config file"<<endl;
			logger.LogThisFast("[ERROR]: Can't find symble \"ClosePriceConst\" in config file");
			initStatus = CONFIG_ERROR;
		}		
		stgArg.floatError = 0.00001;

		if(ALL_GOOD == initStatus)
		{
			totalInstrument = config.ReadInstrumentID(instrumentList);
			cout<<"Load Instrument Map..."<<endl;
			for(int i=0; i<totalInstrument; i++)
			{
				instrumentMap[instrumentList[i]] =  i;
				cout<<"Instrument Map: "<<instrumentList[i]<<" ID: "<<instrumentMap.at(instrumentList[i])<<endl;
			}
		}
		else
		{
			cout<<"Strategy Arguments are not valid, strategy will not be loaded"<<endl;
		}
		lastVolume = 0;
		SetState(IDLE_STATE);
		
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
	void SetState(TRADE_STATE newState)
	{
		tradeState = newState;
		string message = "Current State: ";
		message.append(ShowTradeState());
		logger.LogThisFast(message);
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
			StateMachine(OPEN_OVERTIME);
		}
	}
	// state machine, takes Trade Event as input and actions will be taken based on the current Trade State
	void StateMachine(TRADE_EVENT tradeEvent)
	{
		string message = "Last Event: ";
		message.append(ShowTradeEvent(tradeEvent));
		logger.LogThisFast(message);
		switch(tradeState)
		{
		case IDLE_STATE:
			//若达到开仓条件，则建仓并进入OPEN_STATE
			if(BUY_PRICE_GOOD == tradeEvent)
			{
				//开仓次主力
				
				int posInBufSec = posInBuf[SEC_INSTRUMENT];
				ReqOrderInsert(dataBuf[SEC_INSTRUMENT][posInBufSec].instrumentId,
								dataBuf[SEC_INSTRUMENT][posInBufSec].askPrice + stgArg.openPriceConst,
								stgArg.openCloseVolume,
								THOST_FTDC_D_Buy, 
								THOST_FTDC_OF_Open,
								&lastOrder);
				SetState(OPEN_STATE);
				StartTimer(stgArg.maxOpenTime);//start timer
				tempStream.clear();
				tempStream.str("");
				tempStream<<"Trying to open ["<<instrumentList[SEC_INSTRUMENT]<<"] with price ["<<dataBuf[SEC_INSTRUMENT][posInBufSec].askPrice + stgArg.openPriceConst<<"], order reference is ["<<lastOrder.orderRef<<"].";
				logger.LogThisFast(tempStream.str());
			}
			break;
		case OPEN_STATE:
			//若成交，则建仓成功，开始等待平仓条件，进入SUSPEND_STATE
			if(TRADED == tradeEvent)
			{
				
				SetState(SUSPEND_STATE);	
			}
			else if((OPEN_OVERTIME == tradeEvent))
			{
				tempStream.clear();
				tempStream.str("");
				tempStream<<"Trying to cancel ["<<instrumentList[SEC_INSTRUMENT]<<"], order reference is ["<<lastOrder.orderRef<<"].";
				logger.LogThisFast(tempStream.str());
				ReqOrderAction(instrumentList[SEC_INSTRUMENT],lastOrder.orderRef);
				SetState(CANCEL_OPEN_STATE);
			}
			break;
		case SUSPEND_STATE:
			if(MUST_STOP == tradeEvent)
			{
				TThostFtdcPriceType forceStopPrice;
				int posInBufSec = posInBuf[SEC_INSTRUMENT];
				forceStopPrice = dataBuf[SEC_INSTRUMENT][posInBufSec].lowerLimit;//使用跌停板价格来平仓
				ReqOrderInsert(instrumentList[SEC_INSTRUMENT], 
								forceStopPrice, 
								stgArg.openCloseVolume, 
								THOST_FTDC_D_Sell, 
								THOST_FTDC_OF_CloseToday,
								&lastOrder);
				SetState(FORCE_CLOSE_STATE);

				tempStream.clear();
				tempStream.str("");
				tempStream<<"Trying to force close ["<<instrumentList[SEC_INSTRUMENT]<<"] with price ["<<forceStopPrice<<"].";
				logger.LogThisFast(tempStream.str());
			}
			break;
		case FORCE_CLOSE_STATE:
			if(TRADED == tradeEvent)
			{
				SetState(IDLE_STATE);
			}
			break;
		case CANCEL_OPEN_STATE:
			if(TRADED == tradeEvent)
			{
				TThostFtdcPriceType forceStopPrice;
				int posInBufSec = posInBuf[SEC_INSTRUMENT];
				forceStopPrice = dataBuf[SEC_INSTRUMENT][posInBufSec].lowerLimit;//使用跌停板价格来平仓
				ReqOrderInsert(instrumentList[SEC_INSTRUMENT], 
								forceStopPrice, 
								stgArg.openCloseVolume, 
								THOST_FTDC_D_Sell, 
								THOST_FTDC_OF_CloseToday,
								&lastOrder);
				SetState(FORCE_CLOSE_STATE);

				tempStream.clear();
				tempStream.str("");
				tempStream<<"Trying to force close ["<<instrumentList[SEC_INSTRUMENT]<<"] with price ["<<forceStopPrice<<"].";
				logger.LogThisFast(tempStream.str());
			}
			else if(CANCELED == tradeEvent)
			{
				SetState(IDLE_STATE);
			}
			break;
		default:
			break;
		}
	}

	///成交通知
	void OnRtnTrade(CThostFtdcTradeField* pTrade)
	{
		boost::posix_time::ptime localTime = boost::posix_time::microsec_clock::local_time();
		cout<<"---> Trade return at "<<localTime.time_of_day()<<endl;
		tempStream.clear();
		tempStream.str("");
		if(pTrade->OffsetFlag == THOST_FTDC_OF_Open)
		{
			tempStream<<"Opened ["<<pTrade->InstrumentID<<"] at price ["<<pTrade->Price<<"].";
		}
		// if this trade is close, then calculate how much do i earn in the last trade, and log it
		if((pTrade->OffsetFlag == THOST_FTDC_OF_CloseToday)||
			(pTrade->OffsetFlag == THOST_FTDC_OF_Close)||
			(pTrade->OffsetFlag == THOST_FTDC_OF_ForceClose))
		{
			tempStream<<"Closed ["<<pTrade->InstrumentID<<"] at price ["<<pTrade->Price<<"]. ";
			tempStream<<"Profit during last trade is ["<<pTrade->Price-lastTrade.tradePrice<<"].";
		}
		logger.LogThisFast(tempStream.str());
		StoreLastTrade(pTrade);
	}
	///报单通知
    virtual void OnRtnOrder(CThostFtdcOrderField* pOrder)
	{
		cout<<"---> OnRtnOrder: "<<endl;
		cout<<"------> Instrument ID: "<<pOrder->InstrumentID<<endl;
		cout<<"------> Order Status: "<<pOrder->OrderStatus<<endl;
		cout<<"------> Cancel Time: "<<pOrder->CancelTime<<endl;
		cout<<"------> Status Message: "<<pOrder->StatusMsg<<endl;
		cout<<"------> Order Submit Status: "<<pOrder->OrderSubmitStatus<<endl;
		if((pOrder->OrderStatus == THOST_FTDC_OST_Canceled) && (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted))
		{
			tempStream.clear();
			tempStream.str("");
			tempStream<<"Order ["<<pOrder->OrderRef<<"] has been canceled. ";
			logger.LogThisFast(tempStream.str());
			StateMachine(CANCELED);
		}
		if((pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)&&(pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted))
		{
			CancelTimer();
			StateMachine(TRADED);
		}
	}
	void BufferData(CThostFtdcDepthMarketDataField* pDepthMarketData)
	{
		int instrumentIndex = 0; // index to identify instrument
		instrumentIndex = instrumentMap.at(pDepthMarketData->InstrumentID);
		// increase the index before update the buffer, otherwise you need to decrease the index first to get the latest value
		if(++posInBuf[instrumentIndex] >= STRATEGY_BUFFER_SIZE)
		{
			posInBuf[instrumentIndex] = 0;
		}
		int posIndex = posInBuf[instrumentIndex]; // index to identify the position in FIFO of current data
		
		strcpy(dataBuf[instrumentIndex][posIndex].instrumentId, pDepthMarketData->InstrumentID);
		dataBuf[instrumentIndex][posIndex].lastPrice = pDepthMarketData->LastPrice;
		dataBuf[instrumentIndex][posIndex].volume = pDepthMarketData->Volume;
		dataBuf[instrumentIndex][posIndex].askPrice = pDepthMarketData->AskPrice1;
		dataBuf[instrumentIndex][posIndex].askVolume = pDepthMarketData->AskVolume1;
		dataBuf[instrumentIndex][posIndex].bidPrice = pDepthMarketData->BidPrice1;
		dataBuf[instrumentIndex][posIndex].bidVolume = pDepthMarketData->BidVolume1;
		dataBuf[instrumentIndex][posIndex].upperLimit = pDepthMarketData->UpperLimitPrice;
		dataBuf[instrumentIndex][posIndex].lowerLimit = pDepthMarketData->LowerLimitPrice;
		strcpy(dataBuf[instrumentIndex][posIndex].updateTime, pDepthMarketData->UpdateTime);
		dataBuf[instrumentIndex][posIndex].updateMillisec = pDepthMarketData->UpdateMillisec;
	}
	// close and far strategy
	void CAFStrategy(CThostFtdcDepthMarketDataField* pDepthMarketData)
	{
		tempStream.clear();
		tempStream.str("");
		tempStream<<pDepthMarketData->InstrumentID<<"	"
					<<pDepthMarketData->UpdateTime<<"	"
					<<pDepthMarketData->UpdateMillisec<<"	"
					<<"LST: [$"<<pDepthMarketData->LastPrice<<"/"<<pDepthMarketData->Volume - lastVolume<<"]	"
					<<"ASK: [$"<<pDepthMarketData->AskPrice1<<"/"<<pDepthMarketData->AskVolume1<<"]	"
					<<"BID: [$"<<pDepthMarketData->BidPrice1<<"/"<<pDepthMarketData->BidVolume1<<"]";
		lastVolume = pDepthMarketData->Volume;
		logger.LogThisFast(tempStream.str());

		//judge for BUY_PRICE_GOOD, BUY_PRICE_NOT_GOOD and MUST_STOP
		int instrumentIndex = instrumentMap.at(pDepthMarketData->InstrumentID);// get instrument index
		int posInBufSec = posInBuf[SEC_INSTRUMENT];
		int posInBufMain = posInBuf[MAIN_INSTRUMENT];
		int lastPosInBufMain = posInBufMain-1;
		if(lastPosInBufMain<0)
		{
			lastPosInBufMain = STRATEGY_BUFFER_SIZE-1;
		}
		//最新数据为主力且主力买1发生负向变动时，进行止损
		if((instrumentIndex == MAIN_INSTRUMENT )&&
			(dataBuf[MAIN_INSTRUMENT][posInBufMain].bidPrice > dataBuf[MAIN_INSTRUMENT][posInBufMain].lowerLimit)&&
			(dataBuf[MAIN_INSTRUMENT][posInBufMain].bidPrice < dataBuf[MAIN_INSTRUMENT][posInBufMain].upperLimit)&&
			(dataBuf[MAIN_INSTRUMENT][lastPosInBufMain].bidPrice > dataBuf[MAIN_INSTRUMENT][lastPosInBufMain].lowerLimit)&&
			(dataBuf[MAIN_INSTRUMENT][lastPosInBufMain].bidPrice < dataBuf[MAIN_INSTRUMENT][lastPosInBufMain].upperLimit)&&
			(dataBuf[MAIN_INSTRUMENT][posInBufMain].bidPrice - dataBuf[MAIN_INSTRUMENT][lastPosInBufMain].bidPrice < 0))
		{
			StateMachine(MUST_STOP);
		}
		// wait until the two instrument data are in the same time slot to judge the open condition
		bool sameTime, sameMillisec;
		string mainInsTime = dataBuf[MAIN_INSTRUMENT][posInBuf[MAIN_INSTRUMENT]].updateTime;
		string secInsTime = dataBuf[SEC_INSTRUMENT][posInBuf[SEC_INSTRUMENT]].updateTime;
		double mainMillisec = dataBuf[MAIN_INSTRUMENT][posInBuf[MAIN_INSTRUMENT]].updateMillisec;
		double secMillisec = dataBuf[SEC_INSTRUMENT][posInBuf[SEC_INSTRUMENT]].updateMillisec;
		if((mainMillisec == secMillisec) && (mainInsTime.compare(secInsTime) == 0))
		{
			
			//条件1.次主力盘口买卖价差小于等于stgArg.secPriceDiff
			if((dataBuf[SEC_INSTRUMENT][posInBufSec].askPrice - dataBuf[SEC_INSTRUMENT][posInBufSec].bidPrice) <= stgArg.secPriceDiff)
			{
				//条件2.次主力盘口买卖量差大于等于stgArg.secVolumeDiff
				if(dataBuf[SEC_INSTRUMENT][posInBufSec].bidVolume - dataBuf[SEC_INSTRUMENT][posInBufSec].askVolume >= stgArg.secVolumeDiff)
				{
					//条件3.主力（注意这里是主力）价格位于合理区间，且变动大于等于stgArg.mainPriceDelta
					if((dataBuf[MAIN_INSTRUMENT][posInBufMain].bidPrice > dataBuf[MAIN_INSTRUMENT][posInBufMain].lowerLimit)&&
						(dataBuf[MAIN_INSTRUMENT][posInBufMain].bidPrice < dataBuf[MAIN_INSTRUMENT][posInBufMain].upperLimit)&&
						(dataBuf[MAIN_INSTRUMENT][lastPosInBufMain].bidPrice > dataBuf[MAIN_INSTRUMENT][lastPosInBufMain].lowerLimit)&&
						(dataBuf[MAIN_INSTRUMENT][lastPosInBufMain].bidPrice < dataBuf[MAIN_INSTRUMENT][lastPosInBufMain].upperLimit)&&
						(dataBuf[MAIN_INSTRUMENT][posInBufMain].bidPrice - dataBuf[MAIN_INSTRUMENT][lastPosInBufMain].bidPrice >= stgArg.mainPriceDelta))
					{
						StateMachine(BUY_PRICE_GOOD);
					}
				}
			}
		}

	}


public:
	void StartStratergy(void)
	{
		InitOtherCrap();
		InitTradeProcess();
		InitMarketProcess();

		bool trading = true;
		int tradeCommand;
		int returnCode = -1;
		TThostFtdcOrderRefType orderRef;
		TThostFtdcInstrumentIDType instrumentId;
		TThostFtdcPriceType price;
		TThostFtdcVolumeType volume;
		TThostFtdcDirectionType orderDirection;
		TThostFtdcOffsetFlagType orderType;
		int posInBufMain = posInBuf[MAIN_INSTRUMENT];
		int posInBufSec = posInBuf[SEC_INSTRUMENT];
		while(trading)
		{
			scanf("%d", &tradeCommand);
			switch(tradeCommand)
			{
				case cmdReqOrderInsert:
					printf("Request Order Insert...\n");
					printf("Input Instrument: ");
					scanf("%s", instrumentId);
					printf("Input Price: ");
					scanf("%lf", &price);
					printf("Input Order Direction (0 for buy, 1 for sell): ");
					cin>>orderDirection;
					printf("Input Order Type (0 for open, 1 for close): ");
					cin>>orderType;
					returnCode = ReqOrderInsert(instrumentId, price, 1, orderDirection, orderType);
					printf("Return Code: %d\n", returnCode);
					break;
				case cmdReqQryOrder:
					printf("Request Order Check...\n");
					printf("Input Instrument: ");
					cin>>instrumentId;
					ReqQryOrder(instrumentId);
					break;
				case cmdReqOrderAction:
					printf("Request Order Action...\n");
					printf("Input Instrument: ");
					scanf("%s", instrumentId);
					printf("Input Order Referencd: ");
					scanf("%s", orderRef);
					returnCode = ReqOrderAction(instrumentId, orderRef);
					printf("Return Code: %d\n", returnCode);
					break;
				case cmdReqOrderActionChange:
					printf("Request Order Action Change...\n");
					printf("Input Instrument: ");
					scanf("%s", instrumentId);
					printf("Input Order Referencd: ");
					scanf("%s", orderRef);
					cout<<"Input Price: ";
					cin>>price;
					cout<<"Input Volume Change: ";
					cin>>volume;
					returnCode = ReqOrderAction(instrumentId, orderRef, price, volume);
					printf("Return Code: %d\n", returnCode);
					break;
				case cmdReqQryInvestorPosition:
					cout<<"Request Qry Investor Position..."<<endl;
					cout<<"Input Instrument: ";
					cin>>instrumentId;
					returnCode = ReqQryInvestorPosition(instrumentId);
					cout<<"Return Code: "<<returnCode<<endl;
					break;
				case cmdReqOrderInsertArbitrage:
					posInBufMain = posInBuf[MAIN_INSTRUMENT];
					posInBufSec = posInBuf[SEC_INSTRUMENT];
					ReqOrderInsertArbitrage(dataBuf[MAIN_INSTRUMENT][posInBufMain].instrumentId,
											dataBuf[SEC_INSTRUMENT][posInBufSec].instrumentId,
											dataBuf[MAIN_INSTRUMENT][posInBufMain].bidPrice-dataBuf[SEC_INSTRUMENT][posInBufSec].bidPrice,
											stgArg.openCloseVolume,
											THOST_FTDC_D_Buy,
											THOST_FTDC_OF_Open,
											THOST_FTDC_OF_Open,
											&lastOrder);
					break;
				case cmdReqQryInstrument:
					ReqQryInstrument();
					break;
				default:
					break;
			}
		}
	}


};
