#pragma once
#include "basicclass.h"
#include "MarketProcess.h"
class Stratergy :
	public BasicClass
{
public:
	Stratergy(void)
	{
		MarketProcess marketObj;
		marketObj.SetHook(this);
		marketObj.TriggerHook();
	}
	~Stratergy(void){};
};

