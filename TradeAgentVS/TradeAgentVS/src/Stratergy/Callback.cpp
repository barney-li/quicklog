#include <PrimeryAndSecondary.h>
///��������
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
	BollingerBandData lBoll = mBoll.GetBoll(0);
	cout<<primDataBuf[primBufIndex].lastPrice<<" "<<scndDataBuf[scndBufIndex].lastPrice<<" "<<lBoll.mMidLine<<" "<<lBoll.mOutterUpperLine<<" "<<lBoll.mOutterLowerLine<<endl;
}
///�ɽ�֪ͨ
void PrimeryAndSecondary::OnRtnTrade(CThostFtdcTradeField* pTrade)
{
	// ��Ҫͨ���ɽ��ر���ȷ��ƽ�ֽ����Ҫͨ����ѯ��λ��ȷ������Ϊ�п���û��ȫ��ƽ��
	//if((pTrade->OffsetFlag == THOST_FTDC_OF_CloseToday)||
	//	(pTrade->OffsetFlag == THOST_FTDC_OF_Close)||
	//	(pTrade->OffsetFlag == THOST_FTDC_OF_ForceClose))
	//{
	//	if(strncmp(pTrade->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
	//	{
	//		logger.LogThisFast("[EVENT]: SCND_CLOSED");
	//	}
	//	if(strncmp(pTrade->InstrumentID, stgArg.primaryInst.c_str(), stgArg.primaryInst.length()) == 0)
	//	{
	//		logger.LogThisFast("[EVENT]: PRIM_CLOSED");
	//	}
	//}
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
///����֪ͨ
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
	// ��¼������������Ϣ
	if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
	{
		lastScndOrder.instrument = stgArg.secondaryInst;
		if(pOrder->ExchangeID != NULL)
		{
			lastScndOrder.exchangeId = "";
			lastScndOrder.exchangeId.append(pOrder->ExchangeID, sizeof(TThostFtdcExchangeIDType));
		}
		if(pOrder->OrderRef != NULL)
		{
			lastScndOrder.orderRef = "";
			lastScndOrder.orderRef.append(pOrder->OrderRef, sizeof(TThostFtdcOrderRefType));
		}
		if(pOrder->OrderSysID != NULL)
		{
			lastScndOrder.orderSysId = "";
			lastScndOrder.orderSysId.append(pOrder->OrderSysID, sizeof(TThostFtdcOrderSysIDType));
		}
		lastScndOrder.sessionId = pOrder->SessionID;
		lastScndOrder.frontId = pOrder->FrontID;
	}
	// ��¼����������Ϣ
	if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
	{
		lastPrimOrder.instrument = stgArg.primaryInst;
		if(pOrder->ExchangeID != NULL)
		{
			lastPrimOrder.exchangeId = "";
			lastPrimOrder.exchangeId.append(pOrder->ExchangeID, sizeof(TThostFtdcExchangeIDType));
		}
		if(pOrder->OrderRef != NULL)
		{
			lastPrimOrder.orderRef = "";
			lastPrimOrder.orderRef.append(pOrder->OrderRef, sizeof(TThostFtdcOrderRefType));
		}
		if(pOrder->OrderSysID != NULL)
		{
			lastPrimOrder.orderSysId = "";
			lastPrimOrder.orderSysId.append(pOrder->OrderSysID, sizeof(TThostFtdcOrderSysIDType));
		}
		lastPrimOrder.sessionId = pOrder->SessionID;
		lastPrimOrder.frontId = pOrder->FrontID;
	}
	//�����ر�
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
	// ��Ҫͨ���ɽ��ر���ȷ��ƽ�ֽ����Ҫͨ����ѯ��λ��ȷ������Ϊ�п���û��ȫ��ƽ��
	////ƽ�ֻر�
	//if((pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)
	//	&&(pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted)
	//	&&(pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday))
	//{
	//	if(strncmp(pOrder->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
	//	{
	//		logger.LogThisFast("[EVENT]: SCND_CLOSED");
	//		SetEvent(SCND_CLOSED);
	//	}
	//	if(strncmp(pOrder->InstrumentID, stgArg.primaryInst.c_str(), stgArg.secondaryInst.length()) == 0)
	//	{
	//		logger.LogThisFast("[EVENT]: PRIM_CLOSED");
	//		SetEvent(PRIM_CLOSED);
	//	}
	//}
	//���ֻر�
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
// ��ѯ��λ��Ӧ����
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
				mPrimPosDir = pInvestorPosition->PosiDirection;
				// PositionΪ����Լ��ǰ���гֲ֣�TodayPositionΪ����Լ�Ľ�ֲ֣�YdPosition�ǽ��쿪��ǰ����ֲ�
				if(pInvestorPosition->Position == 0 && pInvestorPosition->TodayPosition == 0)
				{
					logger.LogThisFast("[EVENT]: PRIM_CLOSED (from investor position query)");
					mPrimTodayPosition=0;
					mPrimYdPosition=0;
					SetEvent(PRIM_CLOSED);
				}
				else
				{
					logger.LogThisFast("[EVENT]: PRIM_OPENED (from investor position query)");
					mPrimTodayPosition = pInvestorPosition->TodayPosition;
					mPrimYdPosition = pInvestorPosition->Position - pInvestorPosition->TodayPosition;
						
					SetEvent(PRIM_OPENED);
				}
			}
			if(strncmp(pInvestorPosition->InstrumentID, stgArg.secondaryInst.c_str(), stgArg.secondaryInst.size()) == 0)
			{
				mPrimPosDir = pInvestorPosition->PosiDirection;
				if(pInvestorPosition->Position == 0 && pInvestorPosition->TodayPosition == 0)
				{
					logger.LogThisFast("[EVENT]: SCND_CLOSED (from investor position query)");
					mScndTodayPosition=0;
					mScndYdPosition=0;
					SetEvent(SCND_CLOSED);
				}
				else
				{
					logger.LogThisFast("[EVENT]: SCND_OPENED (from investor position query)");
					mScndTodayPosition = pInvestorPosition->TodayPosition;
					mScndYdPosition = pInvestorPosition->Position - pInvestorPosition->TodayPosition;
					SetEvent(SCND_OPENED);
				}
			}
			cout<<"------> Instrument: "<<pInvestorPosition->InstrumentID<<endl;
			cout<<"------> Today Position: "<<pInvestorPosition->TodayPosition<<endl;
			cout<<"------> Yesterday Position: "<<pInvestorPosition->YdPosition<<endl;
			cout<<"------> Position Direction ( 2 for buy, 3 for sell ): "<<pInvestorPosition->PosiDirection<<endl;//2��  3��
			cout<<"------> Position Profit: "<<pInvestorPosition->PositionProfit<<endl;
		}
			
	}

}
// ��ѯ������Ӧ����
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
		SetEvent(PRIM_CANCELLED);
		SetEvent(SCND_CANCELLED);
	}
}