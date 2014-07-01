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
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	//c++ 结构体提供了拷贝构造函数以及等号的重载
	BollingerBandData lBoll = mBoll.GetBoll(0);
	// use integer instead of float during calculation
	long long lPrimBid = (long long)lPrim.bidPrice*10000;
	long long lPrimAsk = (long long)lPrim.askPrice*10000;
	long long lScndLast = (long long)lScnd.lastPrice*10000;
	long long lScndBid = (long long)lScnd.bidPrice*10000;
	long long lScndAsk = (long long)lScnd.askPrice*10000;
	long long lBollOutterUpper = (long long)lBoll.mOutterUpperLine*10000;
	long long lBollOutterLower = (long long)lBoll.mOutterLowerLine*10000;
	long long lBollAmpLimit = (long long)stgArg.minMove * stgArg.bollAmpLimit*10000;
	long long lAskBidGapLimit = (long long)stgArg.askBidGapLimit*10000;

	if(lBollOutterUpper - lBollOutterLower < lBollAmpLimit)
	{
		return;
	}// if the bollinger band is not wide enough, then return
	
	if(lPrimAsk - lPrimBid > lAskBidGapLimit)
	{
		return;
	}// too wide gap between ask and bid price
	if(lScndAsk - lScndBid > lAskBidGapLimit)
	{
		return;
	}// too wide gap between ask and bid price
	
	if(	lPrimBid - lScndLast > lBollOutterUpper )
	{
		/* condition 1 */
		mOpenCond = OPEN_COND1;
		logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND1	ServerTime: " + (string)pDepthMarketData.TradingDay + " "+(string)pDepthMarketData.UpdateTime);
		tempStream.clear();
		tempStream.str("");
		tempStream<<"[INFO]: Evidence: "<<lPrimBid<<" - "<<lScndLast<<" > "<<lBollOutterUpper;
		logger.LogThisFast(tempStream.str());
		SetEvent(OPEN_PRICE_GOOD);
	}// using bid_price - last_price to do the open timing judge, this is to meet the real situation
	else if( lPrimAsk - lScndLast < lBollOutterLower )
	{
		/* condition 2 */
		mOpenCond = OPEN_COND2;
		logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND2	ServerTime: " + (string)pDepthMarketData.TradingDay + " "+(string)pDepthMarketData.UpdateTime);
		tempStream.clear();
		tempStream.str("");
		tempStream<<"[INFO]: Evidence: "<<lPrimAsk<<" - "<<lScndLast<<" < "<<lBollOutterLower;
		logger.LogThisFast(tempStream.str());
		SetEvent(OPEN_PRICE_GOOD);
	}

}
bool PrimeryAndSecondary::StopLoseJudge(CThostFtdcDepthMarketDataField const& pDepthMarketData)
{
	bool lIsClose = false;
	if(mStateMachine.GetState() == PENDING_STATE)
	{	
		long long lProfit = 0;
		long long lPrimBid = (long long)primDataBuf[primBufIndex].bidPrice*10000;
		long long lPrimAsk = (long long)primDataBuf[primBufIndex].askPrice*10000;
		long long lScndLast = (long long)scndDataBuf[scndBufIndex].lastPrice*10000;
		long long lScndBid = (long long)scndDataBuf[scndBufIndex].bidPrice*10000;
		long long lScndAsk = (long long)scndDataBuf[scndBufIndex].askPrice*10000;
		long long lPrimEnter = (long long)mPrimEnterPrice*10000;
		long long lScndEnter = (long long)mScndEnterPrice*10000;
		long long lStopLossPrice = (long long)stgArg.stopLossPrice*10000;

		if(BUY_SCND_SELL_PRIM == mTradeDir)
		{
			lProfit = (lPrimEnter-lScndEnter)-(lPrimAsk-lScndBid);
		}
		else if(BUY_PRIM_SELL_SCND == mTradeDir)
		{
			lProfit = (lPrimBid-lScndAsk)-(lPrimEnter-lScndEnter);
		}

		if(lProfit<lStopLossPrice)
		{
			//使用绝对浮亏来止损
			logger.LogThisFast("[EVENT]: MUST_STOP (from estimate profit)	ServerTime: " + (string)pDepthMarketData.UpdateTime);
			tempStream.clear();
			tempStream.str("");
			if(BUY_SCND_SELL_PRIM == mTradeDir)
			{
				tempStream<<"[INFO]: Evidence: ("<<lPrimEnter<<" - "<<lScndEnter<<") - ("<<lPrimAsk<<" - "<<lScndBid<<") < "<< lStopLossPrice;
			}
			else
			{
				tempStream<<"[INFO]: Evidence: ("<<lPrimBid<<" - "<<lScndAsk<<") - ("<<lPrimEnter<<" - "<<lScndEnter<<") < "<< lStopLossPrice;
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
		long long lProfit = 0;
		long long lPrimBid = (long long)primDataBuf[primBufIndex].bidPrice*10000;
		long long lPrimAsk = (long long)primDataBuf[primBufIndex].askPrice*10000;
		long long lScndLast = (long long)scndDataBuf[scndBufIndex].lastPrice*10000;
		long long lScndBid = (long long)scndDataBuf[scndBufIndex].bidPrice*10000;
		long long lScndAsk = (long long)scndDataBuf[scndBufIndex].askPrice*10000;
		long long lPrimEnter = (long long)mPrimEnterPrice*10000;
		long long lScndEnter = (long long)mScndEnterPrice*10000;
		long long lStopLossPrice = (long long)stgArg.stopLossPrice*10000;
		long long lStopWinPoint = (long long)stgArg.stopWinPoint*10000;
		long long lBollMid = (long long)lBoll.mMidLine*10000;
		long long lBollBand = (long long)stgArg.winBollAmp*lBoll.mStdDev*10000;

		if(BUY_SCND_SELL_PRIM == mTradeDir)
		{
			lProfit = (lPrimEnter-lScndEnter)-(lPrimAsk-lScndBid);
		}
		else if(BUY_PRIM_SELL_SCND == mTradeDir)
		{
			lProfit = (lPrimBid-lScndAsk)-(lPrimEnter-lScndEnter);
		}//计算收益

		//使用布林带位置判断止盈
		if(OPEN_COND1 == mOpenCond)
		{
			if(lPrimAsk - lScndBid < lBollMid - lBollBand)
			{
				logger.LogThisFast("[EVENT]: CLOSE_PRICE_GOOD_COND1	ServerTime: " + (string)pDepthMarketData.UpdateTime);
				tempStream.clear();
				tempStream.str("");
				tempStream<<"[INFO]: Evidence: "<<lPrimAsk<<" - "<<lScndBid<<" < "<<(lBollMid - lBollBand);
				logger.LogThisFast(tempStream.str());
				SetEvent(CLOSE_PRICE_GOOD);
				lGoodToClose = true;
			}
		}
		else if(OPEN_COND2 == mOpenCond)
		{
			if(lPrimBid - lScndAsk > lBollMid + lBollBand)
			{
				logger.LogThisFast("[EVENT]: CLOSE_PRICE_GOOD_COND2	ServerTime: " + (string)pDepthMarketData.UpdateTime);
				tempStream.clear();
				tempStream.str("");
				tempStream<<"[INFO]: Evidence: "<<lPrimBid<<" - "<<lScndAsk<<" > "<<(lBollMid + lBollBand);
				logger.LogThisFast(tempStream.str());
				SetEvent(CLOSE_PRICE_GOOD);
				lGoodToClose = true;
			}
		}
		//使用绝对收益判断止盈
		if(lProfit>lStopWinPoint)
		{
			//使用绝对浮亏来止损
			logger.LogThisFast("[EVENT]: CLOSE_PRICE_GOOD (from estimate profit)	ServerTime: " + (string)pDepthMarketData.UpdateTime);
			tempStream.clear();
			tempStream.str("");
			if(BUY_SCND_SELL_PRIM == mTradeDir)
			{
				tempStream<<"[INFO]: Evidence: ("<<lPrimEnter<<" - "<<lScndEnter<<") - ("<<lPrimAsk<<" - "<<lScndBid<<") > "<< lStopWinPoint;
			}
			else
			{
				tempStream<<"[INFO]: Evidence: ("<<lPrimBid<<" - "<<lScndAsk<<") - ("<<lPrimEnter<<" - "<<lScndEnter<<") > "<< lStopWinPoint;
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
	tempStream<<"["<<lPrim.updateTime<<" "<<lPrim.askPrice<<"-"<<lPrim.bidPrice<<"] ["\
		<<lScnd.updateTime<<" "<<lScnd.askPrice<<"-"<<lScnd.bidPrice<<"] ["\
		<<tempData.mOutterUpperLine<<" "<<tempData.mInnerUpperLine<<" "\
		<<tempData.mInnerLowerLine<<" "<<tempData.mOutterLowerLine<<"] "\
		<<tempData.mMidLine<<" "<<tempData.mStdDev;
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

	long long lPrimBid = lPrim.bidPrice*10000;
	long long lPrimAsk = lPrim.askPrice*10000;
	long long lScndLast = lScnd.lastPrice*10000;
	long long lBollUpper = lBoll.mInnerUpperLine*10000;
	long long lBollLower = lBoll.mInnerLowerLine*10000;

	if(mStateMachine.GetState() != OPENING_SCND_STATE)
	{
		return;
	}
	else
	{
		
		if(OPEN_COND1 == mOpenCond)
		{
			// FOR TEST ONLY!!! REMOVE AFTER TEST FINISHED!
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[INFO]: Pre Evidence: "<<lPrimBid<<" - "<<lScndLast<<" < "<<lBollUpper;
			logger.LogThisFast(tempStream.str());
			if( lPrimBid - lScndLast < lBollUpper );
			{
				logger.LogThisFast("[EVENT]: OPEN_PRICE_NOT_GOOD	ServerTime: " + (string)pDepthMarketData.UpdateTime);
				tempStream.clear();
				tempStream.str("");
				tempStream<<"[INFO]: Evidence: "<<lPrimBid<<" - "<<lScndLast<<" < "<<lBollUpper;
				logger.LogThisFast(tempStream.str());
				SetEvent(OPEN_PRICE_BAD);
			}
		}
		else if(OPEN_COND2 == mOpenCond)
		{
			// FOR TEST ONLY!!! REMOVE AFTER TEST FINISHED!
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[INFO]: Pre Evidence: "<<lPrimAsk<<" - "<<lScndLast<<" > "<<lBollLower;
			logger.LogThisFast(tempStream.str());
			if( lPrimAsk - lScndLast > lBollLower )
			{
				logger.LogThisFast("[EVENT]: OPEN_PRICE_NOT_GOOD	ServerTime: " + (string)pDepthMarketData.UpdateTime);
				tempStream.clear();
				tempStream.str("");
				tempStream<<"[INFO]: Evidence: "<<lPrimAsk<<" - "<<lScndLast<<" > "<<lBollLower;
				logger.LogThisFast(tempStream.str());
				SetEvent(OPEN_PRICE_BAD);
			}
		}
		else
		{
			logger.LogThisFast("[FATAL ERROR]: ILLEGAL OPEN COND	ServerTime: " + (string)pDepthMarketData.UpdateTime);
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