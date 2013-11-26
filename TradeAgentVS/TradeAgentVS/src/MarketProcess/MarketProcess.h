#pragma once
#include "ThostFtdcMdApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#include <Hook.h>
#include <string.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/unordered_map.hpp>
#include <string>
#include <Log.h>
#include <ConfigReader.h>
#define MAX_FRONT 6
#define FRONT_ADDR_SIZE 128
#define MAX_INSTRUMENT 64
#define INSTRUMENT_SIZE 8
#define OVERFLOW_CHECK_CODE 0x5AA5
using namespace boost;
using namespace std;
class MarketProcess :
	public CThostFtdcMdSpi, public Hook
{
public:
	MarketProcess(void);
	~MarketProcess(void);
private:
	CThostFtdcMdApi* pUserApi;
	bool frontServerReached;
	bool logInSuccess;
	static const int tabMark = 9;
	int iReqID;
	Log logger;
	boost::posix_time::ptime startReqTime;
	boost::posix_time::ptime getRspTime;
	void (*dataPushHook)(CThostFtdcDepthMarketDataField* marketData);
	// unordered map, used to map instrument ID to its index in log handler pool
	typedef unordered_map<string, int> InstMapType;
	InstMapType logInstMap;
	// log handler pool
	Log* logHandlerPool[MAX_INSTRUMENT];
	// temp index
	int logHandlerIndex;
	// this value is used to determine if the data is illegal
	float unreasonableBig;
	// cache market data in string, so it is hard to tell its size, make it big enough to avoid overflow, important!!!
	char marketDataBuf[1024];
	// this is used to check if the array above overflowed, volatile is essential
	volatile int overflowCheck;
	// data save flag
	int saveMarketData;
public:
	TThostFtdcAgentBrokerIDType broker;
	TThostFtdcInvestorIDType investor;
	TThostFtdcPasswordType pwd;
	char* frontAddress[MAX_FRONT];
	int numFrontAddress;
	char* instrumentList[MAX_INSTRUMENT];
	int numInstrument;
	
	
private:
	void ReqConnect(void);
	void OnFrontConnected(void);
	void OnFrontDisconnected(void);
	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void ReqLogIn(void);
	void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData);
	void RecordMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData);
	bool CreateMarketDataFile(char* instrumentID);
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnHeartBeatWarning(int nTimeLapse);
	// insert a log handler into log handler pool based on instrument ID, 
	//other wise the market data of this instrument cannot be preserved
	int InsertLogHandler(string instrumengId);
public:
	void StartMarketProcess(void);
	void StopMarketProcess(void);
	void SetInstrument(char* instrumentID[]);
	void SubscribeInstrument(void);
	void SubscribeInstrument(char* instrumentID[], int numInstrument);
	//这里的hook只能是给到非对象函数，简单的来说就是：不是位于某一个类中的函数
	void SetDataPushHook(void (*inputHook)(CThostFtdcDepthMarketDataField* marketData))
	{
		dataPushHook = inputHook;
	}
	void UnsetDataPushHook(void)
	{
		dataPushHook = NULL;
	}

};