#pragma once
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
using namespace std;
class TryException
{
public:
	TryException()
	{
		;
	}
	~TryException()
	{
		;
	}
	int LookingForTrouble()
	{
		try
		{
			int a;
			cout<<"address of current stack: "<<&a<<endl;
			cout<<"looking for trouble..."<<endl;
			throw exception("my exception");
			int* pInt = NULL;
			*pInt = 1;
			return 2014;
		}
		catch(std::exception ex)
		{
			cout<<"exception in LookingForTrouble: "<<ex.what()<<endl;
			//throw exception(ex);
			//return false;
		}
		catch(...)
		{
			cout<<"exception in LookingForTrouble: unkonwn"<<endl;
			//throw exception();
			//return false;
		}
	}
};