#pragma once
#include <iostream>
#include <excpt.h>
#include <Windows.h>
using namespace std;
class FancierExceptionTest
{
public:
	string PrintException(DWORD aExceptionCode)
	{
		switch(aExceptionCode)
		{
			case EXCEPTION_ACCESS_VIOLATION:
				return "EXCEPTION_ACCESS_VIOLATION";
				break;
			default:
				return "UNKNOWN_EXCEPTION";
		}

	}
	bool TryException()
	{
		__try
		{
			int* a=NULL;
			*a=0;
		}
		__except(true)
		{
			cout<<GetExceptionCode()<<endl;
		}
		//__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
			cout<<"got error"<<endl;
			return false;
		}
		//__finally
		{
			cout<<"finally statement"<<endl;
		}
		return true;
	}
};