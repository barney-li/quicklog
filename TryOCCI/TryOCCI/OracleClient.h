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

				///������				TThostFtdcDateType	TradingDay;
				lSqlStatement += " trading_day varchar2(9),";	
				
				///��Լ����				TThostFtdcInstrumentIDType	InstrumentID;
				lSqlStatement += " instrument_id varchar2(11),";

				///����������			TThostFtdcExchangeIDType	ExchangeID;
				lSqlStatement += " exchange_id varchar2(9),";

				///��Լ�ڽ������Ĵ���	TThostFtdcExchangeInstIDType	ExchangeInstID;
				lSqlStatement += " exchange_instrument_id varchar2(31),";

				///���¼�				TThostFtdcPriceType	LastPrice;
				lSqlStatement += " last_price number,";				

				///�ϴν����			TThostFtdcPriceType	PreSettlementPrice;
				lSqlStatement += " pre_settlement_price number,";

				///������				TThostFtdcPriceType	PreClosePrice;
				lSqlStatement += " pre_close_price number,";

				///��ֲ���				TThostFtdcLargeVolumeType	PreOpenInterest;
				lSqlStatement += " pre_open_interest number,";

				///����				TThostFtdcPriceType	OpenPrice;
				lSqlStatement += " open_price number,";

				///��߼�				TThostFtdcPriceType	HighestPrice;
				lSqlStatement += " highest_price number,";

				///��ͼ�				TThostFtdcPriceType	LowestPrice;
				lSqlStatement += " lowest_price number,";

				///����					TThostFtdcVolumeType	Volume;
				lSqlStatement += " volume number,";

				///�ɽ����				TThostFtdcMoneyType	Turnover;
				lSqlStatement += " turnover number,";

				///�ֲ���				TThostFtdcLargeVolumeType	OpenInterest;
				lSqlStatement += " open_interest number,";

				///������				TThostFtdcPriceType	ClosePrice;
				lSqlStatement += " close_price number,";

				///���ν����			TThostFtdcPriceType	SettlementPrice;
				lSqlStatement += " settlement_price number,";

				///��ͣ���				TThostFtdcPriceType	UpperLimitPrice;
				lSqlStatement += " upper_limit_price number,";

				///��ͣ���				TThostFtdcPriceType	LowerLimitPrice;
				lSqlStatement += " lower_limit_price number,";

				///����ʵ��				TThostFtdcRatioType	PreDelta;
				lSqlStatement += " pre_delta number,";

				///����ʵ��				TThostFtdcRatioType	CurrDelta;
				lSqlStatement += " curr_delta number,";

				///����޸�ʱ��			TThostFtdcTimeType	UpdateTime;
				lSqlStatement += " update_time varchar2(9),";

				///����޸ĺ���			TThostFtdcMillisecType	UpdateMillisec;
				lSqlStatement += " update_millisec number,";

				///�����һ				TThostFtdcPriceType	BidPrice1;
				lSqlStatement += " bid_price_1 number,";

				///������һ				TThostFtdcVolumeType	BidVolume1;
				lSqlStatement += " bid_volume_1 number,";

				///������һ				TThostFtdcPriceType	AskPrice1;
				lSqlStatement += " ask_price_1 number,";

				///������һ				TThostFtdcVolumeType	AskVolume1;
				lSqlStatement += " ask_volume_1 number,";

				///����۶�				TThostFtdcPriceType	BidPrice2;
				lSqlStatement += " bid_price_2 number,";

				///��������				TThostFtdcVolumeType	BidVolume2;
				lSqlStatement += " bid_volume_2 number,";

				///�����۶�				TThostFtdcPriceType	AskPrice2;
				lSqlStatement += " ask_price_2 number,";

				///��������				TThostFtdcVolumeType	AskVolume2;
				lSqlStatement += " ask_volume_2 number,";

				///�������				TThostFtdcPriceType	BidPrice3;
				lSqlStatement += " bid_price_3 number,";

				///��������				TThostFtdcVolumeType	BidVolume3;
				lSqlStatement += " bid_volume_3 number,";

				///��������				TThostFtdcPriceType	AskPrice3;
				lSqlStatement += " ask_price_3 number,";

				///��������				TThostFtdcVolumeType	AskVolume3;
				lSqlStatement += " ask_volume_3 number,";

				///�������				TThostFtdcPriceType	BidPrice4;
				lSqlStatement += " bid_price_4 number,";

				///��������				TThostFtdcVolumeType	BidVolume4;
				lSqlStatement += " bid_volume_4 number,";

				///��������				TThostFtdcPriceType	AskPrice4;
				lSqlStatement += " ask_price_4 number,";

				///��������				TThostFtdcVolumeType	AskVolume4;
				lSqlStatement += " ask_volume_4 number,";

				///�������				TThostFtdcPriceType	BidPrice5;
				lSqlStatement += " bid_price_5 number,";

				///��������				TThostFtdcVolumeType	BidVolume5;
				lSqlStatement += " bid_volume_5 number,";

				///��������				TThostFtdcPriceType	AskPrice5;
				lSqlStatement += " ask_price_5 number,";

				///��������				TThostFtdcVolumeType	AskVolume5;
				lSqlStatement += " ask_volume_5 number,";

				///���վ���				TThostFtdcPriceType	AveragePrice;
				lSqlStatement += " average_price number,";

				///ҵ������				TThostFtdcDateType	ActionDay;
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

				///������				TThostFtdcDateType	TradingDay;
				lSqlStatement += " trading_day varchar2(9),";	
				
				///���վ���				TThostFtdcPriceType	AveragePrice;
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

				///������				TThostFtdcDateType	TradingDay;
				lStat->setString(++i,(string)aMarketData.TradingDay);

				///��Լ����				TThostFtdcInstrumentIDType	InstrumentID;
				lStat->setString(++i,(string)aMarketData.InstrumentID);

				///����������			TThostFtdcExchangeIDType	ExchangeID;
				lStat->setString(++i,(string)aMarketData.ExchangeID);

				///��Լ�ڽ������Ĵ���	TThostFtdcExchangeInstIDType	ExchangeInstID;
				lStat->setString(++i,(string)aMarketData.ExchangeInstID);

				///���¼�				TThostFtdcPriceType	LastPrice;
				lStat->setDouble(++i,aMarketData.LastPrice);

				///�ϴν����			TThostFtdcPriceType	PreSettlementPrice;
				lStat->setDouble(++i,aMarketData.PreSettlementPrice);

				///������				TThostFtdcPriceType	PreClosePrice;
				lStat->setDouble(++i,aMarketData.PreClosePrice);

				///��ֲ���				TThostFtdcLargeVolumeType	PreOpenInterest;
				lStat->setDouble(++i,aMarketData.PreOpenInterest);

				///����				TThostFtdcPriceType	OpenPrice;
				lStat->setDouble(++i,aMarketData.OpenPrice);

				///��߼�				TThostFtdcPriceType	HighestPrice;
				lStat->setDouble(++i,aMarketData.HighestPrice);

				///��ͼ�				TThostFtdcPriceType	LowestPrice;
				lStat->setDouble(++i,aMarketData.LowestPrice);

				///����					TThostFtdcVolumeType	Volume;
				lStat->setInt(++i,aMarketData.Volume);

				///�ɽ����				TThostFtdcMoneyType	Turnover;
				lStat->setDouble(++i,aMarketData.Turnover);

				///�ֲ���				TThostFtdcLargeVolumeType	OpenInterest;
				lStat->setDouble(++i, aMarketData.OpenInterest);

				///������				TThostFtdcPriceType	ClosePrice;
				lStat->setDouble(++i, aMarketData.ClosePrice);

				///���ν����			TThostFtdcPriceType	SettlementPrice;
				lStat->setDouble(++i, aMarketData.SettlementPrice);

				///��ͣ���				TThostFtdcPriceType	UpperLimitPrice;
				lStat->setDouble(++i, aMarketData.UpperLimitPrice);

				///��ͣ���				TThostFtdcPriceType	LowerLimitPrice;
				lStat->setDouble(++i, aMarketData.LowerLimitPrice);

				///����ʵ��				TThostFtdcRatioType	PreDelta;
				lStat->setDouble(++i, aMarketData.PreDelta);

				///����ʵ��				TThostFtdcRatioType	CurrDelta;
				lStat->setDouble(++i, aMarketData.CurrDelta);

				///����޸�ʱ��			TThostFtdcTimeType	UpdateTime;
				lStat->setString(++i, (string)aMarketData.UpdateTime);

				///����޸ĺ���			TThostFtdcMillisecType	UpdateMillisec;
				lStat->setInt(++i, aMarketData.UpdateMillisec);

				///�����һ				TThostFtdcPriceType	BidPrice1;
				lStat->setDouble(++i, aMarketData.BidPrice1);

				///������һ				TThostFtdcVolumeType	BidVolume1;
				lStat->setInt(++i, aMarketData.BidVolume1);

				///������һ				TThostFtdcPriceType	AskPrice1;
				lStat->setDouble(++i, aMarketData.AskPrice1);

				///������һ				TThostFtdcVolumeType	AskVolume1;
				lStat->setInt(++i, aMarketData.AskVolume1);

				///����۶�				TThostFtdcPriceType	BidPrice2;
				lStat->setDouble(++i, aMarketData.BidPrice2);

				///��������				TThostFtdcVolumeType	BidVolume2;
				lStat->setInt(++i, aMarketData.BidVolume2);

				///�����۶�				TThostFtdcPriceType	AskPrice2;
				lStat->setDouble(++i, aMarketData.AskPrice2);

				///��������				TThostFtdcVolumeType	AskVolume2;
				lStat->setInt(++i, aMarketData.AskVolume2);

				///�������				TThostFtdcPriceType	BidPrice3;
				lStat->setDouble(++i, aMarketData.BidPrice3);

				///��������				TThostFtdcVolumeType	BidVolume3;
				lStat->setInt(++i, aMarketData.BidVolume3);

				///��������				TThostFtdcPriceType	AskPrice3;
				lStat->setDouble(++i, aMarketData.AskPrice3);

				///��������				TThostFtdcVolumeType	AskVolume3;
				lStat->setInt(++i, aMarketData.AskVolume3);

				///�������				TThostFtdcPriceType	BidPrice4;
				lStat->setDouble(++i, aMarketData.BidPrice4);

				///��������				TThostFtdcVolumeType	BidVolume4;
				lStat->setInt(++i, aMarketData.BidVolume4);

				///��������				TThostFtdcPriceType	AskPrice4;
				lStat->setDouble(++i, aMarketData.AskPrice4);

				///��������				TThostFtdcVolumeType	AskVolume4;
				lStat->setInt(++i, aMarketData.AskVolume4);

				///�������				TThostFtdcPriceType	BidPrice5;
				lStat->setDouble(++i, aMarketData.BidPrice5);

				///��������				TThostFtdcVolumeType	BidVolume5;
				lStat->setInt(++i, aMarketData.BidVolume5);

				///��������				TThostFtdcPriceType	AskPrice5;
				lStat->setDouble(++i, aMarketData.AskPrice5);

				///��������				TThostFtdcVolumeType	AskVolume5;
				lStat->setInt(++i, aMarketData.AskVolume5);

				///���վ���				TThostFtdcPriceType	AveragePrice;
				lStat->setDouble(++i, aMarketData.AveragePrice);

				///ҵ������				TThostFtdcDateType	ActionDay;
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

				///������				TThostFtdcDateType	TradingDay;
				lStat->setString(++i,(string)aMarketData.TradingDay);

				///���վ���				TThostFtdcPriceType	AveragePrice;
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

				///������				TThostFtdcDateType	TradingDay;
				lSqlStatement += " trading_day varchar2(9),";	
				
				///���վ���				TThostFtdcPriceType	AveragePrice;
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