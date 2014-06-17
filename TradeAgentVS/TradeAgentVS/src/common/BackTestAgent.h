#pragma once
#include "DataReader.h"
//#include <boost/date_time/posix_time/posix_time.hpp>
#include "../Stratergy/PrimeryAndSecondary.h"
//using namespace std;
//using namespace boost::posix_time;
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
	//��ջ������ڵ����ݣ������ӲŻ����µ����ݱ�д��
	void ResetData(CThostFtdcDepthMarketDataField* aMarketData)
	{
		memset(aMarketData, 0, sizeof(CThostFtdcDepthMarketDataField));
		memset(aMarketData, 0, sizeof(CThostFtdcDepthMarketDataField));
	}
	//�ж��Ƿ񻺳��������
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
	// �����������������ʱ������жϽ�����Ӧ��������һ�����ݡ�ʱ��������
	// ����Ӧ�������롣���ʱ�����ͬ����ôһ�����롣ʱ�������TradingDay, 
	// UpdateTime�Լ�UpdateMilliSec��
	/************************************************************************/
	int WhoIsNext(CThostFtdcDepthMarketDataField* aPrimData, CThostFtdcDepthMarketDataField* aScndData)
	{
		ptime lPrimPosixTime = ProcessTime(aPrimData);
		ptime lScndPosixTime = ProcessTime(aScndData);
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
	/************************************************************************/
	// �޸Ľ������ڡ�TradingDay���������ݵ���ʵ���ڣ����Ͼŵ㿪ʼΪ����Ľ���
	// �գ�����������������������Ͼŵ��Ժ������ǰ�����ڣ�����һ����
	/************************************************************************/
	ptime ProcessTime(CThostFtdcDepthMarketDataField* aMarketData)
	{
		string lDateString = aMarketData->TradingDay;
		string lTimeString = aMarketData->UpdateTime;
		string lMsString;
		if(lDateString.size() != 8)
		{
			return time_from_string("2000-01-01 00:00:00.0");
		}
		if(lTimeString.size() != 8)
		{
			return time_from_string("2000-01-01 00:00:00.0");
		}
		lDateString.insert(4, "-");
		lDateString.insert(7, "-");
		char lMsBuf[4];
		sprintf(lMsBuf, "%d", aMarketData->UpdateMillisec);
		lMsString = lMsBuf;
		ptime lDataTime = time_from_string(lDateString+" "+lTimeString+"."+lMsString);
		ptime lCriticalTime = time_from_string(lDateString+" 21:00:00.0");//�������ʱ��㣬trading day�ͻ�+1
		if(lDataTime >= lCriticalTime)
		{
			date_duration lOneDay(1);
			lDataTime = lDataTime - lOneDay;
			
		}
		return lDataTime;
	}
};