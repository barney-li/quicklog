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

#ifdef OPPONENT_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].askPrice-stgArg.minMove*stgArg.scndOrderJump;
#endif
#ifdef LAST_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].lastPrice-stgArg.minMove*stgArg.scndOrderJump;
#endif
#ifdef QUEUE_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].bidPrice-stgArg.minMove*stgArg.scndOrderJump;
#endif
		tempStream.clear();
		tempStream.str("");
		tempStream<<"[ACTION]: BUY_SCND at "<<mScndEnterPrice;
		logger.LogThisFast(tempStream.str());

		mTradeDir = BUY_SCND_SELL_PRIM;

		if(Buy(stgArg.secondaryInst, mScndEnterPrice, stgArg.openShares, &lastScndOrder) != true)
		{
			logger.LogThisFast("[ERROR]: buy scnd error");
		}
		mWaitScndOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitScndOpen, this, ++mOpenScndId));
	}
	else if( OPEN_COND2 == mOpenCond )
	{
		/* condition 2 */

#ifdef OPPONENT_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].bidPrice+stgArg.minMove*stgArg.scndOrderJump;
#endif
#ifdef LAST_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].lastPrice+stgArg.minMove*stgArg.scndOrderJump;
#endif
#ifdef QUEUE_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].askPrice+stgArg.minMove*stgArg.scndOrderJump;
#endif
		tempStream.clear();
		tempStream.str("");
		tempStream<<"[ACTION]: SHORT_SCND at "<<mScndEnterPrice;
		logger.LogThisFast(tempStream.str());

		mTradeDir = BUY_PRIM_SELL_SCND;

		if(SellShort(stgArg.secondaryInst, mScndEnterPrice, stgArg.openShares, &lastScndOrder) != true)
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
		mPrimEnterPrice = primDataBuf[primBufIndex].bidPrice;

		tempStream.clear();
		tempStream.str("");
		tempStream<<"[ACTION]: SHORT_PRIM at "<<mPrimEnterPrice;
		logger.LogThisFast(tempStream.str());

		// ���ּ۸�ʹ�öԼۼ�3������߳ɽ���
		if(SellShort(stgArg.primaryInst, mPrimEnterPrice-3*stgArg.minMove, mTradedShares, &lastPrimOrder) != true)
		{
			logger.LogThisFast("[ERROR]: sell prim error");
		}
		mWaitPrimOpenThread = new boost::thread(boost::bind(&PrimeryAndSecondary::WaitPrimOpen, this, ++mOpenPrimId));
	}
	else if(BUY_PRIM_SELL_SCND == mTradeDir)
	{
		mPrimEnterPrice = primDataBuf[primBufIndex].askPrice;

		tempStream.clear();
		tempStream.str("");
		tempStream<<"[ACTION]: BUY_PRIM at"<<mPrimEnterPrice;
		logger.LogThisFast(tempStream.str());

		// ���ּ۸�ʹ�öԼۼ�3������߳ɽ���
		if(Buy(stgArg.primaryInst, mPrimEnterPrice+3*stgArg.minMove, mTradedShares, &lastPrimOrder) != true)
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
	mScndReqOrderId = CheckScndOrder();
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
	mPrimReqOrderId = CheckPrimOrder();
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
	if(!mQueryCD)
	{
		return;
	}
	mQueryCD = false;
	ReqQryInvestorPosition(stgArg.primaryInst.c_str());
	new boost::thread(boost::bind(&PrimeryAndSecondary::CoolDownQuery, this));
}
void PrimeryAndSecondary::CheckScndPosition()
{
	if(!mQueryCD)
	{
		return;
	}
	mQueryCD = false;
	ReqQryInvestorPosition(stgArg.secondaryInst.c_str());
	new boost::thread(boost::bind(&PrimeryAndSecondary::CoolDownQuery, this));
}
int PrimeryAndSecondary::CheckPrimOrder()
{
	if(!mQueryCD)
	{
		return -1;
	}
	mQueryCD = false;

	logger.LogThisFast("[ACTION]: CHECK_PRIM_ORDER");
	int lReturnVal;
	lReturnVal = ReqQryOrder(stgArg.primaryInst.c_str());

	new boost::thread(boost::bind(&PrimeryAndSecondary::CoolDownQuery, this));
	return lReturnVal;
}
int PrimeryAndSecondary::CheckScndOrder()
{
	if(!mQueryCD)
	{
		return -1;
	}
	mQueryCD = false;

	logger.LogThisFast("[ACTION]: CHECK_SCND_ORDER");
	int lReturnVal;
	lReturnVal = ReqQryOrder(stgArg.secondaryInst.c_str());

	new boost::thread(boost::bind(&PrimeryAndSecondary::CoolDownQuery, this));
	return lReturnVal;
}
#endif
#ifdef SIMULATION
void PrimeryAndSecondary::OpenScnd()
{
	tempStream.clear();
	tempStream.str("");
	
	if( OPEN_COND1 == mOpenCond )
	{
		/* condition 1 */
		tempStream<<"[ACTION]: BUY_SCND at ";
#ifdef OPPONENT_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].askPrice-stgArg.minMove*stgArg.scndOrderJump;
#endif
#ifdef LAST_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].lastPrice-stgArg.minMove*stgArg.scndOrderJump;
#endif
#ifdef QUEUE_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].bidPrice-stgArg.minMove*stgArg.scndOrderJump;
#endif
		tempStream<<mScndEnterPrice;
		logger.LogThisFast(tempStream.str());
		mTradeDir = BUY_SCND_SELL_PRIM;
		mScndOpenTime = "2000-01-01 "+(string)primDataBuf[primBufIndex].updateTime;// ʹ��prim instrument�ĸ���ʱ�䣬��׼ȷ
		mScndTodayLongPosition = 1;
	}
	else if( OPEN_COND2 == mOpenCond )
	{
		/* condition 2 */
		
		tempStream<<"[ACTION]: SHORT_SCND at ";
#ifdef OPPONENT_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].bidPrice+stgArg.minMove*stgArg.scndOrderJump;
#endif
#ifdef LAST_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].lastPrice+stgArg.minMove*stgArg.scndOrderJump;
#endif
#ifdef QUEUE_PRICE_OPEN
		mScndEnterPrice = scndDataBuf[scndBufIndex].askPrice+stgArg.minMove*stgArg.scndOrderJump;
#endif
		tempStream<<mScndEnterPrice;
		logger.LogThisFast(tempStream.str());
		mTradeDir = BUY_PRIM_SELL_SCND;
		mScndOpenTime = "2000-01-01 "+(string)primDataBuf[primBufIndex].updateTime;// ʹ��prim instrument�ĸ���ʱ�䣬��׼ȷ
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
		mPrimEnterPrice = primDataBuf[primBufIndex].bidPrice;
		lPrice<<"[ACTION]: SHORT_PRIM at "<<mPrimEnterPrice;
		logger.LogThisFast(lPrice.str());
		mPrimOpenTime = "2000-01-01 "+(string)primDataBuf[primBufIndex].updateTime;
		mPrimTodayShortPosition = 1;
	}
	else if(BUY_PRIM_SELL_SCND == mTradeDir)
	{
		mPrimEnterPrice = primDataBuf[primBufIndex].askPrice;
		lPrice<<"[ACTION]: BUY_PRIM at "<<mPrimEnterPrice;
		logger.LogThisFast(lPrice.str());
		mPrimOpenTime = "2000-01-01 "+(string)primDataBuf[primBufIndex].updateTime;
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
#ifndef BACK_TEST
	if(!mCloseScndCD)
	{
		return;
	}
#endif
	//ƽ��ͷ
	if(OPEN_COND1 == mOpenCond)
	{
		mScndClosePrice = scndDataBuf[scndBufIndex].bidPrice;
		logger.LogThisFast("[ACTION]: SELL_SCND");
	}
	//ƽ��ͷ
	if(OPEN_COND2 == mOpenCond)
	{
		mScndClosePrice = scndDataBuf[scndBufIndex].askPrice;
		logger.LogThisFast("[ACTION]: COVER_SCND");
	}
#ifndef BACK_TEST
	mCloseScndCD = false;
	mCoolDownCloseScndThread = new boost::thread(boost::bind(&PrimeryAndSecondary::CoolDownCloseScnd, this));
#endif//�ز�ʱ���Բ��������CoolDown����Ϊ�ٶ�̫�죬CoolDown��û��ɻز���ܾͽ�����
}
void PrimeryAndSecondary::ClosePrim()
{
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
		mPrimClosePrice = primDataBuf[primBufIndex].askPrice;
		logger.LogThisFast("[ACTION]: COVER_PRIM");
	}

	//ƽ��ͷ
	if(OPEN_COND2 == mOpenCond)
	{
		mPrimClosePrice = primDataBuf[primBufIndex].bidPrice;
		logger.LogThisFast("[ACTION]: SELL_PRIM");
	}
	
#ifndef BACK_TEST
	mClosePrimCD = false;
	mCoolDownClosePrimThread = new boost::thread(boost::bind(&PrimeryAndSecondary::CoolDownClosePrim, this));
#endif
}
void PrimeryAndSecondary::CancelScnd()
{
	logger.LogThisFast("[ACTION]: CANCELLING_SCND");
}
void PrimeryAndSecondary::CancelPrim()
{
	mCloseScndOnly = true; // �����־λ��ʹ�ûز�ϵͳ��ScndClosed��ʱ�����ӯ��
	logger.LogThisFast("[ACTION]: CANCELLING_PRIM");
}
void PrimeryAndSecondary::CloseBoth()
{
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	//time_period lTradePeriod = time_period(time_from_string("2012-01-01 "+mOpenTime), time_from_string("2012-01-01 "+(string)lPrim.updateTime));
	time_duration lTradeDuration = time_from_string("2012-01-01 "+(string)lPrim.updateTime)-time_from_string("2012-01-01 "+mOpenTime);
	tempStream.clear();
	tempStream.str("");
	tempStream<<"[INFO]: trade period: "<<lTradeDuration;
	logger.LogThisFast(tempStream.str());
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