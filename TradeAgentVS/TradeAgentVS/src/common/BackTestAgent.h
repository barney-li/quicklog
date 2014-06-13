#pragma once
#include "DataReader.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../Stratergy/PrimeryAndSecondary.h"
using namespace std;
using namespace boost::posix_time;
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
		string lPrimTime = aPrimData->UpdateTime;
		string lScndTime = aScndData->UpdateTime;
		if(lPrimTime.size() != 8)
		{
			return NEXT_PRIM;
		}
		if(lScndTime.size() != 8)
		{
			return NEXT_SCND;
		}
		ptime lPrimPosixTime = time_from_string((string)"2000-01-01 "+lPrimTime);
		//对于凌晨时间，使用第二日的日期
		if(lPrimPosixTime<time_from_string("2000-01-01 04:00:00"))
		{
			lPrimPosixTime = time_from_string((string)"2000-01-02 "+lPrimTime);
		}
		ptime lScndPosixTime = time_from_string((string)"2000-01-01 "+lScndTime);
		if(lScndPosixTime<time_from_string("2000-01-01 04:00:00"))
		{
			lScndPosixTime = time_from_string((string)"2000-01-02 "+lScndTime);
		}

		if(lPrimPosixTime < lScndPosixTime)
		{
			return NEXT_PRIM;
		}
		else if(lPrimPosixTime > lScndPosixTime)
		{
			return NEXT_SCND;
		}
		else
		{
			return NEXT_BOTH;
		}
	}

};