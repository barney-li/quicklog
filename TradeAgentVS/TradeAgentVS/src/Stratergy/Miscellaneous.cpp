#include "PrimeryAndSecondary.h"
double PrimeryAndSecondary::EstimateProfit()
{
	double lProfit = 0;
	if(BUY_SCND_SELL_PRIM == mTradeDir)
	{
		lProfit = (mPrimEnterPrice-mScndEnterPrice)-(primDataBuf[primBufIndex].askPrice-scndDataBuf[scndBufIndex].bidPrice);
	}
	else if(BUY_PRIM_SELL_SCND == mTradeDir)
	{
		lProfit = (primDataBuf[primBufIndex].bidPrice-scndDataBuf[scndBufIndex].askPrice)-(mPrimEnterPrice-mScndEnterPrice);
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
	if(!IsOpenTime(pDepthMarketData.UpdateTime))
	{
		return;
	}//don't open if it's not in open time
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	//c++ 结构体提供了拷贝构造函数以及等号的重载
	BollingerBandData lBoll = mBoll.GetBoll(0);
	if(lBoll.mOutterUpperLine - lBoll.mOutterLowerLine < stgArg.bollAmpLimit)
	{
		return;
	}// if the bollinger band is not wide enough, then return
	
	if(lPrim.askPrice - lPrim.bidPrice > stgArg.askBidGapLimit)
	{
		return;
	}// too wide gap between ask and bid price
	if(lScnd.askPrice - lScnd.bidPrice > stgArg.askBidGapLimit)
	{
		return;
	}// too wide gap between ask and bid price
#ifdef OPPONENT_PRICE_OPEN
	if(	lPrim.bidPrice - lScnd.askPrice > lBoll.mOutterUpperLine )
#else
	if(	lPrim.bidPrice - lScnd.lastPrice > lBoll.mOutterUpperLine )
#endif
	{
		/* condition 1 */
		mOpenCond = OPEN_COND1;
		logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND1	ServerTime: " + (string)pDepthMarketData.TradingDay + " "+(string)pDepthMarketData.UpdateTime);
		tempStream.clear();
		tempStream.str("");
		tempStream<<"[INFO]: Evidence: "<<lPrim.bidPrice<<" - "<<lScnd.lastPrice<<" > "<<lBoll.mOutterUpperLine;
		logger.LogThisFast(tempStream.str());
		SetEvent(OPEN_PRICE_GOOD);
	}// using bid_price - last_price to do the open timing judge, this is to meet the real situation
#ifdef OPPONENT_PRICE_OPEN
	else if( lPrim.askPrice - lScnd.bidPrice < lBoll.mOutterLowerLine )
#else
	else if( lPrim.askPrice - lScnd.lastPrice < lBoll.mOutterLowerLine )
#endif
	{
		/* condition 2 */
		mOpenCond = OPEN_COND2;
		logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND2	ServerTime: " + (string)pDepthMarketData.TradingDay + " "+(string)pDepthMarketData.UpdateTime);
		tempStream.clear();
		tempStream.str("");
		tempStream<<"[INFO]: Evidence: "<<lPrim.askPrice<<" - "<<lScnd.lastPrice<<" < "<<lBoll.mOutterLowerLine;
		logger.LogThisFast(tempStream.str());
		SetEvent(OPEN_PRICE_GOOD);
	}

}
bool PrimeryAndSecondary::StopLoseJudge(CThostFtdcDepthMarketDataField const& pDepthMarketData)
{
	bool lIsClose = false;
	if(mStateMachine.GetState() == PENDING_STATE)
	{	
		double lProfit = 0;
		if(BUY_SCND_SELL_PRIM == mTradeDir)
		{
			lProfit = (mPrimEnterPrice-mScndEnterPrice)-(primDataBuf[primBufIndex].askPrice-scndDataBuf[scndBufIndex].bidPrice);
		}
		else if(BUY_PRIM_SELL_SCND == mTradeDir)
		{
			lProfit = (primDataBuf[primBufIndex].bidPrice-scndDataBuf[scndBufIndex].askPrice)-(mPrimEnterPrice-mScndEnterPrice);
		}

		if(lProfit<stgArg.stopLossPrice)
		{
			//使用绝对浮亏来止损
			logger.LogThisFast("[EVENT]: MUST_STOP (from estimate profit)	ServerTime: " + (string)pDepthMarketData.UpdateTime);
			tempStream.clear();
			tempStream.str("");
			if(BUY_SCND_SELL_PRIM == mTradeDir)
			{
				tempStream<<"[INFO]: Evidence: ("<<mPrimEnterPrice<<" - "<<mScndEnterPrice<<") - ("<<primDataBuf[primBufIndex].askPrice<<" - "<<scndDataBuf[scndBufIndex].bidPrice<<") < "<< stgArg.stopLossPrice;
			}
			else
			{
				tempStream<<"[INFO]: Evidence: ("<<primDataBuf[primBufIndex].bidPrice<<" - "<<scndDataBuf[scndBufIndex].askPrice<<") - ("<<mPrimEnterPrice<<" - "<<mScndEnterPrice<<") < "<< stgArg.stopLossPrice;
			}

			logger.LogThisFast(tempStream.str());
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
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	BollingerBandData lBoll = mBoll.GetBoll(0);
	bool lGoodToClose = false;
	if(mStateMachine.GetState() == PENDING_STATE)
	{
		//使用布林带位置判断止盈
		if(OPEN_COND1 == mOpenCond)
		{
			if(lPrim.askPrice - lScnd.bidPrice < lBoll.mMidLine - stgArg.winBollAmp*lBoll.mStdDev)
			{
				logger.LogThisFast("[EVENT]: CLOSE_PRICE_GOOD_COND1	ServerTime: " + (string)pDepthMarketData.UpdateTime);
				tempStream.clear();
				tempStream.str("");
				tempStream<<"[INFO]: Evidence: "<<lPrim.askPrice<<" - "<<lScnd.bidPrice<<" < "<<(lBoll.mMidLine - stgArg.winBollAmp*lBoll.mStdDev);
				logger.LogThisFast(tempStream.str());
				SetEvent(CLOSE_PRICE_GOOD);
				lGoodToClose = true;
			}
		}
		else if(OPEN_COND2 == mOpenCond)
		{
			if(lPrim.bidPrice - lScnd.askPrice > lBoll.mMidLine + stgArg.winBollAmp*lBoll.mStdDev)
			{
				logger.LogThisFast("[EVENT]: CLOSE_PRICE_GOOD_COND2	ServerTime: " + (string)pDepthMarketData.UpdateTime);
				tempStream.clear();
				tempStream.str("");
				tempStream<<"[INFO]: Evidence: "<<lPrim.bidPrice<<" - "<<lScnd.askPrice<<" > "<<(lBoll.mMidLine + stgArg.winBollAmp*lBoll.mStdDev);
				logger.LogThisFast(tempStream.str());
				SetEvent(CLOSE_PRICE_GOOD);
				lGoodToClose = true;
			}
		}
		//使用绝对收益判断止盈
		double lProfit = 0;
		if(BUY_SCND_SELL_PRIM == mTradeDir)
		{
			lProfit = (mPrimEnterPrice-mScndEnterPrice)-(primDataBuf[primBufIndex].askPrice-scndDataBuf[scndBufIndex].bidPrice);
		}
		else if(BUY_PRIM_SELL_SCND == mTradeDir)
		{
			lProfit = (primDataBuf[primBufIndex].bidPrice-scndDataBuf[scndBufIndex].askPrice)-(mPrimEnterPrice-mScndEnterPrice);
		}

		if(lProfit>stgArg.stopWinPoint)
		{
			//使用绝对浮亏来止损
			logger.LogThisFast("[EVENT]: CLOSE_PRICE_GOOD (from estimate profit)	ServerTime: " + (string)pDepthMarketData.UpdateTime);
			tempStream.clear();
			tempStream.str("");
			if(BUY_SCND_SELL_PRIM == mTradeDir)
			{
				tempStream<<"[INFO]: Evidence: ("<<mPrimEnterPrice<<" - "<<mScndEnterPrice<<") - ("<<primDataBuf[primBufIndex].askPrice<<" - "<<scndDataBuf[scndBufIndex].bidPrice<<") > "<< stgArg.stopWinPoint;
			}
			else
			{
				tempStream<<"[INFO]: Evidence: ("<<primDataBuf[primBufIndex].bidPrice<<" - "<<scndDataBuf[scndBufIndex].askPrice<<") - ("<<mPrimEnterPrice<<" - "<<mScndEnterPrice<<") > "<< stgArg.stopWinPoint;
			}

			logger.LogThisFast(tempStream.str());
			SetEvent(CLOSE_PRICE_GOOD);
			lGoodToClose = true;
		}
	}
	return lGoodToClose;
}
void PrimeryAndSecondary::LogBollData()
{
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	BollingerBandData tempData = mBoll.GetBoll(0);
	//先判断是否合法字符串
	if(lPrim.updateTime[8] != '\0')
	{
		return;
	}
	if(lScnd.updateTime[8] != '\0')
	{
		return;
	}
	
	tempStream.clear();
	tempStream.str("");
#ifndef BACK_TEST
	tempStream<<"["<<lPrim.updateTime<<" "<<lPrim.askPrice<<"-"<<lPrim.bidPrice<<"] ["\
		<<lScnd.updateTime<<" "<<lScnd.askPrice<<"-"<<lScnd.bidPrice<<"] ["\
		<<tempData.mOutterUpperLine<<" "<<tempData.mInnerUpperLine<<" "\
		<<tempData.mInnerLowerLine<<" "<<tempData.mOutterLowerLine<<"] "\
		<<tempData.mMidLine<<" "<<tempData.mStdDev;
#else
#ifdef OPPONENT_PRICE_OPEN
	double lDeltaPrice1 = lPrim.bidPrice - lScnd.askPrice;
	double lDeltaPrice2 = lPrim.askPrice - lScnd.bidPrice;
#else
	double lDeltaPrice1 = lPrim.bidPrice - lScnd.lastPrice;
	double lDeltaPrice2 = lPrim.askPrice - lScnd.lastPrice;
#endif
	
	tempStream<<lDeltaPrice1<<"	"<<lDeltaPrice2<<"	"\
		<<tempData.mOutterUpperLine<<"	"<<tempData.mInnerUpperLine<<"	"\
		<<tempData.mInnerLowerLine<<"	"<<tempData.mOutterLowerLine<<"	";
	if(mStateMachine.GetState() == OPENING_SCND_STATE || mStateMachine.GetState() == OPENING_PRIM_STATE)
	{
		if(OPEN_COND1 == mOpenCond)
		{
			tempStream<<lDeltaPrice1<<"	";
		}
		else
		{
			tempStream<<lDeltaPrice2<<"	";
		}
	}
	else
	{
		tempStream<<"0"<<"	";
	}
	if(mStateMachine.GetState() == CLOSING_BOTH_STATE || mStateMachine.GetState() == WAITING_SCND_CLOSE_STATE||mStateMachine.GetState() == WAITING_PRIM_CLOSE_STATE)
	{
		if(OPEN_COND1 == mOpenCond)
		{
			tempStream<<lPrim.askPrice-lScnd.bidPrice<<"	";
		}
		else
		{
			tempStream<<lPrim.bidPrice-lScnd.askPrice<<"	";
		}
	}
	else
	{
		tempStream<<"0"<<"	";
	}
#endif
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
bool PrimeryAndSecondary::VerifyMarketData(BasicMarketData const & pData)
{
	if(pData.lastPrice>stgArg.ceilingPrice || pData.lastPrice<stgArg.floorPrice) return false;
	return true;
}
void PrimeryAndSecondary::StopOpenJudge(CThostFtdcDepthMarketDataField const& pDepthMarketData)
{
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	//c++ 结构体提供了拷贝构造函数以及等号的重载
	BollingerBandData lBoll = mBoll.GetBoll(0);
	
	
	if(OPEN_COND1 == mOpenCond)
	{
		double lPrimPrice;
		double lScndPrice;
		if(mStateMachine.GetState() == OPENING_SCND_STATE)
		{
			lPrimPrice = lPrim.bidPrice;
			lScndPrice = mScndEnterPrice;
		}
		else if(mStateMachine.GetState() == OPENING_PRIM_STATE)
		{
			lPrimPrice = mPrimEnterPrice;
			lScndPrice = mScndEnterPrice;
		}
		else
		{
			return;
		}// return if it is not in opening state

		if( lPrimPrice - lScndPrice < lBoll.mInnerUpperLine )
		{
			logger.LogThisFast("[EVENT]: OPEN_PRICE_BAD	ServerTime: " + (string)pDepthMarketData.UpdateTime);
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[INFO]: Evidence: "<<lPrimPrice<<" - "<<lScndPrice<<" < "<<lBoll.mInnerUpperLine;
			logger.LogThisFast(tempStream.str());
			SetEvent(OPEN_PRICE_BAD);
		}
	}
	else if(OPEN_COND2 == mOpenCond)
	{
		double lPrimPrice;
		double lScndPrice;
		if(mStateMachine.GetState() == OPENING_SCND_STATE)
		{
			lPrimPrice = lPrim.askPrice;
			lScndPrice = mScndEnterPrice;
		}
		else if(mStateMachine.GetState() == OPENING_PRIM_STATE)
		{
			lPrimPrice = mPrimEnterPrice;
			lScndPrice = mScndEnterPrice;
		}
		else
		{
			return;
		}// return if it is not in opening state

		if( lPrimPrice - lScndPrice > lBoll.mInnerLowerLine )
		{
			logger.LogThisFast("[EVENT]: OPEN_PRICE_BAD	ServerTime: " + (string)pDepthMarketData.UpdateTime);
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[INFO]: Evidence: "<<lPrimPrice<<" - "<<lScndPrice<<" > "<<lBoll.mInnerLowerLine;
			logger.LogThisFast(tempStream.str());
			SetEvent(OPEN_PRICE_BAD);
		}
	}
	else
	{
		logger.LogThisFast("[FATAL ERROR]: ILLEGAL OPEN COND	ServerTime: " + (string)pDepthMarketData.UpdateTime);
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
		CloseScnd();
		break;
	case CANCELLING_PRIM_STATE:
		CancelPrim();
		break;
	case WAITING_SCND_CLOSE_STATE:
		CloseScnd();
		break;
	case WAITING_PRIM_CLOSE_STATE:
		ClosePrim();
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
		logger.LogThisFast("[EVENT]: NOT_TRADING_TIME	ServerTime:	"+aDataTime);
		mBoll.InitAllData();
		SetEvent(NOT_TRADING_TIME);
		return false;
	}
}
bool PrimeryAndSecondary::IsOpenTime(string aDataTime)
{
	// any valid time should be in size of 8
	if(aDataTime.size() != 8)
	{
		return false;
	}
	ptime lCurTime = time_from_string((string)"2000-01-01 "+aDataTime);
	// 所有时间区间均为[09:01:00, 11:29:00)的形式，前闭后开
	time_period lTradePeriod1 = time_period(time_from_string("2000-01-01 09:01:00"), time_from_string("2000-01-01 11:20:00"));
	time_period lTradePeriod2 = time_period(time_from_string("2000-01-01 13:31:00"), time_from_string("2000-01-01 14:50:00"));
	time_period lTradePeriod3 = time_period(time_from_string("2000-01-01 21:01:00"), time_from_string("2000-01-01 24:00:00"));
	time_period lTradePeriod4 = time_period(time_from_string("2000-01-01 00:00:00"), time_from_string("2000-01-01 02:20:00"));
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
		return false;
	}
}