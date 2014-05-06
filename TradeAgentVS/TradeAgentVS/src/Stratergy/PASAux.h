#pragma once
#include <ThostFtdcUserApiDataType.h>
#include <ThostFtdcUserApiStruct.h>
#include <boost/date_time/posix_time/posix_time.hpp> 
#include <string>
using namespace boost::posix_time;
using namespace std;
namespace Pas
{
	// trading direction
	typedef enum TRADE_DIR
	{
		BUY_PRIM_SELL_SCND,
		BUY_SCND_SELL_PRIM
	};
	// all the state type used in state machine
	typedef enum TRADE_STATE
	{
		IDLE_STATE,
		OPENING_SCND_STATE,
		OPENING_PRIM_STATE,
		PENDING_STATE,
		CLOSING_BOTH_STATE,
		CANCELLING_SCND_STATE,
		CLOSING_SCND_STATE,
		CANCELLING_PRIM,
		MAX_STATE
	};

	// all the event used in state machine
	typedef enum TRADE_EVENT
	{
		OPEN_PRICE_GOOD,
		OPEN_PRICE_BAD,
		PRIM_OPENED,
		PRIM_CANCELLED,
		SCND_OPENED,
		SCND_CANCELLED,
		CLOSE_PRICE_GOOD,
		MUST_STOP,
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

	// essential data of market data
	typedef struct BasicMarketData
	{
		TThostFtdcInstrumentIDType	instrumentId;
		TThostFtdcPriceType			lastPrice;//最新价
		TThostFtdcVolumeType		volume;//累加成交量
		TThostFtdcVolumeType		trueVolume;//真实成交量
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
		string		primaryInst;		// 主力合约
		string		secondaryInst;		// 次主力合约
		int			bollPeriod;			// 布林带标准差计算长度
		int			outterBollAmp;		// 布林带外部振幅
		int			innerBollAmp;		// 布林带内部振幅
		int			openShares;			// 开仓手数
		double		ceilingPrice;		// 合理价格上限
		double		floorPrice;			// 合理价格下限
		double		mainPriceDelta;		// Delta为本次数据与上次数据的价格变动
		double		secPriceDiff;		// Diff为当前盘口的买卖价差
		double		closePriceConst;	// 平仓时的价格上浮常数
		double		floatToleration;	// 浮点数误差
		int			secVolumeDiff;		// 当前盘口的买卖数量差
		int			maxOpenTime;		// 最大开仓等待时间
		int			minMove;			// 最小价格变动
	};

	
class PASAux
{
protected:
	
	// state instance
	TRADE_STATE tradeState;

	TRADE_RECORD lastTrade;

	
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
		case OPENING_SCND_STATE:
			return "OPENING_SCND_STATE";
			break;
		case OPENING_PRIM_STATE:
			return "OPENING_PRIM_STATE";
			break;
		case PENDING_STATE:
			return "PENDING_STATE";
			break;
		case CLOSING_BOTH_STATE:
			return "CLOSING_BOTH_STATE";
			break;
		case CANCELLING_SCND_STATE:
			return "CANCELLING_SCND_STATE";
			break;
		case CLOSING_SCND_STATE:
			return "CLOSING_SCND_STATE";
			break;
		case CANCELLING_PRIM:
			return "CANCELLING_PRIM";
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
		case OPEN_PRICE_GOOD:
			return "OPEN_PRICE_GOOD";
			break;
		case OPEN_PRICE_BAD:
			return "OPEN_PRICE_BAD";
			break;
		case PRIM_OPENED:
			return "PRIM_OPENED";
			break;
		case PRIM_CANCELLED:
			return "PRIM_CANCELLED";
			break;
		case SCND_OPENED:
			return "SCND_OPENED";
			break;
		case SCND_CANCELLED:
			return "SCND_CANCELLED";
			break;
		case CLOSE_PRICE_GOOD:
			return "CLOSE_PRICE_GOOD";
			break;
		case MUST_STOP:
			return "MUST_STOP";
			break;
		default:
			return "UNKNOWN_EVENT";
			break;
		}
	}
	// overtime call back routine

};
}