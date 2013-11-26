#pragma once
#include <stdafx.h>
#include <stdio.h>
#include <TradeProcess.h>
#include <MarketProcess.h>
#include <boost\thread.hpp>
#include <boost\progress.hpp>
#include <ConfigReader.h>
#include <Hook.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/unordered_map.hpp>
#include <boost/circular_buffer.hpp>
#include <cmath>
#include <Log.h>
#include <strstream>
#include <StrategyAux.h>
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost;
using namespace std;
#define STRATEGY_BUFFER_SIZE 100
#define PRICE_UPPER_LIM 100000
#define CLOSE_AND_FAR_2_0
class Stratergy : public TradeProcess, public Hook, public StrategyAux
{
private:
	ConfigReader config;
	MarketProcess marketObj;
	int totalInstrument;
	char* instrumentList[MAX_INSTRUMENT];
	// container store instrumentID as key, and its serial as value
	typedef unordered_map<string, int> instrumentMapType;
	instrumentMapType instrumentMap;
	// Used to buffer market data. This will consume about 100*MAX_INSTRUMENT*STRATEGY_BUFFER_SIZE bytes in stack
	BasicMarketData dataBuf[MAX_INSTRUMENT][STRATEGY_BUFFER_SIZE];
	// used to store the index position of every instruments' data buf
	int posInBuf[MAX_INSTRUMENT];
	int OPEN_CLOSE_VOLUME;//每次开仓平仓的数量
	ORDER_INDEX_TYPE lastOrder;//记录最新一次的订单索引，用来更改、查询、撤销订单
	Log logger;
	TThostFtdcVolumeType lastVolume;//由于行情数据中的成交量是累加值，因此需要减去上次成交量来获得成交量增量
public:
	double mainPriceDelta;//Delta为本次数据与上次数据的价格变动
	double secPriceDiff;//Diff为当前盘口的买卖价差
	int secVolumeDiff;//当前盘口的买卖数量差
	double closePriceConst;//平仓时的价格上浮常数
	double floatError; // 浮点数误差
private:
	void SetState(TRADE_STATE newState)
	{
		tradeState = newState;
		string message = "Current State: ";
		message.append(ShowTradeState());
		logger.LogThisFast(message);
	}
#ifdef CLOSE_AND_FAR_1_0
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
								dataBuf[SEC_INSTRUMENT][posInBufSec].askPrice,
								OPEN_CLOSE_VOLUME,
								THOST_FTDC_D_Buy, 
								THOST_FTDC_OF_Open,
								&lastOrder);
				SetState(OPEN_STATE);

				stringstream tempStream;
				tempStream<<"Trying to open with price: "<<dataBuf[SEC_INSTRUMENT][posInBufSec].askPrice<<"...";
				logger.LogThisFast(tempStream.str());
			}
			break;
		case OPEN_STATE:
			//若成交，则建仓成功，开始平仓并进入CLOSE_STATE
			if(TRADED == tradeEvent)
			{
				logger.LogThisFast("Trying to close...");
				int posInBufMain = posInBuf[MAIN_INSTRUMENT];
				int posInBufSec = posInBuf[SEC_INSTRUMENT];
				int lastPosInBufMain = posInBufMain-1;
				if(lastPosInBufMain<0)
				{
					lastPosInBufMain = STRATEGY_BUFFER_SIZE-1;
				}
				double closePrice = dataBuf[1][posInBufSec].askPrice + 
									dataBuf[0][posInBufMain].bidPrice - dataBuf[0][lastPosInBufMain].bidPrice + closePriceConst;
				ReqOrderInsert(dataBuf[SEC_INSTRUMENT][posInBufSec].instrumentId,
								closePrice,
								OPEN_CLOSE_VOLUME,
								THOST_FTDC_D_Sell,
								THOST_FTDC_OF_CloseToday,
								&lastOrder);
				SetState(CLOSE_STATE);

				stringstream tempStream;
				tempStream<<"Trying to close with price: "<<closePrice<<"...";
				logger.LogThisFast(tempStream.str());
			}
			else if((BUY_PRICE_NOT_GOOD == tradeEvent)||(MUST_STOP == tradeEvent))
			{
				logger.LogThisFast("Trying to cancel...");

				ReqOrderAction(instrumentList[SEC_INSTRUMENT],lastOrder.orderRef);
				SetState(CANCEL_OPEN_STATE);
			}
			break;
		case CLOSE_STATE:
			if(BUY_PRICE_GOOD == tradeEvent)
			{
				logger.LogThisFast("Trying to cancel...");
				ReqOrderAction(instrumentList[SEC_INSTRUMENT],lastOrder.orderRef);
				SetState(CHANGE_CANCEL_STATE);
			}
			else if(TRADED == tradeEvent)
			{
				SetState(IDLE_STATE);
			}
			else if(MUST_STOP == tradeEvent)
			{
				logger.LogThisFast("Trying to cancel...");
				ReqOrderAction(instrumentList[SEC_INSTRUMENT],lastOrder.orderRef);
				SetState(CANCEL_CLOSE_STATE);
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
								OPEN_CLOSE_VOLUME, 
								THOST_FTDC_D_Sell, 
								THOST_FTDC_OF_CloseToday,
								&lastOrder);
				SetState(FORCE_CLOSE_STATE);

				stringstream tempStream;
				tempStream<<"Trying to force close with price: "<<forceStopPrice<<"...";
				logger.LogThisFast(tempStream.str());
			}
			else if(CANCELED == tradeEvent)
			{
				SetState(IDLE_STATE);
			}
			break;
		case CANCEL_CLOSE_STATE:
			if(TRADED == tradeEvent)
			{
				SetState(IDLE_STATE);
			}
			else if(CANCELED == tradeEvent)
			{
				TThostFtdcPriceType forceStopPrice;
				int posInBufSec = posInBuf[SEC_INSTRUMENT];
				forceStopPrice = dataBuf[SEC_INSTRUMENT][posInBufSec].lowerLimit;//使用跌停板价格来平仓
				ReqOrderInsert(instrumentList[SEC_INSTRUMENT], 
								forceStopPrice, 
								OPEN_CLOSE_VOLUME, 
								THOST_FTDC_D_Sell, 
								THOST_FTDC_OF_CloseToday,
								&lastOrder);
				SetState(FORCE_CLOSE_STATE);

				stringstream tempStream;
				tempStream<<"Trying to force close with price: "<<forceStopPrice<<"...";
				logger.LogThisFast(tempStream.str());
								
			}
			break;
		case CHANGE_CANCEL_STATE:
			if(TRADED == tradeEvent)
			{
				SetState(IDLE_STATE);
			}
			else if(CANCELED == tradeEvent)
			{
				int posInBufMain = posInBuf[MAIN_INSTRUMENT];
				int posInBufSec = posInBuf[SEC_INSTRUMENT];
				int lastPosInBufMain = posInBufMain-1;
				if(lastPosInBufMain<0)
				{
					lastPosInBufMain = STRATEGY_BUFFER_SIZE-1;
				}
				double closePrice = dataBuf[1][posInBufSec].askPrice + 
									dataBuf[0][posInBufMain].bidPrice - dataBuf[0][lastPosInBufMain].bidPrice + closePriceConst;
				ReqOrderInsert(dataBuf[SEC_INSTRUMENT][posInBufSec].instrumentId,
								closePrice,
								OPEN_CLOSE_VOLUME,
								THOST_FTDC_D_Sell,
								THOST_FTDC_OF_CloseToday,
								&lastOrder);
				SetState(CLOSE_STATE);

				stringstream tempStream;
				tempStream<<"Trying to close with price: "<<closePrice<<"...";
				logger.LogThisFast(tempStream.str());
			}
			else if(MUST_STOP == tradeEvent)
			{
				SetState(CANCEL_CLOSE_STATE);
			}
			break;
		default:
			break;
		}
	}
#endif

#ifdef CLOSE_AND_FAR_2_0
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
								dataBuf[SEC_INSTRUMENT][posInBufSec].askPrice,
								OPEN_CLOSE_VOLUME,
								THOST_FTDC_D_Buy, 
								THOST_FTDC_OF_Open,
								&lastOrder);
				SetState(OPEN_STATE);

				stringstream tempStream;
				tempStream<<"Trying to open with price: "<<dataBuf[SEC_INSTRUMENT][posInBufSec].askPrice<<"...";
				logger.LogThisFast(tempStream.str());
			}
			break;
		case OPEN_STATE:
			//若成交，则建仓成功，开始等待平仓条件，进入SUSPEND_STATE
			if(TRADED == tradeEvent)
			{
				
				SetState(SUSPEND_STATE);

				
			}
			else if((BUY_PRICE_NOT_GOOD == tradeEvent)||(MUST_STOP == tradeEvent))
			{
				logger.LogThisFast("Trying to cancel...");

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
								OPEN_CLOSE_VOLUME, 
								THOST_FTDC_D_Sell, 
								THOST_FTDC_OF_CloseToday,
								&lastOrder);
				SetState(FORCE_CLOSE_STATE);

				stringstream tempStream;
				tempStream<<"Trying to force close with price: "<<forceStopPrice<<"...";
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
								OPEN_CLOSE_VOLUME, 
								THOST_FTDC_D_Sell, 
								THOST_FTDC_OF_CloseToday,
								&lastOrder);
				SetState(FORCE_CLOSE_STATE);

				stringstream tempStream;
				tempStream<<"Trying to force close with price: "<<forceStopPrice<<"...";
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
#endif
	void InitOtherCrap()
	{
		memset(dataBuf, 0, sizeof(dataBuf));
		memset(posInBuf, 0, sizeof(posInBuf));
		totalInstrument = config.ReadInstrumentID(instrumentList);
		for(int i=0; i<totalInstrument; i++)
		{
			instrumentMap[instrumentList[i]] =  i;
			cout<<"Instrument Map: "<<instrumentList[i]<<" ID: "<<instrumentMap.at(instrumentList[i])<<endl;
		}
		lastVolume = 0;
		SetState(IDLE_STATE);
		mainPriceDelta = 0.6;
		secPriceDiff = 0.2;
		secVolumeDiff = -10;
		closePriceConst = 0.0;		
		floatError = 0.00001;
		OPEN_CLOSE_VOLUME = 1;
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
		marketObj.SetHook(this); //Register this obj for market data call back
		marketObj.StartMarketProcess();
		delete tempConfig;
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
	void EasyTrade(CThostFtdcDepthMarketDataField* pDepthMarketData)
	{
		
	}
	///成交通知
	void OnRtnTrade(CThostFtdcTradeField* pTrade)
	{
		boost::posix_time::ptime localTime = boost::posix_time::microsec_clock::local_time();
		cout<<"---> Trade return at "<<localTime.time_of_day()<<endl;
		stringstream tempStream;
		tempStream<<"Traded at price:	"<<pTrade->Price;
		// calculate how much do i earn in the last trade, and log it
		if((pTrade->OffsetFlag == THOST_FTDC_OF_CloseToday)||
			(pTrade->OffsetFlag == THOST_FTDC_OF_Close)||
			(pTrade->OffsetFlag == THOST_FTDC_OF_ForceClose))
		{
			
			tempStream<<"	Profit during last trade:	"<<pTrade->Price-lastTrade.tradePrice;
		}
		logger.LogThisFast(tempStream.str());
		StoreLastTrade(pTrade);
	}
	///报单通知
    virtual void OnRtnOrder(CThostFtdcOrderField* pOrder)
	{
		cout<<"---> OnRtnOrder: "<<endl;
		cout<<"------> Order Status: "<<pOrder->OrderStatus<<endl;
		cout<<"------> Cancel Time: "<<pOrder->CancelTime<<endl;
		cout<<"------> Status Message: "<<pOrder->StatusMsg<<endl;
		cout<<"------> Order Submit Status: "<<pOrder->OrderSubmitStatus<<endl;
		if((pOrder->OrderStatus == THOST_FTDC_OST_Canceled) && (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted))
		{
			StateMachine(CANCELED);
		}
		if((pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)&&(pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted))
		{
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
	void CloseAndFar(CThostFtdcDepthMarketDataField* pDepthMarketData)
	{
		stringstream tempStream;
		tempStream<<pDepthMarketData->InstrumentID<<"	"
					<<pDepthMarketData->UpdateTime<<"	"
					<<pDepthMarketData->UpdateMillisec<<"	"
					<<"LST: [$"<<pDepthMarketData->LastPrice<<"/"<<pDepthMarketData->Volume - lastVolume<<"]	"
					<<"ASK: [$"<<pDepthMarketData->AskPrice1<<"/"<<pDepthMarketData->AskVolume1<<"]	"
					<<"BID: [$"<<pDepthMarketData->BidPrice1<<"/"<<pDepthMarketData->BidVolume1<<"]";
		lastVolume = pDepthMarketData->Volume;
		logger.LogThisFast(tempStream.str());
		// wait until the two instrument data are in the same time slot
		bool sameTime, sameMillisec;
		string mainInsTime = dataBuf[0][posInBuf[0]].updateTime;
		string secInsTime = dataBuf[1][posInBuf[1]].updateTime;
		double mainMillisec = dataBuf[0][posInBuf[0]].updateMillisec;
		double secMillisec = dataBuf[1][posInBuf[1]].updateMillisec;
		if((mainMillisec == secMillisec) && (mainInsTime.compare(secInsTime) == 0))
		{
			//judge for BUY_PRICE_GOOD, BUY_PRICE_NOT_GOOD and MUST_STOP
			int instrumentIndex = instrumentMap.at(pDepthMarketData->InstrumentID);
			int posInBufSec = posInBuf[instrumentIndex];
			int posInBufMain = posInBuf[0];
			int lastPosInBufMain = posInBufMain-1;
			if(lastPosInBufMain<0)
			{
				lastPosInBufMain = STRATEGY_BUFFER_SIZE-1;
			}
			//主力买1发生负向变动时，进行止损
			if((dataBuf[0][posInBufMain].bidPrice >0)&&
				(dataBuf[0][posInBufMain].bidPrice <10000)&&
				(dataBuf[0][lastPosInBufMain].bidPrice >0)&&
				(dataBuf[0][lastPosInBufMain].bidPrice <10000)&&
				(dataBuf[0][posInBufMain].bidPrice - dataBuf[0][lastPosInBufMain].bidPrice < 0))
			{
				StateMachine(MUST_STOP);
			}
			//条件1.次主力盘口买卖价差等于secPriceDiff
			else if(abs(dataBuf[instrumentIndex][posInBufSec].askPrice - dataBuf[instrumentIndex][posInBufSec].bidPrice - secPriceDiff) <floatError)
			{
				//条件2.次主力盘口买卖量差大于等于secVolumeDiff
				if(dataBuf[instrumentIndex][posInBufSec].bidVolume - dataBuf[instrumentIndex][posInBufSec].askVolume >= secVolumeDiff)
				{
					//条件3.主力（注意这里是主力）价格位于合理区间，且变动大于等于mainPriceDelta
					if((dataBuf[0][posInBufMain].bidPrice >0)&&
						(dataBuf[0][posInBufMain].bidPrice <10000)&&
						(dataBuf[0][lastPosInBufMain].bidPrice >0)&&
						(dataBuf[0][lastPosInBufMain].bidPrice <10000)&&
						(dataBuf[0][posInBufMain].bidPrice - dataBuf[0][lastPosInBufMain].bidPrice >= mainPriceDelta))
					{
						StateMachine(BUY_PRICE_GOOD);
					}
				}
			}
			else
			{
				StateMachine(BUY_PRICE_NOT_GOOD);
			}
		}

	}
public:
	void Test(void)
	{
		typedef boost::unordered_map<std::string, int> map;
		map x;
		x["one"] = 1;
		x["two"] = 2;
		x["three"] = 3;

		assert(x.at("one") == 1);
		assert(x.find("missing") == x.end());
	}

	Stratergy(void)
	{
		for(int i=0; i<MAX_INSTRUMENT; i++)
		{
			instrumentList[i] = new char[INSTRUMENT_SIZE];
		}
	}
	~Stratergy(void)
	{
		for(int i=0; i<MAX_INSTRUMENT; i++)
		{
			delete instrumentList[i];
		}
	}

	// strategy should be initiated by market data
	virtual void HookOnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
	{
#ifndef MARKET_DATA_SUBSCRIBER
		BufferData(pDepthMarketData);
		CloseAndFar(pDepthMarketData);
#endif

		std::cout<<pDepthMarketData->InstrumentID<<" last price: "<<pDepthMarketData->LastPrice<<std::endl;
	}

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
					//returnCode = tradeObj.ReqOrderInsertSimple("IF1311", THOST_FTDC_OPT_BestPrice, 1, THOST_FTDC_D_Buy, THOST_FTDC_OF_Open);
					printf("Return Code: %d\n", returnCode);
					break;
				case cmdReqQryOrder:
					printf("Request Order Check...\n");
					printf("Input Instrument: ");
					scanf("%s", instrumentId);
					ReqQryOrder("IF1311");
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
				case cmdReqQryInvestorPosition:
					cout<<"Request Qry Investor Position..."<<endl;
					cout<<"Input Instrument: ";
					cin>>instrumentId;
					returnCode = ReqQryInvestorPosition(instrumentId);
					cout<<"Return Code: "<<returnCode<<endl;
				default:
					break;
			}
		}
	}


};
