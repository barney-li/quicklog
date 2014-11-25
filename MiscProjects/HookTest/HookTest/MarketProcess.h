#pragma once
#include "basicclass.h"
class MarketProcess :
	public BasicClass
{
public:
	MarketProcess(void)
	{

	}
	~MarketProcess(void){};
	void TriggerHook(void)
	{
		if(hookedObj != NULL)
		{
			hookedObj->MarketDataCallBack();
		}
	}
};

