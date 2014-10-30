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
		if(obj.ConnectConnection((string)"c##barney",(string)"Lml19870310",(string)"//192.168.0.107:1521/barneydb") == NO_ERROR)
		{
			std::cout<<"connection established"<<endl;
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

