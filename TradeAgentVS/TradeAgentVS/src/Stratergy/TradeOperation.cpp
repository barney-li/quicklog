#include <PrimeryAndSecondary.h>
/************************************************************************/
// 次主力开仓函数。
/************************************************************************/
void PrimeryAndSecondary::OpenScnd()
{
	if( OPEN_COND1 == mOpenCond )
	{
		/* condition 1 */
		
		logger.LogThisFast("[ACTION]: BUY_SCND");
		mTradeDir = BUY_SCND_SELL_PRIM;
		if(Buy(stgArg.secondaryInst, scndDataBuf[scndBufIndex].lastPrice, stgArg.openShares, &lastScndOrder) != true)
		{
			logger.LogThisFast("[ERROR]: buy scnd error");
		}
		mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this, ++mOpenScndId));
	}
	else if( OPEN_COND2 == mOpenCond )
	{
		/* condition 2 */
		
		logger.LogThisFast("[ACTION]: SHORT_SCND");
		mTradeDir = BUY_PRIM_SELL_SCND;
		if(SellShort(stgArg.secondaryInst, scndDataBuf[scndBufIndex].lastPrice, stgArg.openShares, &lastScndOrder) != true)
		{
			logger.LogThisFast("[ERROR]: sell scnd error");
		}
		mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this, ++mOpenScndId));
	}
	else if( OPEN_COND3 == mOpenCond )
	{
		/* condition 3 */
		
		logger.LogThisFast("[ACTION]: SHORT_SCND");
		mTradeDir = BUY_PRIM_SELL_SCND;
		if(SellShort(stgArg.secondaryInst, scndDataBuf[scndBufIndex].lastPrice, stgArg.openShares, &lastScndOrder) != true)
		{
			logger.LogThisFast("[ERROR]: sell scnd error");
		}
		mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this, ++mOpenScndId));
	}
	else if( OPEN_COND4 == mOpenCond )
	{
		/* condition 4 */
		
		logger.LogThisFast("[ACTION]: BUY_SCND");
		mTradeDir = BUY_SCND_SELL_PRIM;
		if(Buy(stgArg.secondaryInst, scndDataBuf[scndBufIndex].lastPrice, stgArg.openShares, &lastScndOrder) != true)
		{
			logger.LogThisFast("[ERROR]: buy scnd error");
		}
		mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this, ++mOpenScndId));
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
		if(SellShort(stgArg.primaryInst, primDataBuf[primBufIndex].lowerLimit, stgArg.openShares, &lastPrimOrder) != true)
		{
			logger.LogThisFast("[ERROR]: sell prim error");
		}
		mWaitPrimOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitPrimOpen, this, ++mOpenPrimId));
	}
	else if(BUY_PRIM_SELL_SCND == mTradeDir)
	{
		logger.LogThisFast("[ACTION]: BUY_PRIM");
		if(Buy(stgArg.primaryInst, primDataBuf[primBufIndex].upperLimit, stgArg.openShares, &lastPrimOrder) != true)
		{
			logger.LogThisFast("[ERROR]: buy prim error");
		}
		mWaitPrimOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitPrimOpen, this, ++mOpenPrimId));
	}
	else
	{
		logger.LogThisFast("[FATAL ERROR]: wrong open prim condition");
		cout<<"[FATAL ERROR]: wrong open prim condition"<<endl;
	}
}
void PrimeryAndSecondary::CloseScnd()
{
	if(!mCloseScndCD)
	{
		return;
	}
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
	mCloseScndCD = false;
	mCoolDownCloseScndThread = new boost::thread(boost::bind(&PrimeryAndSecondary::CoolDownCloseScnd, this));
}
void PrimeryAndSecondary::ClosePrim()
{
	//如果CD还没到就直接返回
	if(!mClosePrimCD)
	{
		return;
	}
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
		logger.LogThisFast("[ACTION]: SELL_PRIM");
		if(Sell(stgArg.primaryInst, primDataBuf[primBufIndex].lowerLimit, mPrimYdLongPosition, &lastPrimOrder, true) != true)
		{
			logger.LogThisFast("[ERROR]: sell yd prim returned error");
		}
	}
	if(mPrimTodayLongPosition>0)
	{
		logger.LogThisFast("[ACTION]: SELL_PRIM");
		if(Sell(stgArg.primaryInst, primDataBuf[primBufIndex].lowerLimit, mPrimTodayLongPosition, &lastPrimOrder) != true)
		{
			logger.LogThisFast("[ERROR]: sell today prim returned error");
		}
	}
	mClosePrimCD = false;
	mCoolDownClosePrimThread = new boost::thread(boost::bind(&PrimeryAndSecondary::CoolDownClosePrim, this));
}
void PrimeryAndSecondary::CancelScnd()
{
	//如果CD还没到就直接返回
	if(!mCancelScndCD)
	{
		return;
	}
	logger.LogThisFast("[ACTION]: CANCEL_SCND");
	if(CancelOrder(&lastScndOrder) != true)
	{
		cout<<"[ERROR]: cancel scnd order returned error"<<endl;
		logger.LogThisFast("[ERROR]: cancel scnd order returned error");
	}
	mCancelScndCD = false;
	mCoolDownCancelScndThread = new boost::thread(boost::bind(&PrimeryAndSecondary::CoolDownCancelScnd, this));
}
void PrimeryAndSecondary::CancelPrim()
{
	//如果CD还没到就直接返回
	if(!mCancelPrimCD)
	{
		return;
	}
	logger.LogThisFast("[ACTION]: CANCEL_PRIM");
	if(CancelOrder(&lastPrimOrder) != true)
	{
		cout<<"[ERROR]: cancel prim order returned error"<<endl;
		logger.LogThisFast("[ERROR]: cancel prim order returned error");
	}
	mCancelPrimCD = false;
	mCoolDownCancelPrimThread = new boost::thread(boost::bind(&PrimeryAndSecondary::CoolDownCancelPrim, this));
}
void PrimeryAndSecondary::CloseBoth()
{
	CloseScnd();
	ClosePrim();
}
void PrimeryAndSecondary::CheckPrimPosition()
{
	ReqQryInvestorPosition(stgArg.primaryInst.c_str());
}
void PrimeryAndSecondary::CheckScndPosition()
{
	ReqQryInvestorPosition(stgArg.secondaryInst.c_str());
}
void PrimeryAndSecondary::CheckPrimOrder()
{
	ReqQryOrder(stgArg.primaryInst.c_str());
}
void PrimeryAndSecondary::CheckScndOrder()
{
	ReqQryOrder(stgArg.secondaryInst.c_str());
}