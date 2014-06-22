#include <PrimeryAndSecondary.h>

#ifndef SIMULATION
/************************************************************************/
// ���������ֺ�����
/************************************************************************/
void PrimeryAndSecondary::OpenScnd()
{
	if( OPEN_COND1 == mOpenCond )
	{
		/* condition 1 */
		
		logger.LogThisFast("[ACTION]: BUY_SCND");
		mTradeDir = BUY_SCND_SELL_PRIM;
		mScndEnterPrice = scndDataBuf[scndBufIndex].lastPrice;
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
		mScndEnterPrice = scndDataBuf[scndBufIndex].lastPrice;
		if(SellShort(stgArg.secondaryInst, scndDataBuf[scndBufIndex].lastPrice, stgArg.openShares, &lastScndOrder) != true)
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
// �������ֺ�����
/************************************************************************/
void PrimeryAndSecondary::OpenPrim()
{
	if(BUY_SCND_SELL_PRIM == mTradeDir)
	{
		logger.LogThisFast("[ACTION]: SHORT_PRIM");
		mPrimEnterPrice = primDataBuf[primBufIndex].bidPrice;
		if(SellShort(stgArg.primaryInst, primDataBuf[primBufIndex].lowerLimit, stgArg.openShares, &lastPrimOrder) != true)
		{
			logger.LogThisFast("[ERROR]: sell prim error");
		}
		mWaitPrimOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitPrimOpen, this, ++mOpenPrimId));
	}
	else if(BUY_PRIM_SELL_SCND == mTradeDir)
	{
		logger.LogThisFast("[ACTION]: BUY_PRIM");
		mPrimEnterPrice = primDataBuf[primBufIndex].askPrice;
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
	//ƽ��ͷ
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
	//ƽ��ͷ
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
	//���CD��û����ֱ�ӷ���
	if(!mClosePrimCD)
	{
		return;
	}
	//ƽ��ͷ
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

	//ƽ��ͷ
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
	//���CD��û����ֱ�ӷ���
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
	//���CD��û����ֱ�ӷ���
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
	tempStream<<"estimate profit: "<<EstimateProfit();
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
void PrimeryAndSecondary::CheckPrimOrder()
{
	ReqQryOrder(stgArg.primaryInst.c_str());
}
void PrimeryAndSecondary::CheckScndOrder()
{
	ReqQryOrder(stgArg.secondaryInst.c_str());
}
#endif
#ifdef SIMULATION
void PrimeryAndSecondary::OpenScnd()
{
	stringstream lPrice;
	lPrice<<scndDataBuf[scndBufIndex].lastPrice;
	if( OPEN_COND1 == mOpenCond )
	{
		/* condition 1 */
		
		logger.LogThisFast("[ACTION]: BUY_SCND");
		logger.LogThisFast(lPrice.str());
		mTradeDir = BUY_SCND_SELL_PRIM;
		mScndEnterPrice = scndDataBuf[scndBufIndex].lastPrice;
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
		logger.LogThisFast(lPrice.str());
		mTradeDir = BUY_PRIM_SELL_SCND;
		mScndEnterPrice = scndDataBuf[scndBufIndex].lastPrice;
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
	lPrice<<primDataBuf[primBufIndex].lastPrice;
	if(BUY_SCND_SELL_PRIM == mTradeDir)
	{
		logger.LogThisFast("[ACTION]: SHORT_PRIM");
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
	lPrice<<scndDataBuf[scndBufIndex].lastPrice;
#ifndef BACK_TEST
	if(!mCloseScndCD)
	{
		return;
	}
#endif
	//ƽ��ͷ
	if(OPEN_COND1 == mOpenCond)
	{
		logger.LogThisFast("[ACTION]: SELL_SCND");
		logger.LogThisFast(lPrice.str());
#ifdef BACK_TEST
		mEventQueue.push(SCND_CLOSED);
#else
		new boost::thread(&PrimeryAndSecondary::AsyncEventPoster, this, SCND_CLOSED);
#endif
	}
	//ƽ��ͷ
	if(OPEN_COND2 == mOpenCond)
	{
		logger.LogThisFast("[ACTION]: COVER_SCND");
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
#endif//�ز�ʱ���Բ��������CoolDown����Ϊ�ٶ�̫�죬CoolDown��û��ɻز���ܾͽ�����
}
void PrimeryAndSecondary::ClosePrim()
{
	stringstream lPrice;
	lPrice<<primDataBuf[primBufIndex].lastPrice;
#ifndef BACK_TEST
	//���CD��û����ֱ�ӷ���
	if(!mClosePrimCD)
	{
		return;
	}
#endif
	//ƽ��ͷ
	if(OPEN_COND1 == mOpenCond)
	{
		logger.LogThisFast("[ACTION]: COVER_PRIM");
		logger.LogThisFast(lPrice.str());
#ifdef BACK_TEST
		mEventQueue.push(PRIM_CLOSED);
#else
		new boost::thread(&PrimeryAndSecondary::AsyncEventPoster, this, PRIM_CLOSED);
#endif
	}

	//ƽ��ͷ
	if(OPEN_COND2 == mOpenCond)
	{
		logger.LogThisFast("[ACTION]: SELL_PRIM");
		logger.LogThisFast(lPrice.str());
#ifdef BACK_TEST
		mEventQueue.push(PRIM_CLOSED);
#else
		new boost::thread(&PrimeryAndSecondary::AsyncEventPoster, this, PRIM_CLOSED);
#endif
	}
	tempStream.clear();
	tempStream.str("");
	tempStream<<"estimate profit:	"<<EstimateProfit();
	logger.LogThisFast(tempStream.str());
#ifdef BACK_TEST
	mProfitLog.LogThisFast(tempStream.str());
	double lTempProfit = EstimateProfit();
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
void PrimeryAndSecondary::CheckPrimOrder()
{
	;
}
void PrimeryAndSecondary::CheckScndOrder()
{
	;
}
#endif