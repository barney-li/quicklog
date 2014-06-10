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
		SCND_OPEN_TIMEOUT,
		SCND_CANCELLED,
		SCND_CLOSED,
		CLOSE_PRICE_GOOD,
		MUST_STOP,
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
		TThostFtdcPriceType				tradePrice;//�ɽ���
		TThostFtdcVolumeType			tradeVolume;//�ɽ���
		///��������
		TThostFtdcDirectionType			direction;
		///��ƽ��־
		TThostFtdcOffsetFlagType		offsetFlag;
	}TRADE_RECORD;

	// essential data of market data
	typedef struct 
	{
		TThostFtdcInstrumentIDType	instrumentId;
		TThostFtdcPriceType			lastPrice;//���¼�
		TThostFtdcVolumeType		volume;//�ۼӳɽ���
		TThostFtdcVolumeType		trueVolume;//��ʵ�ɽ���
		TThostFtdcPriceType			askPrice;//��һ��
		TThostFtdcVolumeType		askVolume;//��һ��
		TThostFtdcPriceType			bidPrice;//��һ��
		TThostFtdcVolumeType		bidVolume;//��һ��
		TThostFtdcPriceType			upperLimit;//��ͣ���
		TThostFtdcPriceType			lowerLimit;//��ͣ���
		TThostFtdcTimeType			updateTime;//����ʱ��
		TThostFtdcMillisecType		updateMillisec;//���º�����
		boost::posix_time::ptime	localTime;//����ʱ���
	}BasicMarketData;

	// strategy parameters
	typedef struct 
	{
		string		primaryInst;		// ������Լ
		string		secondaryInst;		// ��������Լ
		int			bollPeriod;			// ���ִ���׼����㳤��
		int			outterBollAmp;		// ���ִ��ⲿ���
		int			innerBollAmp;		// ���ִ��ڲ����
		int			stopBollAmp;		// ֹ�����
		int			winBollAmp;			// ֹӯ���
		int			bollAmpLimit;		// ����ʱ�Ĳ��ִ�Ҫ�ﵽ�����
		double		stopLossPrice;		// ֹ���
		int			openShares;			// ��������
		double		ceilingPrice;		// �����۸�����
		double		floorPrice;			// �����۸�����
		double		mainPriceDelta;		// DeltaΪ�����������ϴ����ݵļ۸�䶯
		double		secPriceDiff;		// DiffΪ��ǰ�̿ڵ������۲�
		double		closePriceConst;	// ƽ��ʱ�ļ۸��ϸ�����
		double		floatToleration;	// ���������
		int			secVolumeDiff;		// ��ǰ�̿ڵ�����������
		int			primOpenTime;		// ������Լ��󿪲ֵȴ�ʱ��
		int			scndOpenTime;		// ��������Լ��󿪲ֵȴ�ʱ��
		int			primCloseTime;		// ������Լ���ƽ�ֵȴ�ʱ��
		int			scndCloseTime;		// ��������Լ���ƽ�ֵȴ�ʱ��
		int			primCancelTime;		// ������Լ��󳷵��ȴ�ʱ��
		int			scndCancelTime;		// ��������Լ��󳷵��ȴ�ʱ��
		int			minMove;			// ��С�۸�䶯
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
		default:
			lThisEvent = "UNKNOWN_EVENT";
			break;
		}
		return lThisEvent;
	}
	// overtime call back routine

};
}