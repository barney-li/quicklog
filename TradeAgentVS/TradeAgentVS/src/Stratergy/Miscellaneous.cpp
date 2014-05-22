#include "PrimeryAndSecondary.h"
bool PrimeryAndSecondary::StopLoseJudge(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
	bool lIsClose = false;
	if(mStateMachine.GetState() == PENDING_STATE)
	{
		if(OPEN_COND1 == mOpenCond)
		{
			if(primDataBuf[primBufIndex].lastPrice - scndDataBuf[scndBufIndex].lastPrice > stgArg.stopBollAmp*mBoll.GetBoll(0).mStdDev + mBoll.GetBoll(0).mMidLine);
			{
				SetEvent(MUST_STOP);
				lIsClose = true;
			}
		}
		else if(OPEN_COND2 == mOpenCond)
		{
			if(primDataBuf[primBufIndex].lastPrice - scndDataBuf[scndBufIndex].lastPrice < (-1)*stgArg.stopBollAmp*mBoll.GetBoll(0).mStdDev + mBoll.GetBoll(0).mMidLine)
			{
				SetEvent(MUST_STOP);
				lIsClose = true;
			}
		}
		else if(OPEN_COND3 == mOpenCond)
		{
			if(scndDataBuf[scndBufIndex].lastPrice - primDataBuf[primBufIndex].lastPrice > stgArg.stopBollAmp*mBoll.GetBoll(0).mStdDev + mBoll.GetBoll(0).mMidLine)
			{
				SetEvent(MUST_STOP);
				lIsClose = true;
			}
		}
		else if(OPEN_COND4 == mOpenCond)
		{
			if(scndDataBuf[scndBufIndex].lastPrice - primDataBuf[primBufIndex].lastPrice < (-1)*stgArg.stopBollAmp*mBoll.GetBoll(0).mStdDev + mBoll.GetBoll(0).mMidLine)
			{
				SetEvent(MUST_STOP);
				lIsClose = true;
			}
		}
	}
		
	return lIsClose;
}
void PrimeryAndSecondary::LogBollData()
{
	BollingerBandData tempData = mBoll.GetBoll(0);
	tempStream.clear();
	tempStream.str("");
	tempStream<<primDataBuf[primBufIndex].lastPrice<<"	"<<scndDataBuf[scndBufIndex].lastPrice<<"	"<<tempData.mMidLine<<"	"<<tempData.mStdDev<<"	"<<tempData.mOutterUpperLine<<"	"<<tempData.mOutterLowerLine<<"	"<<tempData.mInnerUpperLine<<"	"<<tempData.mInnerLowerLine;
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
void PrimeryAndSecondary::StopOpenJudge()
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
				logger.LogThisFast("[EVENT]: OPEN_PRICE_NOT_GOOD");
				SetEvent(OPEN_PRICE_BAD);
			}
		}
		else if(OPEN_COND2 == mOpenCond)
		{
			if(primDataBuf[primBufIndex].askPrice - scndDataBuf[scndBufIndex].bidPrice > mBoll.GetBoll(0).mInnerLowerLine)
			{
				logger.LogThisFast("[EVENT]: OPEN_PRICE_NOT_GOOD");
				SetEvent(OPEN_PRICE_BAD);
			}
		}
		else if(OPEN_COND3 == mOpenCond)
		{
			if(scndDataBuf[scndBufIndex].bidPrice - primDataBuf[primBufIndex].askPrice < mBoll.GetBoll(0).mInnerUpperLine)
			{
				logger.LogThisFast("[EVENT]: OPEN_PRICE_NOT_GOOD");
				SetEvent(OPEN_PRICE_BAD);
			}
		}
		else if(OPEN_COND4 == mOpenCond)
		{
			if(scndDataBuf[scndBufIndex].askPrice - primDataBuf[primBufIndex].bidPrice > mBoll.GetBoll(0).mInnerLowerLine)
			{
				logger.LogThisFast("[EVENT]: OPEN_PRICE_NOT_GOOD");
				SetEvent(OPEN_PRICE_BAD);
			}
		}
		else
		{
			logger.LogThisFast("[FATAL ERROR]: ILLEGAL OPEN COND");
		}
	}
}
void PrimeryAndSecondary::OpenJudge(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
	// if the bollinger band is not wide enough, then return
	if(mBoll.GetBoll(0).mOutterUpperLine - mBoll.GetBoll(0).mOutterLowerLine < stgArg.bollAmpLimit)
	{
		return;
	}
	// using bid_price - ask_price to do the open timing judge, this is rougher to meet
	if( primDataBuf[primBufIndex].bidPrice - scndDataBuf[scndBufIndex].askPrice > 0 &&
		primDataBuf[primBufIndex].bidPrice - scndDataBuf[scndBufIndex].askPrice > mBoll.GetBoll(0).mOutterUpperLine )
	{
		/* condition 1 */
		logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND1");
		SetEvent(OPEN_PRICE_GOOD);
	}
	else if( primDataBuf[primBufIndex].askPrice - scndDataBuf[scndBufIndex].bidPrice > 0 &&
		primDataBuf[primBufIndex].askPrice - scndDataBuf[scndBufIndex].bidPrice < mBoll.GetBoll(0).mOutterLowerLine )
	{
		/* condition 2 */
		logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND2");
		SetEvent(OPEN_PRICE_GOOD);
	}
	else if( scndDataBuf[scndBufIndex].bidPrice - primDataBuf[primBufIndex].askPrice > 0 &&
		scndDataBuf[scndBufIndex].bidPrice - primDataBuf[primBufIndex].askPrice > mBoll.GetBoll(0).mOutterUpperLine )
	{
		/* condition 3 */
		logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND3");
		SetEvent(OPEN_PRICE_GOOD);
	}
	else if( scndDataBuf[scndBufIndex].askPrice - primDataBuf[primBufIndex].bidPrice > 0 &&
		scndDataBuf[scndBufIndex].askPrice - primDataBuf[primBufIndex].bidPrice < mBoll.GetBoll(0).mOutterLowerLine )
	{
		/* condition 4 */
		logger.LogThisFast("[EVENT]: OPEN_PRICE_GOOD_COND4");
		SetEvent(OPEN_PRICE_GOOD);
	}
}
bool PrimeryAndSecondary::StopWinJudge()
{
	bool lGoodToClose = false;
	if(mStateMachine.GetState() == PENDING_STATE)
	{
		if(OPEN_COND1 == mOpenCond)
		{
			if(primDataBuf[primBufIndex].lastPrice - scndDataBuf[scndBufIndex].lastPrice <= mBoll.GetBoll(0).mOutterLowerLine);
			{
				SetEvent(MUST_STOP);
				lGoodToClose = true;
			}
		}
		else if(OPEN_COND2 == mOpenCond)
		{
			if(primDataBuf[primBufIndex].lastPrice - scndDataBuf[scndBufIndex].lastPrice >= mBoll.GetBoll(0).mOutterUpperLine)
			{
				SetEvent(MUST_STOP);
				lGoodToClose = true;
			}
		}
		else if(OPEN_COND3 == mOpenCond)
		{
			if(scndDataBuf[scndBufIndex].lastPrice - primDataBuf[primBufIndex].lastPrice <= mBoll.GetBoll(0).mOutterLowerLine)
			{
				SetEvent(MUST_STOP);
				lGoodToClose = true;
			}
		}
		else if(OPEN_COND4 == mOpenCond)
		{
			if(scndDataBuf[scndBufIndex].lastPrice - primDataBuf[primBufIndex].lastPrice >= mBoll.GetBoll(0).mOutterUpperLine)
			{
				SetEvent(MUST_STOP);
				lGoodToClose = true;
			}
		}
	}
	return lGoodToClose;
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
		/* do nothing */
		break;
	case CLOSING_BOTH_STATE:
		CloseBoth();
		break;
	case CANCELLING_SCND_STATE:
		CancelScnd();
		CheckOrder();
		break;
	case CLOSING_SCND_STATE:
		CloseScnd();
		break;
	case CANCELLING_PRIM_STATE:
		CancelPrim();
		CheckOrder();
		break;
	case WAITING_SCND_CLOSE_STATE:
		break;
	case WAITING_PRIM_CLOSE_STATE:
		break;
	default:
		break;
	}
		
}
