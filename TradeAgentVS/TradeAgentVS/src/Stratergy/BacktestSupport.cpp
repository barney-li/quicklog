#include "PrimeryAndSecondary.h"
#ifdef SIMULATION
void PrimeryAndSecondary::OpeningScndStateAsyncEventGenerator(void)
{
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	string lCurTime = "2000-01-01 "+(string)lScnd.updateTime;
	if(lScnd.updateMillisec == 500)
	{
		lCurTime = lCurTime + ".500";
	}
	if(time_from_string(lCurTime) <= time_from_string(mScndOpenTime))
	{
		return;
	}//如果当前时间没有大于下单时间，不做任何判断
	time_duration lOpenDuration = time_from_string(lCurTime)-time_from_string(mScndOpenTime);
	if(lOpenDuration.total_milliseconds()>stgArg.scndOpenTime)
	{
		tempStream.clear();
		tempStream.str("");
		tempStream<<"[EVENT]: SCND_OPEN_TIMEOUT (from async poster) Evidance: from "<<mScndOpenTime<<" to "<<lCurTime;
		logger.LogThisFast(tempStream.str());
		SetEvent(SCND_OPEN_TIMEOUT);
	}
	if(OPEN_COND1 == mOpenCond)
	{
		if(mScndEnterPrice>=lScnd.askPrice && mLatestInstType == SCND_INSTRUMENT)
		{
			mScndEnterPrice = lScnd.askPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: SCND_OPENED (from async poster) at: "<<mScndEnterPrice;
			logger.LogThisFast(tempStream.str());
			SetEvent(SCND_OPENED);
		}// 买开，当我的出价大于等于盘口卖价时成交，并且只在最新数据为次主力时有效
	}// 买次主力
	else
	{
		if(mScndEnterPrice<=lScnd.bidPrice && mLatestInstType == SCND_INSTRUMENT)
		{
			mScndEnterPrice = lScnd.bidPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: SCND_OPENED (from async poster) at: "<<mScndEnterPrice;
			logger.LogThisFast(tempStream.str());
			SetEvent(SCND_OPENED);
		}// 卖开，当我的出价小于等于盘口买价时成交，并且只在最新数据为次主力时有效
	}// 卖次主力
}

void PrimeryAndSecondary::OpeningPrimStateAsyncEventGenerator(void)
{
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	string lCurTime = "2000-01-01 "+(string)lPrim.updateTime;
	if(lPrim.updateMillisec == 500)
	{
		lCurTime = lCurTime + ".500";
	}
	if(time_from_string(lCurTime) <= time_from_string(mPrimOpenTime))
	{
		return;
	}//如果当前时间没有大于下单时间，不做任何判断
	time_duration lOpenDuration = time_from_string(lCurTime)-time_from_string(mPrimOpenTime);
	if(lOpenDuration.total_milliseconds()>stgArg.primOpenTime)
	{
		tempStream.clear();
		tempStream.str("");
		tempStream<<"[EVENT]: PRIM_OPEN_TIMEOUT (from async poster) Evidance: from "<<mScndOpenTime<<" to "<<lCurTime;
		logger.LogThisFast(tempStream.str());
		SetEvent(PRIM_OPEN_TIMEOUT);
	}
	if(OPEN_COND2 == mOpenCond)
	{
		if(mPrimEnterPrice+3*stgArg.minMove>=lPrim.askPrice && mLatestInstType == PRIM_INSTRUMENT)
		{
			mPrimEnterPrice = lPrim.askPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: PRIM_OPENED (from async poster) at: "<<mPrimEnterPrice;
			logger.LogThisFast(tempStream.str());
			SetEvent(PRIM_OPENED);
		}// 买开，当我的出价大于等于盘口卖价时成交，并且只在最新数据为主力数据时有效
	}// 买主力
	else
	{
		if(mPrimEnterPrice-3*stgArg.minMove<=lPrim.bidPrice && mLatestInstType == PRIM_INSTRUMENT)
		{
			mPrimEnterPrice = lPrim.bidPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: PRIM_OPENED (from async poster) at: "<<mPrimEnterPrice;
			logger.LogThisFast(tempStream.str());
			SetEvent(PRIM_OPENED);
		}// 卖开，当我的出价小于等于盘口买价时成交，并且只在最新数据为主力数据时有效
	}// 卖主力
}

void PrimeryAndSecondary::ClosingBothStateAsyncEventGenerator()
{
	ClosingScndStateAsyncEventGenerator();
	ClosingPrimStateAsyncEventGenerator();
}

void PrimeryAndSecondary::ClosingScndStateAsyncEventGenerator()
{
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	string lCurTime = "2000-01-01 "+(string)lScnd.updateTime;
	if(lScnd.updateMillisec == 500)
	{
		lCurTime = lCurTime + ".500";
	}
	if(time_from_string(lCurTime) <= time_from_string(mScndCloseTime))
	{
		return;
	}//如果当前时间没有大于平仓下单时间，不做任何判断
	if(OPEN_COND1 == mOpenCond)
	{
		if(mScndClosePrice<=lScnd.bidPrice)
		{
			mScndClosePrice = lScnd.bidPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: SCND_CLOSED (from async poster) at: "<<mScndClosePrice;
			logger.LogThisFast(tempStream.str());
			SetEvent(SCND_CLOSED);

			if(mCloseScndOnly)
			{
				mCloseScndOnly = false;
				double lNetProfit;
				double lProfit;
				if(OPEN_COND1 == mOpenCond)
				{
					lNetProfit = mScndClosePrice - mScndEnterPrice;
				}
				else
				{
					lNetProfit = mScndEnterPrice - mScndClosePrice;
				}
				lProfit = lNetProfit - stgArg.cost;// 是不是应该乘minmove，不用，初始化的时候算过了
				tempStream.clear();
				tempStream.str("");
				tempStream<<"[INFO]: estimate profit:	"<<lNetProfit;
				logger.LogThisFast(tempStream.str());
				if(lProfit>0)
				{
					mWin++;
				}
				else
				{
					mLose++;
				}
				mTotalProfit += lProfit;
			}
		}// 卖平，当我的出价小于等于盘口买价时成交
	}// 卖平次主力
	else
	{
		if(mScndClosePrice>=lScnd.askPrice)
		{
			mScndClosePrice = lScnd.askPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: SCND_CLOSED (from async poster) at: "<<mScndClosePrice;
			logger.LogThisFast(tempStream.str());
			SetEvent(SCND_CLOSED);

			if(mCloseScndOnly)
			{
				mCloseScndOnly = false;
				double lNetProfit;
				double lProfit;
				if(OPEN_COND1 == mOpenCond)
				{
					lNetProfit = mScndClosePrice - mScndEnterPrice;
				}
				else
				{
					lNetProfit = mScndEnterPrice - mScndClosePrice;
				}
				lProfit = lNetProfit - stgArg.cost;// 是不是应该乘minmove，不用，初始化的时候算过了
				tempStream.clear();
				tempStream.str("");
				tempStream<<"[INFO]: estimate profit:	"<<lNetProfit;
				logger.LogThisFast(tempStream.str());
				if(lProfit>0)
				{
					mWin++;
				}
				else
				{
					mLose++;
				}
				mTotalProfit += lProfit;
			}
		}// 买平，当我的出价大于等于盘口卖价时成交
	}// 买平次主力

	
}

void PrimeryAndSecondary::ClosingPrimStateAsyncEventGenerator()
{
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	string lCurTime = "2000-01-01 "+(string)lPrim.updateTime;
	if(lPrim.updateMillisec == 500)
	{
		lCurTime = lCurTime + ".500";
	}
	if(time_from_string(lCurTime) <= time_from_string(mPrimCloseTime))
	{
		return;
	}//如果当前时间没有大于下单时间，不做任何判断
	if(OPEN_COND2 == mOpenCond)
	{
		if(mPrimClosePrice<=lPrim.bidPrice)
		{
			mPrimClosePrice = lPrim.bidPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: PRIM_CLOSED (from async poster) at: "<<mPrimClosePrice;
			logger.LogThisFast(tempStream.str());
			SetEvent(PRIM_CLOSED);

			tempStream.clear();
			tempStream.str("");
			tempStream<<"[INFO]: estimate profit:	"<<EstimateProfit();
			logger.LogThisFast(tempStream.str());
		#ifdef BACK_TEST
			mProfitLog.LogThisFast(tempStream.str());
			double lPureProfit = EstimateProfit()-stgArg.cost;// 连手续费一起算上
			if(lPureProfit > 0)
			{
				mWin++;
			}
			else
			{
				mLose++;
			}
			mTotalProfit += lPureProfit;
		#endif
		}// 卖平，当我的出价小于等于盘口买价时成交
	}// 卖平主力
	else
	{
		if(mPrimClosePrice>=lPrim.askPrice)
		{
			mPrimClosePrice = lPrim.askPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: PRIM_CLOSED (from async poster) at: "<<mPrimClosePrice;
			logger.LogThisFast(tempStream.str());
			SetEvent(PRIM_CLOSED);

			tempStream.clear();
			tempStream.str("");
			tempStream<<"[INFO]: estimate profit:	"<<EstimateProfit();
			logger.LogThisFast(tempStream.str());
		#ifdef BACK_TEST
			mProfitLog.LogThisFast(tempStream.str());
			double lPureProfit = EstimateProfit()-stgArg.cost;// 连手续费一起算上
			if(lPureProfit > 0)
			{
				mWin++;
			}
			else
			{
				mLose++;
			}
			mTotalProfit += lPureProfit;
		#endif
		}// 买平，当我的出价大于等于盘口卖价时成交
	}// 买平主力

	
}

void PrimeryAndSecondary::WaitingPrimCloseStateAsyncEventGenerator()
{
	ClosingPrimStateAsyncEventGenerator();
}

void PrimeryAndSecondary::WaitingScndCloseStateAsyncEventGenerator()
{
	ClosingScndStateAsyncEventGenerator();
}

void PrimeryAndSecondary::CancellingPrimStateAsyncEventGenerator()
{
	logger.LogThisFast("[EVENT]: PRIM_CANCELLED");
	SetEvent(PRIM_CANCELLED);
}

void PrimeryAndSecondary::CancellingScndStateAsyncEventGenerator()
{
	logger.LogThisFast("[EVENT]: SCND_CANCELLED");
	SetEvent(SCND_CANCELLED);
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
			OpeningScndStateAsyncEventGenerator();
		break;
	case CHECKING_SCND_STATE:
		break;
	case OPENING_PRIM_STATE:
			OpeningPrimStateAsyncEventGenerator();
		break;
	case PENDING_STATE:
		break;
	case CLOSING_BOTH_STATE:
		ClosingBothStateAsyncEventGenerator();
		break;
	case CANCELLING_SCND_STATE:
		CancellingScndStateAsyncEventGenerator();
		break;
	case CLOSING_SCND_STATE:
		ClosingScndStateAsyncEventGenerator();
		break;
	case CANCELLING_PRIM_STATE:
		CancellingPrimStateAsyncEventGenerator();
		break;
	case WAITING_SCND_CLOSE_STATE:
		WaitingScndCloseStateAsyncEventGenerator();
		break;
	case WAITING_PRIM_CLOSE_STATE:
		WaitingPrimCloseStateAsyncEventGenerator();
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