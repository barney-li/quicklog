#pragma once
#include "stdafx.h"
#include <Log.h>
#include <OracleClient.h>
#include "MarketDataType.h"
#include "MarketDataTypeMap.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#include <strstream>
#include <boost/atomic.hpp>
using namespace Utilities;
using namespace DatabaseUtilities;
class NonBlockDatabase
{
public:
	enum NON_BLOCK_STATUS
	{
		NONBLOCK_NO_ERROR,
		NONBLOCK_BUFFER_OVERFLOW,
		NONBLOCK_UNKNOWN_EXCEPTION
	};
private:
	struct DataPkg
	{
		DataPkg(string aTimeStamp, string aTimeStampFormat, string aTableName, CThostFtdcDepthMarketDataField* aData)
		{
			mTimeStamp = aTimeStamp;
			mTimeStampFormat = aTimeStampFormat;
			mTableName = aTableName;
			memcpy(&mData, aData, sizeof(CThostFtdcDepthMarketDataField));
		}
		string mTimeStamp;
		string mTimeStampFormat;
		string mTableName;
		CThostFtdcDepthMarketDataField mData;
	};
	
	string mUser;
	string mPwd;
	string mDb;
	unsigned long long mCacheSize;
	vector<DataPkg> mQueue;
	vector<string> mTableQueue;
	bool mDestroyTask;
	boost::thread* mInsertThread;
	boost::mutex mInsertThreadMutex;
	boost::condition_variable mInsertThreadCV;
	unsigned int mMaxSyncPeriod;
	boost::mutex mQueueMutex;
	boost::atomic<bool> mInsertTaskInitFinished;
	boost::atomic<bool> mStuffedBuffer;
	unsigned long long mMaxCacheUsage;
	Utilities::Log* mLogger;
public:
	bool InitFinished()
	{
		return mInsertTaskInitFinished;
	}
	NonBlockDatabase(string aUser, string aPwd, string aDb, unsigned long long aCacheSize, unsigned int aMaxSyncPeriod)
	{
		mUser = aUser;
		mPwd = aPwd;
		mDb = aDb;
		mCacheSize = aCacheSize;
		mQueue.reserve(mCacheSize);
		mTableQueue.reserve(mCacheSize);
		mDestroyTask = false;
		mMaxSyncPeriod = aMaxSyncPeriod;
		mInsertTaskInitFinished = false;
		mStuffedBuffer = false;
		mMaxCacheUsage = 0;
		mInsertThread = new boost::thread(&NonBlockDatabase::InsertTask, this);
		mLogger = new Utilities::Log("./log/", "NonBlockClient.log", 1024, true, 10);
	}
	~NonBlockDatabase()
	{
		cout<<"max cache usage is "<<(double)mMaxCacheUsage/(double)mCacheSize<<endl;
		mDestroyTask = true;
		mInsertThread->join();
		delete mInsertThread;
		delete mLogger;
	}
	void ValidateData(CThostFtdcDepthMarketDataField* aData)
	{
		const double lUpLmt = 1000000000;
		const double lInvalidValue = -3.1415926;
		/////������
		//TThostFtdcDateType	TradingDay;
		const int lTradingDayBreak = sizeof(TThostFtdcDateType) - 1;
		aData->TradingDay[lTradingDayBreak] = NULL;
		/////��Լ����
		//TThostFtdcInstrumentIDType	InstrumentID;
		const int lInstrumentIDBreak = sizeof(TThostFtdcInstrumentIDType) - 1;
		aData->InstrumentID[lInstrumentIDBreak] = NULL;
		/////����������
		//TThostFtdcExchangeIDType	ExchangeID;
		const int lExchangeIDBreak = sizeof(TThostFtdcExchangeIDType) - 1;
		aData->ExchangeID[lExchangeIDBreak] = NULL;
		/////��Լ�ڽ������Ĵ���
		//TThostFtdcExchangeInstIDType	ExchangeInstID;
		const int lExchangeInstIDBreak = sizeof(TThostFtdcExchangeInstIDType) - 1;
		aData->ExchangeInstID[lExchangeIDBreak] = NULL;
		/////���¼�
		//TThostFtdcPriceType	LastPrice;
		aData->LastPrice = (aData->LastPrice>lUpLmt)?lInvalidValue:aData->LastPrice;
		/////�ϴν����
		//TThostFtdcPriceType	PreSettlementPrice;
		aData->PreSettlementPrice = (aData->PreSettlementPrice>lUpLmt)?lInvalidValue:aData->PreSettlementPrice; 
		/////������
		//TThostFtdcPriceType	PreClosePrice;
		aData->PreClosePrice = (aData->PreClosePrice>lUpLmt)?lInvalidValue:aData->PreClosePrice;
		/////��ֲ���
		//TThostFtdcLargeVolumeType	PreOpenInterest;
		aData->PreOpenInterest = (aData->PreOpenInterest>lUpLmt)?lInvalidValue:aData->PreOpenInterest;
		/////����
		//TThostFtdcPriceType	OpenPrice;
		aData->OpenPrice = (aData->OpenPrice>lUpLmt)?lInvalidValue:aData->OpenPrice;
		/////��߼�
		//TThostFtdcPriceType	HighestPrice;
		aData->HighestPrice = (aData->HighestPrice>lUpLmt)?lInvalidValue:aData->HighestPrice;
		/////��ͼ�
		//TThostFtdcPriceType	LowestPrice;
		aData->LowestPrice = (aData->LowestPrice>lUpLmt)?lInvalidValue:aData->LowestPrice;
		/////����
		//TThostFtdcVolumeType	Volume;
		/////�ɽ����
		//TThostFtdcMoneyType	Turnover;
		aData->Turnover = (aData->Turnover>lUpLmt)?lInvalidValue:aData->Turnover;
		/////�ֲ���
		//TThostFtdcLargeVolumeType	OpenInterest;
		aData->OpenInterest = (aData->OpenInterest>lUpLmt)?lInvalidValue:aData->OpenInterest;
		/////������
		//TThostFtdcPriceType	ClosePrice;
		aData->ClosePrice = (aData->ClosePrice>lUpLmt)?lInvalidValue:aData->ClosePrice;
		/////���ν����
		//TThostFtdcPriceType	SettlementPrice;
		aData->SettlementPrice = (aData->SettlementPrice>lUpLmt)?lInvalidValue:aData->SettlementPrice;
		/////��ͣ���
		//TThostFtdcPriceType	UpperLimitPrice;
		aData->UpperLimitPrice = (aData->UpperLimitPrice>lUpLmt)?lInvalidValue:aData->UpperLimitPrice;
		/////��ͣ���
		//TThostFtdcPriceType	LowerLimitPrice;
		aData->LowerLimitPrice = (aData->LowerLimitPrice>lUpLmt)?lInvalidValue:aData->LowerLimitPrice;
		/////����ʵ��
		//TThostFtdcRatioType	PreDelta;
		aData->PreDelta = (aData->PreDelta>lUpLmt)?lInvalidValue:aData->PreDelta;
		/////����ʵ��
		//TThostFtdcRatioType	CurrDelta;
		aData->CurrDelta = (aData->CurrDelta>lUpLmt)?lInvalidValue:aData->CurrDelta;
		/////����޸�ʱ��
		//TThostFtdcTimeType	UpdateTime;
		const int lUpdateTimeBreak = sizeof(TThostFtdcTimeType) - 1;
		aData->UpdateTime[lUpdateTimeBreak] = NULL;
		/////����޸ĺ���
		//TThostFtdcMillisecType	UpdateMillisec;
		/////�����һ
		//TThostFtdcPriceType	BidPrice1;
		aData->BidPrice1 = (aData->BidPrice1>lUpLmt)?lInvalidValue:aData->BidPrice1;
		/////������һ
		//TThostFtdcVolumeType	BidVolume1;
		/////������һ
		//TThostFtdcPriceType	AskPrice1;
		aData->AskPrice1 = (aData->AskPrice1>lUpLmt)?lInvalidValue:aData->AskPrice1;
		/////������һ
		//TThostFtdcVolumeType	AskVolume1;
		/////����۶�
		//TThostFtdcPriceType	BidPrice2;
		aData->BidPrice2 = (aData->BidPrice2>lUpLmt)?lInvalidValue:aData->BidPrice2;
		/////��������
		//TThostFtdcVolumeType	BidVolume2;
		/////�����۶�
		//TThostFtdcPriceType	AskPrice2;
		aData->AskPrice2 = (aData->AskPrice2>lUpLmt)?lInvalidValue:aData->AskPrice2;
		/////��������
		//TThostFtdcVolumeType	AskVolume2;
		/////�������
		//TThostFtdcPriceType	BidPrice3;
		aData->BidPrice3 = (aData->BidPrice3>lUpLmt)?lInvalidValue:aData->BidPrice3;
		/////��������
		//TThostFtdcVolumeType	BidVolume3;
		/////��������
		//TThostFtdcPriceType	AskPrice3;
		aData->AskPrice3 = (aData->AskPrice3>lUpLmt)?lInvalidValue:aData->AskPrice3;
		/////��������
		//TThostFtdcVolumeType	AskVolume3;
		/////�������
		//TThostFtdcPriceType	BidPrice4;
		aData->BidPrice4 = (aData->BidPrice4>lUpLmt)?lInvalidValue:aData->BidPrice4;
		/////��������
		//TThostFtdcVolumeType	BidVolume4;
		/////��������
		//TThostFtdcPriceType	AskPrice4;
		aData->AskPrice4 = (aData->AskPrice4>lUpLmt)?lInvalidValue:aData->AskPrice4;
		/////��������
		//TThostFtdcVolumeType	AskVolume4;
		/////�������
		//TThostFtdcPriceType	BidPrice5;
		aData->BidPrice5 = (aData->BidPrice5>lUpLmt)?lInvalidValue:aData->BidPrice5;
		/////��������
		//TThostFtdcVolumeType	BidVolume5;
		/////��������
		//TThostFtdcPriceType	AskPrice5;
		aData->AskPrice5 = (aData->AskPrice5>lUpLmt)?lInvalidValue:aData->AskPrice5;
		/////��������
		//TThostFtdcVolumeType	AskVolume5;
		/////���վ���
		//TThostFtdcPriceType	AveragePrice;
		aData->AveragePrice = (aData->AveragePrice>lUpLmt)?lInvalidValue:aData->AveragePrice;
		/////ҵ������
		//TThostFtdcDateType	ActionDay;
		const int lActionDayBreak = sizeof(TThostFtdcDateType) - 1;
		aData->ActionDay[lActionDayBreak] = NULL;

	}
	NON_BLOCK_STATUS InsertData(string aTableName, CThostFtdcDepthMarketDataField* aData, string& aErrMsg)
	{
		NON_BLOCK_STATUS lReturn = NONBLOCK_NO_ERROR;	
		boost::lock_guard<boost::mutex> lQueueLock(mQueueMutex);
		boost::posix_time::ptime lLocalTime = boost::posix_time::microsec_clock::local_time();
		stringstream lTimeStamp;
		lTimeStamp.str("");
		lTimeStamp<<boost::gregorian::to_iso_extended_string(lLocalTime.date())<<" "<<lLocalTime.time_of_day();
		/* valid the input data here */
		ValidateData(aData);
		DataPkg lData(string(lTimeStamp.str()), "yyyy-mm-dd hh24:mi:ss.ff", aTableName, aData);
		if(mQueue.size()<=mCacheSize)
		{
			mQueue.push_back(lData);
		}
		else
		{
			//mQueue.pop_back();
			/* serialize and store the data here */
			lReturn = NONBLOCK_BUFFER_OVERFLOW;
			aErrMsg += "[WARNING]: buffer over flowed in InsertData(string, CThostFtdcDepthMarketDataField, string&)\r\n";
		}
		mStuffedBuffer = true;
		/* notify insert task here */
		mInsertThreadCV.notify_one();
		if(mMaxCacheUsage < mQueue.size())
		{
			mMaxCacheUsage = mQueue.size();
		}
		
		return lReturn;
	}
	void InsertTask()
	{
		boost::unique_lock<boost::mutex> lInsertTaskLock(mInsertThreadMutex);
		vector<DataPkg> lTempQueue;
		int lErrCode = 0;
		string lErrMsg = "";
		/* initialize oracle client here */
		OracleClient* lClient = new OracleClient();
		oracle::occi::Environment* lEnv = lClient->GetEnvironment();
		oracle::occi::Timestamp lTimeStamp;
		MarketDataTypeMap(lEnv);
		MarketDataType* lMarketData = new MarketDataType();
		if(lClient->Connect(mUser, mPwd, mDb, mCacheSize, lErrCode, lErrMsg) == TRANS_NO_ERROR)
		{
			mLogger->LogThisAdvance("nonblock client connected", Utilities::LOG_INFO);
			mInsertTaskInitFinished = true;
		}
		else
		{
			mLogger->LogThisAdvance("nonblock client can't connect to database, error message: "+lErrMsg, Utilities::LOG_ERROR);
		}
		while(mDestroyTask == false || mStuffedBuffer)
		{
			try
			{
				mInsertThreadCV.wait_for(lInsertTaskLock, boost::chrono::seconds(mMaxSyncPeriod));

				{
					boost::lock_guard<boost::mutex> lQueueLock(mQueueMutex);
					lTempQueue = mQueue;	
					mQueue.clear();
					mStuffedBuffer = false;
				}// release mQueueMutex
				for(int i=0; i<lTempQueue.size(); i++)
				{
					/* insert lTempQueue into lClient */;
					FlushBufferToDatabase(lMarketData, lClient, lTimeStamp, &lTempQueue[i], lEnv, lErrCode, lErrMsg);
				}
				lTempQueue.clear();
			}
			catch(SQLException ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in InsertTast(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
				mLogger->LogThisAdvance(tempStream.str(), Utilities::LOG_ERROR);
			}
			catch(...)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in InsertTask(), error message: unknown";
				mLogger->LogThisAdvance(tempStream.str(), Utilities::LOG_ERROR);
			}
		}
		delete lMarketData;
		delete lClient;
	}
	TRANSACTION_RESULT_TYPE FlushBufferToDatabase(MarketDataType* const aMarketData, OracleClient* const aClient, oracle::occi::Timestamp aTimeStamp, DataPkg* const aData, oracle::occi::Environment* const aEnv, int& aErrCode, string& aErrMsg)
	{
		try
		{
			aMarketData->setdata_type_version(1.0);
			aTimeStamp.fromText(aData->mTimeStamp, aData->mTimeStampFormat, "", aEnv);
			aMarketData->settime_stamp(aTimeStamp);
			/////������
			//TThostFtdcDateType	TradingDay;
			aMarketData->settrading_day(aData->mData.TradingDay);
			/////��Լ����
			//TThostFtdcInstrumentIDType	InstrumentID;
			aMarketData->setinstrument_id(aData->mData.InstrumentID);
			/////����������
			//TThostFtdcExchangeIDType	ExchangeID;
			aMarketData->setexchange_id(aData->mData.ExchangeID);
			/////��Լ�ڽ������Ĵ���
			//TThostFtdcExchangeInstIDType	ExchangeInstID;
			aMarketData->setexchange_instrument_id(aData->mData.ExchangeInstID);
			/////���¼�
			//TThostFtdcPriceType	LastPrice;
			aMarketData->setlast_price(aData->mData.LastPrice);
			/////�ϴν����
			//TThostFtdcPriceType	PreSettlementPrice;
			aMarketData->setpre_settlement_price(aData->mData.PreSettlementPrice);
			/////������
			//TThostFtdcPriceType	PreClosePrice;
			aMarketData->setpre_close_price(aData->mData.PreClosePrice);
			/////��ֲ���
			//TThostFtdcLargeVolumeType	PreOpenInterest;
			aMarketData->setpre_open_interest(aData->mData.PreOpenInterest);
			/////����
			//TThostFtdcPriceType	OpenPrice;
			aMarketData->setopen_price(aData->mData.OpenPrice);
			/////��߼�
			//TThostFtdcPriceType	HighestPrice;
			aMarketData->sethighest_price(aData->mData.HighestPrice);
			/////��ͼ�
			//TThostFtdcPriceType	LowestPrice;
			aMarketData->setlowest_price(aData->mData.LowestPrice);
			/////����
			//TThostFtdcVolumeType	Volume;
			aMarketData->setvolume(aData->mData.Volume);
			/////�ɽ����
			//TThostFtdcMoneyType	Turnover;
			aMarketData->setturnover(aData->mData.Turnover);
			/////�ֲ���
			//TThostFtdcLargeVolumeType	OpenInterest;
			aMarketData->setopen_interest(aData->mData.OpenInterest);
			/////������
			//TThostFtdcPriceType	ClosePrice;
			aMarketData->setclose_price(aData->mData.ClosePrice);
			/////���ν����
			//TThostFtdcPriceType	SettlementPrice;
			aMarketData->setsettlement_price(aData->mData.SettlementPrice);
			/////��ͣ���
			//TThostFtdcPriceType	UpperLimitPrice;
			aMarketData->setupper_limit_price(aData->mData.UpperLimitPrice);
			/////��ͣ���
			//TThostFtdcPriceType	LowerLimitPrice;
			aMarketData->setlower_limit_price(aData->mData.LowerLimitPrice);
			/////����ʵ��
			//TThostFtdcRatioType	PreDelta;
			aMarketData->setpre_delta(aData->mData.PreDelta);
			/////����ʵ��
			//TThostFtdcRatioType	CurrDelta;
			aMarketData->setcurr_delta(aData->mData.CurrDelta);
			/////����޸�ʱ��
			//TThostFtdcTimeType	UpdateTime;
			aMarketData->setupdate_time(aData->mData.UpdateTime);
			/////����޸ĺ���
			//TThostFtdcMillisecType	UpdateMillisec;
			aMarketData->setupdate_millisec(aData->mData.UpdateMillisec);
			/////�����һ
			//TThostFtdcPriceType	BidPrice1;
			aMarketData->setbid_price_1(aData->mData.BidPrice1);
			/////������һ
			//TThostFtdcVolumeType	BidVolume1;
			aMarketData->setbid_volume_1(aData->mData.BidVolume1);
			/////������һ
			//TThostFtdcPriceType	AskPrice1;
			aMarketData->setask_price_1(aData->mData.AskPrice1);
			/////������һ
			//TThostFtdcVolumeType	AskVolume1;
			aMarketData->setask_volume_1(aData->mData.AskVolume1);
			/////����۶�
			//TThostFtdcPriceType	BidPrice2;
			aMarketData->setbid_price_2(aData->mData.BidPrice2);
			/////��������
			//TThostFtdcVolumeType	BidVolume2;
			aMarketData->setbid_volume_2(aData->mData.BidVolume2);
			/////�����۶�
			//TThostFtdcPriceType	AskPrice2;
			aMarketData->setask_price_2(aData->mData.AskPrice2);
			/////��������
			//TThostFtdcVolumeType	AskVolume2;
			aMarketData->setask_volume_2(aData->mData.AskVolume2);
			/////�������
			//TThostFtdcPriceType	BidPrice3;
			aMarketData->setbid_price_3(aData->mData.BidPrice3);
			/////��������
			//TThostFtdcVolumeType	BidVolume3;
			aMarketData->setbid_volume_3(aData->mData.BidVolume3);
			/////��������
			//TThostFtdcPriceType	AskPrice3;
			aMarketData->setask_price_3(aData->mData.AskPrice3);
			/////��������
			//TThostFtdcVolumeType	AskVolume3;
			aMarketData->setask_volume_3(aData->mData.AskVolume3);
			/////�������
			//TThostFtdcPriceType	BidPrice4;
			aMarketData->setbid_price_4(aData->mData.BidPrice4);
			/////��������
			//TThostFtdcVolumeType	BidVolume4;
			aMarketData->setbid_volume_4(aData->mData.BidVolume4);
			/////��������
			//TThostFtdcPriceType	AskPrice4;
			aMarketData->setask_price_4(aData->mData.AskPrice4);
			/////��������
			//TThostFtdcVolumeType	AskVolume4;
			aMarketData->setask_volume_4(aData->mData.AskVolume4);
			/////�������
			//TThostFtdcPriceType	BidPrice5;
			aMarketData->setbid_price_5(aData->mData.BidPrice5);
			/////��������
			//TThostFtdcVolumeType	BidVolume5;
			aMarketData->setbid_volume_5(aData->mData.BidVolume5);
			/////��������
			//TThostFtdcPriceType	AskPrice5;
			aMarketData->setask_price_5(aData->mData.AskPrice5);
			/////��������
			//TThostFtdcVolumeType	AskVolume5;
			aMarketData->setask_volume_5(aData->mData.AskVolume5);
			/////���վ���
			//TThostFtdcPriceType	AveragePrice;
			aMarketData->setaverage_price(aData->mData.AveragePrice);
			/////ҵ������
			//TThostFtdcDateType	ActionDay;
			aMarketData->setaction_day(aData->mData.ActionDay);

			return aClient->InsertData(aData->mTableName, aMarketData, aErrCode, aErrMsg);
		}
		catch(SQLException ex)
		{
			std::stringstream tempStream;
			tempStream.str("");	
			tempStream<<"exception in FlushBufferToDatabase(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
			mLogger->LogThisAdvance(tempStream.str(), Utilities::LOG_ERROR);
			aErrCode = ex.getErrorCode();
			aErrMsg = ex.getMessage();
			return DatabaseUtilities::SQL_EXCEPTION;
		}
		catch(...)
		{
			std::stringstream tempStream;
			tempStream.str("");	
			tempStream<<"exception in FlushBufferToDatabase(), error message: unknown";
			mLogger->LogThisAdvance(tempStream.str(), Utilities::LOG_ERROR);
			aErrCode = -1;
			aErrMsg = "unknown exception in FlushBufferToDatabase()";
			return DatabaseUtilities::UNKNOWN_EXCEPTION;
		}
	}
};