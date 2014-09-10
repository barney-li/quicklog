#include <TradeProcess.h>
/************************************************************************/
// ��ƽ�ַ�����
// �۸��������޼�
// �ɽ���������������
// ����ֵ��True����/False�쳣
/************************************************************************/
bool TradeProcess::OrderInsert(string aInstrument, double aPrice, int aVolume, OrderType aOrderType, ORDER_INDEX_TYPE* aOrderIndex, bool aYDay)
{
	bool lReturnCode = true;
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, basicTradeProcessData.brokerId);
	//�û�����
	strcpy(req.UserID, basicTradeProcessData.investorId);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, basicTradeProcessData.investorId);
	///��Լ����
	strncpy(req.InstrumentID, aInstrument.c_str(), aInstrument.size());
	///��������
	sprintf(req.OrderRef, "%d", ++orderRef);
	//strcpy(req.OrderRef, orderRef);
	///�û�����
	//	TThostFtdcUserIDType	UserID;
	if(aPrice == 0)
	{
		///�����۸�����: �����
		req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
		///����۸�
		req.LimitPrice = 0;
		///��Ч������: ������Ч
		req.TimeCondition = THOST_FTDC_TC_IOC;
	}
	else
	{
		///�����۸�����: �޼�
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		///�۸�
		req.LimitPrice = aPrice;
		///��Ч������: ������Ч
		req.TimeCondition = THOST_FTDC_TC_GFD;
	}

	switch(aOrderType)
	{
	case mBuy:
		///��������: ��
		req.Direction = THOST_FTDC_D_Buy;
		req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
		break;
	case mSell:
		///��������: ��
		req.Direction = THOST_FTDC_D_Sell;
		///��Ͽ�ƽ��־: ƽ��ƽ��
		if(aYDay)
		{
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
		}
		else
		{
			req.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
		}
		break;
	case mSellShort:
		///��������: ��
		req.Direction = THOST_FTDC_D_Sell;
		req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
		break;
	case mBuyToCover:
		///��������: ��
		req.Direction = THOST_FTDC_D_Buy;
		///��Ͽ�ƽ��־: ƽ��ƽ��
		if(aYDay)
		{
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
		}
		else
		{
			req.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
		}
		break;
	default:
		printf("wrong order type");
		return false;
	}
	
	///ǿƽԭ��: ��ǿƽ
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�û�ǿ����־: ��
	req.UserForceClose = 0;

	///���Ͷ���ױ���־
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation; //Ͷ�����������ױ�
	
	///����
	req.VolumeTotalOriginal = aVolume;
	
	///GTD����
	//	TThostFtdcDateType	GTDDate;
	///�ɽ�������: ��������
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���: 1
	req.MinVolume = 1;
	///��������: ����
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///ֹ���
	//	TThostFtdcPriceType	StopPrice;

	///�Զ������־: ��
	req.IsAutoSuspend = 0;
	///ҵ��Ԫ
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///������
	//	TThostFtdcRequestIDType	RequestID;
	
	printf("<--- order insert, request ID: %d\n", ++iReqID);
	printf("<------ order reference: %d\n", orderRef);

	if(pUserApi->ReqOrderInsert(&req, iReqID) != 0)
	{
		printf("****** ReqOrderInsert return error ******\n");
		lReturnCode = false;
	}
	else
	{
		// update ORDER_INDEX, necessary for cancelling orders
		TThostFtdcOrderRefType tempOrderRef;
		sprintf(tempOrderRef, "%d", orderRef);
		aOrderIndex->orderRef = "";
		aOrderIndex->orderSysId = "";
		aOrderIndex->exchangeId = "";
		aOrderIndex->orderRef.append(tempOrderRef, sizeof(TThostFtdcOrderRefType));
		aOrderIndex->frontId = frontId;
		aOrderIndex->sessionId = sessionId;
		aOrderIndex->instrument = aInstrument;
	}
	return lReturnCode;
}
/************************************************************************/
// ����ַ�����
// �۸��������޼�
// �ɽ���������������
// ����ֵ��True����/False�쳣
/************************************************************************/
bool TradeProcess::Buy(string aInstrument, double aPrice, int aVolume, ORDER_INDEX_TYPE* aOrderIndex)
{
	return OrderInsert(aInstrument, aPrice, aVolume, mBuy, aOrderIndex);
}
/************************************************************************/
// ƽ��ַ�����
// �۸��������޼�
// �ɽ���������������
// ����ֵ��True����/False�쳣
/************************************************************************/
bool TradeProcess::Sell(string aInstrument, double aPrice, int aVolume, ORDER_INDEX_TYPE* aOrderIndex, bool aYDay)
{
	return OrderInsert(aInstrument, aPrice, aVolume, mSell, aOrderIndex, aYDay);
}
/************************************************************************/
// ���ղַ�����
// �۸��������޼�
// �ɽ���������������
// ����ֵ��True����/False�쳣
/************************************************************************/
bool TradeProcess::SellShort(string aInstrument, double aPrice, int aVolume, ORDER_INDEX_TYPE* aOrderIndex)
{
	return OrderInsert(aInstrument, aPrice, aVolume, mSellShort, aOrderIndex);
}
/************************************************************************/
// ƽ�ղַ�����
// �۸��������޼�
// �ɽ���������������
// ����ֵ��True����/False�쳣
/************************************************************************/
bool TradeProcess::BuyToCover(string aInstrument, double aPrice, int aVolume, ORDER_INDEX_TYPE* aOrderIndex, bool aYDay)
{
	return OrderInsert(aInstrument, aPrice, aVolume, mBuyToCover, aOrderIndex, aYDay);
}
/************************************************************************/
// ����������
// ����ֵ��True����/False�쳣
/************************************************************************/
bool TradeProcess::CancelOrder(const ORDER_INDEX_TYPE* aOrderIndex)
{
	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strncpy(req.BrokerID, basicTradeProcessData.brokerId, sizeof(TThostFtdcBrokerIDType));
	///Ͷ���ߴ���
	strncpy(req.InvestorID, basicTradeProcessData.investorId, sizeof(TThostFtdcInvestorIDType));
	///�û�����
	//strncpy(req.UserID, basicTradeProcessData.investorId, sizeof(TThostFtdcUserIDType));
	if(aOrderIndex == NULL)
	{
		logger.LogThisFast("[ERROR]: empty order index in cancellation");
		return false;
	}
	///��������
	strncpy(req.OrderRef, aOrderIndex->orderRef.c_str(), sizeof(TThostFtdcOrderRefType));
	///ǰ�ñ��
	req.FrontID = aOrderIndex->frontId;
	///�Ự���
	req.SessionID = aOrderIndex->sessionId;
	if(aOrderIndex->exchangeId.size()>0 && aOrderIndex->orderSysId.size()>0)
	{
		///����������
		strncpy(req.ExchangeID, aOrderIndex->exchangeId.c_str(), sizeof(TThostFtdcExchangeIDType));
		///�������
		strncpy(req.OrderSysID, aOrderIndex->orderSysId.c_str(), sizeof(TThostFtdcOrderSysIDType));
	}
	///������־
	req.ActionFlag = THOST_FTDC_AF_Delete;
	///��Լ����
	strncpy(req.InstrumentID, aOrderIndex->instrument.c_str(), sizeof(TThostFtdcInstrumentIDType));
	int lReturnCode = pUserApi->ReqOrderAction(&req, ++iReqID);
	if( lReturnCode == 0)// 0 for no error
	{
		return true;
	}
	else
	{
		if(lReturnCode == -1)
		{
			logger.LogThisFast("[ERROR]: network connection failed");
		}
		else if(lReturnCode == -2)
		{
			logger.LogThisFast("[ERROR]: too many unhandled request");
		}
		else
		{
			logger.LogThisFast("[ERROR]: too many request per second");
		}
		return false;
	}
}
