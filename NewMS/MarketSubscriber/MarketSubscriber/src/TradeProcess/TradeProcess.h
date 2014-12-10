#ifndef TRADEPROCESS_H_INCLUDED
#define TRADEPROCESS_H_INCLUDED
//#define BOOST_DATE_TIME_SOURCE
#include "stddef.h"
#include <ThostFtdcTraderApi.h>
#include <ThostFtdcUserApiDataType.h>
#include <ThostFtdcUserApiStruct.h>
#include <DataTypes.h>
#include <boost/date_time/posix_time/posix_time.hpp> 
#include <CommonLog.h>
#include <vector>
#define MAX_FRONT 6
#define FRONT_ADDR_SIZE 128
using namespace boost::posix_time;
class TradeProcess : public CThostFtdcTraderSpi
{
private:
	ptime startTime;
	ptime endTime;
	int orderRef;
	bool finishInitFlag;
	bool connectedFlag;
	TThostFtdcFrontIDType frontId;
	TThostFtdcSessionIDType sessionId;
	TThostFtdcOrderSysIDType orderSysId;
	vector<string> mInstrumentList;	
	bool mInstrumentListFinished;
	//Log logger;
protected:
	CommonLog logger;
public:
    int iReqID;
    CThostFtdcTraderApi* pUserApi;
	//Basic data used in initialization
	struct BASIC_TRADE_PROCESS_DATA
	{
		TThostFtdcAgentBrokerIDType brokerId;
		TThostFtdcInvestorIDType investorId;
		TThostFtdcPasswordType investorPassword;
		char* frontAddress[MAX_FRONT];//Max front address limitation
		int numFrontAddress;
	}basicTradeProcessData;
	typedef enum OrderType
	{
		mBuy,
		mSell,
		mSellShort,
		mBuyToCover
	};
	
public:
	/** Default constructor */
    TradeProcess();
    /** Default destructor */
    ~TradeProcess();
	//the first method to invoke
	void InitializeProcess(void);
	// Check if the initialize was finished
	bool InitializeFinished(void);
	bool Buy(string aInstrument, double aPrice, int aVolume, ORDER_INDEX_TYPE* aOrderIndex);
	bool Sell(string aInstrument, double aPrice, int aVolume, ORDER_INDEX_TYPE* aOrderIndex, bool aYDay=false);
	bool SellShort(string aInstrument, double aPrice, int aVolume, ORDER_INDEX_TYPE* aOrderIndex);
	bool BuyToCover(string aInstrument, double aPrice, int aVolume, ORDER_INDEX_TYPE* aOrderIndex, bool aYDay=false);
	bool OrderInsert(string aInstrument, double aPrice, int aVolume, OrderType aOrderType, ORDER_INDEX_TYPE* aOrderIndex, bool aYDay=false);
	// 基本报单接口
	int ReqOrderInsert(	TThostFtdcInstrumentIDType instrument, 
						TThostFtdcPriceType price, 
						TThostFtdcVolumeType volume, 
						TThostFtdcDirectionType orderDirection, 
						TThostFtdcOffsetFlagType orderType,
						TThostFtdcHedgeFlagType combHedgeFlag = THOST_FTDC_HF_Speculation);
	// basic order insert interface, with order reference returned
	int ReqOrderInsert(	TThostFtdcInstrumentIDType instrument,								// in: instrument ID
						TThostFtdcPriceType price,											// in: price
						TThostFtdcVolumeType volume,										// in: volume
						TThostFtdcDirectionType orderDirection,								// in: order direction
						TThostFtdcOffsetFlagType orderType,									// in: order type, open or close or...
						ORDER_INDEX_TYPE* orderIndex,										// out: order index
						TThostFtdcHedgeFlagType combHedgeFlag = THOST_FTDC_HF_Speculation);	// in: speculation(default) or arbitrage		
	// 简单报单接口，根据订单类型报单，不过貌似还不支持
	int ReqOrderInsertSimple(	TThostFtdcInstrumentIDType instrument, 
								TThostFtdcOrderPriceTypeType priceType, 
								TThostFtdcVolumeType volume, 
								TThostFtdcDirectionType orderDirection, 
								TThostFtdcOffsetFlagType orderType,
								TThostFtdcHedgeFlagType combHedgeFlag = THOST_FTDC_HF_Speculation);
	// 套利单接口
	int ReqOrderInsertArbitrage(TThostFtdcInstrumentIDType instrument1,		// in: instrument 1 ID
								TThostFtdcInstrumentIDType instrument2,		// in: instrument 2 ID
								TThostFtdcPriceType deltaPrice,				// in: price difference
								TThostFtdcVolumeType volume,				// in: volume
								TThostFtdcDirectionType orderDirection1,	// in: instrument 1 direction
								TThostFtdcOffsetFlagType orderType1,		// in: instrument 1 type, open or close or...
								TThostFtdcOffsetFlagType orderType2,		// in: instrument 2 type, open or close or...
								ORDER_INDEX_TYPE* orderIndex);				// out: order index
	// subscribe market data
	void ReqQryDepthMarketData(TThostFtdcInstrumentIDType instrument);
	// 查询报单
	int ReqQryOrder(const TThostFtdcInstrumentIDType instrument);
	// 撤单接口，根据下单编号进行撤单 
	int ReqOrderAction(TThostFtdcInstrumentIDType instrument, TThostFtdcOrderRefType orderRef);
	// 撤单接口，根据订单记录来撤单
	bool CancelOrder(const ORDER_INDEX_TYPE* aOrderIndex);
	// change the price or value or the order, not supported by CTP yet...
	int ReqOrderAction(	TThostFtdcInstrumentIDType instrument, 
						TThostFtdcOrderRefType orderRef, 
						TThostFtdcPriceType price, 
						TThostFtdcVolumeType volumeChange);
	
	// 查询投资者持仓
	int ReqQryInvestorPosition(const TThostFtdcInstrumentIDType instrument);
	// 查询合约
	int ReqQryInstrument(void);
	// 获取合约列表
	vector<string> GetInstrumentList(void);
	// 合约查询是否已经完成
	bool InstrumentListReady(void);
private:
	void ReqConnect(void);
	void ReqLogin(void);
	// Confirm the settlement information
	void ReqSettlementInfoConfirm(void);

public:
    ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    virtual void OnFrontConnected() ;
    ///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    ///@param nReason 错误原因
    ///        0x1001 网络读失败
    ///        0x1002 网络写失败
    ///        0x2001 接收心跳超时
    ///        0x2002 发送心跳失败
    ///        0x2003 收到错误报文
    virtual void OnFrontDisconnected(int nReason) ;
    ///心跳超时警告。当长时间未收到报文时，该方法被调用。
    ///@param nTimeLapse 距离上次接收报文的时间
    virtual void OnHeartBeatWarning(int nTimeLapse) ;
    ///客户端认证响应
    virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;
    ///登录请求响应
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;
	///投资者结算结果确认响应
    virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;
	///请求查询结算信息确认响应
    virtual void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;
	///请求查询投资者结算结果响应
    //virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField* pSettlementInfo, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;
	///报单录入请求响应
    virtual void OnRspOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;
	///报单录入错误回报
    virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo) ;
    ///报单通知，同时也是撤单的响应
    virtual void OnRtnOrder(CThostFtdcOrderField* pOrder) ;
    ///成交通知
    virtual void OnRtnTrade(CThostFtdcTradeField* pTrade) ;
	///请求查询行情响应
    virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;
	 ///请求查询报单响应
    virtual void OnRspQryOrder(CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;
	///报单操作请求响应
    virtual void OnRspOrderAction(CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;
	///报单操作错误回报
    virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField* pOrderAction, CThostFtdcRspInfoField* pRspInfo) ;
	///请求查询投资者持仓响应
    virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;
	///请求查询合约响应
    virtual void OnRspQryInstrument(CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

#if 0
    ///登出请求响应
    virtual void OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///用户口令更新请求响应
    virtual void OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField* pUserPasswordUpdate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///资金账户口令更新请求响应
    virtual void OnRspTradingAccountPasswordUpdate(CThostFtdcTradingAccountPasswordUpdateField* pTradingAccountPasswordUpdate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///预埋单录入请求响应
    virtual void OnRspParkedOrderInsert(CThostFtdcParkedOrderField* pParkedOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///预埋撤单录入请求响应
    virtual void OnRspParkedOrderAction(CThostFtdcParkedOrderActionField* pParkedOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///查询最大报单数量响应
    virtual void OnRspQueryMaxOrderVolume(CThostFtdcQueryMaxOrderVolumeField* pQueryMaxOrderVolume, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///删除预埋单响应
    virtual void OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField* pRemoveParkedOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///删除预埋撤单响应
    virtual void OnRspRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField* pRemoveParkedOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

	
    ///请求查询成交响应
    virtual void OnRspQryTrade(CThostFtdcTradeField* pTrade, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    

    ///请求查询资金账户响应
    virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField* pTradingAccount, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询投资者响应
    virtual void OnRspQryInvestor(CThostFtdcInvestorField* pInvestor, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询交易编码响应
    virtual void OnRspQryTradingCode(CThostFtdcTradingCodeField* pTradingCode, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询合约保证金率响应
    virtual void OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField* pInstrumentMarginRate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询合约手续费率响应
    virtual void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField* pInstrumentCommissionRate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询交易所响应
    virtual void OnRspQryExchange(CThostFtdcExchangeField* pExchange, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    
    
    ///请求查询转帐银行响应
    virtual void OnRspQryTransferBank(CThostFtdcTransferBankField* pTransferBank, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询投资者持仓明细响应
    virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField* pInvestorPositionDetail, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询客户通知响应
    virtual void OnRspQryNotice(CThostFtdcNoticeField* pNotice, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

   ///请求查询投资者持仓明细响应
    virtual void OnRspQryInvestorPositionCombineDetail(CThostFtdcInvestorPositionCombineDetailField* pInvestorPositionCombineDetail, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///查询保证金监管系统经纪公司资金账户密钥响应
    virtual void OnRspQryCFMMCTradingAccountKey(CThostFtdcCFMMCTradingAccountKeyField* pCFMMCTradingAccountKey, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询仓单折抵信息响应
    virtual void OnRspQryEWarrantOffset(CThostFtdcEWarrantOffsetField* pEWarrantOffset, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询转帐流水响应
    virtual void OnRspQryTransferSerial(CThostFtdcTransferSerialField* pTransferSerial, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询银期签约关系响应
    virtual void OnRspQryAccountregister(CThostFtdcAccountregisterField* pAccountregister, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///错误应答
    virtual void OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;



    
    

    ///合约交易状态通知
    virtual void OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField* pInstrumentStatus) ;

    ///交易通知
    virtual void OnRtnTradingNotice(CThostFtdcTradingNoticeInfoField* pTradingNoticeInfo) ;

    ///提示条件单校验错误
    virtual void OnRtnErrorConditionalOrder(CThostFtdcErrorConditionalOrderField* pErrorConditionalOrder) ;

    ///请求查询签约银行响应
    virtual void OnRspQryContractBank(CThostFtdcContractBankField* pContractBank, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询预埋单响应
    virtual void OnRspQryParkedOrder(CThostFtdcParkedOrderField* pParkedOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询预埋撤单响应
    virtual void OnRspQryParkedOrderAction(CThostFtdcParkedOrderActionField* pParkedOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询交易通知响应
    virtual void OnRspQryTradingNotice(CThostFtdcTradingNoticeField* pTradingNotice, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询经纪公司交易参数响应
    virtual void OnRspQryBrokerTradingParams(CThostFtdcBrokerTradingParamsField* pBrokerTradingParams, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///请求查询经纪公司交易算法响应
    virtual void OnRspQryBrokerTradingAlgos(CThostFtdcBrokerTradingAlgosField* pBrokerTradingAlgos, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///银行发起银行资金转期货通知
    virtual void OnRtnFromBankToFutureByBank(CThostFtdcRspTransferField* pRspTransfer) ;

    ///银行发起期货资金转银行通知
    virtual void OnRtnFromFutureToBankByBank(CThostFtdcRspTransferField* pRspTransfer) ;

    ///银行发起冲正银行转期货通知
    virtual void OnRtnRepealFromBankToFutureByBank(CThostFtdcRspRepealField* pRspRepeal) ;

    ///银行发起冲正期货转银行通知
    virtual void OnRtnRepealFromFutureToBankByBank(CThostFtdcRspRepealField* pRspRepeal) ;

    ///期货发起银行资金转期货通知
    virtual void OnRtnFromBankToFutureByFuture(CThostFtdcRspTransferField* pRspTransfer) ;

    ///期货发起期货资金转银行通知
    virtual void OnRtnFromFutureToBankByFuture(CThostFtdcRspTransferField* pRspTransfer) ;

    ///系统运行时期货端手工发起冲正银行转期货请求，银行处理完毕后报盘发回的通知
    virtual void OnRtnRepealFromBankToFutureByFutureManual(CThostFtdcRspRepealField* pRspRepeal) ;

    ///系统运行时期货端手工发起冲正期货转银行请求，银行处理完毕后报盘发回的通知
    virtual void OnRtnRepealFromFutureToBankByFutureManual(CThostFtdcRspRepealField* pRspRepeal) ;

    ///期货发起查询银行余额通知
    virtual void OnRtnQueryBankBalanceByFuture(CThostFtdcNotifyQueryAccountField* pNotifyQueryAccount) ;

    ///期货发起银行资金转期货错误回报
    virtual void OnErrRtnBankToFutureByFuture(CThostFtdcReqTransferField* pReqTransfer, CThostFtdcRspInfoField* pRspInfo) ;

    ///期货发起期货资金转银行错误回报
    virtual void OnErrRtnFutureToBankByFuture(CThostFtdcReqTransferField* pReqTransfer, CThostFtdcRspInfoField* pRspInfo) ;

    ///系统运行时期货端手工发起冲正银行转期货错误回报
    virtual void OnErrRtnRepealBankToFutureByFutureManual(CThostFtdcReqRepealField* pReqRepeal, CThostFtdcRspInfoField* pRspInfo) ;

    ///系统运行时期货端手工发起冲正期货转银行错误回报
    virtual void OnErrRtnRepealFutureToBankByFutureManual(CThostFtdcReqRepealField* pReqRepeal, CThostFtdcRspInfoField* pRspInfo) ;

    ///期货发起查询银行余额错误回报
    virtual void OnErrRtnQueryBankBalanceByFuture(CThostFtdcReqQueryAccountField* pReqQueryAccount, CThostFtdcRspInfoField* pRspInfo) ;

    ///期货发起冲正银行转期货请求，银行处理完毕后报盘发回的通知
    virtual void OnRtnRepealFromBankToFutureByFuture(CThostFtdcRspRepealField* pRspRepeal) ;

    ///期货发起冲正期货转银行请求，银行处理完毕后报盘发回的通知
    virtual void OnRtnRepealFromFutureToBankByFuture(CThostFtdcRspRepealField* pRspRepeal) ;

    ///期货发起银行资金转期货应答
    virtual void OnRspFromBankToFutureByFuture(CThostFtdcReqTransferField* pReqTransfer, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///期货发起期货资金转银行应答
    virtual void OnRspFromFutureToBankByFuture(CThostFtdcReqTransferField* pReqTransfer, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///期货发起查询银行余额应答
    virtual void OnRspQueryBankAccountMoneyByFuture(CThostFtdcReqQueryAccountField* pReqQueryAccount, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) ;

    ///银行发起银期开户通知
    virtual void OnRtnOpenAccountByBank(CThostFtdcOpenAccountField* pOpenAccount) ;

    ///银行发起银期销户通知
    virtual void OnRtnCancelAccountByBank(CThostFtdcCancelAccountField* pCancelAccount) ;

    ///银行发起变更银行账号通知
    virtual void OnRtnChangeAccountByBank(CThostFtdcChangeAccountField* pChangeAccount) ;
#endif


private:

    bool IsRspError(CThostFtdcRspInfoField* pRspInfo);
};
#endif // TRADEPROCESS_H_INCLUDED
