#pragma once
#include <ThostFtdcUserApiDataType.h>
#include <ThostFtdcUserApiStruct.h>
#include <boost/date_time/posix_time/posix_time.hpp> 
using namespace boost::posix_time;
class StrategyAux
{
protected:
	// all the state type used in state machine
	typedef enum TRADE_STATE
	{
		IDLE_STATE,
		OPEN_STATE,
		CLOSE_STATE,
		FORCE_CLOSE_STATE,
		CANCEL_OPEN_STATE,
		CANCEL_CLOSE_STATE,
		CHANGE_CANCEL_STATE,// this is to modify order
		SUSPEND_STATE,
		MAX_STATE
	};
	// state instance
	TRADE_STATE tradeState;

	// all the event used in state machine
	typedef enum TRADE_EVENT
	{
		BUY_PRICE_GOOD,
		BUY_PRICE_NOT_GOOD,
		TRADED,
		CANCELED,
		MUST_STOP,
		OPEN_OVERTIME,
		MAX_EVENT
	};

	// used to identify main and secondary instrument, in multi-instrument strategy
	typedef enum MAIN_N_SEC
	{
		MAIN_INSTRUMENT	= 0,
		SEC_INSTRUMENT	= 1
	};

	// used to store last trade info, kind of useful
	typedef struct TRADE_RECORD
	{
		TThostFtdcInstrumentIDType		instrumentId;
		TThostFtdcPriceType				tradePrice;//成交价
		TThostFtdcVolumeType			tradeVolume;//成交量
		///买卖方向
		TThostFtdcDirectionType			direction;
		///开平标志
		TThostFtdcOffsetFlagType		offsetFlag;
	};
	TRADE_RECORD lastTrade;

	// essential data of market data
	typedef struct BasicMarketData
	{
		TThostFtdcInstrumentIDType	instrumentId;
		TThostFtdcPriceType			lastPrice;//最新价
		TThostFtdcVolumeType		volume;//累加成交量
		TThostFtdcPriceType			askPrice;//卖一价
		TThostFtdcVolumeType		askVolume;//卖一量
		TThostFtdcPriceType			bidPrice;//买一价
		TThostFtdcVolumeType		bidVolume;//买一量
		TThostFtdcPriceType			upperLimit;//涨停板价
		TThostFtdcPriceType			lowerLimit;//跌停板价
		TThostFtdcTimeType			updateTime;//更新时间
		TThostFtdcMillisecType		updateMillisec;//更新毫秒数
		boost::posix_time::ptime	localTime;//本地时间戳
	};

	// strategy parameters
	typedef struct StrategyParameter
	{
		double		mainPriceDelta;		// Delta为本次数据与上次数据的价格变动
		double		secPriceDiff;		// Diff为当前盘口的买卖价差
		double		closePriceConst;	// 平仓时的价格上浮常数
		double		openPriceConst;		// 开仓价格上浮常数
		double		floatError;			// 浮点数误差
		int			secVolumeDiff;		// 当前盘口的买卖数量差
		int			openCloseVolume;	// 每次开仓平仓的数量
		int			maxOpenTime;		// 最大开仓等待时间
		int			minMove;			// 最小价格变动
	};

	// strategy init error type
	typedef enum InitErrorType
	{
		ALL_GOOD = 0,
		CONFIG_ERROR,
		ALLOC_ERROR,
		OTHER_ERROR

	};
	InitErrorType initStatus;
protected:
	void StoreLastTrade(CThostFtdcTradeField *pTrade)
	{
		strcpy(lastTrade.instrumentId, pTrade->InstrumentID);
		lastTrade.tradePrice = pTrade->Price;
		lastTrade.tradeVolume = pTrade->Volume;
		lastTrade.direction = pTrade->Direction;
		lastTrade.offsetFlag = pTrade->OffsetFlag;
	}
	// transfer the current trade state into string
	char* ShowTradeState(void)
	{
		switch(tradeState)
		{
		case IDLE_STATE:
			return "IDLE_STATE";
			break;
		case OPEN_STATE:
			return "OPEN_STATE";
			break;
		case CLOSE_STATE:
			return "CLOSE_STATE";
			break;
		case FORCE_CLOSE_STATE:
			return "FORCE_CLOSE_STATE";
			break;
		case CANCEL_OPEN_STATE:
			return "CANCEL_OPEN_STATE";
			break;
		case CANCEL_CLOSE_STATE:
			return "CANCEL_CLOSE_STATE";
			break;
		case CHANGE_CANCEL_STATE:
			return "CHANGE_CANCEL_STATE";
			break;
		case SUSPEND_STATE:
			return "SUSPEND_STATE";
			break;
		default:
			return "UNKNOWN_STATE";
			break;

		}
	}
	// transfer the input trade event into string
	char* ShowTradeEvent(TRADE_EVENT thisEvent)
	{
		switch(thisEvent)
		{
		case BUY_PRICE_GOOD:
			return "BUY_PRICE_GOOD";
			break;
		case BUY_PRICE_NOT_GOOD:
			return "BUY_PRICE_NOT_GOOD";
			break;
		case TRADED:
			return "TRADED";
			break;
		case CANCELED:
			return "CANCELED";
			break;
		case MUST_STOP:
			return "MUST_STOP";
			break;
		case OPEN_OVERTIME:
			return "OPEN_OVERTIME";
			break;
		default:
			return "UNKNOWN_EVENT";
			break;
		}
	}
	// overtime call back routine

};