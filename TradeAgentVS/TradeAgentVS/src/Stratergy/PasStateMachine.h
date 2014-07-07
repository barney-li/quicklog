#define BOOST_DATE_TIME_SOURCE
#include <PASAux.h>
#include <Log.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
using namespace Pas;
namespace Pas
{
class PasStateMachine
{
private:
	TRADE_STATE mTradeState;
	Log mLogger;
	boost::mutex mTradeStateMutex;
public:
	PasStateMachine()
	{
		mLogger.SetLogFile("./Data/Log/", "StateMachine.log");
		mTradeState = IDLE_STATE;
		return;
	}
public:
	TRADE_STATE SetEvent(TRADE_EVENT aLatestEvent)
	{
		TRADE_STATE lNewState = IDLE_STATE;
		switch(mTradeState)
		{
		case IDLE_STATE:
			lNewState = HandleIdleState(aLatestEvent);
			break;
		case OPENING_SCND_STATE:
			lNewState = HandleOpeningScndState(aLatestEvent);
			break;
		case OPENING_PRIM_STATE:
			lNewState = HandleOpeningPrimState(aLatestEvent);
			break;
		case PENDING_STATE:
			lNewState = HandlePendingState(aLatestEvent);
			break;
		case CLOSING_BOTH_STATE:
			lNewState = HandleClosingBothState(aLatestEvent);
			break;
		case CANCELLING_SCND_STATE:
			lNewState = HandleCancellingScndState(aLatestEvent);
			break;
		case CLOSING_SCND_STATE:
			lNewState = HandleClosingScndState(aLatestEvent);
			break;
		case CANCELLING_PRIM_STATE:
			lNewState = HandleCancellingPrimState(aLatestEvent);
			break;
		case WAITING_SCND_CLOSE_STATE:
			lNewState = HandleWaitingScndCloseState(aLatestEvent);
			break;
		case WAITING_PRIM_CLOSE_STATE:
			lNewState = HandleWaitingPrimCloseState(aLatestEvent);
			break;
		default:
			break;
		}
		// record state transfer only when state get shifted
		if(lNewState != mTradeState)
		{
			mLogger.LogThisFast("["+PASAux::ShowTradeState(mTradeState)+"] + <" + PASAux::ShowTradeEvent(aLatestEvent) + "> --> [" + PASAux::ShowTradeState(lNewState) + "]");
			if(lNewState == IDLE_STATE)
			{
				mLogger.LogThisFastNoTimeStamp(" ");
			}
		}
		mTradeState = lNewState;
		return lNewState;
	}
	TRADE_STATE GetState()
	{
		return mTradeState;
	}

private:
	TRADE_STATE HandleIdleState(TRADE_EVENT aLatestEvent)
	{
		if(OPEN_PRICE_GOOD == aLatestEvent)
		{
			return OPENING_SCND_STATE;
		}
		else if(PRIM_OPENED == aLatestEvent)
		{
			return CLOSING_BOTH_STATE;
		}
		else if(SCND_OPENED == aLatestEvent)
		{
			return CLOSING_BOTH_STATE;
		}
		else
		{
			return IDLE_STATE;
		}
	}
	TRADE_STATE HandleOpeningScndState(TRADE_EVENT aLatestEvent)
	{
		if(SCND_OPENED == aLatestEvent)
		{
			return OPENING_PRIM_STATE;
		}
		else if(OPEN_PRICE_BAD == aLatestEvent || SCND_OPEN_TIMEOUT == aLatestEvent || NOT_TRADING_TIME == aLatestEvent)
		{
			return CANCELLING_SCND_STATE;
		}
		else
		{
			return OPENING_SCND_STATE;
		}
	}
	TRADE_STATE HandleOpeningPrimState(TRADE_EVENT aLatestEvent)
	{
		if(PRIM_OPENED == aLatestEvent)
		{
			return PENDING_STATE;
		}
		else if(OPEN_PRICE_BAD == aLatestEvent || PRIM_OPEN_TIMEOUT == aLatestEvent || NOT_TRADING_TIME == aLatestEvent)
		{
			return CANCELLING_PRIM_STATE;
		}
		else
		{
			return OPENING_PRIM_STATE;
		}
	}
	TRADE_STATE HandlePendingState(TRADE_EVENT aLatestEvent)
	{
		if(CLOSE_PRICE_GOOD == aLatestEvent || MUST_STOP == aLatestEvent || NOT_TRADING_TIME == aLatestEvent)
		{
			return CLOSING_BOTH_STATE;
		}
		else
		{
			return PENDING_STATE;
		}
	}
	TRADE_STATE HandleClosingBothState(TRADE_EVENT aLatestEvent)
	{
		if(PRIM_CLOSED == aLatestEvent)
		{
			return WAITING_SCND_CLOSE_STATE;
		}
		else if(SCND_CLOSED == aLatestEvent)
		{
			return WAITING_PRIM_CLOSE_STATE;
		}
		else
		{
			return CLOSING_BOTH_STATE;
		}
	}
	TRADE_STATE HandleWaitingScndCloseState(TRADE_EVENT aLatestEvent)
	{
		if(SCND_CLOSED == aLatestEvent)
		{
			return IDLE_STATE;
		}
		else
		{
			return WAITING_SCND_CLOSE_STATE;
		}
	}
	TRADE_STATE HandleWaitingPrimCloseState(TRADE_EVENT aLatestEvent)
	{
		if(PRIM_CLOSED == aLatestEvent)
		{
			return IDLE_STATE;
		}
		else
		{
			return WAITING_PRIM_CLOSE_STATE;
		}
	}
	TRADE_STATE HandleCancellingScndState(TRADE_EVENT aLatestEvent)
	{
		if(SCND_CANCELLED == aLatestEvent)
		{
			return IDLE_STATE;
		}
		else if(SCND_OPENED == aLatestEvent)
		{
			return CLOSING_SCND_STATE;
		}
		else
		{
			return CANCELLING_SCND_STATE;
		}
	}
	TRADE_STATE HandleClosingScndState(TRADE_EVENT aLatestEvent)
	{
		if(SCND_CLOSED == aLatestEvent)
		{
			return IDLE_STATE;
		}
		else
		{
			return CLOSING_SCND_STATE;
		}
	}
	TRADE_STATE HandleCancellingPrimState(TRADE_EVENT aLatestEvent)
	{
		if(PRIM_CANCELLED == aLatestEvent)
		{
			return CLOSING_SCND_STATE;
		}
		else if(PRIM_OPENED == aLatestEvent)
		{
			return CLOSING_BOTH_STATE;
		}
		else
		{
			return CANCELLING_PRIM_STATE;
		}
	}



};
}