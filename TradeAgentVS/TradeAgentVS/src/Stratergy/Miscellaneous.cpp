#include "PrimeryAndSecondary.h"
double PrimeryAndSecondary::EstimateProfit()
{
	double lProfit = 0;
	if(BUY_SCND_SELL_PRIM == mTradeDir)
	{
		lProfit = (mPrimEnterPrice-mScndEnterPrice)-(primDataBuf[primBufIndex].askPrice-scndDataBuf[scndBufIndex].bidPrice);
		//lProfit = scndDataBuf[scndBufIndex].lastPrice - mScndEnterPrice + mPrimEnterPrice - primDataBuf[primBufIndex].lastPrice;
	}
	else if(BUY_PRIM_SELL_SCND == mTradeDir)
	{
		lProfit = (primDataBuf[primBufIndex].bidPrice-scndDataBuf[scndBufIndex].askPrice)-(mPrimEnterPrice-mScndEnterPrice);
		//lProfit = mScndEnterPrice - scndDataBuf[scndBufIndex].lastPrice + primDataBuf[primBufIndex].lastPrice - mPrimEnterPrice;
	}
	else
	{
		lProfit = 0;
		logger.LogThisFast("[ERROR]: illegal trade direction");
	}
	return lProfit;
}
void PrimeryAndSecondary::OpenJudge(CThostFtdcDepthMarketDataField const& pDepthMarketData)
{
	// if the bollinger band is not wide enough, then return
	if(mBoll.GetBoll(0).mOutterUpperLine - mBoll.GetBoll(0).mOutterLowerLine < stgArg.bollAmpLimit)
	{
		return;
	}
	// using bid_price - ask_price to do the open timing judge, this is rougher to meet
	if(	primDataBuf[primBufIndex].bidPrice - scndDataBuf[scndBufIndex].askPrice > mBoll.GetBoll(0).mOutterUpperLine )
	//if(	primDataBuf[primBufIndex].lastPrice - scndDataBuf[scndBufIndex].lastPrice > mBoll.GetBoll(0).mOutterUpperLine )
	{
		/* condition 1 */
		mOpenCond = OPEN_COND1;
		logger.LogThisFast("ServerTime: " + (string)pDepthMarketData.UpdateTime + "	[EVENT]: OPEN_PRICE_GOOD_COND1");
		SetEvent(OPEN_PRICE_GOOD);
	}
	else if( primDataBuf[primBufIndex].askPrice - scndDataBuf[scndBufIndex].bidPrice < mBoll.GetBoll(0).mOutterLowerLine )
	//else if( primDataBuf[primBufIndex].lastPrice - scndDataBuf[scndBufIndex].lastPrice < mBoll.GetBoll(0).mOutterLowerLine )
	{
		/* condition 2 */
		mOpenCond = OPEN_COND2;
		logger.LogThisFast("ServerTime: " + (string)pDepthMarketData.UpdateTime + "	[EVENT]: OPEN_PRICE_GOOD_COND2");
		SetEvent(OPEN_PRICE_GOOD);
	}

}
bool PrimeryAndSecondary::StopLoseJudge(CThostFtdcDepthMarketDataField const& pDepthMarketData)
{
	bool lIsClose = false;
	if(mStateMachine.GetState() == PENDING_STATE)
	{	
		if(EstimateProfit()<stgArg.stopLossPrice)
		{
			//使用绝对浮亏来止损
			logger.LogThisFast("ServerTime: " + (string)pDepthMarketData.UpdateTime + (string)"	[EVENT]: MUST_STOP (from estimate profit)");
			SetEvent(MUST_STOP);
			lIsClose = true;
		}

		//if(OPEN_COND1 == mOpenCond)
		//{
		//	if(primDataBuf[primBufIndex].lastPrice - scndDataBuf[scndBufIndex].lastPrice > mBoll.GetBoll(0).mMidLine + stgArg.stopBollAmp*mBoll.GetBoll(0).mStdDev);
		//	{
		//		logger.LogThisFast("ServerTime: " + (string)pDepthMarketData.UpdateTime + "	[EVENT]: MUST_STOP");
		//		SetEvent(MUST_STOP);
		//		lIsClose = true;
		//	}
		//}
		//else if(OPEN_COND2 == mOpenCond)
		//{
		//	if(primDataBuf[primBufIndex].lastPrice - scndDataBuf[scndBufIndex].lastPrice < mBoll.GetBoll(0).mMidLine - stgArg.stopBollAmp*mBoll.GetBoll(0).mStdDev)
		//	{
		//		logger.LogThisFast("ServerTime: " + (string)pDepthMarketData.UpdateTime + "	[EVENT]: MUST_STOP");
		//		SetEvent(MUST_STOP);
		//		lIsClose = true;
		//	}
		//}
	}
		
	return lIsClose;
}
bool PrimeryAndSecondary::StopWinJudge(CThostFtdcDepthMarketDataField const& pDepthMarketData)
{
	bool lGoodToClose = false;
	if(mStateMachine.GetState() == PENDING_STATE)
	{
		if(OPEN_COND1 == mOpenCond)
		{
			if(primDataBuf[primBufIndex].lastPrice - scndDataBuf[scndBufIndex].lastPrice < mBoll.GetBoll(0).mMidLine - stgArg.winBollAmp*mBoll.GetBoll(0).mStdDev)
			{
				logger.LogThisFast("ServerTime: " + (string)pDepthMarketData.UpdateTime + "	[EVENT]: CLOSE_PRICE_GOOD_COND1");
				SetEvent(CLOSE_PRICE_GOOD);
				lGoodToClose = true;
			}
		}
		else if(OPEN_COND2 == mOpenCond)
		{
			if(primDataBuf[primBufIndex].lastPrice - scndDataBuf[scndBufIndex].lastPrice > mBoll.GetBoll(0).mMidLine + stgArg.winBollAmp*mBoll.GetBoll(0).mStdDev)
			{
				logger.LogThisFast("ServerTime: " + (string)pDepthMarketData.UpdateTime + "	[EVENT]: CLOSE_PRICE_GOOD_COND2");
				SetEvent(CLOSE_PRICE_GOOD);
				lGoodToClose = true;
			}
		}
	}
	return lGoodToClose;
}
void PrimeryAndSecondary::LogBollData()
{
	//先判断是否合法字符串
	if(primDataBuf[primBufIndex].updateTime[8] != '\0')
	{
		return;
	}
	if(scndDataBuf[scndBufIndex].updateTime[8] != '\0')
	{
		return;
	}
	BollingerBandData tempData = mBoll.GetBoll(0);
	tempStream.clear();
	tempStream.str("");
	tempStream<<primDataBuf[primBufIndex].lastPrice<<"	"<<primDataBuf[primBufIndex].updateTime<<"	"<<scndDataBuf[scndBufIndex].lastPrice<<"	"<<scndDataBuf[scndBufIndex].updateTime<<"	"<<tempData.mMidLine<<"	"<<tempData.mStdDev<<"	"<<tempData.mOutterUpperLine<<"	"<<tempData.mOutterLowerLine;
	mBollLog.LogThisFast(tempStream.str());
}
bool PrimeryAndSecondary::BufferData(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
	boost::lock_guard<boost::mutex> lLockGuard(mBufferDataMutex);
	if (VerifyMarketData(*pDepthMarketData))
	{
		//recognize instrument id and put data into where they should go
		if(strncmp(pDepthMarketData->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.size()) == 0)
		{
			// don't forget to increase the index
			primBufIndex++;
			// increase vector size when it's almost full
			if(primDataBuf.size() - primBufIndex < 100)
			{
				primDataBuf.resize(primDataBuf.size()+STRATEGY_BUFFER_SIZE);
			}
			strncpy(primDataBuf[primBufIndex].instrumentId, pDepthMarketData->InstrumentID, sizeof(primDataBuf[primBufIndex].instrumentId));
			primDataBuf[primBufIndex].askPrice = pDepthMarketData->AskPrice1;
			primDataBuf[primBufIndex].askVolume = pDepthMarketData->AskVolume1;
			primDataBuf[primBufIndex].bidPrice = pDepthMarketData->BidPrice1;
			primDataBuf[primBufIndex].bidVolume = pDepthMarketData->BidVolume1;
			primDataBuf[primBufIndex].lastPrice = pDepthMarketData->LastPrice;
			primDataBuf[primBufIndex].volume = pDepthMarketData->Volume;
			primDataBuf[primBufIndex].trueVolume = pDepthMarketData->Volume - primLastVolume;
			primLastVolume = pDepthMarketData->Volume;
			primDataBuf[primBufIndex].upperLimit = pDepthMarketData->UpperLimitPrice;
			primDataBuf[primBufIndex].lowerLimit = pDepthMarketData->LowerLimitPrice;
			strncpy(primDataBuf[primBufIndex].updateTime, pDepthMarketData->UpdateTime, sizeof(primDataBuf[primBufIndex].updateTime));
			primDataBuf[primBufIndex].updateMillisec = pDepthMarketData->UpdateMillisec;
			primDataBuf[primBufIndex].localTime = boost::posix_time::microsec_clock::local_time();
				
		}
		else if(strncmp(pDepthMarketData->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.size()) == 0)
		{
			// don't forget to increase the index
			scndBufIndex++;
			// increase vector size when it's almost full
			if(scndDataBuf.size() - scndBufIndex < 100)
			{
				scndDataBuf.resize(scndDataBuf.size()+STRATEGY_BUFFER_SIZE);
			}
			strncpy(scndDataBuf[scndBufIndex].instrumentId, pDepthMarketData->InstrumentID, sizeof(scndDataBuf[scndBufIndex].instrumentId));
			scndDataBuf[scndBufIndex].askPrice = pDepthMarketData->AskPrice1;
			scndDataBuf[scndBufIndex].askVolume = pDepthMarketData->AskVolume1;
			scndDataBuf[scndBufIndex].bidPrice = pDepthMarketData->BidPrice1;
			scndDataBuf[scndBufIndex].bidVolume = pDepthMarketData->BidVolume1;
			scndDataBuf[scndBufIndex].lastPrice = pDepthMarketData->LastPrice;
			scndDataBuf[scndBufIndex].volume = pDepthMarketData->Volume;
			scndDataBuf[scndBufIndex].trueVolume = pDepthMarketData->Volume - scndLastVolume;
			scndLastVolume = pDepthMarketData->Volume;
			scndDataBuf[scndBufIndex].upperLimit = pDepthMarketData->UpperLimitPrice;
			scndDataBuf[scndBufIndex].lowerLimit = pDepthMarketData->LowerLimitPrice;
			strncpy(scndDataBuf[scndBufIndex].updateTime, pDepthMarketData->UpdateTime, sizeof(scndDataBuf[scndBufIndex].updateTime));
			scndDataBuf[scndBufIndex].updateMillisec = pDepthMarketData->UpdateMillisec;
			scndDataBuf[scndBufIndex].localTime = boost::posix_time::microsec_clock::local_time();
				
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}
bool PrimeryAndSecondary::VerifyMarketData(CThostFtdcDepthMarketDataField const & pData)
{
	if(null == pData.InstrumentID) return false;
	if(pData.LastPrice>stgArg.ceilingPrice || pData.LastPrice<stgArg.floorPrice) return false;
	if(pData.AskPrice1>stgArg.ceilingPrice || pData.AskPrice1<stgArg.floorPrice) return false;
	if(pData.BidPrice1>stgArg.ceilingPrice || pData.BidPrice1<stgArg.floorPrice) return false;
	if(pData.UpperLimitPrice>stgArg.ceilingPrice || pData.UpperLimitPrice<stgArg.floorPrice) return false;
	if(pData.LowerLimitPrice>stgArg.ceilingPrice || pData.LowerLimitPrice<stgArg.floorPrice) return false;
	return true;
}
void PrimeryAndSecondary::StopOpenJudge(CThostFtdcDepthMarketDataField const& pDepthMarketData)
{
	if(mStateMachine.GetState() != OPENING_SCND_STATE)
	{
		return;
	}
	else
	{
		if(OPEN_COND1 == mOpenCond)
		{
			if(primDataBuf[primBufIndex].bidPrice - scndDataBuf[scndBufIndex].askPrice < mBoll.GetBoll(0).mInnerUpperLine);
			{
				logger.LogThisFast("ServerTime: " + (string)pDepthMarketData.UpdateTime + "	[EVENT]: OPEN_PRICE_NOT_GOOD");
				SetEvent(OPEN_PRICE_BAD);
			}
		}
		else if(OPEN_COND2 == mOpenCond)
		{
			if(primDataBuf[primBufIndex].askPrice - scndDataBuf[scndBufIndex].bidPrice > mBoll.GetBoll(0).mInnerLowerLine)
			{
				logger.LogThisFast("ServerTime: " + (string)pDepthMarketData.UpdateTime + "	[EVENT]: OPEN_PRICE_NOT_GOOD");
				SetEvent(OPEN_PRICE_BAD);
			}
		}
		else
		{
			logger.LogThisFast("ServerTime: " + (string)pDepthMarketData.UpdateTime + "	[FATAL ERROR]: ILLEGAL OPEN COND");
		}
	}
}
void PrimeryAndSecondary::SetEvent(TRADE_EVENT aLatestEvent)
{
	// using lock guard to invoke lock and unlock automatically
	boost::lock_guard<boost::mutex> lLockGuard(mStateMachineMutex);
	TRADE_STATE lLastState = mStateMachine.GetState();
	TRADE_STATE lNextState = mStateMachine.SetEvent(aLatestEvent);
	
	switch(lNextState)
	{
	case IDLE_STATE:
		/* do nothing */
		break;
	case OPENING_SCND_STATE:
		// avoiding multiple open
		if(lNextState != lLastState)
		{
			OpenScnd();
		}
		break;
	case OPENING_PRIM_STATE:
		// avoiding multiple open
		if(lNextState != lLastState)
		{
			OpenPrim();
		}
		break;
	case PENDING_STATE:
		break;
	case CLOSING_BOTH_STATE:
		CloseBoth();
		break;
	case CANCELLING_SCND_STATE:
		CancelScnd();
		break;
	case CLOSING_SCND_STATE:
		CloseBoth();
		break;
	case CANCELLING_PRIM_STATE:
		CancelPrim();
		break;
	case WAITING_SCND_CLOSE_STATE:
		CloseBoth();
		break;
	case WAITING_PRIM_CLOSE_STATE:
		CloseBoth();
		break;
	default:
		break;
	}
	
}

bool PrimeryAndSecondary::IsTradeTime(string aDataTime)
{
	// any valid time should be in size of 8
	if(aDataTime.size() != 8)
	{
		return false;
	}
	ptime lCurTime = time_from_string((string)"2000-01-01 "+aDataTime);
	// 所有时间区间均为[09:01:00, 11:29:00)的形式，前闭后开
	time_period lTradePeriod1 = time_period(time_from_string("2000-01-01 09:01:00"), time_from_string("2000-01-01 11:29:00"));
	time_period lTradePeriod2 = time_period(time_from_string("2000-01-01 13:31:00"), time_from_string("2000-01-01 14:59:00"));
	time_period lTradePeriod3 = time_period(time_from_string("2000-01-01 21:01:00"), time_from_string("2000-01-01 24:00:00"));
	time_period lTradePeriod4 = time_period(time_from_string("2000-01-01 00:00:00"), time_from_string("2000-01-01 02:29:00"));
	if(lTradePeriod1.contains(lCurTime))
	{
		return true;
	}
	else if(lTradePeriod2.contains(lCurTime))
	{
		return true;
	}
	else if(lTradePeriod3.contains(lCurTime))
	{
		return true;
	}
	else if(lTradePeriod4.contains(lCurTime))
	{
		return true;
	}
	else
	{
		logger.LogThisFast("ServerTime:	"+aDataTime+"	[EVENT]: NOT_TRADING_TIME");
		mBoll.InitAllData();
		SetEvent(NOT_TRADING_TIME);
		return false;
	}
}