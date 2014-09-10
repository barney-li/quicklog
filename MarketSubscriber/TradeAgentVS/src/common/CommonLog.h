#pragma once
#include "Log.h"
using namespace std;
class CommonLog : public Log
{
public:
	CommonLog(string aLogName = "log.log"):Log("data/Log/", aLogName, 32, 10)
	{
		;	
	}
};