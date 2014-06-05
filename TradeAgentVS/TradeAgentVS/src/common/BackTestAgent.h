#pragma once
#include "DataReader.h"
#include "../Stratergy/PrimeryAndSecondary.h"
using namespace std;
class BackTestAgent
{
private:
	typedef enum
	{
		NEXT_PRIM,
		NEXT_SCND,
		NEXT_BOTH
	}NEXT_DATA_TYPE;
public:
	BackTestAgent()
	{
		strategy.StartStrategy();
		ResetData(&mPrimData);
		ResetData(&mScndData);
		ResetData(&mEmptyData);
	}
	void StartBackTest()
	{
		DataReader* primDataReader = new DataReader("data/MarketData/ag1412.dat");
		DataReader* scndDataReader = new DataReader("data/MarketData/ag1406.dat");
		while(1)
		{
			CThostFtdcDepthMarketDataField* tempData = NULL;
			if(IsDataReseted(&mPrimData))
			{
				if(primDataReader->GetMarketDataLine( &tempData ) == DataReader::READ_NO_ERROR)
				{
					if(tempData != NULL)
					{
						memcpy(&mPrimData, tempData, sizeof(CThostFtdcDepthMarketDataField));
					}
				}
				else
				{
					break;
				}
			}
			if(IsDataReseted(&mScndData))
			{
				if(scndDataReader->GetMarketDataLine( &tempData ) == DataReader::READ_NO_ERROR)
				{
					if(tempData != NULL)
					{
						memcpy(&mScndData, tempData, sizeof(CThostFtdcDepthMarketDataField));
					}
				}
				else
				{
					break;
				}
			}
			if(!IsDataReseted(&mPrimData) && !IsDataReseted(&mScndData))
			{
				int lNextData = WhoIsNext(&mPrimData, &mScndData);
				if(lNextData == NEXT_PRIM)
				{
					strategy.HookOnRtnDepthMarketData(&mPrimData);
					ResetData(&mPrimData);
				}
				else if(lNextData == NEXT_SCND)
				{
					strategy.HookOnRtnDepthMarketData(&mScndData);
					ResetData(&mScndData);
				}
				else if(lNextData == NEXT_BOTH)
				{
					strategy.HookOnRtnDepthMarketData(&mPrimData);
					ResetData(&mPrimData);
					strategy.HookOnRtnDepthMarketData(&mScndData);
					ResetData(&mScndData);
				}
			}
			
			//boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		}
		cout<<"simulation finished"<<endl;
	}
private:
	Pas::PrimeryAndSecondary strategy;
	CThostFtdcDepthMarketDataField mPrimData;
	CThostFtdcDepthMarketDataField mScndData;
	CThostFtdcDepthMarketDataField mEmptyData;
private:
	void ResetData(CThostFtdcDepthMarketDataField* aMarketData)
	{
		memset(aMarketData, 0, sizeof(CThostFtdcDepthMarketDataField));
	}
	bool IsDataReseted(CThostFtdcDepthMarketDataField* aMarketData)
	{
		if(memcmp(aMarketData, &mEmptyData, sizeof(CThostFtdcDepthMarketDataField))==0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	/************************************************************************/
	// 根据主力与次主力的时间戳来判断接下来应该输入哪一组数据。时间戳更早的
	// 数据应当被输入。如果时间戳相同，那么一起输入。时间戳包含TradingDay, 
	// UpdateTime以及UpdateMilliSec。
	/************************************************************************/
	int WhoIsNext(CThostFtdcDepthMarketDataField* aPrimData, CThostFtdcDepthMarketDataField* aScndData)
	{
		string lPrimDate = aPrimData->TradingDay;
		string lScndDate = aScndData->TradingDay;
		string lPrimTime;
		string lScndTime;
		double lPrimDateTime = 0;
		double lScndDateTime = 0;
		char lTime[7];
		lTime[0] = aPrimData->UpdateTime[0];
		lTime[1] = aPrimData->UpdateTime[1];
		lTime[2] = aPrimData->UpdateTime[3];
		lTime[3] = aPrimData->UpdateTime[4];
		lTime[4] = aPrimData->UpdateTime[6];
		lTime[5] = aPrimData->UpdateTime[7];
		lTime[6] = aPrimData->UpdateTime[8];
		lPrimTime = (char*)lTime;
		lTime[0] = aScndData->UpdateTime[0];
		lTime[1] = aScndData->UpdateTime[1];
		lTime[2] = aScndData->UpdateTime[3];
		lTime[3] = aScndData->UpdateTime[4];
		lTime[4] = aScndData->UpdateTime[6];
		lTime[5] = aScndData->UpdateTime[7];
		lTime[6] = aScndData->UpdateTime[8];
		lScndTime = (char*)lTime;
		string test = lPrimDate+lPrimTime;
		lPrimDateTime = atof((lPrimDate+lPrimTime).c_str())*1000+aPrimData->UpdateMillisec;
		lScndDateTime = atof((lScndDate+lScndTime).c_str())*1000+aScndData->UpdateMillisec;
		if(lPrimDateTime < lScndDateTime)
		{
			return NEXT_PRIM;
		}
		else if(lPrimDateTime > lScndDateTime)
		{
			return NEXT_SCND;
		}
		else
		{
			return NEXT_BOTH;
		}
	}

};