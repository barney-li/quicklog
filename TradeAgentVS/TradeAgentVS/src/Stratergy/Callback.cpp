#include <PrimeryAndSecondary.h>
///行情数据
void PrimeryAndSecondary::HookOnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
	if(BufferData(pDepthMarketData) 
		&& strncmp(pDepthMarketData->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
	{
		//calculate Boll Band
		mBoll.CalcBoll(primDataBuf[primBufIndex].lastPrice-scndDataBuf[scndBufIndex].lastPrice, stgArg.bollPeriod, stgArg.outterBollAmp, stgArg.innerBollAmp);
		LogBollData();
		/* put open judge here*/
		if(mBoll.IsBollReady() && mStart)
		{
			OpenJudge(pDepthMarketData);
			StopOpenJudge();
			StopLoseJudge(pDepthMarketData);
			StopWinJudge();
		}
			
	}
	//CheckPosition();
	BollingerBandData lBoll = mBoll.GetBoll(0);
	cout<<primDataBuf[primBufIndex].lastPrice<<" "<<scndDataBuf[scndBufIndex].lastPrice<<" "<<lBoll.mMidLine<<" "<<lBoll.mOutterUpperLine<<" "<<lBoll.mOutterLowerLine<<endl;
}
///成交通知
void PrimeryAndSecondary::OnRtnTrade(CThostFtdcTradeField* pTrade)
{
	if((pTrade->OffsetFlag == THOST_FTDC_OF_CloseToday)||
		(pTrade->OffsetFlag == THOST_FTDC_OF_Close)||
		(pTrade->OffsetFlag == THOST_FTDC_OF_ForceClose))
	{
		if(strncmp(pTrade->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: SCND_CLOSED");
		}
		if(strncmp(pTrade->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: PRIM_CLOSED");
		}
	}
	if(pTrade->OffsetFlag == THOST_FTDC_OF_Open)
	{
		if(strncmp(pTrade->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: SCND_OPENED");
			SetEvent(SCND_OPENED);
		}
		if(strncmp(pTrade->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: PRIM_OPENED");
			SetEvent(PRIM_OPENED);
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
	//// 记录次主力订单信息
	//if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
	//{
	//	lastScndOrder.instrument = stgArg.secondaryInst;
	//	if(pOrder->ExchangeID != NULL)
	//	{
	//		lastScndOrder.exchangeId = "";
	//		lastScndOrder.exchangeId.append(pOrder->ExchangeID, sizeof(TThostFtdcExchangeIDType));
	//	}
	//	if(pOrder->OrderRef != NULL)
	//	{
	//		lastScndOrder.orderRef = "";
	//		lastScndOrder.orderRef.append(pOrder->OrderRef, sizeof(TThostFtdcOrderRefType));
	//	}
	//	if(pOrder->OrderSysID != NULL)
	//	{
	//		lastScndOrder.orderSysId = "";
	//		lastScndOrder.orderSysId.append(pOrder->OrderSysID, sizeof(TThostFtdcOrderSysIDType));
	//	}
	//	lastScndOrder.sessionId = pOrder->SessionID;
	//	lastScndOrder.frontId = pOrder->FrontID;
	//}
	//// 记录主力订单信息
	//if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
	//{
	//	lastPrimOrder.instrument = stgArg.primaryInst;
	//	if(pOrder->ExchangeID != NULL)
	//	{
	//		lastPrimOrder.exchangeId = "";
	//		lastPrimOrder.exchangeId.append(pOrder->ExchangeID, sizeof(TThostFtdcExchangeIDType));
	//	}
	//	if(pOrder->OrderRef != NULL)
	//	{
	//		lastPrimOrder.orderRef = "";
	//		lastPrimOrder.orderRef.append(pOrder->OrderRef, sizeof(TThostFtdcOrderRefType));
	//	}
	//	if(pOrder->OrderSysID != NULL)
	//	{
	//		lastPrimOrder.orderSysId = "";
	//		lastPrimOrder.orderSysId.append(pOrder->OrderSysID, sizeof(TThostFtdcOrderSysIDType));
	//	}
	//	lastPrimOrder.sessionId = pOrder->SessionID;
	//	lastPrimOrder.frontId = pOrder->FrontID;
	//}
	//撤单回报
	if((pOrder->OrderStatus == THOST_FTDC_OST_Canceled) && (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted))
	{
		if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: SCND_CANCELLED");
			SetEvent(SCND_CANCELLED);
		}
		if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: PRIM_CANCELLED");
			SetEvent(PRIM_CANCELLED);
		}
	}
	////平仓回报
	if((pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)
		&&(pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted)
		&&(pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday || pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Close))
	{
		if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: SCND_CLOSED");
			SetEvent(SCND_CLOSED);
		}
		if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: PRIM_CLOSED");
			SetEvent(PRIM_CLOSED);
		}
	}
	//开仓回报
	if((pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)
		&&(pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted)
		&&(pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Open))
	{
		if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: SCND_OPENED");
			SetEvent(SCND_OPENED);
		}
		if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
		{
			logger.LogThisFast("[EVENT]: PRIM_OPENED");
			SetEvent(PRIM_OPENED);
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
			if(mPrimTodayLongPosition == 0 
				&& mPrimYdLongPosition == 0
				&& mPrimTodayShortPosition == 0
				&& mPrimYdShortPosition == 0)
			{
				logger.LogThisFast("[EVENT]: PRIM_CLOSED (from investor position query)");
				SetEvent(PRIM_CLOSED);
			}
			else
			{
				logger.LogThisFast("[EVENT]: PRIM_OPENED (from investor position query)");
				SetEvent(PRIM_OPENED);
			}
			if(mScndTodayLongPosition == 0 
				&& mScndYdLongPosition == 0
				&& mScndTodayShortPosition == 0
				&& mScndYdShortPosition == 0)
			{
				logger.LogThisFast("[EVENT]: SCND_CLOSED (from investor position query)");
				SetEvent(SCND_CLOSED);
			}
			else
			{
				logger.LogThisFast("[EVENT]: SCND_OPENED (from investor position query)");
				SetEvent(SCND_OPENED);
			}
		}
			
	}

}
// 查询订单的应答函数
void PrimeryAndSecondary::OnRspQryOrder(CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) 
{
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
				if(pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
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
				if(pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
				{
					logger.LogThisFast("[EVENT]: SCND_CANCELLED (from order query)");
					SetEvent(SCND_CANCELLED);
				}
			}
		}
	}
	else
	{
		logger.LogThisFast("[EVENT]: PRIM_CANCELLED (no order record)");
		logger.LogThisFast("[EVENT]: SCND_CANCELLED (no order record)");
		SetEvent(PRIM_CANCELLED);
		SetEvent(SCND_CANCELLED);
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
				logger.LogThisFast("[EVENT]: PRIM_CANCELLED (from cancel response)");
				SetEvent(PRIM_CANCELLED);
			}
		}
	}
}