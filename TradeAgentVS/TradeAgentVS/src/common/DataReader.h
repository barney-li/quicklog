#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <ThostFtdcUserApiStruct.h>
#include <string.h>
#include <boost/regex.hpp>
using namespace std;
using namespace boost;
class DataReader
{
private:
	fstream mDataFile;
	CThostFtdcDepthMarketDataField mMarketData;
	typedef enum
	{
		BLANK_MARKET_DATA = 0,
		TRADING_DAY,
		INSTRUMENT_ID,
		LAST_PRICE,
		PRE_SETTLEMENT_PRICE,
		PRE_CLOSE_PRICE,
		PRE_OPEN_INTEREST,
		OPEN_PRICE,
		HIGHEST_PRICE,
		LOWEST_PRICE,
		VOLUME,
		OPEN_INTEREST,
		UPPER_LIMIT_PRICE,
		LOWER_LIMIT_PRICE,
		UPDATE_TIME,
		UPDATE_MILLISEC,
		BID_PRICE_1,
		BID_VOLUME_1,
		ASK_PRICE_1,
		ASK_VOLUME_1,
		MAX_MARKET_DATA
	}MARKET_DATA_MEMBER;
public:
	DataReader(string aDataFilePath)
	{
		mDataFile.open(aDataFilePath, ios::in);
	}
	~DataReader()
	{
		mDataFile.close();
	}
	CThostFtdcDepthMarketDataField* GetMarketDataLine()
	{
		// get these:               TradingDay  InstrumentID        LastPrice       PSPrice         PCPrice         POInterest      OpenPrice       HighestPrice    LowestPrice     Volume  OPI             UpperLimit      LowerLimit      UpdateTime                  Ms      BidPrice1       BidVol1 AskPrice1       AskVol1
		string lPatternStr = ".*	(\\d+)	([a-zA-Z]{1,2}\\w+)	(\\d+\\.\\d+)	(\\d+\\.\\d+)	(\\d+\\.\\d+)	(\\d+\\.\\d+)	(\\d+\\.\\d+)	(\\d+\\.\\d+)	(\\d+\\.\\d+)	(\\d+)	(\\d+\\.\\d+)	(\\d+\\.\\d+)	(\\d+\\.\\d+)	(\\d{2}\\:\\d{2}\\:\\d{2})	(\\d+)	(\\d+\\.\\d+)	(\\d+)	(\\d+\\.\\d+)	(\\d+)	.*";
		boost::regex lPattern(lPatternStr);
		boost::smatch lResult;
		if(mDataFile.is_open() && !mDataFile.eof())
		{
			string lDataLine;
			getline(mDataFile, lDataLine);
			if(regex_match(lDataLine, lResult, lPattern))
			{
				if(lResult.size() == MAX_MARKET_DATA)
				{
					return TransferData(lResult);
				}
			}
		}
		return NULL;
	}
private:
	CThostFtdcDepthMarketDataField* TransferData(boost::smatch aRawData)
	{
		memset(&mMarketData, 0, sizeof(mMarketData));
		strncpy((char*)&mMarketData.TradingDay, aRawData[TRADING_DAY].str().c_str(), sizeof(TThostFtdcDateType));
		strncpy((char*)&mMarketData.InstrumentID, aRawData[INSTRUMENT_ID].str().c_str(), sizeof(TThostFtdcInstrumentIDType));
		mMarketData.LastPrice = atof(aRawData[LAST_PRICE].str().c_str());
		mMarketData.PreSettlementPrice = atof(aRawData[PRE_SETTLEMENT_PRICE].str().c_str());
		mMarketData.PreClosePrice = atof(aRawData[PRE_CLOSE_PRICE].str().c_str());
		mMarketData.PreOpenInterest = atof(aRawData[PRE_OPEN_INTEREST].str().c_str());
		mMarketData.OpenPrice = atof(aRawData[OPEN_PRICE].str().c_str());
		mMarketData.HighestPrice = atof(aRawData[HIGHEST_PRICE].str().c_str());
		mMarketData.LowestPrice = atof(aRawData[LOWEST_PRICE].str().c_str());
		mMarketData.Volume = atoi(aRawData[VOLUME].str().c_str());
		mMarketData.OpenInterest = atof(aRawData[OPEN_INTEREST].str().c_str());
		mMarketData.UpperLimitPrice = atof(aRawData[UPPER_LIMIT_PRICE].str().c_str());
		mMarketData.LowerLimitPrice = atof(aRawData[LOWER_LIMIT_PRICE].str().c_str());
		strncpy((char*)&mMarketData.UpdateTime, aRawData[UPDATE_TIME].str().c_str(), sizeof(TThostFtdcTimeType));
		mMarketData.UpdateMillisec = atoi(aRawData[UPDATE_MILLISEC].str().c_str());
		mMarketData.BidPrice1 = atof(aRawData[BID_PRICE_1].str().c_str());
		mMarketData.BidVolume1 = atoi(aRawData[BID_VOLUME_1].str().c_str());
		mMarketData.AskPrice1 = atof(aRawData[ASK_PRICE_1].str().c_str());
		mMarketData.AskVolume1 = atoi(aRawData[ASK_VOLUME_1].str().c_str());
		return &mMarketData;
	}
};