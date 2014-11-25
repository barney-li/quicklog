// ExceptionTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TryException.h"
#include "FancierExceptionTest.h"
int _tmain(int argc, _TCHAR* argv[])
{
	FancierExceptionTest obj;
	obj.TryException();
	//try
	{
	TryException testobj = TryException();
	int i = testobj.LookingForTrouble();
	cout<<i<<endl;
	}
	//catch(std::exception ex)
	{
	//	cout<<"exception in main: "<<ex.what()<<endl;
	}
	//catch(...)
	{
	//	cout<<"exception in main: unkonwn"<<endl;
	}
	return 0;
}

