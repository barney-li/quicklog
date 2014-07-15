#include <PrimeryAndSecondary.h>

#ifndef SIMULATION
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
#ifdef OPPONENT_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].askPrice;
		if(Buy(stgArg.secondaryInst, scndDataBuf[scndBufIndex].askPrice, stgArg.openShares, &lastScndOrder) != true)
#else
		mScndEnterPrice = scndDataBuf[scndBufIndex].lastPrice;
		if(Buy(stgArg.secondaryInst, scndDataBuf[scndBufIndex].lastPrice, stgArg.openShares, &lastScndOrder) != true)
#endif
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
#ifdef OPPONENT_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].bidPrice;
		if(SellShort(stgArg.secondaryInst, scndDataBuf[scndBufIndex].bidPrice, stgArg.openShares, &lastScndOrder) != true)
#else
		mScndEnterPrice = scndDataBuf[scndBufIndex].lastPrice;
		if(SellShort(stgArg.secondaryInst, scndDataBuf[scndBufIndex].lastPrice, stgArg.openShares, &lastScndOrder) != true)
#endif
		{
			logger.LogThisFast("[ERROR]: sell scnd error");
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
		mPrimEnterPrice = primDataBuf[primBufIndex].bidPrice;
		if(SellShort(stgArg.primaryInst, primDataBuf[primBufIndex].lowerLimit, mTradedShares, &lastPrimOrder) != true)
		{
			logger.LogThisFast("[ERROR]: sell prim error");
		}
		mWaitPrimOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitPrimOpen, this, ++mOpenPrimId));
	}
	else if(BUY_PRIM_SELL_SCND == mTradeDir)
	{
		logger.LogThisFast("[ACTION]: BUY_PRIM");
		mPrimEnterPrice = primDataBuf[primBufIndex].askPrice;
		if(Buy(stgArg.primaryInst, primDataBuf[primBufIndex].upperLimit, mTradedShares, &lastPrimOrder) != true)
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
	tempStream.clear();
	tempStream.str("");
	tempStream<<"[INFO]: estimate profit: "<<EstimateProfit();
	logger.LogThisFast(tempStream.str());
}
void PrimeryAndSecondary::CheckPrimPosition()
{
	ReqQryInvestorPosition(stgArg.primaryInst.c_str());
}
void PrimeryAndSecondary::CheckScndPosition()
{
	ReqQryInvestorPosition(stgArg.secondaryInst.c_str());
}
int PrimeryAndSecondary::CheckPrimOrder()
{
	return ReqQryOrder(stgArg.primaryInst.c_str());
}
int PrimeryAndSecondary::CheckScndOrder()
{
	return ReqQryOrder(stgArg.secondaryInst.c_str());
}
#endif
#ifdef SIMULATION
void PrimeryAndSecondary::OpenScnd()
{
	stringstream lPrice;
	
	if( OPEN_COND1 == mOpenCond )
	{
		/* condition 1 */
		
		logger.LogThisFast("[ACTION]: BUY_SCND");
#ifdef OPPONENT_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].askPrice;
		lPrice<<scndDataBuf[scndBufIndex].askPrice;
#else
		mScndEnterPrice = scndDataBuf[scndBufIndex].lastPrice;
		lPrice<<scndDataBuf[scndBufIndex].lastPrice;
#endif
		logger.LogThisFast(lPrice.str());
		mTradeDir = BUY_SCND_SELL_PRIM;
		
#ifdef BACK_TEST
		mEventQueue.push(SCND_OPENED);
#else
		new boost::thread(&PrimeryAndSecondary::AsyncEventPoster, this, SCND_OPENED);
		mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this, ++mOpenScndId));
#endif
		mScndTodayLongPosition = 1;
	}
	else if( OPEN_COND2 == mOpenCond )
	{
		/* condition 2 */
		
		logger.LogThisFast("[ACTION]: SHORT_SCND");
#ifdef OPPONENT_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].bidPrice;
		lPrice<<scndDataBuf[scndBufIndex].bidPrice;
#else
		mScndEnterPrice = scndDataBuf[scndBufIndex].lastPrice;
		lPrice<<scndDataBuf[scndBufIndex].lastPrice;
#endif
		logger.LogThisFast(lPrice.str());
		mTradeDir = BUY_PRIM_SELL_SCND;
#ifdef BACK_TEST
		mEventQueue.push(SCND_OPENED);
#else
		new boost::thread(&PrimeryAndSecondary::AsyncEventPoster, this, SCND_OPENED);
		mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this, ++mOpenScndId));
#endif
		mScndTodayShortPosition = 1;
	}
	else
	{
		logger.LogThisFast("[ERROR]: wrong open scnd condition");
		cout<<"[ERROR]: wrong open scnd condition"<<endl;
	}
}
void PrimeryAndSecondary::OpenPrim()
{
	stringstream lPrice;
	if(BUY_SCND_SELL_PRIM == mTradeDir)
	{
		logger.LogThisFast("[ACTION]: SHORT_PRIM");
		lPrice<<primDataBuf[primBufIndex].bidPrice;
		logger.LogThisFast(lPrice.str());
		mPrimEnterPrice = primDataBuf[primBufIndex].bidPrice;
#ifdef BACK_TEST
		mEventQueue.push(PRIM_OPENED);
#else
		new boost::thread(&PrimeryAndSecondary::AsyncEventPoster, this, PRIM_OPENED);
		mWaitPrimOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitPrimOpen, this, ++mOpenPrimId));
#endif
		mPrimTodayShortPosition = 1;
	}
	else if(BUY_PRIM_SELL_SCND == mTradeDir)
	{
		logger.LogThisFast("[ACTION]: BUY_PRIM");
		lPrice<<primDataBuf[primBufIndex].askPrice;
		logger.LogThisFast(lPrice.str());
		mPrimEnterPrice = primDataBuf[primBufIndex].askPrice;
#ifdef BACK_TEST
		mEventQueue.push(PRIM_OPENED);
#else
		new boost::thread(&PrimeryAndSecondary::AsyncEventPoster, this, PRIM_OPENED);
		mWaitPrimOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitPrimOpen, this, ++mOpenPrimId));
#endif
		mPrimTodayLongPosition = 1;
	}
	else
	{
		logger.LogThisFast("[FATAL ERROR]: wrong open prim condition");
		cout<<"[FATAL ERROR]: wrong open prim condition"<<endl;
	}
}
void PrimeryAndSecondary::CloseScnd()
{
	stringstream lPrice;
#ifndef BACK_TEST
	if(!mCloseScndCD)
	{
		return;
	}
#endif
	//平多头
	if(OPEN_COND1 == mOpenCond)
	{
		logger.LogThisFast("[ACTION]: SELL_SCND");
		lPrice<<scndDataBuf[scndBufIndex].bidPrice;
		logger.LogThisFast(lPrice.str());
#ifdef BACK_TEST
		mEventQueue.push(SCND_CLOSED);
#else
		new boost::thread(&PrimeryAndSecondary::AsyncEventPoster, this, SCND_CLOSED);
#endif
	}
	//平空头
	if(OPEN_COND2 == mOpenCond)
	{
		logger.LogThisFast("[ACTION]: COVER_SCND");
		lPrice<<scndDataBuf[scndBufIndex].askPrice;
		logger.LogThisFast(lPrice.str());
#ifdef BACK_TEST
		mEventQueue.push(SCND_CLOSED);
#else
		new boost::thread(&PrimeryAndSecondary::AsyncEventPoster, this, SCND_CLOSED);
#endif
	}
#ifndef BACK_TEST
	mCloseScndCD = false;
	mCoolDownCloseScndThread = new boost::thread(boost::bind(&PrimeryAndSecondary::CoolDownCloseScnd, this));
#endif//回测时绝对不能用这个CoolDown，因为速度太快，CoolDown还没完成回测可能就结束了
}
void PrimeryAndSecondary::ClosePrim()
{
	stringstream lPrice;
#ifndef BACK_TEST
	//如果CD还没到就直接返回
	if(!mClosePrimCD)
	{
		return;
	}
#endif
	//平空头
	if(OPEN_COND1 == mOpenCond)
	{
		logger.LogThisFast("[ACTION]: COVER_PRIM");
		lPrice<<primDataBuf[primBufIndex].askPrice;
		logger.LogThisFast(lPrice.str());
#ifdef BACK_TEST
		mEventQueue.push(PRIM_CLOSED);
#else
		new boost::thread(&PrimeryAndSecondary::AsyncEventPoster, this, PRIM_CLOSED);
#endif
	}

	//平多头
	if(OPEN_COND2 == mOpenCond)
	{
		logger.LogThisFast("[ACTION]: SELL_PRIM");
		lPrice<<primDataBuf[primBufIndex].bidPrice;
		logger.LogThisFast(lPrice.str());
#ifdef BACK_TEST
		mEventQueue.push(PRIM_CLOSED);
#else
		new boost::thread(&PrimeryAndSecondary::AsyncEventPoster, this, PRIM_CLOSED);
#endif
	}
	tempStream.clear();
	tempStream.str("");
	tempStream<<"[INFO]: estimate profit:	"<<EstimateProfit();
	logger.LogThisFast(tempStream.str());
#ifdef BACK_TEST
	mProfitLog.LogThisFast(tempStream.str());
	double lTempProfit = EstimateProfit()-stgArg.cost;// 连手续费一起算上
	if(lTempProfit > 0)
	{
		mWin++;
	}
	else
	{
		mLose++;
	}
	mTotalProfit += lTempProfit;
#endif
#ifndef BACK_TEST
	mClosePrimCD = false;
	mCoolDownClosePrimThread = new boost::thread(boost::bind(&PrimeryAndSecondary::CoolDownClosePrim, this));
#endif
}
void PrimeryAndSecondary::CancelScnd()
{
	;
}
void PrimeryAndSecondary::CancelPrim()
{
	;
}
void PrimeryAndSecondary::CloseBoth()
{
	CloseScnd();
	ClosePrim();
}
void PrimeryAndSecondary::CheckPrimPosition()
{
	;
}
void PrimeryAndSecondary::CheckScndPosition()
{
	;
}
int PrimeryAndSecondary::CheckPrimOrder()
{
	return 0;
}
int PrimeryAndSecondary::CheckScndOrder()
{
	return 0;
}
#endif