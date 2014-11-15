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
		Statement* mStat;
		
	public:
		OracleClient(void)
		{
			mEnv = Environment::createEnvironment(Environment::THREADED_MUTEXED);
			mConn=NULL;
			mStat=NULL;
		}
		virtual ~OracleClient()
		{
			//mEnv->terminateConnection(mConn);
			Environment::terminateEnvironment(mEnv);
		}
		virtual void Disconnect()
		{
			if(mStat != NULL)
			{
				mConn->terminateStatement(mStat);
				mStat=NULL;
			}
			if(mConn != NULL)
			{
				mEnv->terminateConnection(mConn);
				mConn=NULL;
			}
		}
		virtual TRANSACTION_RESULT_TYPE Connect(string aUser, string aPwd, string aDb)
		{
			try
			{
				mConn = mEnv->createConnection(aUser, aPwd, aDb);
				mStat = mConn->createStatement();
				return NO_ERROR;
			}
			catch(SQLException ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateConnection(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
				cout<<tempStream.str()<<endl;
				return SQL_EXCEPTION;
			}
			catch(std::exception ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateConnection(), error message: "<<ex.what();
				cout<<tempStream.str()<<endl;
				return UNKNOWN_EXCEPTION; 
			}
			catch(...)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateConnection(), error message: unknown";
				cout<<tempStream.str()<<endl;
				return UNKNOWN_EXCEPTION; 
			}
		}
		virtual TRANSACTION_RESULT_TYPE CreateMarketDataTable(string aTableName)
		{
			try
			{
				int i=0;
				string lSqlStatement = "CREATE TABLE " + aTableName + "(";

				lSqlStatement += " time_stamp timestamp,";

				///交易日				TThostFtdcDateType	TradingDay;
				lSqlStatement += " trading_day varchar2(9),";	
				
				///合约代码				TThostFtdcInstrumentIDType	InstrumentID;
				lSqlStatement += " instrument_id varchar2(11),";

				///交易所代码			TThostFtdcExchangeIDType	ExchangeID;
				lSqlStatement += " exchange_id varchar2(9),";

				///合约在交易所的代码	TThostFtdcExchangeInstIDType	ExchangeInstID;
				lSqlStatement += " exchange_instrument_id varchar2(31),";

				///最新价				TThostFtdcPriceType	LastPrice;
				lSqlStatement += " last_price number,";				

				///上次结算价			TThostFtdcPriceType	PreSettlementPrice;
				lSqlStatement += " pre_settlement_price number,";

				///昨收盘				TThostFtdcPriceType	PreClosePrice;
				lSqlStatement += " pre_close_price number,";

				///昨持仓量				TThostFtdcLargeVolumeType	PreOpenInterest;
				lSqlStatement += " pre_open_interest number,";

				///今开盘				TThostFtdcPriceType	OpenPrice;
				lSqlStatement += " open_price number,";

				///最高价				TThostFtdcPriceType	HighestPrice;
				lSqlStatement += " highest_price number,";

				///最低价				TThostFtdcPriceType	LowestPrice;
				lSqlStatement += " lowest_price number,";

				///数量					TThostFtdcVolumeType	Volume;
				lSqlStatement += " volume number,";

				///成交金额				TThostFtdcMoneyType	Turnover;
				lSqlStatement += " turnover number,";

				///持仓量				TThostFtdcLargeVolumeType	OpenInterest;
				lSqlStatement += " open_interest number,";

				///今收盘				TThostFtdcPriceType	ClosePrice;
				lSqlStatement += " close_price number,";

				///本次结算价			TThostFtdcPriceType	SettlementPrice;
				lSqlStatement += " settlement_price number,";

				///涨停板价				TThostFtdcPriceType	UpperLimitPrice;
				lSqlStatement += " upper_limit_price number,";

				///跌停板价				TThostFtdcPriceType	LowerLimitPrice;
				lSqlStatement += " lower_limit_price number,";

				///昨虚实度				TThostFtdcRatioType	PreDelta;
				lSqlStatement += " pre_delta number,";

				///今虚实度				TThostFtdcRatioType	CurrDelta;
				lSqlStatement += " curr_delta number,";

				///最后修改时间			TThostFtdcTimeType	UpdateTime;
				lSqlStatement += " update_time varchar2(9),";

				///最后修改毫秒			TThostFtdcMillisecType	UpdateMillisec;
				lSqlStatement += " update_millisec number,";

				///申买价一				TThostFtdcPriceType	BidPrice1;
				lSqlStatement += " bid_price_1 number,";

				///申买量一				TThostFtdcVolumeType	BidVolume1;
				lSqlStatement += " bid_volume_1 number,";

				///申卖价一				TThostFtdcPriceType	AskPrice1;
				lSqlStatement += " ask_price_1 number,";

				///申卖量一				TThostFtdcVolumeType	AskVolume1;
				lSqlStatement += " ask_volume_1 number,";

				///申买价二				TThostFtdcPriceType	BidPrice2;
				lSqlStatement += " bid_price_2 number,";

				///申买量二				TThostFtdcVolumeType	BidVolume2;
				lSqlStatement += " bid_volume_2 number,";

				///申卖价二				TThostFtdcPriceType	AskPrice2;
				lSqlStatement += " ask_price_2 number,";

				///申卖量二				TThostFtdcVolumeType	AskVolume2;
				lSqlStatement += " ask_volume_2 number,";

				///申买价三				TThostFtdcPriceType	BidPrice3;
				lSqlStatement += " bid_price_3 number,";

				///申买量三				TThostFtdcVolumeType	BidVolume3;
				lSqlStatement += " bid_volume_3 number,";

				///申卖价三				TThostFtdcPriceType	AskPrice3;
				lSqlStatement += " ask_price_3 number,";

				///申卖量三				TThostFtdcVolumeType	AskVolume3;
				lSqlStatement += " ask_volume_3 number,";

				///申买价四				TThostFtdcPriceType	BidPrice4;
				lSqlStatement += " bid_price_4 number,";

				///申买量四				TThostFtdcVolumeType	BidVolume4;
				lSqlStatement += " bid_volume_4 number,";

				///申卖价四				TThostFtdcPriceType	AskPrice4;
				lSqlStatement += " ask_price_4 number,";

				///申卖量四				TThostFtdcVolumeType	AskVolume4;
				lSqlStatement += " ask_volume_4 number,";

				///申买价五				TThostFtdcPriceType	BidPrice5;
				lSqlStatement += " bid_price_5 number,";

				///申买量五				TThostFtdcVolumeType	BidVolume5;
				lSqlStatement += " bid_volume_5 number,";

				///申卖价五				TThostFtdcPriceType	AskPrice5;
				lSqlStatement += " ask_price_5 number,";

				///申卖量五				TThostFtdcVolumeType	AskVolume5;
				lSqlStatement += " ask_volume_5 number,";

				///当日均价				TThostFtdcPriceType	AveragePrice;
				lSqlStatement += " average_price number,";

				///业务日期				TThostFtdcDateType	ActionDay;
				lSqlStatement += " action_day varchar2(9)";

				lSqlStatement += ")";
				mStat->executeUpdate(lSqlStatement);
				cout<<"create market data table \""<<aTableName<<"\" successful"<<endl;
				return NO_ERROR; 
			}
			catch(SQLException ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateMarketDataTable(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
				cout<<tempStream.str()<<endl;
				return SQL_EXCEPTION;
			}
			catch(std::exception ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateMarketDataTable(), error message: "<<ex.what();
				cout<<tempStream.str()<<endl;
				return UNKNOWN_EXCEPTION; 
			}
			catch(...)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateMarketDataTable(), error message: unknown";
				cout<<tempStream.str()<<endl;
				return UNKNOWN_EXCEPTION; 
			}
		}
		virtual TRANSACTION_RESULT_TYPE TestCreateMarketDataTable(string aTableName)
		{
			try
			{
				int i=0;
				string lSqlStatement = "CREATE TABLE " + aTableName + "(";

				lSqlStatement += " time_stamp timestamp,";

				///交易日				TThostFtdcDateType	TradingDay;
				lSqlStatement += " trading_day varchar2(9),";	
				
				///当日均价				TThostFtdcPriceType	AveragePrice;
				lSqlStatement += " average_price number";

				lSqlStatement += ")";
				mStat->executeUpdate(lSqlStatement);
				cout<<"create market data table \""<<aTableName<<"\" successful"<<endl;
				return NO_ERROR; 
			}
			catch(SQLException ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateMarketDataTable(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
				cout<<tempStream.str()<<endl;
				return SQL_EXCEPTION;
			}
			catch(std::exception ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateMarketDataTable(), error message: "<<ex.what();
				cout<<tempStream.str()<<endl;
				return UNKNOWN_EXCEPTION; 
			}
			catch(...)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in CreateMarketDataTable(), error message: unknown";
				cout<<tempStream.str()<<endl;
				return UNKNOWN_EXCEPTION; 
			}
		}
		virtual TRANSACTION_RESULT_TYPE InsertMarketData(string aTableName, const CThostFtdcDepthMarketDataField& aMarketData)
		{
			int i=0;
			const string lSqlStatement = "INSERT INTO " + aTableName + " VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23,:24,:25,:26,:27,:28,:29,:30,:31,:32,:33,:34,:35,:36,:37,:38,:39,:40,:41,:42,:43,:44)";
			Statement* lStat = NULL;
			try
			{
				lStat = mConn->createStatement(); 
				if(lStat == NULL) throw(std::exception("create statement failed"));
			
				lStat->setSQL(lSqlStatement);
				oracle::occi::Timestamp lTimeStamp;
				lTimeStamp.fromText("2014-12-25 15:58:59.789000", "yyyy-mm-dd hh24:mi:ss.ff", "", NULL);
				lStat->setTimestamp(++i, lTimeStamp);

				///交易日				TThostFtdcDateType	TradingDay;
				lStat->setString(++i,(string)aMarketData.TradingDay);

				///合约代码				TThostFtdcInstrumentIDType	InstrumentID;
				lStat->setString(++i,(string)aMarketData.InstrumentID);

				///交易所代码			TThostFtdcExchangeIDType	ExchangeID;
				lStat->setString(++i,(string)aMarketData.ExchangeID);

				///合约在交易所的代码	TThostFtdcExchangeInstIDType	ExchangeInstID;
				lStat->setString(++i,(string)aMarketData.ExchangeInstID);

				///最新价				TThostFtdcPriceType	LastPrice;
				lStat->setDouble(++i,aMarketData.LastPrice);

				///上次结算价			TThostFtdcPriceType	PreSettlementPrice;
				lStat->setDouble(++i,aMarketData.PreSettlementPrice);

				///昨收盘				TThostFtdcPriceType	PreClosePrice;
				lStat->setDouble(++i,aMarketData.PreClosePrice);

				///昨持仓量				TThostFtdcLargeVolumeType	PreOpenInterest;
				lStat->setDouble(++i,aMarketData.PreOpenInterest);

				///今开盘				TThostFtdcPriceType	OpenPrice;
				lStat->setDouble(++i,aMarketData.OpenPrice);

				///最高价				TThostFtdcPriceType	HighestPrice;
				lStat->setDouble(++i,aMarketData.HighestPrice);

				///最低价				TThostFtdcPriceType	LowestPrice;
				lStat->setDouble(++i,aMarketData.LowestPrice);

				///数量					TThostFtdcVolumeType	Volume;
				lStat->setInt(++i,aMarketData.Volume);

				///成交金额				TThostFtdcMoneyType	Turnover;
				lStat->setDouble(++i,aMarketData.Turnover);

				///持仓量				TThostFtdcLargeVolumeType	OpenInterest;
				lStat->setDouble(++i, aMarketData.OpenInterest);

				///今收盘				TThostFtdcPriceType	ClosePrice;
				lStat->setDouble(++i, aMarketData.ClosePrice);

				///本次结算价			TThostFtdcPriceType	SettlementPrice;
				lStat->setDouble(++i, aMarketData.SettlementPrice);

				///涨停板价				TThostFtdcPriceType	UpperLimitPrice;
				lStat->setDouble(++i, aMarketData.UpperLimitPrice);

				///跌停板价				TThostFtdcPriceType	LowerLimitPrice;
				lStat->setDouble(++i, aMarketData.LowerLimitPrice);

				///昨虚实度				TThostFtdcRatioType	PreDelta;
				lStat->setDouble(++i, aMarketData.PreDelta);

				///今虚实度				TThostFtdcRatioType	CurrDelta;
				lStat->setDouble(++i, aMarketData.CurrDelta);

				///最后修改时间			TThostFtdcTimeType	UpdateTime;
				lStat->setString(++i, (string)aMarketData.UpdateTime);

				///最后修改毫秒			TThostFtdcMillisecType	UpdateMillisec;
				lStat->setInt(++i, aMarketData.UpdateMillisec);

				///申买价一				TThostFtdcPriceType	BidPrice1;
				lStat->setDouble(++i, aMarketData.BidPrice1);

				///申买量一				TThostFtdcVolumeType	BidVolume1;
				lStat->setInt(++i, aMarketData.BidVolume1);

				///申卖价一				TThostFtdcPriceType	AskPrice1;
				lStat->setDouble(++i, aMarketData.AskPrice1);

				///申卖量一				TThostFtdcVolumeType	AskVolume1;
				lStat->setInt(++i, aMarketData.AskVolume1);

				///申买价二				TThostFtdcPriceType	BidPrice2;
				lStat->setDouble(++i, aMarketData.BidPrice2);

				///申买量二				TThostFtdcVolumeType	BidVolume2;
				lStat->setInt(++i, aMarketData.BidVolume2);

				///申卖价二				TThostFtdcPriceType	AskPrice2;
				lStat->setDouble(++i, aMarketData.AskPrice2);

				///申卖量二				TThostFtdcVolumeType	AskVolume2;
				lStat->setInt(++i, aMarketData.AskVolume2);

				///申买价三				TThostFtdcPriceType	BidPrice3;
				lStat->setDouble(++i, aMarketData.BidPrice3);

				///申买量三				TThostFtdcVolumeType	BidVolume3;
				lStat->setInt(++i, aMarketData.BidVolume3);

				///申卖价三				TThostFtdcPriceType	AskPrice3;
				lStat->setDouble(++i, aMarketData.AskPrice3);

				///申卖量三				TThostFtdcVolumeType	AskVolume3;
				lStat->setInt(++i, aMarketData.AskVolume3);

				///申买价四				TThostFtdcPriceType	BidPrice4;
				lStat->setDouble(++i, aMarketData.BidPrice4);

				///申买量四				TThostFtdcVolumeType	BidVolume4;
				lStat->setInt(++i, aMarketData.BidVolume4);

				///申卖价四				TThostFtdcPriceType	AskPrice4;
				lStat->setDouble(++i, aMarketData.AskPrice4);

				///申卖量四				TThostFtdcVolumeType	AskVolume4;
				lStat->setInt(++i, aMarketData.AskVolume4);

				///申买价五				TThostFtdcPriceType	BidPrice5;
				lStat->setDouble(++i, aMarketData.BidPrice5);

				///申买量五				TThostFtdcVolumeType	BidVolume5;
				lStat->setInt(++i, aMarketData.BidVolume5);

				///申卖价五				TThostFtdcPriceType	AskPrice5;
				lStat->setDouble(++i, aMarketData.AskPrice5);

				///申卖量五				TThostFtdcVolumeType	AskVolume5;
				lStat->setInt(++i, aMarketData.AskVolume5);

				///当日均价				TThostFtdcPriceType	AveragePrice;
				lStat->setDouble(++i, aMarketData.AveragePrice);

				///业务日期				TThostFtdcDateType	ActionDay;
				lStat->setString(++i, (string)aMarketData.ActionDay);

				lStat->executeUpdate();
				cout<<"insert market data successful"<<endl;
				return NO_ERROR;
			}
			catch(SQLException ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in InsertMarketData(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
				cout<<tempStream.str()<<endl;
				if(lStat != NULL) mConn->terminateStatement(lStat);
				return SQL_EXCEPTION;
			}
			catch(std::exception ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in InsertMarketData(), error message: "<<ex.what();
				cout<<tempStream.str()<<endl;
				if(lStat != NULL) mConn->terminateStatement(lStat);
				return UNKNOWN_EXCEPTION; 
			}
			catch(...)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in InsertMarketData(), error message: unknown";
				cout<<tempStream.str()<<endl;
				if(lStat != NULL) mConn->terminateStatement(lStat);
				return UNKNOWN_EXCEPTION; 
			}
		}
		virtual TRANSACTION_RESULT_TYPE TestInsertMarketData(string aTableName, const CThostFtdcDepthMarketDataField& aMarketData)
		{
			int i=0;
			const string lSqlStatement = "INSERT INTO " + aTableName + " VALUES (:1,:2,:3)";
			Statement* lStat = NULL;
			try
			{
				lStat = mConn->createStatement(); 
				if(lStat == NULL) throw(std::exception("create statement failed"));
			
				lStat->setSQL(lSqlStatement);
				oracle::occi::Timestamp lTimeStamp;
				lTimeStamp.fromText("2014-12-25 15:58:59.789000", "yyyy-mm-dd hh24:mi:ss.ff", "", mEnv);
				lStat->setTimestamp(++i, lTimeStamp);

				///交易日				TThostFtdcDateType	TradingDay;
				lStat->setString(++i,(string)aMarketData.TradingDay);

				///当日均价				TThostFtdcPriceType	AveragePrice;
				lStat->setDouble(++i, aMarketData.AveragePrice);

				lStat->executeUpdate();
				mConn->commit();
				cout<<"insert market data successful"<<endl;
				return NO_ERROR;
			}
			catch(SQLException ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in InsertMarketData(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
				cout<<tempStream.str()<<endl;
				if(lStat != NULL) mConn->terminateStatement(lStat);
				return SQL_EXCEPTION;
			}
			catch(std::exception ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in InsertMarketData(), error message: "<<ex.what();
				cout<<tempStream.str()<<endl;
				if(lStat != NULL) mConn->terminateStatement(lStat);
				return UNKNOWN_EXCEPTION; 
			}
			catch(...)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in InsertMarketData(), error message: unknown";
				cout<<tempStream.str()<<endl;
				if(lStat != NULL) mConn->terminateStatement(lStat);
				return UNKNOWN_EXCEPTION; 
			}
		}
		virtual TRANSACTION_RESULT_TYPE TestQueryMarketData(string aTableName, CThostFtdcDepthMarketDataField& aMarketData)
		{
			try
			{
				oracle::occi::ResultSet* lResultSet = mStat->executeQuery("select * from "+aTableName);
				lResultSet->next();
				Timestamp ts = lResultSet->getTimestamp(1);
				if(!ts.isNull())
				{
					//string lTsString = ts.toText("dd/mm/yyyy hh:mi:ss [tzh:tzm]", 0, NULL);
					string tsstr=ts.toText("yyyy-mm-dd hh24:mi:ss.ff",6);
					cout<<tsstr<<endl;
				}
				return NO_ERROR;
			}
			catch(SQLException ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in QueryMarketData(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
				cout<<tempStream.str()<<endl;
				return SQL_EXCEPTION;
			}
			catch(std::exception ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in QueryMarketData(), error message: "<<ex.what();
				cout<<tempStream.str()<<endl;
				return UNKNOWN_EXCEPTION; 
			}
			catch(...)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in QueryMarketData(), error message: unknown";
				cout<<tempStream.str()<<endl;
				return UNKNOWN_EXCEPTION; 
			}
		}
		virtual TRANSACTION_RESULT_TYPE TestCreateMarketDataType(string aTypeName)
		{
			try
			{
				int i=0;
				string lSqlStatement = "CREATE TYPE " + aTypeName + " AS OBJECT(";

				lSqlStatement += " time_stamp timestamp,";

				///交易日				TThostFtdcDateType	TradingDay;
				lSqlStatement += " trading_day varchar2(9),";	
				
				///当日均价				TThostFtdcPriceType	AveragePrice;
				lSqlStatement += " average_price number";

				lSqlStatement += ")";
				mStat->executeUpdate(lSqlStatement);
				cout<<"create market data type \""<<aTypeName<<"\" successful"<<endl;
				return NO_ERROR; 
			}
			catch(SQLException ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in TestCreateMarketDataType(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
				cout<<tempStream.str()<<endl;
				return SQL_EXCEPTION;
			}
			catch(std::exception ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in TestCreateMarketDataType(), error message: "<<ex.what();
				cout<<tempStream.str()<<endl;
				return UNKNOWN_EXCEPTION; 
			}
			catch(...)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in TestCreateMarketDataType(), error message: unknown";
				cout<<tempStream.str()<<endl;
				return UNKNOWN_EXCEPTION; 
			}
		}
		virtual TRANSACTION_RESULT_TYPE TestCreateMarketDataTableFromType(string aTableName, string aTypeName)
		{
			try
			{
				int i=0;
				string lSqlStatement = "CREATE TABLE " + aTableName + " of " + aTypeName;
				mStat->executeUpdate(lSqlStatement);
				cout<<"create market data type \""<<aTableName<<"\" successful"<<endl;
				return NO_ERROR; 
			}
			catch(SQLException ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in TestCreateMarketDataTableFromType(), error message: "<<ex.getMessage()<<" error code: "<<ex.getErrorCode();
				cout<<tempStream.str()<<endl;
				return SQL_EXCEPTION;
			}
			catch(std::exception ex)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in TestCreateMarketDataTableFromType(), error message: "<<ex.what();
				cout<<tempStream.str()<<endl;
				return UNKNOWN_EXCEPTION; 
			}
			catch(...)
			{
				std::stringstream tempStream;
				tempStream.str("");	
				tempStream<<"exception in TestCreateMarketDataTableFromType(), error message: unknown";
				cout<<tempStream.str()<<endl;
				return UNKNOWN_EXCEPTION; 
			}
		}
	};
}