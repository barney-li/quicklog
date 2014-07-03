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
		TThostFtdcPriceType				tradePrice;//�ɽ���
		TThostFtdcVolumeType			tradeVolume;//�ɽ���
		///��������
		TThostFtdcDirectionType			direction;
		///��ƽ��־
		TThostFtdcOffsetFlagType		offsetFlag;
	};
	TRADE_RECORD lastTrade;

	// essential data of market data
	typedef struct BasicMarketData
	{
		TThostFtdcInstrumentIDType	instrumentId;
		TThostFtdcPriceType			lastPrice;//���¼�
		TThostFtdcVolumeType		volume;//�ۼӳɽ���
		TThostFtdcPriceType			askPrice;//��һ��
		TThostFtdcVolumeType		askVolume;//��һ��
		TThostFtdcPriceType			bidPrice;//��һ��
		TThostFtdcVolumeType		bidVolume;//��һ��
		TThostFtdcPriceType			upperLimit;//��ͣ���
		TThostFtdcPriceType			lowerLimit;//��ͣ���
		TThostFtdcTimeType			updateTime;//����ʱ��
		TThostFtdcMillisecType		updateMillisec;//���º�����
		boost::posix_time::ptime	localTime;//����ʱ���
	};

	// strategy parameters
	typedef struct StrategyParameter
	{
		double		mainPriceDelta;		// DeltaΪ�����������ϴ����ݵļ۸�䶯
		double		secPriceDiff;		// DiffΪ��ǰ�̿ڵ������۲�
		double		closePriceConst;	// ƽ��ʱ�ļ۸��ϸ�����
		double		openPriceConst;		// ���ּ۸��ϸ�����
		double		floatError;			// ���������
		int			secVolumeDiff;		// ��ǰ�̿ڵ�����������
		int			openCloseVolume;	// ÿ�ο���ƽ�ֵ�����
		int			maxOpenTime;		// ��󿪲ֵȴ�ʱ��
		int			minMove;			// ��С�۸�䶯
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