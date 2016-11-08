#include <stdafx.h>
#include "TradeProcess.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;

TradeProcess::TradeProcess()
{
    iReqID = 0;
	for(int i=0; i<MAX_FRONT; i++)
	{
		basicTradeProcessData.frontAddress[i] = new char[FRONT_ADDR_SIZE];
	}
	basicTradeProcessData.numFrontAddress = 0;
	orderRef = 0;
    pUserApi = NULL;	//方便程序判断是否连接
	finishInitFlag = false;//判断初始化是否完成
	frontId = 0;//记录前置ID
	sessionId = 0;//记录会话I 
	mInstrumentListFinished = false;//初始化合约列表为未完成
}

TradeProcess::~TradeProcess()
{
	for(int i=0; i<MAX_FRONT; i++)
	{
		delete basicTradeProcessData.frontAddress[i];
	}
    if(pUserApi)
	{
		pUserApi->Release();
	}
}

void TradeProcess::InitializeProcess(void)
{
	printf("########################################################\n");
	printf("#          Start Trade Process Initialization          #\n");
	printf("########################################################\n");
	ReqConnect();
}

void TradeProcess::ReqConnect(void)
{
	try
	{
		pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi("TradeData");
		pUserApi->RegisterSpi((CThostFtdcTraderSpi*)this);
		pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);
		pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);
		printf("<--- Request connection\n");
		startTime = microsec_clock::universal_time()+hours(8);
		std::cout<<"start connection in "<<boost::posix_time::to_simple_string(startTime)<<std::endl;
		for(int i=0; i<basicTradeProcessData.numFrontAddress; i++)
		{
			pUserApi->RegisterFront(basicTradeProcessData.frontAddress[i]);
		}
		pUserApi->Init();
	}
	catch(std::exception ex)
	{
		stringstream tempStream;
		tempStream.str("");
		tempStream<<"[EXCEPTION]: exception in ReqConnect(), error message: "<<ex.what();
		cout<<tempStream.rdbuf()<<endl;
		logger.LogThisFast(tempStream.str());
		logger.Sync();
		throw std::exception(ex);
	}
	catch(...)
	{
		stringstream tempStream;
		tempStream.str("");
		tempStream<<"[EXCEPTION]: exception in ReqConnect(), error message: unkonwn";
		cout<<tempStream.rdbuf()<<endl;
		logger.LogThisFast(tempStream.str());
		logger.Sync();
		throw std::exception();
	}
}

void TradeProcess::OnFrontConnected()
{
	printf("---> Request connection response:\n");
    printf("------> Front connected\n");
	connectedFlag = true;
	ReqLogin();
}

void TradeProcess::OnFrontDisconnected(int nReason)
{
    printf("---> Front disconnected, reason: %d\n", nReason);
	connectedFlag = false;
}

void TradeProcess::ReqLogin(void)
{
	try
	{
		CThostFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, basicTradeProcessData.brokerId);
		strcpy(req.UserID, basicTradeProcessData.investorId);
		strcpy(req.Password, basicTradeProcessData.investorPassword);
		printf("<--- Request log in, request ID: %d\n", ++iReqID);
		pUserApi->ReqUserLogin(&req, iReqID);
	}
	catch(std::exception ex)
	{
		stringstream tempStream;
		tempStream.str("");
		tempStream<<"[EXCEPTION]: exception in ReqLogin(), error message: "<<ex.what();
		cout<<tempStream.rdbuf()<<endl;
		logger.LogThisFast(tempStream.str());
		logger.Sync();
		throw std::exception(ex);
	}
	catch(...)
	{
		stringstream tempStream;
		tempStream.str("");
		tempStream<<"[EXCEPTION]: exception in ReqLogin(), error message: unkonwn";
		cout<<tempStream.rdbuf()<<endl;
		logger.LogThisFast(tempStream.str());
		logger.Sync();
		throw std::exception();
	}
}

void TradeProcess::OnRspUserLogin(	CThostFtdcRspUserLoginField* pRspUserLogin, 
									CThostFtdcRspInfoField* pRspInfo, 
									int nRequestID, 
									bool bIsLast)
{
	try
	{
		bool errorResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
		printf("---> Log in response, request ID: %d\n", nRequestID);
		if(errorResult)
		{
			printf("------>ErrorID = %d, ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		}
		else
		{
			printf("------> Log in success!\n");
			//记录登陆信息中的frontId与sessionId
			frontId = pRspUserLogin->FrontID;
			sessionId = pRspUserLogin->SessionID;
			//计算出本次会话的起始订单号码
			orderRef = atoi(pRspUserLogin->MaxOrderRef);
			printf("------> frontId: %d, sessionId: %d\n", frontId, sessionId);
			printf("------> max order reference: %s\n", pRspUserLogin->MaxOrderRef);
			printf("------> log in time: %s\n", pRspUserLogin->LoginTime);
			//是否本次响应的最后一次回调
			if(bIsLast)
			{
				printf("------> Request %d over\n", nRequestID);
				printf("########################################################\n");
				printf("#       Trade Process Initialization Accomplished      #\n");
				printf("########################################################\n");
			}
			finishInitFlag = true;//完成登陆就算初始化完成
			//查询当日结算，每日首次登陆必须查询并确认结算
			//printf("<--- Request QRY settlement information confirm, requrest ID: %d\n", ++iReqID);
			//CThostFtdcQrySettlementInfoConfirmField* pSetInfoConfirm = new CThostFtdcQrySettlementInfoConfirmField;
			//memcpy(pSetInfoConfirm->BrokerID, basicTradeProcessData.brokerId, strlen(basicTradeProcessData.brokerId)+1);
			//memcpy(pSetInfoConfirm->InvestorID, basicTradeProcessData.investorId, strlen(basicTradeProcessData.investorId)+1);
			//pUserApi->ReqQrySettlementInfoConfirm(pSetInfoConfirm, iReqID);
			//delete pSetInfoConfirm;
		}
	}
	catch(std::exception ex)
	{
		stringstream tempStream;
		tempStream.str("");
		tempStream<<"[EXCEPTION]: exception in OnRspUsrLogin(), error message: "<<ex.what();
		cout<<tempStream.rdbuf()<<endl;
		logger.LogThisFast(tempStream.str());
		logger.Sync();
		throw std::exception(ex);
	}
	catch(...)
	{
		stringstream tempStream;
		tempStream.str("");
		tempStream<<"[EXCEPTION]: exception in OnRspUsrLogin(), error message: unkonwn";
		cout<<tempStream.rdbuf()<<endl;
		logger.LogThisFast(tempStream.str());
		logger.Sync();
		throw std::exception();
	}
}

void TradeProcess::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	bool errorResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	printf("---> Request QRY settlement information confirm response, request ID: %d\n", nRequestID);
	if(errorResult)
	{
		printf("------>ErrorID = %d, ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
	else
	{
		printf("------> Request QRY settlement information confirm success!\n");
		//是否本次响应的最后一次回调
		if(bIsLast)
		{
			printf("------> Request %d over\n", nRequestID);
		}
		//查询结算信息成功后，确认结算
		ReqSettlementInfoConfirm();
	}
}
//Confirm settlement information 必须先确认结算，然后才能进行接下来的交易
void TradeProcess::ReqSettlementInfoConfirm(void)
{
	CThostFtdcSettlementInfoConfirmField settlementInfoConfirm;
	printf("<--- Request settlement information confirm, request ID: %d\n", ++iReqID);
	strcpy(settlementInfoConfirm.BrokerID, basicTradeProcessData.brokerId);
	strcpy(settlementInfoConfirm.InvestorID, basicTradeProcessData.investorId);
	strcpy(settlementInfoConfirm.ConfirmDate, "20131022");
	strcpy(settlementInfoConfirm.ConfirmTime, "2110");
	pUserApi->ReqSettlementInfoConfirm(&settlementInfoConfirm, iReqID);
}

void TradeProcess::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	bool errorResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	printf("---> Request settlement information confirm response, request ID: %d\n", nRequestID);
	if(errorResult)
	{
		printf("------>ErrorID = %d, ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
	else
	{
		printf("------> Request settlement information confirm success!\n");
		//是否本次响应的最后一次回调
		if(bIsLast)
		{
			printf("------> Request %d over\n", nRequestID);
		}
		
	}
}

bool TradeProcess::IsRspError(CThostFtdcRspInfoField* pRspInfo)
{
    // 如果ErrorID != 0, 说明收到了错误的响应
    bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
    if (bResult)
    {
        printf("--->>> ErrorID = %d, ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    return bResult;
}

void TradeProcess::OnHeartBeatWarning(int timeLapse)
{
    printf("---> Heart beat warning: %d seconds\n", timeLapse);
}

void TradeProcess::OnRspAuthenticate(CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	printf("---> Authenticate response, request ID: %d\n", nRequestID);
	//是否本次响应的最后一次回调
	if(bIsLast)
	{
		printf("------> Request %d over\n", nRequestID);
	}
}
// 基本的报单指令，用来报普通的限价单或市价投机单，最小成交量为1。
int TradeProcess::ReqOrderInsert(	TThostFtdcInstrumentIDType instrument, 
									TThostFtdcPriceType price, 
									TThostFtdcVolumeType volume, 
									TThostFtdcDirectionType orderDirection, 
									TThostFtdcOffsetFlagType orderType,
									TThostFtdcHedgeFlagType combHedgeFlag)
{
	int returnCode = -1;
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, basicTradeProcessData.brokerId);
	//用户代码
	strcpy(req.UserID, basicTradeProcessData.investorId);
	///投资者代码
	strcpy(req.InvestorID, basicTradeProcessData.investorId);
	///合约代码
	strcpy(req.InstrumentID, instrument);
	///报单引用
	sprintf(req.OrderRef, "%d", ++orderRef);
	//strcpy(req.OrderRef, orderRef);
	///用户代码
//	TThostFtdcUserIDType	UserID;
	if(price>0)
	{
		///报单价格条件: 限价
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		///价格
		req.LimitPrice = price;
		///有效期类型: 当日有效
		req.TimeCondition = THOST_FTDC_TC_GFD;
	}
	else if(price == 0)
	{
		///报单价格条件: 市价
		req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
		///价格
		req.LimitPrice = 0;
		///有效期类型: 立即有效
		req.TimeCondition = THOST_FTDC_TC_IOC;
	}
	else
	{
		printf("****** Illegal Price ******\n");
		returnCode = -1;
		return returnCode;
	}
	
	///买卖方向: 
	if(orderDirection >= THOST_FTDC_D_Buy && orderDirection <= THOST_FTDC_D_Sell)
	{
		req.Direction = orderDirection;
	}
	else
	{
		printf("****** Illegal Order Direction ******\n");
		returnCode = -1;
		return returnCode;
	}
	
	
	///组合开平标志: 开仓
	if(orderType >= THOST_FTDC_OF_Open && orderType <= THOST_FTDC_OF_LocalForceClose)
	{
		req.CombOffsetFlag[0] = orderType;
		if(orderType == THOST_FTDC_OF_ForceClose)
		{
			///强平原因: 保证金不足
			req.ForceCloseReason = THOST_FTDC_FCC_LackDeposit;
			///用户强评标志: 是
			req.UserForceClose = 1;
		}
		else
		{
			///强平原因: 非强平
			req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
			///用户强评标志: 否
			req.UserForceClose = 0;
		}
	}
	else
	{
		printf("****** Illegal Order Type ******\n");
		returnCode = -1;
		return returnCode;
	}
	
	
	///组合投机套保标志
	req.CombHedgeFlag[0] = combHedgeFlag; //投机，套利或套保
	
	///数量
	req.VolumeTotalOriginal = volume;
	
	///GTD日期
//	TThostFtdcDateType	GTDDate;
	///成交量类型: 任何数量
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
	
	printf("<--- Request order insert, request ID: %d\n", ++iReqID);
	printf("<------ Order reference: %d\n", orderRef);
	returnCode = pUserApi->ReqOrderInsert(&req, iReqID);

	if(returnCode != 0)
	{
		printf("****** ReqOrderInsert return error ******\n");
	}
	return returnCode;
}

// basic order insert interface, with order reference returned
int TradeProcess::ReqOrderInsert(	TThostFtdcInstrumentIDType instrument,		// in: instrument ID
									TThostFtdcPriceType price,					// in: price
									TThostFtdcVolumeType volume,				// in: volume
									TThostFtdcDirectionType orderDirection,		// in: order direction
									TThostFtdcOffsetFlagType orderType,			// in: order type, open or close or...
									ORDER_INDEX_TYPE* orderIndex,				// out: order index
									TThostFtdcHedgeFlagType combHedgeFlag)		// in: speculation(default) or arbitrage
{
	int returnCode = -1;
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, basicTradeProcessData.brokerId);
	//用户代码
	strcpy(req.UserID, basicTradeProcessData.investorId);
	///投资者代码
	strcpy(req.InvestorID, basicTradeProcessData.investorId);
	///合约代码
	strcpy(req.InstrumentID, instrument);
	///报单引用
	sprintf(req.OrderRef, "%d", ++orderRef);
	//strcpy(req.OrderRef, orderRef);
	///用户代码
//	TThostFtdcUserIDType	UserID;
	if(price>0)
	{
		///报单价格条件: 限价
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		///价格
		req.LimitPrice = price;
		///有效期类型: 当日有效
		req.TimeCondition = THOST_FTDC_TC_GFD;
	}
	else if(price == 0)
	{
		///报单价格条件: 市价
		req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
		///价格
		req.LimitPrice = 0;
		///有效期类型: 立即有效
		req.TimeCondition = THOST_FTDC_TC_IOC;
	}
	else
	{
		printf("****** Illegal Price ******\n");
		returnCode = -1;
		return returnCode;
	}
	
	///买卖方向: 
	if(orderDirection >= THOST_FTDC_D_Buy && orderDirection <= THOST_FTDC_D_Sell)
	{
		req.Direction = orderDirection;
	}
	else
	{
		printf("****** Illegal Order Direction ******\n");
		returnCode = -1;
		return returnCode;
	}
	
	
	///组合开平标志: 开仓
	if(orderType >= THOST_FTDC_OF_Open && orderType <= THOST_FTDC_OF_LocalForceClose)
	{
		req.CombOffsetFlag[0] = orderType;
		if(orderType == THOST_FTDC_OF_ForceClose)
		{
			///强平原因: 保证金不足
			req.ForceCloseReason = THOST_FTDC_FCC_LackDeposit;
			///用户强评标志: 是
			req.UserForceClose = 1;
		}
		else
		{
			///强平原因: 非强平
			req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
			///用户强评标志: 否
			req.UserForceClose = 0;
		}
	}
	else
	{
		printf("****** Illegal Order Type ******\n");
		returnCode = -1;
		return returnCode;
	}
	
	
	///组合投机套保标志
	req.CombHedgeFlag[0] = combHedgeFlag; //投机，套利或套保
	
	///数量
	req.VolumeTotalOriginal = volume;
	
	///GTD日期
//	TThostFtdcDateType	GTDDate;
	///成交量类型: 任何数量
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
	
	printf("<--- Request order insert, request ID: %d\n", ++iReqID);
	printf("<------ Order reference: %d\n", orderRef);
	returnCode = pUserApi->ReqOrderInsert(&req, iReqID);
	// update ORDER_INDEX
	TThostFtdcOrderRefType tempOrderRef;
	sprintf(tempOrderRef, "%d", orderRef);
	orderIndex->orderRef = "";
	orderIndex->orderSysId = "";
	orderIndex->exchangeId = "";
	orderIndex->orderRef.append(tempOrderRef, sizeof(TThostFtdcOrderRefType));
	orderIndex->frontId = frontId;
	orderIndex->sessionId = sessionId;
	if(returnCode != 0)
	{
		printf("****** ReqOrderInsert return error ******\n");
	}
	return returnCode;
}
//简单报单接口
int TradeProcess::ReqOrderInsertSimple(	TThostFtdcInstrumentIDType instrument, 
						TThostFtdcOrderPriceTypeType priceType, 
						TThostFtdcVolumeType volume, 
						TThostFtdcDirectionType orderDirection, 
						TThostFtdcOffsetFlagType orderType,
						TThostFtdcHedgeFlagType combHedgeFlag)
{
	int returnCode = -1;
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, basicTradeProcessData.brokerId);
	//用户代码
	strcpy(req.UserID, basicTradeProcessData.investorId);
	///投资者代码
	strcpy(req.InvestorID, basicTradeProcessData.investorId);
	///合约代码
	strcpy(req.InstrumentID, instrument);
	///报单引用
	sprintf(req.OrderRef, "%d", ++orderRef);
	//strcpy(req.OrderRef, orderRef);
	///用户代码
//	TThostFtdcUserIDType	UserID;
	if(priceType >= THOST_FTDC_OPT_AnyPrice && priceType <= THOST_FTDC_OPT_BidPrice1PlusThreeTicks)
	{
		///直接以价格类型报单
		req.OrderPriceType = priceType;
		///价格
		req.LimitPrice = 0;
		///有效期类型: 瞬时有效，过期取消
		req.TimeCondition = THOST_FTDC_TC_IOC;
	}
	else
	{
		printf("****** Illegal Price Type ******\n");
		returnCode = -1;
		return returnCode;
	}
	
	///买卖方向: 
	if(orderDirection >= THOST_FTDC_D_Buy && orderDirection <= THOST_FTDC_D_Sell)
	{
		req.Direction = orderDirection;
	}
	else
	{
		printf("****** Illegal Order Direction ******\n");
		returnCode = -1;
		return returnCode;
	}
	
	
	///组合开平标志: 开仓
	if(orderType >= THOST_FTDC_OF_Open && orderType <= THOST_FTDC_OF_LocalForceClose)
	{
		req.CombOffsetFlag[0] = orderType;
	}
	else
	{
		printf("****** Illegal Order Type ******\n");
		returnCode = -1;
		return returnCode;
	}
	
	
	///组合投机套保标志
	req.CombHedgeFlag[0] = combHedgeFlag; //投机，套利或套保
	
	///数量
	req.VolumeTotalOriginal = volume;
	
	///GTD日期
//	TThostFtdcDateType	GTDDate;
	///成交量类型: 任何数量
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///最小成交量: 1
	req.MinVolume = 1;
	///触发条件: 立即
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///止损价
//	TThostFtdcPriceType	StopPrice;
	///强平原因: 非强平
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///自动挂起标志: 否
	req.IsAutoSuspend = 0;
	///业务单元
//	TThostFtdcBusinessUnitType	BusinessUnit;
	///请求编号
//	TThostFtdcRequestIDType	RequestID;
	///用户强评标志: 否
	req.UserForceClose = 0;
	printf("<--- Request order insert, request ID: %d\n", ++iReqID);
	printf("<------ Order reference: %d\n", orderRef);
	returnCode = pUserApi->ReqOrderInsert(&req, iReqID);
	if(returnCode != 0)
	{
		printf("****** ReqOrderInsert return error ******\n");
	}
	return returnCode;
}

int TradeProcess::ReqOrderInsertArbitrage(TThostFtdcInstrumentIDType instrument1,	// in: instrument 1 ID
										TThostFtdcInstrumentIDType instrument2,		// in: instrument 2 ID
										TThostFtdcPriceType deltaPrice,				// in: price difference
										TThostFtdcVolumeType volume,				// in: volume
										TThostFtdcDirectionType orderDirection1,	// in: instrument 1 direction
										TThostFtdcOffsetFlagType orderType1,		// in: instrument 1 type, open or close or...
										TThostFtdcOffsetFlagType orderType2,		// in: instrument 2 type, open or close or...
										ORDER_INDEX_TYPE* orderIndex)				// out: order index
{
	int returnCode = -1;
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, basicTradeProcessData.brokerId);
	//用户代码
	strcpy(req.UserID, basicTradeProcessData.investorId);
	///投资者代码
	strcpy(req.InvestorID, basicTradeProcessData.investorId);
	///合约代码，组合合约
	string tempString;
	tempString.append("SP ");
	tempString.append(instrument1);
	tempString.append("&");
	tempString.append(instrument2);
	strcpy(req.InstrumentID, tempString.c_str());
	///报单引用
	sprintf(req.OrderRef, "%d", ++orderRef);
	///报单价格条件: 限价
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///价格，套利价差
	req.LimitPrice = deltaPrice;
	///有效期类型: 当日有效
	req.TimeCondition = THOST_FTDC_TC_GFD;
	///买卖方向，第一腿合约的买卖方向
	if(orderDirection1 >= THOST_FTDC_D_Buy && orderDirection1 <= THOST_FTDC_D_Sell)
	{
		req.Direction = orderDirection1;
	}
	else
	{
		printf("****** Illegal Order Direction ******\n");
		returnCode = -1;
		return returnCode;
	}
	
	///组合开平标志
	if(orderType1 >= THOST_FTDC_OF_Open && 
		orderType1 <= THOST_FTDC_OF_LocalForceClose &&
		orderType2 >= THOST_FTDC_OF_Open && 
		orderType2 <= THOST_FTDC_OF_LocalForceClose)
	{
		req.CombOffsetFlag[0] = orderType1;
		req.CombOffsetFlag[1] = orderType2;
		if(orderType1 == THOST_FTDC_OF_ForceClose)
		{
			///强平原因: 保证金不足
			req.ForceCloseReason = THOST_FTDC_FCC_LackDeposit;
			///用户强评标志: 是
			req.UserForceClose = 1;
		}
		else
		{
			///强平原因: 非强平
			req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
			///用户强评标志: 否
			req.UserForceClose = 0;
		}
	}
	else
	{
		printf("****** Illegal Order Type ******\n");
		returnCode = -1;
		return returnCode;
	}
	
	
	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Arbitrage; //套利
	req.CombHedgeFlag[1] = THOST_FTDC_HF_Arbitrage; //套利
	
	///数量
	req.VolumeTotalOriginal = volume;
	
	///GTD日期
//	TThostFtdcDateType	GTDDate;
	///成交量类型: 任何数量
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
	
	printf("<--- Request order insert, request ID: %d\n", ++iReqID);
	printf("<------ Order reference: %d\n", orderRef);
	returnCode = pUserApi->ReqOrderInsert(&req, iReqID);
	// update ORDER_INDEX
	TThostFtdcOrderRefType tempOrderRef;
	sprintf(tempOrderRef, "%d", orderRef);
	orderIndex->orderRef = "";
	orderIndex->orderSysId = "";
	orderIndex->exchangeId = "";
	orderIndex->orderRef.append(tempOrderRef, sizeof(TThostFtdcOrderRefType));
	orderIndex->frontId = frontId;
	orderIndex->sessionId = sessionId;
	if(returnCode != 0)
	{
		printf("****** ReqOrderInsert return error ******\n");
	}
	return returnCode;
}

//报单录入响应，据说只有在报单错误时才会被调用
void TradeProcess::OnRspOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	printf("---> Request order insert response, request ID: %d\n", nRequestID);
	bool errorResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if(errorResult)
	{
		printf("------> ErrorID = %d, ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
	else
	{
		printf("------> Request order insert success!\n");
		//是否本次响应的最后一次回调
		if(bIsLast)
		{
			printf("------> Request %d over\n", nRequestID);
		}
	}
}
//报单错误响应
void TradeProcess::OnErrRtnOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo) 
{
	printf("---> Request order insert ERROR response\n");
	bool errorResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if(pRspInfo)
	{
		printf("------> ErrorID = %d, ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
	else
	{
		printf("------> Error info empty\n");
	}
}

//报单反馈回调函数，报单一次，会收到两次调用，第一次是CTP服务器的响应，第二次是交易所的响应
void TradeProcess::OnRtnOrder(CThostFtdcOrderField* pOrder)
{
	printf("---> Request order return\n");
	printf("------> Instrument ID: %s", pOrder->InstrumentID);
	printf("------> Order reference: %s\n", pOrder->OrderRef);
	printf("------> Order system ID: %s\n", pOrder->OrderSysID);
	strcpy(this->orderSysId, pOrder->OrderSysID);
	printf("------> Order detail ---> TYPE: %c%c, TIME: %s %s\n", 
		pOrder->Direction, 
		pOrder->OrderType,
		pOrder->InsertDate, 
		pOrder->InsertTime);
}

//成交反馈回调函数，在报单被成交后被调用
void TradeProcess::OnRtnTrade(CThostFtdcTradeField* pTrade)
{
	printf("---> Trade return\n");
	printf("------> Order reference: %s\n", pTrade->OrderRef);
	printf("------> Instrument ID: %s, Price: %f, Trade Type: %c, Direction: %c, Trade Time: %s, Trade Volume: %d\n", 
		pTrade->InstrumentID, 
		pTrade->Price, 
		pTrade->TradeType, 
		pTrade->Direction,
		pTrade->TradeTime,
		pTrade->Volume);
	
}

bool TradeProcess::InitializeFinished(void)
{
	return this->finishInitFlag;
}

void TradeProcess::ReqQryDepthMarketData(TThostFtdcInstrumentIDType instrument)
{
	CThostFtdcQryDepthMarketDataField orderMDInfo;
	strcpy(orderMDInfo.InstrumentID, instrument);
	printf("<--- Request qry depth market data, request ID: %d\n", ++iReqID);
	pUserApi->ReqQryDepthMarketData(&orderMDInfo, iReqID);
}

void TradeProcess::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) 
{
	printf("---> Request qry depth market data response, request ID: %d\n", nRequestID);
	bool errorResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if(errorResult)
	{
		printf("------> ErrorID = %d, ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
	else
	{
		printf("------> Request qry depth market data success!\n");
	}
}

int TradeProcess::ReqQryOrder(const TThostFtdcInstrumentIDType instrument)
{
	CThostFtdcQryOrderField qryOrder;
	++iReqID;
	//printf("<--- Request qry order, request ID: %d\n", iReqID);
	strcpy(qryOrder.BrokerID, basicTradeProcessData.brokerId);
	strcpy(qryOrder.InvestorID, basicTradeProcessData.investorId);
	strcpy(qryOrder.OrderSysID, this->orderSysId);
	strcpy(qryOrder.InstrumentID, instrument);
	strcpy(qryOrder.InsertTimeStart, "00:00:00");
	strcpy(qryOrder.InsertTimeEnd, "23:59:59");
	pUserApi->ReqQryOrder(&qryOrder, iReqID);
	return iReqID;
}

void TradeProcess::OnRspQryOrder(CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) 
{
	printf("---> Request qry order response, request ID: %d\n", nRequestID);
	bool errorResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if(errorResult)
	{
		printf("------> ErrorID = %d, ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
	else
	{
		printf("------> Request qry order response success!\n");
	}
	if(pOrder)
	{
		printf("------> Order Info ---> INSTRUMENT_NAME: %s, INSERT_TIME: %s %s, ORDER_REF: %s, SUBMIT_STATUS: %c\n", 
			pOrder->InstrumentID,
			pOrder->InsertDate,
			pOrder->InsertTime,
			pOrder->OrderRef,
			pOrder->OrderSubmitStatus);
		printf("------> Order status: ");
		switch(pOrder->OrderStatus)
		{
		case THOST_FTDC_OST_AllTraded:
			printf("all treaded\n");
			break;
		case THOST_FTDC_OST_PartTradedQueueing:
			printf("part queuing\n");
			break;
		case THOST_FTDC_OST_PartTradedNotQueueing:
			printf("part not queuing\n");
			break;
		case THOST_FTDC_OST_NoTradeQueueing:
			printf("not traded queuing\n");
			break;
		case THOST_FTDC_OST_NoTradeNotQueueing:
			printf("not traded not queuing\n");
			break;
		case THOST_FTDC_OST_Canceled:
			printf("canceled\n");
			break;
		case THOST_FTDC_OST_Unknown:
			printf("unkonwn\n");
			break;
		case THOST_FTDC_OST_NotTouched:
			printf("not touched\n");
			break;
		case THOST_FTDC_OST_Touched:
			printf("touched\n");
			break;
		default:
			break;
		}
	}
	else
	{
		printf("------> No order record\n");
	}
	//是否本次响应的最后一次回调
	if(bIsLast)
	{
		printf("------> Request %d over\n", nRequestID);
	}
}

int TradeProcess::ReqOrderAction(TThostFtdcInstrumentIDType instrument, TThostFtdcOrderRefType orderRef)
{
	int returnCode = -1;
	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, basicTradeProcessData.brokerId);
	///投资者代码
	strcpy(req.InvestorID, basicTradeProcessData.investorId);
	///报单操作引用
//	TThostFtdcOrderActionRefType	OrderActionRef;
	///报单引用
	if(NULL == orderRef)
	{
		printf("****** Order Ref Empty ******\n");
		return returnCode;
	}
	else
	{
		strcpy(req.OrderRef, orderRef);
	}
	///请求编号
//	TThostFtdcRequestIDType	RequestID;
	///前置编号
	req.FrontID = this->frontId;
	///会话编号
	req.SessionID = this->sessionId;
	///交易所代码
//	TThostFtdcExchangeIDType	ExchangeID;
	///报单编号
//	TThostFtdcOrderSysIDType	OrderSysID;
	///操作标志
	req.ActionFlag = THOST_FTDC_AF_Delete;
	///价格
//	TThostFtdcPriceType	LimitPrice;
	///数量变化
//	TThostFtdcVolumeType	VolumeChange;
	///用户代码
//	TThostFtdcUserIDType	UserID;
	///合约代码
	if(NULL == instrument)
	{
		printf("****** Instrument ID Empty ******\n");
		return returnCode;
	}
	strcpy(req.InstrumentID, instrument);

	returnCode = pUserApi->ReqOrderAction(&req, ++iReqID);
	return returnCode;
}

int TradeProcess::ReqOrderAction(	TThostFtdcInstrumentIDType instrument, 
									TThostFtdcOrderRefType orderRef, 
									TThostFtdcPriceType price, 
									TThostFtdcVolumeType volumeChange)
{
	int returnCode = -1;
	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, basicTradeProcessData.brokerId);
	///投资者代码
	strcpy(req.InvestorID, basicTradeProcessData.investorId);
	///报单操作引用
//	TThostFtdcOrderActionRefType	OrderActionRef;
	///报单引用
	if(NULL == orderRef)
	{
		printf("****** Order Ref Empty ******\n");
		return returnCode;
	}
	else
	{
		strcpy(req.OrderRef, orderRef);
	}
	///请求编号
//	TThostFtdcRequestIDType	RequestID;
	///前置编号
	req.FrontID = this->frontId;
	///会话编号
	req.SessionID = this->sessionId;
	///交易所代码
//	TThostFtdcExchangeIDType	ExchangeID;
	///报单编号
//	TThostFtdcOrderSysIDType	OrderSysID;
	///操作标志
	req.ActionFlag = THOST_FTDC_AF_Modify;
	///价格
	req.LimitPrice = price;
	///数量变化
	req.VolumeChange = volumeChange;
	///用户代码
//	TThostFtdcUserIDType	UserID;
	///合约代码
	if(NULL == instrument)
	{
		printf("****** Instrument ID Empty ******\n");
		return returnCode;
	}
	strcpy(req.InstrumentID, instrument);

	returnCode = pUserApi->ReqOrderAction(&req, ++iReqID);
	return returnCode;
}

void TradeProcess::OnRspOrderAction(CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	printf("---> Request order action response, Request ID: %d\n", nRequestID);
	bool errorResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if(errorResult)
	{
		printf("------> ErrorID = %d, ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
	else
	{
		printf("------> Request order action response success!\n");
	}
	if(pInputOrderAction)
	{
		printf("------> Order Action Flag: [%c], Instrumeng ID: [%s], Order Ref: [%s]",	
			pInputOrderAction->ActionFlag, 
			pInputOrderAction->InstrumentID, 
			pInputOrderAction->OrderRef);
	}

}

void TradeProcess::OnErrRtnOrderAction(CThostFtdcOrderActionField* pOrderAction, CThostFtdcRspInfoField* pRspInfo)
{
	printf("---> Request order action error\n");
	bool errorResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if(pRspInfo == NULL)
	{
		return;
	}
	else if(pRspInfo->ErrorMsg == NULL)
	{
		return;
	}
	else
	{
		printf("------> ErrorID = %d, ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
}

int TradeProcess::ReqQryInvestorPosition(const TThostFtdcInstrumentIDType instrument)
{
	CThostFtdcQryInvestorPositionField qryPosition;
	memset(&qryPosition, 0, sizeof(qryPosition));
	strcpy(qryPosition.BrokerID, basicTradeProcessData.brokerId);
	strcpy(qryPosition.InvestorID, basicTradeProcessData.investorId);
	strcpy(qryPosition.InstrumentID, instrument);
	//printf("<--- Request qry investor position, request ID: %d\n", ++iReqID);
	return pUserApi->ReqQryInvestorPosition(&qryPosition, iReqID);
}

void TradeProcess::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if(pRspInfo != NULL)
	{
		bool errorResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
		if(errorResult)
		{
			printf("------> ErrorID = %d, ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		}
	}
	if(pInvestorPosition != NULL)
	{
		cout<<"---> Request qry investor position response, request ID: "<<nRequestID<<endl;
		if(pInvestorPosition->InstrumentID != NULL)
		{
			cout<<"------> Instrument: "<<pInvestorPosition->InstrumentID<<endl;
		}
		cout<<"------> Today Position: "<<pInvestorPosition->TodayPosition<<endl;
		cout<<"------> Yesterday Position: "<<pInvestorPosition->YdPosition<<endl;
		cout<<"------> Position Direction ( 2 for buy, 3 for sell ): "<<pInvestorPosition->PosiDirection<<endl;//2多  3空
		cout<<"------> Position Profit: "<<pInvestorPosition->PositionProfit<<endl;
	}
}

int TradeProcess::ReqQryInstrument(void)
{
	mInstrumentListFinished = false;
	if(!mInstrumentList.empty())
	{
		mInstrumentList.clear();
	}
	CThostFtdcQryInstrumentField qryInstrument;
	memset(&qryInstrument, 0, sizeof(qryInstrument));
	printf("<--- Request qry instrument, request ID: %d\n", ++iReqID);
	return pUserApi->ReqQryInstrument(&qryInstrument, iReqID);
}

void TradeProcess::OnRspQryInstrument(CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	cout<<"---> Request qry instrument response, request ID: "<<nRequestID<<endl;
	if(NULL != pInstrument)
	{
		mInstrumentList.push_back(pInstrument->InstrumentID);
		cout<<"------> Instrument ID: "<<pInstrument->InstrumentID<<endl;
		//logger.LogThisFastNoTimeStamp(pInstrument->InstrumentID);
	}
	if(bIsLast)
	{
		mInstrumentListFinished = true;
		cout<<"Request "<<nRequestID<<" over"<<endl;
	}
}

vector<string> TradeProcess::GetInstrumentList(void)
{
	if(mInstrumentListFinished)
	{
		return mInstrumentList;
	}
	else
	{
		return (vector<string>)NULL;
	}
}

bool TradeProcess::InstrumentListReady(void)
{
	return mInstrumentListFinished;
}