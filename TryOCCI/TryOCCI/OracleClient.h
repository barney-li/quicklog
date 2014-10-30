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

				///������				TThostFtdcDateType	TradingDay;
				mStat->setString(++i,(string)aMarketData.TradingDay);

				///��Լ����				TThostFtdcInstrumentIDType	InstrumentID;
				mStat->setString(++i,(string)aMarketData.InstrumentID);

				///����������			TThostFtdcExchangeIDType	ExchangeID;
				mStat->setString(++i,(string)aMarketData.ExchangeID);

				///��Լ�ڽ������Ĵ���	TThostFtdcExchangeInstIDType	ExchangeInstID;
				mStat->setString(++i,(string)aMarketData.ExchangeInstID);

				///���¼�				TThostFtdcPriceType	LastPrice;
				mStat->setDouble(++i,aMarketData.LastPrice);

				///�ϴν����			TThostFtdcPriceType	PreSettlementPrice;
				mStat->setDouble(++i,aMarketData.PreSettlementPrice);

				///������				TThostFtdcPriceType	PreClosePrice;
				mStat->setDouble(++i,aMarketData.PreClosePrice);

				///��ֲ���				TThostFtdcLargeVolumeType	PreOpenInterest;
				mStat->setDouble(++i,aMarketData.PreOpenInterest);

				///����				TThostFtdcPriceType	OpenPrice;
				mStat->setDouble(++i,aMarketData.OpenPrice);

				///��߼�				TThostFtdcPriceType	HighestPrice;
				mStat->setDouble(++i,aMarketData.HighestPrice);

				///��ͼ�				TThostFtdcPriceType	LowestPrice;
				mStat->setDouble(++i,aMarketData.LowestPrice);

				///����					TThostFtdcVolumeType	Volume;
				mStat->setInt(++i,aMarketData.Volume);

				///�ɽ����				TThostFtdcMoneyType	Turnover;
				mStat->setDouble(++i,aMarketData.Turnover);

				///�ֲ���				TThostFtdcLargeVolumeType	OpenInterest;
				mStat->setDouble(++i, aMarketData.OpenInterest);

				///������				TThostFtdcPriceType	ClosePrice;
				mStat->setDouble(++i, aMarketData.ClosePrice);

				///���ν����			TThostFtdcPriceType	SettlementPrice;
				mStat->setDouble(++i, aMarketData.SettlementPrice);

				///��ͣ���				TThostFtdcPriceType	UpperLimitPrice;
				mStat->setDouble(++i, aMarketData.UpperLimitPrice);

				///��ͣ���				TThostFtdcPriceType	LowerLimitPrice;
				mStat->setDouble(++i, aMarketData.LowerLimitPrice);

				///����ʵ��				TThostFtdcRatioType	PreDelta;
				mStat->setDouble(++i, aMarketData.PreDelta);

				///����ʵ��				TThostFtdcRatioType	CurrDelta;
				mStat->setDouble(++i, aMarketData.CurrDelta);

				///����޸�ʱ��			TThostFtdcTimeType	UpdateTime;
				mStat->setString(++i, (string)aMarketData.UpdateTime);

				///����޸ĺ���			TThostFtdcMillisecType	UpdateMillisec;
				mStat->setInt(++i, aMarketData.UpdateMillisec);

				///�����һ				TThostFtdcPriceType	BidPrice1;
				mStat->setDouble(++i, aMarketData.BidPrice1);

				///������һ				TThostFtdcVolumeType	BidVolume1;
				mStat->setInt(++i, aMarketData.BidVolume1);

				///������һ				TThostFtdcPriceType	AskPrice1;
				mStat->setDouble(++i, aMarketData.AskPrice1);

				///������һ				TThostFtdcVolumeType	AskVolume1;
				mStat->setInt(++i, aMarketData.AskVolume1);

				///����۶�				TThostFtdcPriceType	BidPrice2;
				mStat->setDouble(++i, aMarketData.BidPrice2);

				///��������				TThostFtdcVolumeType	BidVolume2;
				mStat->setInt(++i, aMarketData.BidVolume2);

				///�����۶�				TThostFtdcPriceType	AskPrice2;
				mStat->setDouble(++i, aMarketData.AskPrice2);

				///��������				TThostFtdcVolumeType	AskVolume2;
				mStat->setInt(++i, aMarketData.AskVolume2);

				///�������				TThostFtdcPriceType	BidPrice3;
				mStat->setDouble(++i, aMarketData.BidPrice3);

				///��������				TThostFtdcVolumeType	BidVolume3;
				mStat->setInt(++i, aMarketData.BidVolume3);

				///��������				TThostFtdcPriceType	AskPrice3;
				mStat->setDouble(++i, aMarketData.AskPrice3);

				///��������				TThostFtdcVolumeType	AskVolume3;
				mStat->setInt(++i, aMarketData.AskVolume3);

				///�������				TThostFtdcPriceType	BidPrice4;
				mStat->setDouble(++i, aMarketData.BidPrice4);

				///��������				TThostFtdcVolumeType	BidVolume4;
				mStat->setInt(++i, aMarketData.BidVolume4);

				///��������				TThostFtdcPriceType	AskPrice4;
				mStat->setDouble(++i, aMarketData.AskPrice4);

				///��������				TThostFtdcVolumeType	AskVolume4;
				mStat->setInt(++i, aMarketData.AskVolume4);

				///�������				TThostFtdcPriceType	BidPrice5;
				mStat->setDouble(++i, aMarketData.BidPrice5);

				///��������				TThostFtdcVolumeType	BidVolume5;
				mStat->setInt(++i, aMarketData.BidVolume5);

				///��������				TThostFtdcPriceType	AskPrice5;
				mStat->setDouble(++i, aMarketData.AskPrice5);

				///��������				TThostFtdcVolumeType	AskVolume5;
				mStat->setInt(++i, aMarketData.AskVolume5);

				///���վ���				TThostFtdcPriceType	AveragePrice;
				mStat->setDouble(++i, aMarketData.AveragePrice);

				///ҵ������				TThostFtdcDateType	ActionDay;
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

				///������				TThostFtdcDateType	TradingDay;
				mStat->setString(++i,(string)aMarketData.TradingDay);

				///��Լ����				TThostFtdcInstrumentIDType	InstrumentID;
				mStat->setString(++i,(string)aMarketData.InstrumentID);

				///����������			TThostFtdcExchangeIDType	ExchangeID;
				mStat->setString(++i,(string)aMarketData.ExchangeID);

				///��Լ�ڽ������Ĵ���	TThostFtdcExchangeInstIDType	ExchangeInstID;
				mStat->setString(++i,(string)aMarketData.ExchangeInstID);

				///���¼�				TThostFtdcPriceType	LastPrice;
				mStat->setDouble(++i,aMarketData.LastPrice);

				///�ϴν����			TThostFtdcPriceType	PreSettlementPrice;
				mStat->setDouble(++i,aMarketData.PreSettlementPrice);

				///������				TThostFtdcPriceType	PreClosePrice;
				mStat->setDouble(++i,aMarketData.PreClosePrice);

				///��ֲ���				TThostFtdcLargeVolumeType	PreOpenInterest;
				mStat->setDouble(++i,aMarketData.PreOpenInterest);

				///����				TThostFtdcPriceType	OpenPrice;
				mStat->setDouble(++i,aMarketData.OpenPrice);

				///��߼�				TThostFtdcPriceType	HighestPrice;
				mStat->setDouble(++i,aMarketData.HighestPrice);

				///��ͼ�				TThostFtdcPriceType	LowestPrice;
				mStat->setDouble(++i,aMarketData.LowestPrice);

				///����					TThostFtdcVolumeType	Volume;
				mStat->setInt(++i,aMarketData.Volume);

				///�ɽ����				TThostFtdcMoneyType	Turnover;
				mStat->setDouble(++i,aMarketData.Turnover);

				///�ֲ���				TThostFtdcLargeVolumeType	OpenInterest;
				mStat->setDouble(++i, aMarketData.OpenInterest);

				///������				TThostFtdcPriceType	ClosePrice;
				mStat->setDouble(++i, aMarketData.ClosePrice);

				///���ν����			TThostFtdcPriceType	SettlementPrice;
				mStat->setDouble(++i, aMarketData.SettlementPrice);

				///��ͣ���				TThostFtdcPriceType	UpperLimitPrice;
				mStat->setDouble(++i, aMarketData.UpperLimitPrice);

				///��ͣ���				TThostFtdcPriceType	LowerLimitPrice;
				mStat->setDouble(++i, aMarketData.LowerLimitPrice);

				///����ʵ��				TThostFtdcRatioType	PreDelta;
				mStat->setDouble(++i, aMarketData.PreDelta);

				///����ʵ��				TThostFtdcRatioType	CurrDelta;
				mStat->setDouble(++i, aMarketData.CurrDelta);

				///����޸�ʱ��			TThostFtdcTimeType	UpdateTime;
				mStat->setString(++i, (string)aMarketData.UpdateTime);

				///����޸ĺ���			TThostFtdcMillisecType	UpdateMillisec;
				mStat->setInt(++i, aMarketData.UpdateMillisec);

				///�����һ				TThostFtdcPriceType	BidPrice1;
				mStat->setDouble(++i, aMarketData.BidPrice1);

				///������һ				TThostFtdcVolumeType	BidVolume1;
				mStat->setInt(++i, aMarketData.BidVolume1);

				///������һ				TThostFtdcPriceType	AskPrice1;
				mStat->setDouble(++i, aMarketData.AskPrice1);

				///������һ				TThostFtdcVolumeType	AskVolume1;
				mStat->setInt(++i, aMarketData.AskVolume1);

				///����۶�				TThostFtdcPriceType	BidPrice2;
				mStat->setDouble(++i, aMarketData.BidPrice2);

				///��������				TThostFtdcVolumeType	BidVolume2;
				mStat->setInt(++i, aMarketData.BidVolume2);

				///�����۶�				TThostFtdcPriceType	AskPrice2;
				mStat->setDouble(++i, aMarketData.AskPrice2);

				///��������				TThostFtdcVolumeType	AskVolume2;
				mStat->setInt(++i, aMarketData.AskVolume2);

				///�������				TThostFtdcPriceType	BidPrice3;
				mStat->setDouble(++i, aMarketData.BidPrice3);

				///��������				TThostFtdcVolumeType	BidVolume3;
				mStat->setInt(++i, aMarketData.BidVolume3);

				///��������				TThostFtdcPriceType	AskPrice3;
				mStat->setDouble(++i, aMarketData.AskPrice3);

				///��������				TThostFtdcVolumeType	AskVolume3;
				mStat->setInt(++i, aMarketData.AskVolume3);

				///�������				TThostFtdcPriceType	BidPrice4;
				mStat->setDouble(++i, aMarketData.BidPrice4);

				///��������				TThostFtdcVolumeType	BidVolume4;
				mStat->setInt(++i, aMarketData.BidVolume4);

				///��������				TThostFtdcPriceType	AskPrice4;
				mStat->setDouble(++i, aMarketData.AskPrice4);

				///��������				TThostFtdcVolumeType	AskVolume4;
				mStat->setInt(++i, aMarketData.AskVolume4);

				///�������				TThostFtdcPriceType	BidPrice5;
				mStat->setDouble(++i, aMarketData.BidPrice5);

				///��������				TThostFtdcVolumeType	BidVolume5;
				mStat->setInt(++i, aMarketData.BidVolume5);

				///��������				TThostFtdcPriceType	AskPrice5;
				mStat->setDouble(++i, aMarketData.AskPrice5);

				///��������				TThostFtdcVolumeType	AskVolume5;
				mStat->setInt(++i, aMarketData.AskVolume5);

				///���վ���				TThostFtdcPriceType	AveragePrice;
				mStat->setDouble(++i, aMarketData.AveragePrice);

				///ҵ������				TThostFtdcDateType	ActionDay;
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