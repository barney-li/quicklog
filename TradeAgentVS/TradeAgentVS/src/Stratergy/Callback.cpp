#include <PrimeryAndSecondary.h>
///行情数据
void PrimeryAndSecondary::HookOnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
	if(BufferData(pDepthMarketData))
	{
		
		if(VerifyMarketData(primDataBuf[primBufIndex]) && VerifyMarketData(scndDataBuf[scndBufIndex]))
		{
			if(strncmp(pDepthMarketData->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
			{
				//only calculate Boll Band when primary instrument data comes
				mBoll.CalcBoll(primDataBuf[primBufIndex].lastPrice-scndDataBuf[scndBufIndex].lastPrice, stgArg.bollPeriod, stgArg.outterBollAmp, stgArg.innerBollAmp);
			}
#ifndef BACK_TEST
			LogBollData();
#endif
#ifdef KEEP_BOLL
			LogBollData();
#endif
			if(mBoll.IsBollReady())
			{
				TRADE_STATE lCurState = mStateMachine.GetState();
				//这里的action在每次新数据到来的时候都会执行
				switch(lCurState)
				{
				case IDLE_STATE:
					if(mStart && IsTradeTime(pDepthMarketData->UpdateTime))
					{
						if(IDLE_STATE != mLastState)
						{
							logger.LogThisFastNoTimeStamp(" ");
					
						}
						OpenJudge(*pDepthMarketData);
					}
					break;
				case OPENING_SCND_STATE:
					IsTradeTime(pDepthMarketData->UpdateTime);
					StopOpenJudge(*pDepthMarketData);
					break;
				case CHECKING_SCND_STATE:
					CancelScnd();
					break;
				case OPENING_PRIM_STATE:
					IsTradeTime(pDepthMarketData->UpdateTime);
					StopOpenJudge(*pDepthMarketData);
					break;
				case PENDING_STATE:
					IsTradeTime(pDepthMarketData->UpdateTime);
					StopLoseJudge(*pDepthMarketData);
					StopWinJudge(*pDepthMarketData);
					break;
#ifndef BACK_TEST
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
#endif
				default:
					break;
				}
				mLastState = lCurState;
			}
		}
			
	}
#ifndef BACK_TEST
	BollingerBandData lBoll = mBoll.GetBoll(0);
	cout<<pDepthMarketData->UpdateTime<<"	"<<primDataBuf[primBufIndex].lastPrice<<" "<<scndDataBuf[scndBufIndex].lastPrice<<" "<<lBoll.mMidLine<<" "<<lBoll.mOutterUpperLine<<" "<<lBoll.mOutterLowerLine<<endl;
#endif
	//cout<<pDepthMarketData->UpdateTime<<endl;
}
///成交通知
void PrimeryAndSecondary::OnRtnTrade(CThostFtdcTradeField* pTrade)
{
	double lProfit = 0;
	if((pTrade->OffsetFlag == THOST_FTDC_OF_CloseToday)||
		(pTrade->OffsetFlag == THOST_FTDC_OF_Close)||
		(pTrade->OffsetFlag == THOST_FTDC_OF_ForceClose))
	{
		if(strncmp(pTrade->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: SCND_CLOSED (from trade return)");
			// calculate and record profit
			if(BUY_SCND_SELL_PRIM == mTradeDir)
			{
				lProfit = pTrade->Price - mScndEnterPrice;
			}
			else if(BUY_PRIM_SELL_SCND == mTradeDir)
			{
				lProfit = mScndEnterPrice - pTrade->Price;
			}
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[INFO]: scnd close price: "<<pTrade->Price<<" scnd profit: "<<lProfit;
			logger.LogThisFast(tempStream.str());
		}
		if(strncmp(pTrade->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: PRIM_CLOSED (from trade return)");
			// calculate and record profit
			if(BUY_SCND_SELL_PRIM == mTradeDir)
			{
				lProfit = mPrimEnterPrice - pTrade->Price;
			}
			else if(BUY_PRIM_SELL_SCND == mTradeDir)
			{
				lProfit = pTrade->Price - mPrimEnterPrice;
			}
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[INFO]: prim close price: "<<pTrade->Price<<" prim profit: "<<lProfit;
			logger.LogThisFast(tempStream.str());
		}
	}
	if(pTrade->OffsetFlag == THOST_FTDC_OF_Open)
	{
		if(strncmp(pTrade->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			if(mTradedShares == stgArg.openShares)
			{
				logger.LogThisFast("[EVENT]: SCND_OPENED (from trade return)");
				// record the "real" enter price
				mScndEnterPrice = pTrade->Price;
				tempStream.clear();
				tempStream.str("");
				tempStream<<"[INFO]: scnd open price: "<<mScndEnterPrice;
				logger.LogThisFast(tempStream.str());
			}
			else if(mTradedShares < stgArg.openShares)
			{
				logger.LogThisFast("[EVENT]: SCND_PARTLY_OPENED (from trade return)");
				// record the "real" enter price
				mScndEnterPrice = pTrade->Price;
				tempStream.clear();
				tempStream.str("");
				tempStream<<"[INFO]: scnd open price: "<<mScndEnterPrice;
				logger.LogThisFast(tempStream.str());
			}
			else
			{
				logger.LogThisFast("[FATAL ERROR]: opened more shares than configured, stop trading");
				tempStream.clear();
				tempStream.str("");
				tempStream<<"[INFO]: scnd open price: "<<mScndEnterPrice<<" number of shares: "<<mTradedShares<<"/"<<stgArg.openShares;
				logger.LogThisFast(tempStream.str());
				SetEvent(NOT_TRADING_TIME);
			}
		}
		if(strncmp(pTrade->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: PRIM_OPENED (from trade return)");
			// record the "real" enter price
			mPrimEnterPrice = pTrade->Price;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[INFO]: prim open price: "<<mPrimEnterPrice<<" number of shares: "<<pTrade->Volume;
			logger.LogThisFast(tempStream.str());
			// as long as the response of primary instrument has been received, we move on. because we assumed that primary 
			// instrument can always be traded
		}
	}
	
}
///报单通知
void PrimeryAndSecondary::OnRtnOrder(CThostFtdcOrderField* pOrder)
{
	if(pOrder == NULL)
	{
		return;
	}
	cout<<"---> OnRtnOrder: "<<std::endl;
	cout<<"------> Instrument ID: "<<pOrder->InstrumentID<<endl;
	cout<<"------> Order Status: "<<pOrder->OrderStatus<<endl;
	cout<<"------> Cancel Time: "<<pOrder->CancelTime<<endl;
	cout<<"------> Status Message: "<<pOrder->StatusMsg<<endl;
	cout<<"------> Order Submit Status: "<<pOrder->OrderSubmitStatus<<endl;

	//开仓回报，全部成交
	if((pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)
		&&(pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Open))
	{
		if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: SCND_OPENED (from order return, order all traded: "<<pOrder->VolumeTraded<<"/"<<pOrder->VolumeTotal+pOrder->VolumeTraded<<")";
			logger.LogThisFast(tempStream.str());
			mTradedShares = pOrder->VolumeTraded;
			SetEvent(SCND_OPENED);
		}
		if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
		{
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: PRIM_OPENED (from order return, order all traded: "<<pOrder->VolumeTraded<<"/"<<pOrder->VolumeTotal+pOrder->VolumeTraded<<")";
			logger.LogThisFast(tempStream.str());
			//mTradedShares = pOrder->VolumeTraded;//主力不需要更新mTradedShares
			SetEvent(PRIM_OPENED);
		}
	}
	//开仓回报，部分成交，剩余排队
	else if((pOrder->OrderStatus == THOST_FTDC_OST_PartTradedQueueing)
		&&(pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Open))
	{
		if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: SCND_OPENED (from order return, order partly traded: "<<pOrder->VolumeTraded<<"/"<<pOrder->VolumeTotal+pOrder->VolumeTraded<<")";
			logger.LogThisFast(tempStream.str());
			mTradedShares = pOrder->VolumeTraded;
			SetEvent(SCND_PARTLY_OPENED);
		}
		if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
		{
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: PRIM_OPENED (from order return, order partly traded: "<<pOrder->VolumeTraded<<"/"<<pOrder->VolumeTotal+pOrder->VolumeTraded<<")";
			logger.LogThisFast(tempStream.str());
			//mTradedShares = pOrder->VolumeTraded;//主力不需要更新已成交手数，也不需要触发事件，主力必须全部成交
		}
	}
	//开仓回报，部分成交，剩余没在排队，不知道干嘛的
	else if((pOrder->OrderStatus == THOST_FTDC_OST_PartTradedNotQueueing)
		&&(pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Open))
	{
		if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: SCND_OPENED (from order return, order partly traded, not queuing: "<<pOrder->VolumeTraded<<"/"<<pOrder->VolumeTotal+pOrder->VolumeTraded<<")";
			logger.LogThisFast(tempStream.str());
		}
		if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
		{
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: PRIM_OPENED (from order return, order partly traded, not queuing: "<<pOrder->VolumeTraded<<"/"<<pOrder->VolumeTotal+pOrder->VolumeTraded<<")";
			logger.LogThisFast(tempStream.str());
		}
	}

	//撤单回报
	if((pOrder->OrderStatus == THOST_FTDC_OST_Canceled) && (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted))
	{
		if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: SCND_CANCELLED");
			SetEvent(SCND_CANCELLED);
		}
		if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: PRIM_CANCELLED");
			SetEvent(PRIM_CANCELLED);
		}
	}
	////平仓回报
	if((pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)
		&&(pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday || pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Close))
	{
		if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: SCND_CLOSED (from order return)");
			SetEvent(SCND_CLOSED);
		}
		if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: PRIM_CLOSED (from order return)");
			SetEvent(PRIM_CLOSED);
		}
	}
	
}
// 查询仓位的应答函数
void PrimeryAndSecondary::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if(pRspInfo != NULL)
	{
		bool errorResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
		if(errorResult)
		{
			printf("------> ErrorID = %d, ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		}
	}
	if(pInvestorPosition == NULL)
	{
		return;
	}
	else
	{
		if(pInvestorPosition->InstrumentID != NULL)
		{
			if(strncmp(pInvestorPosition->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.size()) == 0)
			{
				// Position为本合约当前所有持仓，TodayPosition为本合约的今持仓，YdPosition是今天开盘前的昨持仓
				if(pInvestorPosition->Position == 0 && pInvestorPosition->TodayPosition == 0)
				{
					if(pInvestorPosition->PosiDirection == THOST_FTDC_PD_Long)
					{
						mPrimTodayLongPosition = 0;
						mPrimYdLongPosition = 0;
					}
					else if(pInvestorPosition->PosiDirection == THOST_FTDC_PD_Short)
					{
						mPrimTodayShortPosition = 0;
						mPrimYdShortPosition = 0;
					}
				}
				else
				{
					if(pInvestorPosition->PosiDirection == THOST_FTDC_PD_Long)
					{
						mPrimTodayLongPosition = pInvestorPosition->TodayPosition;
						mPrimYdLongPosition = pInvestorPosition->Position - pInvestorPosition->TodayPosition;
					}
					else if(pInvestorPosition->PosiDirection == THOST_FTDC_PD_Short)
					{
						mPrimTodayShortPosition = pInvestorPosition->TodayPosition;
						mPrimYdShortPosition = pInvestorPosition->Position - pInvestorPosition->TodayPosition;
					}
				}
			}
			if(strncmp(pInvestorPosition->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.size()) == 0)
			{
				if(pInvestorPosition->Position == 0 && pInvestorPosition->TodayPosition == 0)
				{
					if(pInvestorPosition->PosiDirection == THOST_FTDC_PD_Long)
					{
						mScndTodayLongPosition = 0;
						mScndYdLongPosition = 0;
					}
					else if(pInvestorPosition->PosiDirection == THOST_FTDC_PD_Short)
					{
						mScndTodayShortPosition = 0;
						mScndYdShortPosition = 0;
					}
				}
				else
				{
					if(pInvestorPosition->PosiDirection == THOST_FTDC_PD_Long)
					{
						mScndTodayLongPosition = pInvestorPosition->TodayPosition;
						mScndYdLongPosition = pInvestorPosition->Position - pInvestorPosition->TodayPosition;
					}
					else if(pInvestorPosition->PosiDirection == THOST_FTDC_PD_Short)
					{
						mScndTodayShortPosition = pInvestorPosition->TodayPosition;
						mScndYdShortPosition = pInvestorPosition->Position - pInvestorPosition->TodayPosition;
					}
				}
			}
			cout<<"------> Instrument: "<<pInvestorPosition->InstrumentID<<endl;
			cout<<"------> Today Position: "<<pInvestorPosition->TodayPosition<<endl;
			cout<<"------> Yesterday Position: "<<pInvestorPosition->YdPosition<<endl;
			cout<<"------> Position Direction ( 2 for buy, 3 for sell ): "<<pInvestorPosition->PosiDirection<<endl;//2多  3空
			cout<<"------> Position Profit: "<<pInvestorPosition->PositionProfit<<endl;
		}
		if(bIsLast)
		{
			/*以下代码存在隐患，在解决之前不启用*/

			if(mPrimTodayLongPosition == 0 
				&& mPrimYdLongPosition == 0
				&& mPrimTodayShortPosition == 0
				&& mPrimYdShortPosition == 0)
			{
				//logger.LogThisFast("[EVENT]: PRIM_CLOSED (from investor position query)");
				SetEvent(PRIM_CLOSED);
			}
			else
			{
				if(mStateMachine.GetState() == IDLE_STATE)
				{
					logger.LogThisFast("[EVENT]: PRIM_OPENED (from investor position query)");
					SetEvent(PRIM_OPENED);
				}
			}
			if(mScndTodayLongPosition == 0 
				&& mScndYdLongPosition == 0
				&& mScndTodayShortPosition == 0
				&& mScndYdShortPosition == 0)
			{
				//logger.LogThisFast("[EVENT]: SCND_CLOSED (from investor position query)");
				SetEvent(SCND_CLOSED);
			}
			else
			{
				if(mStateMachine.GetState() == IDLE_STATE)
				{
					logger.LogThisFast("[EVENT]: SCND_OPENED (from investor position query)");
					SetEvent(SCND_OPENED);
				}
			}
		}
			
	}

}
// 查询订单的应答函数
void PrimeryAndSecondary::OnRspQryOrder(CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) 
{
	TRADE_STATE lCurState = mStateMachine.GetState();
	if(pOrder)
	{
		if(pOrder->InstrumentID == NULL)
		{
			return;
		}
		else if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.size()) == 0)
		{
			if(pOrder->OrderRef == lastPrimOrder.orderRef && pOrder->SessionID == lastPrimOrder.sessionId)
			{
				if(pOrder->OrderStatus == THOST_FTDC_OST_Canceled && lCurState == CANCELLING_PRIM_STATE)
				{
					logger.LogThisFast("[EVENT]: PRIM_CANCELLED (from order query)");
					SetEvent(PRIM_CANCELLED);
				}
			}
		}
		else if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.size()) == 0)
		{
			if(pOrder->OrderRef == lastScndOrder.orderRef && pOrder->SessionID == lastScndOrder.sessionId)
			{
				if(pOrder->OrderStatus == THOST_FTDC_OST_Canceled && lCurState == CANCELLING_SCND_STATE)
				{
					logger.LogThisFast("[EVENT]: SCND_CANCELLED (from order query)");
					SetEvent(SCND_CANCELLED);
				}
			}
		}
	}
	else
	{
		if(nRequestID == mPrimReqOrderId && lCurState == CANCELLING_PRIM_STATE)
		{
			logger.LogThisFast("[EVENT]: PRIM_CANCELLED (no order record)");
			SetEvent(PRIM_CANCELLED);
		}
		if(nRequestID == mScndReqOrderId && (lCurState == CANCELLING_SCND_STATE || CHECKING_SCND_STATE))
		{
			logger.LogThisFast("[EVENT]: SCND_CANCELLED (no order record)");
			SetEvent(SCND_CANCELLED);
		}
	}
}
// 撤单的应答函数
void PrimeryAndSecondary::OnRspOrderAction(CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	//ErrorID = 25, ErrorMsg = 综合交易平台：撤单找不到相应报单
	if(pRspInfo->ErrorID == 25)
	{
		if(pInputOrderAction != null)
		{
			if(strncmp(pInputOrderAction->OrderRef, lastPrimOrder.orderRef.c_str(), lastPrimOrder.orderRef.size()) == 0
				&& pInputOrderAction->SessionID == lastPrimOrder.sessionId
				&& strncmp(pInputOrderAction->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.size()) == 0)
			{
				logger.LogThisFast("[EVENT]: PRIM_CANCELLED (from cancel response)");
				SetEvent(PRIM_CANCELLED);
			}
			if(strncmp(pInputOrderAction->OrderRef, lastScndOrder.orderRef.c_str(),  lastScndOrder.orderRef.size()) == 0
				&& pInputOrderAction->SessionID == lastScndOrder.sessionId
				&& strncmp(pInputOrderAction->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.size()) == 0)
			{
				logger.LogThisFast("[EVENT]: SCND_CANCELLED (from cancel response)");
				SetEvent(SCND_CANCELLED);
			}
		}
	}
}