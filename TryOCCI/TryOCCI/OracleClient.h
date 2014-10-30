#pragma once
#include <stdexcept>
#include <occi.h>
#include <iostream>
#include <string>
#include <sstream>
#include "ThostFtdcUserApiStruct.h"
using namespace oracle::occi;
using namespace std;
namespace DatabaseUtilities
{
	enum TRANSACTION_RESULT_TYPE
	{
		NO_ERROR,
		CAN_NOT_REACH_DATABASE,
		SQL_EXCEPTION,
		ILLEGAL_MARKET_DATA,
		UNKNOWN_EXCEPTION
	};
	class OracleClient
	{
	private:
		oracle::occi::Environment* mEnv;
		Connection* mConn;
		
	public:
		OracleClient(void)
		{
			mEnv = Environment::createEnvironment(Environment::THREADED_MUTEXED);
		}
		virtual ~OracleClient()
		{
			//mEnv->terminateConnection(mConn);
			Environment::terminateEnvironment(mEnv);
		}
		virtual void Disconnect()
		{
			mEnv->terminateConnection(mConn);
		}
		virtual TRANSACTION_RESULT_TYPE ConnectConnection(string aUser, string aPwd, string aDb)
		{
			try
			{
				mConn = mEnv->createConnection(aUser, aPwd, aDb);
				return NO_ERROR;
			}
			catch(SQLException ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateConnection(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
				cout<<tempStream<<endl;
				return SQL_EXCEPTION;
			}
			catch(std::exception ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateConnection(), error message: "<<ex.what();
				cout<<tempStream<<endl;
				return UNKNOWN_EXCEPTION; 
			}
			catch(...)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateConnection(), error message: unknown";
				cout<<tempStream<<endl;
				return UNKNOWN_EXCEPTION; 
			}
		}
		/*virtual TRANSACTION_RESULT_TYPE CreateMarketDataTable(string aTableName)
		{
			return TRANSACTION_RESULT_TYPE::NO_ERROR;
			Statement* mStat = NULL;
			try
			{
				int i=0;
				const string lSqlStatement = "CREATE TABLE " + aTableName + " (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23,:24,:25,:26,:27,:28,:29,:30,:31,:32,:33,:34,:35,:36,:37,:38,:39,:40,:41,:42,:43,:44)";
				mStat = mConn->createStatement();
				if(mStat == NULL) throw(std::exception("create statement failed"));
				mStat->setSQL(lSqlStatement);

				///交易日				TThostFtdcDateType	TradingDay;
				mStat->setString(++i,(string)aMarketData.TradingDay);

				///合约代码				TThostFtdcInstrumentIDType	InstrumentID;
				mStat->setString(++i,(string)aMarketData.InstrumentID);

				///交易所代码			TThostFtdcExchangeIDType	ExchangeID;
				mStat->setString(++i,(string)aMarketData.ExchangeID);

				///合约在交易所的代码	TThostFtdcExchangeInstIDType	ExchangeInstID;
				mStat->setString(++i,(string)aMarketData.ExchangeInstID);

				///最新价				TThostFtdcPriceType	LastPrice;
				mStat->setDouble(++i,aMarketData.LastPrice);

				///上次结算价			TThostFtdcPriceType	PreSettlementPrice;
				mStat->setDouble(++i,aMarketData.PreSettlementPrice);

				///昨收盘				TThostFtdcPriceType	PreClosePrice;
				mStat->setDouble(++i,aMarketData.PreClosePrice);

				///昨持仓量				TThostFtdcLargeVolumeType	PreOpenInterest;
				mStat->setDouble(++i,aMarketData.PreOpenInterest);

				///今开盘				TThostFtdcPriceType	OpenPrice;
				mStat->setDouble(++i,aMarketData.OpenPrice);

				///最高价				TThostFtdcPriceType	HighestPrice;
				mStat->setDouble(++i,aMarketData.HighestPrice);

				///最低价				TThostFtdcPriceType	LowestPrice;
				mStat->setDouble(++i,aMarketData.LowestPrice);

				///数量					TThostFtdcVolumeType	Volume;
				mStat->setInt(++i,aMarketData.Volume);

				///成交金额				TThostFtdcMoneyType	Turnover;
				mStat->setDouble(++i,aMarketData.Turnover);

				///持仓量				TThostFtdcLargeVolumeType	OpenInterest;
				mStat->setDouble(++i, aMarketData.OpenInterest);

				///今收盘				TThostFtdcPriceType	ClosePrice;
				mStat->setDouble(++i, aMarketData.ClosePrice);

				///本次结算价			TThostFtdcPriceType	SettlementPrice;
				mStat->setDouble(++i, aMarketData.SettlementPrice);

				///涨停板价				TThostFtdcPriceType	UpperLimitPrice;
				mStat->setDouble(++i, aMarketData.UpperLimitPrice);

				///跌停板价				TThostFtdcPriceType	LowerLimitPrice;
				mStat->setDouble(++i, aMarketData.LowerLimitPrice);

				///昨虚实度				TThostFtdcRatioType	PreDelta;
				mStat->setDouble(++i, aMarketData.PreDelta);

				///今虚实度				TThostFtdcRatioType	CurrDelta;
				mStat->setDouble(++i, aMarketData.CurrDelta);

				///最后修改时间			TThostFtdcTimeType	UpdateTime;
				mStat->setString(++i, (string)aMarketData.UpdateTime);

				///最后修改毫秒			TThostFtdcMillisecType	UpdateMillisec;
				mStat->setInt(++i, aMarketData.UpdateMillisec);

				///申买价一				TThostFtdcPriceType	BidPrice1;
				mStat->setDouble(++i, aMarketData.BidPrice1);

				///申买量一				TThostFtdcVolumeType	BidVolume1;
				mStat->setInt(++i, aMarketData.BidVolume1);

				///申卖价一				TThostFtdcPriceType	AskPrice1;
				mStat->setDouble(++i, aMarketData.AskPrice1);

				///申卖量一				TThostFtdcVolumeType	AskVolume1;
				mStat->setInt(++i, aMarketData.AskVolume1);

				///申买价二				TThostFtdcPriceType	BidPrice2;
				mStat->setDouble(++i, aMarketData.BidPrice2);

				///申买量二				TThostFtdcVolumeType	BidVolume2;
				mStat->setInt(++i, aMarketData.BidVolume2);

				///申卖价二				TThostFtdcPriceType	AskPrice2;
				mStat->setDouble(++i, aMarketData.AskPrice2);

				///申卖量二				TThostFtdcVolumeType	AskVolume2;
				mStat->setInt(++i, aMarketData.AskVolume2);

				///申买价三				TThostFtdcPriceType	BidPrice3;
				mStat->setDouble(++i, aMarketData.BidPrice3);

				///申买量三				TThostFtdcVolumeType	BidVolume3;
				mStat->setInt(++i, aMarketData.BidVolume3);

				///申卖价三				TThostFtdcPriceType	AskPrice3;
				mStat->setDouble(++i, aMarketData.AskPrice3);

				///申卖量三				TThostFtdcVolumeType	AskVolume3;
				mStat->setInt(++i, aMarketData.AskVolume3);

				///申买价四				TThostFtdcPriceType	BidPrice4;
				mStat->setDouble(++i, aMarketData.BidPrice4);

				///申买量四				TThostFtdcVolumeType	BidVolume4;
				mStat->setInt(++i, aMarketData.BidVolume4);

				///申卖价四				TThostFtdcPriceType	AskPrice4;
				mStat->setDouble(++i, aMarketData.AskPrice4);

				///申卖量四				TThostFtdcVolumeType	AskVolume4;
				mStat->setInt(++i, aMarketData.AskVolume4);

				///申买价五				TThostFtdcPriceType	BidPrice5;
				mStat->setDouble(++i, aMarketData.BidPrice5);

				///申买量五				TThostFtdcVolumeType	BidVolume5;
				mStat->setInt(++i, aMarketData.BidVolume5);

				///申卖价五				TThostFtdcPriceType	AskPrice5;
				mStat->setDouble(++i, aMarketData.AskPrice5);

				///申卖量五				TThostFtdcVolumeType	AskVolume5;
				mStat->setInt(++i, aMarketData.AskVolume5);

				///当日均价				TThostFtdcPriceType	AveragePrice;
				mStat->setDouble(++i, aMarketData.AveragePrice);

				///业务日期				TThostFtdcDateType	ActionDay;
				mStat->setString(++i, (string)aMarketData.ActionDay);

				mStat->executeUpdate();
				cout<<"insert market data successful"<<endl;
				return NO_ERROR; 
			}
			catch(SQLException ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateMarketDataTable(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
				cout<<tempStream<<endl;
				if(mStat != NULL) mConn->terminateStatement(mStat);
				return SQL_EXCEPTION;
			}
			catch(std::exception ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateMarketDataTable(), error message: "<<ex.what();
				cout<<tempStream<<endl;
				if(mStat != NULL) mConn->terminateStatement(mStat);
				return UNKNOWN_EXCEPTION; 
			}
			catch(...)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateMarketDataTable(), error message: unknown";
				cout<<tempStream<<endl;
				if(mStat != NULL) mConn->terminateStatement(mStat);
				return UNKNOWN_EXCEPTION; 
			}
		}*/
		virtual TRANSACTION_RESULT_TYPE InsertMarketData(string aTableName, const CThostFtdcDepthMarketDataField& aMarketData)
		{
			int i=0;
			const string lSqlStatement = "INSERT INTO " + aTableName + " VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23,:24,:25,:26,:27,:28,:29,:30,:31,:32,:33,:34,:35,:36,:37,:38,:39,:40,:41,:42,:43,:44)";
			Statement* mStat = NULL;
			try
			{
				mStat = mConn->createStatement(); 
				if(mStat == NULL) throw(std::exception("create statement failed"));
			
				mStat->setSQL(lSqlStatement);

				///交易日				TThostFtdcDateType	TradingDay;
				mStat->setString(++i,(string)aMarketData.TradingDay);

				///合约代码				TThostFtdcInstrumentIDType	InstrumentID;
				mStat->setString(++i,(string)aMarketData.InstrumentID);

				///交易所代码			TThostFtdcExchangeIDType	ExchangeID;
				mStat->setString(++i,(string)aMarketData.ExchangeID);

				///合约在交易所的代码	TThostFtdcExchangeInstIDType	ExchangeInstID;
				mStat->setString(++i,(string)aMarketData.ExchangeInstID);

				///最新价				TThostFtdcPriceType	LastPrice;
				mStat->setDouble(++i,aMarketData.LastPrice);

				///上次结算价			TThostFtdcPriceType	PreSettlementPrice;
				mStat->setDouble(++i,aMarketData.PreSettlementPrice);

				///昨收盘				TThostFtdcPriceType	PreClosePrice;
				mStat->setDouble(++i,aMarketData.PreClosePrice);

				///昨持仓量				TThostFtdcLargeVolumeType	PreOpenInterest;
				mStat->setDouble(++i,aMarketData.PreOpenInterest);

				///今开盘				TThostFtdcPriceType	OpenPrice;
				mStat->setDouble(++i,aMarketData.OpenPrice);

				///最高价				TThostFtdcPriceType	HighestPrice;
				mStat->setDouble(++i,aMarketData.HighestPrice);

				///最低价				TThostFtdcPriceType	LowestPrice;
				mStat->setDouble(++i,aMarketData.LowestPrice);

				///数量					TThostFtdcVolumeType	Volume;
				mStat->setInt(++i,aMarketData.Volume);

				///成交金额				TThostFtdcMoneyType	Turnover;
				mStat->setDouble(++i,aMarketData.Turnover);

				///持仓量				TThostFtdcLargeVolumeType	OpenInterest;
				mStat->setDouble(++i, aMarketData.OpenInterest);

				///今收盘				TThostFtdcPriceType	ClosePrice;
				mStat->setDouble(++i, aMarketData.ClosePrice);

				///本次结算价			TThostFtdcPriceType	SettlementPrice;
				mStat->setDouble(++i, aMarketData.SettlementPrice);

				///涨停板价				TThostFtdcPriceType	UpperLimitPrice;
				mStat->setDouble(++i, aMarketData.UpperLimitPrice);

				///跌停板价				TThostFtdcPriceType	LowerLimitPrice;
				mStat->setDouble(++i, aMarketData.LowerLimitPrice);

				///昨虚实度				TThostFtdcRatioType	PreDelta;
				mStat->setDouble(++i, aMarketData.PreDelta);

				///今虚实度				TThostFtdcRatioType	CurrDelta;
				mStat->setDouble(++i, aMarketData.CurrDelta);

				///最后修改时间			TThostFtdcTimeType	UpdateTime;
				mStat->setString(++i, (string)aMarketData.UpdateTime);

				///最后修改毫秒			TThostFtdcMillisecType	UpdateMillisec;
				mStat->setInt(++i, aMarketData.UpdateMillisec);

				///申买价一				TThostFtdcPriceType	BidPrice1;
				mStat->setDouble(++i, aMarketData.BidPrice1);

				///申买量一				TThostFtdcVolumeType	BidVolume1;
				mStat->setInt(++i, aMarketData.BidVolume1);

				///申卖价一				TThostFtdcPriceType	AskPrice1;
				mStat->setDouble(++i, aMarketData.AskPrice1);

				///申卖量一				TThostFtdcVolumeType	AskVolume1;
				mStat->setInt(++i, aMarketData.AskVolume1);

				///申买价二				TThostFtdcPriceType	BidPrice2;
				mStat->setDouble(++i, aMarketData.BidPrice2);

				///申买量二				TThostFtdcVolumeType	BidVolume2;
				mStat->setInt(++i, aMarketData.BidVolume2);

				///申卖价二				TThostFtdcPriceType	AskPrice2;
				mStat->setDouble(++i, aMarketData.AskPrice2);

				///申卖量二				TThostFtdcVolumeType	AskVolume2;
				mStat->setInt(++i, aMarketData.AskVolume2);

				///申买价三				TThostFtdcPriceType	BidPrice3;
				mStat->setDouble(++i, aMarketData.BidPrice3);

				///申买量三				TThostFtdcVolumeType	BidVolume3;
				mStat->setInt(++i, aMarketData.BidVolume3);

				///申卖价三				TThostFtdcPriceType	AskPrice3;
				mStat->setDouble(++i, aMarketData.AskPrice3);

				///申卖量三				TThostFtdcVolumeType	AskVolume3;
				mStat->setInt(++i, aMarketData.AskVolume3);

				///申买价四				TThostFtdcPriceType	BidPrice4;
				mStat->setDouble(++i, aMarketData.BidPrice4);

				///申买量四				TThostFtdcVolumeType	BidVolume4;
				mStat->setInt(++i, aMarketData.BidVolume4);

				///申卖价四				TThostFtdcPriceType	AskPrice4;
				mStat->setDouble(++i, aMarketData.AskPrice4);

				///申卖量四				TThostFtdcVolumeType	AskVolume4;
				mStat->setInt(++i, aMarketData.AskVolume4);

				///申买价五				TThostFtdcPriceType	BidPrice5;
				mStat->setDouble(++i, aMarketData.BidPrice5);

				///申买量五				TThostFtdcVolumeType	BidVolume5;
				mStat->setInt(++i, aMarketData.BidVolume5);

				///申卖价五				TThostFtdcPriceType	AskPrice5;
				mStat->setDouble(++i, aMarketData.AskPrice5);

				///申卖量五				TThostFtdcVolumeType	AskVolume5;
				mStat->setInt(++i, aMarketData.AskVolume5);

				///当日均价				TThostFtdcPriceType	AveragePrice;
				mStat->setDouble(++i, aMarketData.AveragePrice);

				///业务日期				TThostFtdcDateType	ActionDay;
				mStat->setString(++i, (string)aMarketData.ActionDay);

				mStat->executeUpdate();
				cout<<"insert market data successful"<<endl;
				return NO_ERROR;
			}
			catch(SQLException ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in InsertMarketData(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
				cout<<tempStream<<endl;
				if(mStat != NULL) mConn->terminateStatement(mStat);
				return SQL_EXCEPTION;
			}
			catch(std::exception ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in InsertMarketData(), error message: "<<ex.what();
				cout<<tempStream<<endl;
				if(mStat != NULL) mConn->terminateStatement(mStat);
				return UNKNOWN_EXCEPTION; 
			}
			catch(...)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in InsertMarketData(), error message: unknown";
				cout<<tempStream<<endl;
				if(mStat != NULL) mConn->terminateStatement(mStat);
				return UNKNOWN_EXCEPTION; 
			}
		}
		//virtual TRANSACTION_RESULT_TYPE QueryMarketData(string aTableName, CThostFtdcDepthMarketDataField& aMarketData); 

	};
}