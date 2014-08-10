#pragma once
#include <ThostFtdcUserApiDataType.h>
#include <ThostFtdcUserApiStruct.h>
#include <boost/date_time/posix_time/posix_time.hpp> 
#include <string>
using namespace boost::posix_time;
using namespace std;
namespace Pas
{
	// open condition
	typedef enum 
	{
		OPEN_COND1,
		OPEN_COND2,
		ILLEGAL_COND
	}OPEN_CONDITION;
	// trading direction
	typedef enum 
	{
		BUY_PRIM_SELL_SCND,
		BUY_SCND_SELL_PRIM
	}TRADE_DIR;
	// all the state type used in state machine
	typedef enum 
	{
		IDLE_STATE,
		OPENING_SCND_STATE,
		CHECKING_SCND_STATE,
		OPENING_PRIM_STATE,
		PENDING_STATE,
		CLOSING_BOTH_STATE,
		CANCELLING_SCND_STATE,
		CLOSING_SCND_STATE,
		CANCELLING_PRIM_STATE,
		WAITING_SCND_CLOSE_STATE,
		WAITING_PRIM_CLOSE_STATE,
		MAX_STATE
	}TRADE_STATE;

	// all the event used in state machine
	typedef enum 
	{
		OPEN_PRICE_GOOD,
		OPEN_PRICE_BAD,
		PRIM_OPENED,
		PRIM_OPEN_TIMEOUT,
		PRIM_CANCELLED,
		PRIM_CLOSED,
		SCND_OPENED,
		SCND_PARTLY_OPENED,
		SCND_OPEN_TIMEOUT,
		SCND_CANCELLED,
		SCND_CLOSED,
		CLOSE_PRICE_GOOD,
		MUST_STOP,
		NOT_TRADING_TIME,
		MAX_EVENT
	}TRADE_EVENT;

	// used to identify main and secondary instrument, in multi-instrument strategy
	typedef enum 
	{
		MAIN_INSTRUMENT	= 0,
		SEC_INSTRUMENT	= 1
	}MAIN_N_SEC;

	// used to store last trade info, kind of useful
	typedef struct 
	{
		TThostFtdcInstrumentIDType		instrumentId;
		TThostFtdcPriceType				tradePrice;//成交价
		TThostFtdcVolumeType			tradeVolume;//成交量
		///买卖方向
		TThostFtdcDirectionType			direction;
		///开平标志
		TThostFtdcOffsetFlagType		offsetFlag;
	}TRADE_RECORD;

	// essential data of market data
	typedef struct 
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
	}BasicMarketData;

	// strategy parameters
	typedef struct 
	{
		string		primaryInst;		// 主力合约
		string		secondaryInst;		// 次主力合约
		int			bollPeriod;			// 布林带标准差计算长度
		double		outterBollAmp;		// 布林带外部振幅
		double		innerBollAmp;		// 布林带内部振幅
		double		stopBollAmp;		// 止损振幅
		double		winBollAmp;			// 止盈振幅
		int			openShares;			// 开仓手数
		int			secVolumeDiff;		// 当前盘口的买卖数量差
		int			primOpenTime;		// 主力合约最大开仓等待时间
		int			scndOpenTime;		// 次主力合约最大开仓等待时间
		int			primCloseTime;		// 主力合约最大平仓等待时间
		int			scndCloseTime;		// 次主力合约最大平仓等待时间
		int			primCancelTime;		// 主力合约最大撤单等待时间
		int			scndCancelTime;		// 次主力合约最大撤单等待时间
		int			bollAmpLimit;		// 开仓时的布林带要达到的最小价差，以跳为单位
		double		askBidGapLimit;		// 盘口价差上限
		double		minMove;			// 最小价格变动
		double		stopLossPrice;		// 止损价
		double		ceilingPrice;		// 合理价格上限
		double		floorPrice;			// 合理价格下限
		double		mainPriceDelta;		// Delta为本次数据与上次数据的价格变动
		double		secPriceDiff;		// Diff为当前盘口的买卖价差
		double		closePriceConst;	// 平仓时的价格上浮常数
		double		floatToleration;	// 浮点数误差
		double		cost;				// 手续费，以跳为单位
		double		stopWinPoint;		// 止盈点，以跳为单位
		double		durationStep;		// 每隔这么多时间（s），winBollAmp就会被调整一次
		double		winBollAmpAdjust;	// 每次调整winBollAmp的调整量
	}StrategyParameter;

	
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
	string ShowTradeState(void)
	{
		string lTradeState;
		switch(tradeState)
		{
		case IDLE_STATE:
			lTradeState = "IDLE_STATE";
			break;
		case OPENING_SCND_STATE:
			lTradeState = "OPENING_SCND_STATE";
			break;
		case CHECKING_SCND_STATE:
			lTradeState = "CHECKING_SCND_STATE";
			break;
		case OPENING_PRIM_STATE:
			lTradeState = "OPENING_PRIM_STATE";
			break;
		case PENDING_STATE:
			lTradeState = "PENDING_STATE";
			break;
		case CLOSING_BOTH_STATE:
			lTradeState = "CLOSING_BOTH_STATE";
			break;
		case CANCELLING_SCND_STATE:
			lTradeState = "CANCELLING_SCND_STATE";
			break;
		case CLOSING_SCND_STATE:
			lTradeState = "CLOSING_SCND_STATE";
			break;
		case CANCELLING_PRIM_STATE:
			lTradeState = "CANCELLING_PRIM_STATE";
			break;
		case WAITING_SCND_CLOSE_STATE:
			lTradeState = "WAITING_SCND_CLOSE_STATE";
			break;
		case WAITING_PRIM_CLOSE_STATE:
			lTradeState = "WAITING_PRIM_CLOSE_STATE";
			break;
		default:
			lTradeState = "UNKNOWN_STATE";
			break;

		}
		return lTradeState;
	}
public:
	static string ShowTradeState(TRADE_STATE aTradeState)
	{
		string lTradeState;
		switch(aTradeState)
		{
		case IDLE_STATE:
			lTradeState = "IDLE_STATE";
			break;
		case OPENING_SCND_STATE:
			lTradeState = "OPENING_SCND_STATE";
			break;
		case CHECKING_SCND_STATE:
			lTradeState = "CHECKING_SCND_STATE";
			break;
		case OPENING_PRIM_STATE:
			lTradeState = "OPENING_PRIM_STATE";
			break;
		case PENDING_STATE:
			lTradeState = "PENDING_STATE";
			break;
		case CLOSING_BOTH_STATE:
			lTradeState = "CLOSING_BOTH_STATE";
			break;
		case CANCELLING_SCND_STATE:
			lTradeState = "CANCELLING_SCND_STATE";
			break;
		case CLOSING_SCND_STATE:
			lTradeState = "CLOSING_SCND_STATE";
			break;
		case CANCELLING_PRIM_STATE:
			lTradeState = "CANCELLING_PRIM_STATE";
			break;
		case WAITING_SCND_CLOSE_STATE:
			lTradeState = "WAITING_SCND_CLOSE_STATE";
			break;
		case WAITING_PRIM_CLOSE_STATE:
			lTradeState = "WAITING_PRIM_CLOSE_STATE";
			break;
		default:
			lTradeState = "UNKNOWN_STATE";
			break;

		}
		return lTradeState;
	}
	// transfer the input trade event into string
	static string ShowTradeEvent(TRADE_EVENT thisEvent)
	{
		string lThisEvent;
		switch(thisEvent)
		{
		case OPEN_PRICE_GOOD:
			lThisEvent = "OPEN_PRICE_GOOD";
			break;
		case OPEN_PRICE_BAD:
			lThisEvent = "OPEN_PRICE_BAD";
			break;
		case PRIM_OPENED:
			lThisEvent = "PRIM_OPENED";
			break;
		case PRIM_OPEN_TIMEOUT:
			lThisEvent = "PRIM_OPEN_TIMEOUT";
			break;
		case PRIM_CANCELLED:
			lThisEvent = "PRIM_CANCELLED";
			break;
		case PRIM_CLOSED:
			lThisEvent = "PRIM_CLOSED";
			break;
		case SCND_OPENED:
			lThisEvent = "SCND_OPENED";
			break;
		case SCND_PARTLY_OPENED:
			lThisEvent = "SCND_PARTLY_OPENED";
			break;
		case SCND_OPEN_TIMEOUT:
			lThisEvent = "SCND_OPEN_TIMEOUT";
			break;
		case SCND_CANCELLED:
			lThisEvent = "SCND_CANCELLED";
			break;
		case SCND_CLOSED:
			lThisEvent = "SCND_CLOSED";
			break;
		case CLOSE_PRICE_GOOD:
			lThisEvent = "CLOSE_PRICE_GOOD";
			break;
		case MUST_STOP:
			lThisEvent = "MUST_STOP";
			break;
		case NOT_TRADING_TIME:
			lThisEvent = "NOT_TRADING_TIME";
			break;
		default:
			lThisEvent = "UNKNOWN_EVENT";
			break;
		}
		return lThisEvent;
	}
	// overtime call back routine

};
}