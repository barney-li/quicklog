// TryOCCI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "OracleClient.h"
#include "TestClass.h"
#include <iostream>
using namespace std;
using namespace DatabaseUtilities;
int _tmain(int argc, _TCHAR* argv[])
{
	cout<<"try occi started"<<endl;
	try
	{
		DatabaseUtilities::OracleClient obj;
		if(obj.Connect((string)"c##barney",(string)"Lml19870310",(string)"//192.168.183.128:1521/barneydb") == NO_ERROR)
		{
			std::cout<<"connection established"<<endl;
			/*if(obj.TestCreateMarketDataTable("jd1406") == NO_ERROR)
			{
				std::cout<<"create market data table successed"<<endl;
			}
			else
			{
				std::cout<<"create market data table failed"<<endl;
			}*/
			/*CThostFtdcDepthMarketDataField lMarketData;
			memcpy(&lMarketData.TradingDay, "20141225", 9);
			lMarketData.AveragePrice = 1234.5678;
			obj.TestInsertMarketData("jd1406", lMarketData);
			obj.TestQueryMarketData("jd1406", lMarketData);*/
			//obj.TestCreateMarketDataType("TestMarketDataType");
			obj.TestCreateMarketDataTableFromType("jd1407", "TestMarketDataType");
		}
		else
		{
			std::cout<<"connection failed"<<endl;
		}
		
		
	}
	catch (...)
	{
		std::cout<<"something was wrong"<<std::endl;
	}
	return 0;
}

