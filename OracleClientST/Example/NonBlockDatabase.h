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
		NO_ERROR,
		BUFFER_OVERFLOW
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
		mLogger = new Utilities::Log("./Log/", "NonBlockClient.log", 128, true, 10);
	}
	~NonBlockDatabase()
	{
		cout<<"max cache usage is "<<(double)mMaxCacheUsage/(double)mCacheSize<<endl;
		mDestroyTask = true;
		mInsertThread->join();
		delete mInsertThread;
	}

	NON_BLOCK_STATUS InsertData(string aTableName, CThostFtdcDepthMarketDataField* aData, string& aErrMsg)
	{
		NON_BLOCK_STATUS lReturn = NO_ERROR;	
		boost::lock_guard<boost::mutex> lQueueLock(mQueueMutex);
		boost::posix_time::ptime lLocalTime = boost::posix_time::microsec_clock::local_time();
		stringstream lTimeStamp;
		lTimeStamp.str("");
		lTimeStamp<<boost::gregorian::to_iso_extended_string(lLocalTime.date())<<" "<<lLocalTime.time_of_day();
		DataPkg lData(string(lTimeStamp.str()), "yyyy-mm-dd hh24:mi:ss.ff", aTableName, aData);
		if(mQueue.size()>mCacheSize)
		{
			mQueue.pop_back();
			lReturn = BUFFER_OVERFLOW;
			aErrMsg += "[ERROR]: buffer over flowed in InsertData(string, CThostFtdcDepthMarketDataField, string&)\r\n";
		}
		mQueue.push_back(lData);
		mStuffedBuffer = true;
		//cout<<"queue size: "<<mQueue.size()<<" notify insert task "<<endl;
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
		/* initialize oracle client here */
		OracleClient* lClient = new OracleClient();
		oracle::occi::Environment* lEnv = lClient->GetEnvironment();
		oracle::occi::Timestamp lTimeStamp;
		MarketDataTypeMap(lEnv);
		MarketDataType* lMarketData = new MarketDataType();
		if(lClient->Connect("c##barney", "Lml19870310", "//192.168.183.128:1521/barneydb", mCacheSize) == TRANS_NO_ERROR)
		{
			mLogger->LogThisAdvance("nonblock client connected", Utilities::LOG_INFO);
			mInsertTaskInitFinished = true;
		}
		else
		{
			mLogger->LogThisAdvance("nonblock client can't connect to database", Utilities::LOG_ERROR);
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
					/*lMarketData->setdata_type_version(1.0);
					lTimeStamp.fromText(lTempQueue[i].mTimeStamp, lTempQueue[i].mTimeStampFormat, "", lEnv);
					lMarketData->settime_stamp(lTimeStamp);
					lMarketData->settrading_day(lTempQueue[i].mData.TradingDay);
					lMarketData->setlast_price(lTempQueue[i].mData.LastPrice);*/
					FlushBufferToDatabase(lMarketData, lClient, lTimeStamp, &lTempQueue[i], lEnv);
					//lClient->InsertData(lTempQueue[i].mTableName, lMarketData);
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
	void FlushBufferToDatabase(MarketDataType* const aMarketData, OracleClient* const aClient, oracle::occi::Timestamp aTimeStamp, DataPkg* const aData, oracle::occi::Environment* const aEnv)
	{
		try
		{
			aMarketData->setdata_type_version(1.0);
			aTimeStamp.fromText(aData->mTimeStamp, aData->mTimeStampFormat, "", aEnv);
			aMarketData->settime_stamp(aTimeStamp);
			/////交易日
			//TThostFtdcDateType	TradingDay;
			aMarketData->settrading_day(aData->mData.TradingDay);
			/////合约代码
			//TThostFtdcInstrumentIDType	InstrumentID;
			aMarketData->setinstrument_id(aData->mData.InstrumentID);
			/////交易所代码
			//TThostFtdcExchangeIDType	ExchangeID;
			aMarketData->setexchange_id(aData->mData.ExchangeID);
			/////合约在交易所的代码
			//TThostFtdcExchangeInstIDType	ExchangeInstID;
			aMarketData->setexchange_instrument_id(aData->mData.ExchangeInstID);
			/////最新价
			//TThostFtdcPriceType	LastPrice;
			/////上次结算价
			//TThostFtdcPriceType	PreSettlementPrice;
			/////昨收盘
			//TThostFtdcPriceType	PreClosePrice;
			/////昨持仓量
			//TThostFtdcLargeVolumeType	PreOpenInterest;
			/////今开盘
			//TThostFtdcPriceType	OpenPrice;
			/////最高价
			//TThostFtdcPriceType	HighestPrice;
			/////最低价
			//TThostFtdcPriceType	LowestPrice;
			/////数量
			//TThostFtdcVolumeType	Volume;
			/////成交金额
			//TThostFtdcMoneyType	Turnover;
			/////持仓量
			//TThostFtdcLargeVolumeType	OpenInterest;
			/////今收盘
			//TThostFtdcPriceType	ClosePrice;
			/////本次结算价
			//TThostFtdcPriceType	SettlementPrice;
			/////涨停板价
			//TThostFtdcPriceType	UpperLimitPrice;
			/////跌停板价
			//TThostFtdcPriceType	LowerLimitPrice;
			/////昨虚实度
			//TThostFtdcRatioType	PreDelta;
			/////今虚实度
			//TThostFtdcRatioType	CurrDelta;
			/////最后修改时间
			//TThostFtdcTimeType	UpdateTime;
			/////最后修改毫秒
			//TThostFtdcMillisecType	UpdateMillisec;
			/////申买价一
			//TThostFtdcPriceType	BidPrice1;
			/////申买量一
			//TThostFtdcVolumeType	BidVolume1;
			/////申卖价一
			//TThostFtdcPriceType	AskPrice1;
			/////申卖量一
			//TThostFtdcVolumeType	AskVolume1;
			/////申买价二
			//TThostFtdcPriceType	BidPrice2;
			/////申买量二
			//TThostFtdcVolumeType	BidVolume2;
			/////申卖价二
			//TThostFtdcPriceType	AskPrice2;
			/////申卖量二
			//TThostFtdcVolumeType	AskVolume2;
			/////申买价三
			//TThostFtdcPriceType	BidPrice3;
			/////申买量三
			//TThostFtdcVolumeType	BidVolume3;
			/////申卖价三
			//TThostFtdcPriceType	AskPrice3;
			/////申卖量三
			//TThostFtdcVolumeType	AskVolume3;
			/////申买价四
			//TThostFtdcPriceType	BidPrice4;
			/////申买量四
			//TThostFtdcVolumeType	BidVolume4;
			/////申卖价四
			//TThostFtdcPriceType	AskPrice4;
			/////申卖量四
			//TThostFtdcVolumeType	AskVolume4;
			/////申买价五
			//TThostFtdcPriceType	BidPrice5;
			/////申买量五
			//TThostFtdcVolumeType	BidVolume5;
			/////申卖价五
			//TThostFtdcPriceType	AskPrice5;
			/////申卖量五
			//TThostFtdcVolumeType	AskVolume5;
			/////当日均价
			//TThostFtdcPriceType	AveragePrice;
			/////业务日期
			//TThostFtdcDateType	ActionDay;

			aClient->InsertData(aData->mTableName, aMarketData);
		}
		catch(SQLException ex)
		{
			std::stringstream tempStream;
			tempStream.str("");	
			tempStream<<"exception in FlushBufferToDatabase(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
			mLogger->LogThisAdvance(tempStream.str(), Utilities::LOG_ERROR);
		}
		catch(...)
		{
			std::stringstream tempStream;
			tempStream.str("");	
			tempStream<<"exception in FlushBufferToDatabase(), error message: unknown";
			mLogger->LogThisAdvance(tempStream.str(), Utilities::LOG_ERROR);
		}
	}
};