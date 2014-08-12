#include "PrimeryAndSecondary.h"
#ifdef SIMULATION
void PrimeryAndSecondary::OpeningScndStateAsyncEventGenerator(void)
{
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	if(OPEN_COND1 == mOpenCond)
	{
		if(mScndEnterPrice>=lScnd.askPrice)
		{
			mScndEnterPrice = lScnd.askPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: SCND_OPENED (from async poster) at: "<<mScndEnterPrice;
			SetEvent(SCND_OPENED);
		}// 买开，当我的出价大于等于盘口卖价时成交
	}// 买次主力
	else
	{
		if(mScndEnterPrice<=lScnd.bidPrice)
		{
			mScndEnterPrice = lScnd.bidPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: SCND_OPENED (from async poster) at: "<<mScndEnterPrice;
			SetEvent(SCND_OPENED);
		}// 卖开，当我的出价小于等于盘口买价时成交
	}// 卖次主力
}
void PrimeryAndSecondary::OpeningPrimStateAsyncEventGenerator(void)
{
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	if(OPEN_COND2 == mOpenCond)
	{
		if(mPrimEnterPrice>=lPrim.askPrice)
		{
			mPrimEnterPrice = lPrim.askPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: PRIM_OPENED (from async poster) at: "<<mPrimEnterPrice;
			SetEvent(PRIM_OPENED);
		}// 买开，当我的出价大于等于盘口卖价时成交
	}// 买主力
	else
	{
		if(mPrimEnterPrice<=lPrim.bidPrice)
		{
			mPrimEnterPrice = lPrim.bidPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: PRIM_OPENED (from async poster) at: "<<mPrimEnterPrice;
			SetEvent(PRIM_OPENED);
		}// 卖开，当我的出价小于等于盘口买价时成交
	}// 卖主力
}
void PrimeryAndSecondary::AsyncEventPoster(void)
{
	int lPostTime=0;
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	switch(mStateMachine.GetState())
	{
	case IDLE_STATE:
		break;
	case OPENING_SCND_STATE:
		break;
	case CHECKING_SCND_STATE:
		SetEvent(SCND_OPENED);
		break;
	case OPENING_PRIM_STATE:
		break;
	case PENDING_STATE:
		break;
	case CLOSING_BOTH_STATE:
		break;
	case CANCELLING_SCND_STATE:
		break;
	case CLOSING_SCND_STATE:
		break;
	case CANCELLING_PRIM_STATE:
		break;
	case WAITING_SCND_CLOSE_STATE:
		break;
	case WAITING_PRIM_CLOSE_STATE:
		break;
	default:
		break;
	}
}
void PrimeryAndSecondary::AsyncEventPoster(TRADE_EVENT aEvent)
{
	boost::this_thread::sleep(boost::posix_time::seconds(1));
	SetEvent(aEvent);
}
#endif