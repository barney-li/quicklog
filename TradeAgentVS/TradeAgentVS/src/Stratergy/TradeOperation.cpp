#include <PrimeryAndSecondary.h>
/************************************************************************/
// 次主力开仓函数。
/************************************************************************/
void PrimeryAndSecondary::OpenScnd()
{
	if( primDataBuf[primBufIndex].bidPrice - scndDataBuf[scndBufIndex].askPrice > 0 &&
		primDataBuf[primBufIndex].bidPrice - scndDataBuf[scndBufIndex].askPrice > mBoll.GetBoll(0).mOutterUpperLine )
	{
		/* condition 1 */
		mOpenCond = OPEN_COND1;
		logger.LogThisFast("[ACTION]: BUY_SCND");
		mTradeDir = BUY_SCND_SELL_PRIM;
		if(Buy(stgArg.secondaryInst, scndDataBuf[scndBufIndex].lastPrice, stgArg.openShares, &lastScndOrder) != true)
		{
			logger.LogThisFast("[ERROR]: buy scnd error");
		}
		mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this));
	}
	else if( primDataBuf[primBufIndex].askPrice - scndDataBuf[scndBufIndex].bidPrice > 0 &&
		primDataBuf[primBufIndex].askPrice - scndDataBuf[scndBufIndex].bidPrice < mBoll.GetBoll(0).mOutterLowerLine )
	{
		/* condition 2 */
		mOpenCond = OPEN_COND2;
		logger.LogThisFast("[ACTION]: SHORT_SCND");
		mTradeDir = BUY_PRIM_SELL_SCND;
		if(Sell(stgArg.secondaryInst, scndDataBuf[scndBufIndex].lastPrice, stgArg.openShares, &lastScndOrder) != true)
		{
			logger.LogThisFast("[ERROR]: sell scnd error");
		}
		mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this));
	}
	else if( scndDataBuf[scndBufIndex].bidPrice - primDataBuf[primBufIndex].askPrice > 0 &&
		scndDataBuf[scndBufIndex].bidPrice - primDataBuf[primBufIndex].askPrice > mBoll.GetBoll(0).mOutterUpperLine )
	{
		/* condition 3 */
		mOpenCond = OPEN_COND3;
		logger.LogThisFast("[ACTION]: SHORT_SCND");
		mTradeDir = BUY_PRIM_SELL_SCND;
		if(Sell(stgArg.secondaryInst, scndDataBuf[scndBufIndex].lastPrice, stgArg.openShares, &lastScndOrder) != true)
		{
			logger.LogThisFast("[ERROR]: sell scnd error");
		}
		mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this));
	}
	else if( scndDataBuf[scndBufIndex].askPrice - primDataBuf[primBufIndex].bidPrice > 0 &&
		scndDataBuf[scndBufIndex].askPrice - primDataBuf[primBufIndex].bidPrice < mBoll.GetBoll(0).mOutterLowerLine )
	{
		/* condition 4 */
		mOpenCond = OPEN_COND4;
		logger.LogThisFast("[ACTION]: BUY_SCND");
		mTradeDir = BUY_SCND_SELL_PRIM;
		if(Buy(stgArg.secondaryInst, scndDataBuf[scndBufIndex].lastPrice, stgArg.openShares, &lastScndOrder) != true)
		{
			logger.LogThisFast("[ERROR]: buy scnd error");
		}
		mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this));
	}
	else
	{
		logger.LogThisFast("[ERROR]: wrong open scnd condition");
		cout<<"[ERROR]: wrong open scnd condition"<<endl;
	}
}
/************************************************************************/
// 主力开仓函数。
/************************************************************************/
void PrimeryAndSecondary::OpenPrim()
{
	if(BUY_SCND_SELL_PRIM == mTradeDir)
	{
		logger.LogThisFast("[ACTION]: SHORT_PRIM");
		if(Sell(stgArg.primaryInst, primDataBuf[primBufIndex].lowerLimit, stgArg.openShares, &lastPrimOrder) != true)
		{
			logger.LogThisFast("[ERROR]: sell prim error");
		}
		mWaitPrimOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitPrimOpen, this));
	}
	else if(BUY_PRIM_SELL_SCND == mTradeDir)
	{
		logger.LogThisFast("[ACTION]: BUY_PRIM");
		if(Buy(stgArg.primaryInst, primDataBuf[primBufIndex].upperLimit, stgArg.openShares, &lastPrimOrder) != true)
		{
			logger.LogThisFast("[ERROR]: buy prim error");
		}
		mWaitPrimOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitPrimOpen, this));
	}
	else
	{
		logger.LogThisFast("[FATAL ERROR]: wrong open prim condition");
		cout<<"[FATAL ERROR]: wrong open prim condition"<<endl;
	}
}
void PrimeryAndSecondary::CloseScnd()
{
	//平多头
	if(mScndYdLongPosition>0)
	{
		logger.LogThisFast("[ACTION]: SELL_SCND");
		if(Sell(stgArg.secondaryInst, scndDataBuf[scndBufIndex].lowerLimit, mScndYdLongPosition, &lastScndOrder, true) != true)
		{
			logger.LogThisFast("[ERROR]: sell yd scnd returned error");
		}
	}
	if(mScndTodayLongPosition>0)
	{
		logger.LogThisFast("[ACTION]: SELL_SCND");
		if(Sell(stgArg.secondaryInst, scndDataBuf[scndBufIndex].lowerLimit, mScndTodayLongPosition, &lastScndOrder) != true)
		{
			logger.LogThisFast("[ERROR]: sell today scnd returned error");
		}
	}
	//平空头
	if(mScndYdShortPosition>0)
	{
		logger.LogThisFast("[ACTION]: COVER_SCND");
		if(BuyToCover(stgArg.secondaryInst, scndDataBuf[scndBufIndex].upperLimit, mScndYdShortPosition, &lastScndOrder, true) != true)
		{
			logger.LogThisFast("[ERROR]: cover yd scnd returned error");
		}
	}
	if(mScndTodayShortPosition>0)
	{
		logger.LogThisFast("[ACTION]: COVER_SCND");
		if(BuyToCover(stgArg.secondaryInst, scndDataBuf[scndBufIndex].upperLimit, mScndTodayShortPosition, &lastScndOrder) != true)
		{
			logger.LogThisFast("[ERROR]: cover today scnd returned error");
		}
	} 
}
void PrimeryAndSecondary::ClosePrim()
{
	//平空头
	if(mPrimYdShortPosition>0)
	{
		logger.LogThisFast("[ACTION]: COVER_PRIM");
		if(BuyToCover(stgArg.primaryInst, primDataBuf[primBufIndex].upperLimit, mPrimYdShortPosition, &lastPrimOrder, true) != true)
		{
			logger.LogThisFast("[ERROR]: cover yd prim returned error");
		}
	}
	if(mPrimTodayShortPosition>0)
	{
		logger.LogThisFast("[ACTION]: COVER_PRIM");
		if(BuyToCover(stgArg.primaryInst, primDataBuf[primBufIndex].upperLimit, mPrimTodayShortPosition, &lastPrimOrder) != true)
		{
			logger.LogThisFast("[ERROR]: cover today prim returned error");
		}
	} 

	//平多头
	if(mPrimYdLongPosition>0)
	{
		logger.LogThisFast("[ACTION]: SELL_SCND");
		if(Sell(stgArg.primaryInst, primDataBuf[primBufIndex].lowerLimit, mPrimYdLongPosition, &lastPrimOrder, true) != true)
		{
			logger.LogThisFast("[ERROR]: sell yd prim returned error");
		}
	}
	if(mPrimTodayLongPosition>0)
	{
		logger.LogThisFast("[ACTION]: SELL_SCND");
		if(Sell(stgArg.primaryInst, primDataBuf[primBufIndex].lowerLimit, mPrimTodayLongPosition, &lastPrimOrder) != true)
		{
			logger.LogThisFast("[ERROR]: sell today prim returned error");
		}
	}
}
void PrimeryAndSecondary::CancelScnd()
{
	if(CancelOrder(&lastScndOrder) != true)
	{
		cout<<"[ERROR]: cancel scnd order returned error"<<endl;
		logger.LogThisFast("[ERROR]: cancel scnd order returned error");
	}
}
void PrimeryAndSecondary::CancelPrim()
{
	if(CancelOrder(&lastPrimOrder) != true)
	{
		cout<<"[ERROR]: cancel prim order returned error"<<endl;
		logger.LogThisFast("[ERROR]: cancel prim order returned error");
	}
}
void PrimeryAndSecondary::CloseBoth()
{
	CloseScnd();
	ClosePrim();
}
void PrimeryAndSecondary::CheckPosition()
{
	ReqQryInvestorPosition(stgArg.secondaryInst.c_str());
	ReqQryInvestorPosition(stgArg.primaryInst.c_str());
}
void PrimeryAndSecondary::CheckOrder()
{
	ReqQryOrder(stgArg.secondaryInst.c_str());
	ReqQryOrder(stgArg.primaryInst.c_str());
}