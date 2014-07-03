#include "stdafx.h"
#include "MarketProcess.h"
#include <iostream>
#include <boost\thread.hpp>

using namespace boost::posix_time;
MarketProcess::MarketProcess(void)
{
	unreasonableBig = 10000000000.0;
	overflowCheck = OVERFLOW_CHECK_CODE;
	logHandlerIndex = 0;
	pUserApi = NULL;
	dataPushHook = NULL;
	hookObj = NULL;
	frontServerReached = false;
	logInSuccess = false;
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi("MarketData", false);
    pUserApi->RegisterSpi(this);
	for(int i=0; i<MAX_FRONT; i++)
	{
		frontAddress[i] = new char[FRONT_ADDR_SIZE];
	}
	for(int i=0; i<MAX_INSTRUMENT; i++)
	{
		instrumentList[i] = new char[INSTRUMENT_SIZE];
	}
	ConfigReader config;
	config.ReadInteger(saveMarketData, "SaveMarketData");
}


MarketProcess::~MarketProcess(void)
{
	for(int i=0; i<logHandlerIndex; i++)
	{
		delete logHandlerPool[i];
	}
	for(int i=0; i<MAX_FRONT; i++)
	{
		delete frontAddress[i];
	}
	for(int i=0; i<MAX_INSTRUMENT; i++)
	{
		delete instrumentList[i];
	}
	if(pUserApi)
	{
		pUserApi->Release();
	}
}


void MarketProcess::StartMarketProcess(void)
{
	printf("########################################################\n");
	printf("#         Start Market Process Initialization          #\n");
	printf("########################################################\n");
	ReqConnect();
}
void MarketProcess::StopMarketProcess(void)
{
	CThostFtdcUserLogoutField logOutField;
	memset(&logOutField, 0, sizeof(logOutField));
	strcpy((char*)&logOutField.BrokerID, broker);
	strcpy((char*)&logOutField.UserID, investor);
	pUserApi->ReqUserLogout(&logOutField, ++iReqID);
}
void MarketProcess::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(pRspInfo->ErrorID != 0)
    {
        printf("\nLog out failed, error msg: %s.\n", pRspInfo->ErrorMsg);
		printf("Error ID: %d.\n", pRspInfo->ErrorID);
    }
    else
    {
		printf("\nLog out successfully.\n");
		pUserApi->Release();
	}
}
void MarketProcess::SubscribeInstrument(void)
{
if(1 == saveMarketData)
{
	for(int i=0; i<numInstrument; i++)
	{
		if(instrumentList[i] != NULL)
		{
			// create and insert log handler into log handler pool
			InsertLogHandler(instrumentList[i]);
			CreateMarketDataFile(instrumentList[i]);
			cout<<"Instrument Map: "<<instrumentList[i]<<"["<<logInstMap.at(instrumentList[i])<<"]"<<endl;
		}
	}
}
	printf("\nsubscribe market data return: %d\n", pUserApi->SubscribeMarketData(instrumentList, numInstrument));
}
void MarketProcess::SubscribeInstrument(char* instrumentID[], int numInstrument)
{
	if(numInstrument<1)
	{
		printf("\ndidn't get instrument IDs, quiting program\n");
		return;
	}
	char** instrumentIDFit = (char**)calloc(numInstrument,sizeof(int));
	memcpy(instrumentIDFit, instrumentID, numInstrument*sizeof(int));
	if(1 == saveMarketData)
	{
		for(int i=0; i<numInstrument; i++)
		{
			if(instrumentList[i] != NULL)
			{
				// create and insert log handler into log handler pool
				InsertLogHandler(instrumentList[i]);
				CreateMarketDataFile(instrumentList[i]);
			}
		
		}
	}
	printf("\nsubscribe market data return: %d\n", pUserApi->SubscribeMarketData(instrumentIDFit, numInstrument));
	if(instrumentIDFit)
	{
		free(instrumentIDFit);
		instrumentIDFit = NULL;
	}
}
void MarketProcess::ReqConnect(void)
{
	startReqTime = boost::posix_time::microsec_clock::local_time();
	for(int i=0; i<numFrontAddress; i++)
	{
		pUserApi->RegisterFront(frontAddress[i]);
	}
    pUserApi->Init();
}
//When connection restored, this method will be invoked again
void MarketProcess::OnFrontConnected(void)
{
	frontServerReached = true;
	getRspTime = boost::posix_time::microsec_clock::local_time();
	std::cout<<"\nFront server reached, delay= "<<getRspTime - startReqTime<<", now logging in..."<<std::endl;
	logger.LogThisFast("[EVENT]: Front server connected");
	ReqLogIn();
}

void MarketProcess::OnFrontDisconnected(void)
{
	frontServerReached = false;
	printf("\nlost connection with front server\n");
}

void MarketProcess::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	
	if(pRspInfo->ErrorID != 0)
	{
		printf("\nsubscribe instrument error, error msg: %s\n", pRspInfo->ErrorMsg);
		printf("error ID: %d\n", pRspInfo->ErrorID);
	}
	else
	{
		printf("\nsubscribe instrument successful, instrument ID: %s\n", pSpecificInstrument->InstrumentID);
	}
}

void MarketProcess::OnHeartBeatWarning(int nTimeLapse)
{
	cerr<<"[LOCAL ERROR]: Heart beat over time: "<<nTimeLapse<<endl;
	logger.LogThisFast("[LOCAL ERROR]: Heart beat over time");
}
void MarketProcess::ReqLogIn(void)
{
	CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, broker);
    strcpy(req.UserID, investor);
    strcpy(req.Password, pwd);
    pUserApi->ReqUserLogin(&req, ++iReqID);
}

void MarketProcess::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if(pRspInfo->ErrorID != 0)
    {
		logger.LogThisFast("[ERROR]: Log in failed: ", false);
		logger.LogThisFastNoTimeStamp(pRspInfo->ErrorMsg);
        printf("\nlog in failed, error msg: %s\n", pRspInfo->ErrorMsg);
		printf("error ID: %d\n", pRspInfo->ErrorID);
    }
    else
    {
		printf("\nlog in successful, now subscribe instruments...\n");
		logger.LogThisFast("[EVENT]: Log in successful");
		logInSuccess = true;
		//Market process initialization accomplished at this point
		printf("########################################################\n");
		printf("#      Market Process Initialization Accomplished      #\n");
		printf("########################################################\n");
		SubscribeInstrument();
    }
}

void MarketProcess::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	printf("\nsomething wrong\n");
}

void MarketProcess::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
	// push data via hook first
	if(dataPushHook)
	{
		dataPushHook(pDepthMarketData);
	}
	if(hookObj)
	{
		hookObj->HookOnRtnDepthMarketData(pDepthMarketData);
	}
	if(1 == saveMarketData)
	{
		RecordMarketData(pDepthMarketData);
	}
}

void MarketProcess::RecordMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
	sprintf(marketDataBuf,   "%s	" //TradingDay
			        "%s	" //InstrumentID
					//"%s	" //ExchangeID
					//"%s	" //ExchangeInstID
					"%.2f	" //LastPrice
					"%.2f	" //PreSettlementPrice
					"%.2f	" //PreClosePrice
					"%.2f	" //PreOpenInterest
					"%.2f	" //OpenPrice
					"%.2f	" //HighestPrice
					"%.2f	" //LowestPrice
					"%d	" //Volume
					//"%.2f	" //Turnover
					"%.2f	" //OpenInterest
					//"%.2f	" //ClosePrice
					//"%.2f	" //SettlementPrice
					"%.2f	" //UpperLimitPrice
					"%.2f	" //LowerLimitPrice
					//"%.2f	" //PreDelta
					//"%.2f	" //CurrDelta
					"%s	" //UpdateTime
					"%d	" //UpdateMillisec
					"%.2f	" //BidPrice1
					"%d	" //BidVolume1
					"%.2f	" //AskPrice1
					"%d	" //AskVolume1
					//"%f	" //BidPrice2
					//"%d	" //BidVolume2
					//"%f	" //AskPrice2
					//"%d	" //AskVolume2
					//"%f	" //BidPrice3
					//"%d	" //BidVolume3
					//"%f	" //AskPrice3
					//"%d	" //AskVolume3
					//"%f	" //BidPrice4
					//"%d	" //BidVolume4
					//"%f	" //AskPrice4
					//"%d	" //AskVolume4
					//"%f	" //BidPrice5
					//"%d	" //BidVolume5
					//"%f	" //AskPrice5
					//"%d	" //AskVolume5
					//"%f	" //AveragePrice
					,
					pDepthMarketData->TradingDay,
					pDepthMarketData->InstrumentID,
					//pDepthMarketData->ExchangeID,
					//pDepthMarketData->ExchangeInstID,
					pDepthMarketData->LastPrice > unreasonableBig? 0.0 : pDepthMarketData->LastPrice,
					pDepthMarketData->PreSettlementPrice > unreasonableBig? 0.0 : pDepthMarketData->PreSettlementPrice,
					pDepthMarketData->PreClosePrice > unreasonableBig? 0.0 : pDepthMarketData->PreClosePrice,
					pDepthMarketData->PreOpenInterest > unreasonableBig? 0.0 : pDepthMarketData->PreOpenInterest,
					pDepthMarketData->OpenPrice > unreasonableBig? 0.0 : pDepthMarketData->OpenPrice,
					pDepthMarketData->HighestPrice > unreasonableBig? 0.0 : pDepthMarketData->HighestPrice,
					pDepthMarketData->LowestPrice > unreasonableBig? 0.0 : pDepthMarketData->LowestPrice,
					pDepthMarketData->Volume,
					//pDepthMarketData->Turnover,
					pDepthMarketData->OpenInterest > unreasonableBig? 0.0 : pDepthMarketData->OpenInterest,
					//pDepthMarketData->ClosePrice,
					//pDepthMarketData->SettlementPrice,
					pDepthMarketData->UpperLimitPrice > unreasonableBig? 0.0 : pDepthMarketData->UpperLimitPrice,
					pDepthMarketData->LowerLimitPrice > unreasonableBig? 0.0 : pDepthMarketData->LowerLimitPrice,
					//pDepthMarketData->PreDelta,
					//pDepthMarketData->CurrDelta,
					pDepthMarketData->UpdateTime,
					pDepthMarketData->UpdateMillisec,
					pDepthMarketData->BidPrice1 > unreasonableBig? 0.0 : pDepthMarketData->BidPrice1,
					pDepthMarketData->BidVolume1,
					pDepthMarketData->AskPrice1 > unreasonableBig? 0.0 : pDepthMarketData->AskPrice1,
					pDepthMarketData->AskVolume1/*,
					pDepthMarketData->BidPrice2,
					pDepthMarketData->BidVolume2,
					pDepthMarketData->AskPrice2,
					pDepthMarketData->AskVolume2,
					pDepthMarketData->BidPrice3,
					pDepthMarketData->BidVolume3,
					pDepthMarketData->AskPrice3,
					pDepthMarketData->AskVolume3,
					pDepthMarketData->BidPrice4,
					pDepthMarketData->BidVolume4,
					pDepthMarketData->AskPrice4,
					pDepthMarketData->AskVolume4,
					pDepthMarketData->BidPrice5,
					pDepthMarketData->BidVolume5,
					pDepthMarketData->AskPrice5,
					pDepthMarketData->AskVolume5,
					pDepthMarketData->AveragePrice*/);
	if(OVERFLOW_CHECK_CODE != overflowCheck)
	{
		logger.LogThisFast("[LOCAL ERROR]: Temporary market data buffer overflowed!");
		cerr<<"[LOCAL ERROR]: Temporary market data buffer overflowed!"<<endl;
		overflowCheck = OVERFLOW_CHECK_CODE;
	}
	if(logInstMap.find(pDepthMarketData->InstrumentID) == logInstMap.end())
	{
		logger.LogThisFast("[LOCAL ERROR]: Don't have a record file for this instrument: ", false);
		logger.LogThisFastNoTimeStamp(pDepthMarketData->InstrumentID);
		cerr<<"[LOCAL ERROR]: Don't have a record file for this instrument: "<<pDepthMarketData->InstrumentID<<endl;
	}
	else
	{
		logHandlerPool[logInstMap.at(pDepthMarketData->InstrumentID)]->LogThisFast(marketDataBuf);
	}
	
}

bool MarketProcess::CreateMarketDataFile(char* instrumentID)
{
	bool status = true;
	
	char* data = new char[1000];
	sprintf(data, "LocalTime	"
				  "TradingDay	"
				  "InstrumentID	"
				  //"ExchangeID	"
				  //"ExchangeInstID "
				  "LastPrice	"
				  "PreSettlementPrice	"
				  "PreClosePrice	"
				  "PreOpenInterest	"
				  "OpenPrice	"
				  "HighestPrice	"
				  "LowestPrice	"
				  "Volume	"
				  //"Turnover	"
				  "OpenInterest	"
				  //"ClosePrice	"
				  //"SettlementPrice	"
				  "UpperLimitPrice	"
				  "LowerLimitPrice	"
				  //"PreDelta	"
				  //"CurrDelta	"
				  "UpdateTime	"
				  "UpdateMillisec	"
				  "BidPrice1	"
				  "BidVolume1	"
				  "AskPrice1	"
				  "AskVolume1	"
				  //"BidPrice2	"
				  //"BidVolume2	"
				  //"AskPrice2	"
				  //"AskVolume2	"
				  //"BidPrice3	"
				  //"BidVolume3	"
				  //"AskPrice3	"
				  //"AskVolume3	"
				  //"BidPrice4	"
				  //"BidVolume4	"
				  //"AskPrice4	"
				  //"AskVolume4	"
				  //"BidPrice5	"
				  //"BidVolume5	"
				  //"AskPrice5	"
				  //"AskVolume5	"
				  //"AveragePrice	"
				  );
	logHandlerPool[logInstMap.at(instrumentID)]->LogThisFastNoTimeStamp(data);
	if(data)
	{
		delete [] data;
		data = NULL;
	}
	return status;
}

void MarketProcess::SetInstrument(char* instrumentID[])
{
	int length = sizeof(instrumentID)/sizeof(int);

}

int MarketProcess::InsertLogHandler(string instrumentId)
{
	// only if the log handler number has not exceedes maximum number allowed
	if(logHandlerIndex<MAX_INSTRUMENT)
	{
		// only if this instrumentId has not been registered
		if(logInstMap.find(instrumentId) == logInstMap.end())
		{
			logInstMap[instrumentId] = logHandlerIndex;
			logHandlerIndex++;
			logHandlerPool[logInstMap.at(instrumentId)] = new Log(instrumentId);
		}
	}
	else
	{
		
		logger.LogThisFast("[LOCAL ERROR]: Subscribed too much instrument");
		cerr<<"[LOCAL ERROR]: Subscribed too much instrument"<<endl;
		return -1;
	}
	return 0;
}