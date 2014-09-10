#include <TradeProcess.h>
/************************************************************************/
// 开平仓服务函数
// 价格条件：限价
// 成交条件：任意数量
// 返回值：True正常/False异常
/************************************************************************/
bool TradeProcess::OrderInsert(string aInstrument, double aPrice, int aVolume, OrderType aOrderType, ORDER_INDEX_TYPE* aOrderIndex, bool aYDay)
{
	bool lReturnCode = true;
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, basicTradeProcessData.brokerId);
	//用户代码
	strcpy(req.UserID, basicTradeProcessData.investorId);
	///投资者代码
	strcpy(req.InvestorID, basicTradeProcessData.investorId);
	///合约代码
	strncpy(req.InstrumentID, aInstrument.c_str(), aInstrument.size());
	///报单引用
	sprintf(req.OrderRef, "%d", ++orderRef);
	//strcpy(req.OrderRef, orderRef);
	///用户代码
	//	TThostFtdcUserIDType	UserID;
	if(aPrice == 0)
	{
		///报单价格条件: 任意价
		req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
		///任意价格
		req.LimitPrice = 0;
		///有效期类型: 立即有效
		req.TimeCondition = THOST_FTDC_TC_IOC;
	}
	else
	{
		///报单价格条件: 限价
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		///价格
		req.LimitPrice = aPrice;
		///有效期类型: 当日有效
		req.TimeCondition = THOST_FTDC_TC_GFD;
	}

	switch(aOrderType)
	{
	case mBuy:
		///买卖方向: 买
		req.Direction = THOST_FTDC_D_Buy;
		req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
		break;
	case mSell:
		///买卖方向: 卖
		req.Direction = THOST_FTDC_D_Sell;
		///组合开平标志: 平昨、平今
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
		///买卖方向: 卖
		req.Direction = THOST_FTDC_D_Sell;
		req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
		break;
	case mBuyToCover:
		///买卖方向: 买
		req.Direction = THOST_FTDC_D_Buy;
		///组合开平标志: 平昨、平今
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
	
	///强平原因: 非强平
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///用户强评标志: 否
	req.UserForceClose = 0;

	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation; //投机，套利或套保
	
	///数量
	req.VolumeTotalOriginal = aVolume;
	
	///GTD日期
	//	TThostFtdcDateType	GTDDate;
	///成交量类型: 任意数量
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///最小成交量: 1
	req.MinVolume = 1;
	///触发条件: 立即
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///止损价
	//	TThostFtdcPriceType	StopPrice;

	///自动挂起标志: 否
	req.IsAutoSuspend = 0;
	///业务单元
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///请求编号
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
// 开多仓服务函数
// 价格条件：限价
// 成交条件：任意数量
// 返回值：True正常/False异常
/************************************************************************/
bool TradeProcess::Buy(string aInstrument, double aPrice, int aVolume, ORDER_INDEX_TYPE* aOrderIndex)
{
	return OrderInsert(aInstrument, aPrice, aVolume, mBuy, aOrderIndex);
}
/************************************************************************/
// 平多仓服务函数
// 价格条件：限价
// 成交条件：任意数量
// 返回值：True正常/False异常
/************************************************************************/
bool TradeProcess::Sell(string aInstrument, double aPrice, int aVolume, ORDER_INDEX_TYPE* aOrderIndex, bool aYDay)
{
	return OrderInsert(aInstrument, aPrice, aVolume, mSell, aOrderIndex, aYDay);
}
/************************************************************************/
// 开空仓服务函数
// 价格条件：限价
// 成交条件：任意数量
// 返回值：True正常/False异常
/************************************************************************/
bool TradeProcess::SellShort(string aInstrument, double aPrice, int aVolume, ORDER_INDEX_TYPE* aOrderIndex)
{
	return OrderInsert(aInstrument, aPrice, aVolume, mSellShort, aOrderIndex);
}
/************************************************************************/
// 平空仓服务函数
// 价格条件：限价
// 成交条件：任意数量
// 返回值：True正常/False异常
/************************************************************************/
bool TradeProcess::BuyToCover(string aInstrument, double aPrice, int aVolume, ORDER_INDEX_TYPE* aOrderIndex, bool aYDay)
{
	return OrderInsert(aInstrument, aPrice, aVolume, mBuyToCover, aOrderIndex, aYDay);
}
/************************************************************************/
// 撤单服务函数
// 返回值：True正常/False异常
/************************************************************************/
bool TradeProcess::CancelOrder(const ORDER_INDEX_TYPE* aOrderIndex)
{
	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strncpy(req.BrokerID, basicTradeProcessData.brokerId, sizeof(TThostFtdcBrokerIDType));
	///投资者代码
	strncpy(req.InvestorID, basicTradeProcessData.investorId, sizeof(TThostFtdcInvestorIDType));
	///用户代码
	//strncpy(req.UserID, basicTradeProcessData.investorId, sizeof(TThostFtdcUserIDType));
	if(aOrderIndex == NULL)
	{
		logger.LogThisFast("[ERROR]: empty order index in cancellation");
		return false;
	}
	///报单引用
	strncpy(req.OrderRef, aOrderIndex->orderRef.c_str(), sizeof(TThostFtdcOrderRefType));
	///前置编号
	req.FrontID = aOrderIndex->frontId;
	///会话编号
	req.SessionID = aOrderIndex->sessionId;
	if(aOrderIndex->exchangeId.size()>0 && aOrderIndex->orderSysId.size()>0)
	{
		///交易所代码
		strncpy(req.ExchangeID, aOrderIndex->exchangeId.c_str(), sizeof(TThostFtdcExchangeIDType));
		///报单编号
		strncpy(req.OrderSysID, aOrderIndex->orderSysId.c_str(), sizeof(TThostFtdcOrderSysIDType));
	}
	///操作标志
	req.ActionFlag = THOST_FTDC_AF_Delete;
	///合约代码
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
