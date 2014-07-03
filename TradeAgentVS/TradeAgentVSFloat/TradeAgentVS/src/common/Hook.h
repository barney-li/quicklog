// Provide hook service between two class objects
#pragma once
#include <ThostFtdcUserApiStruct.h>
#include <ThostFtdcUserApiDataType.h>
#include <iostream>
class Hook
{
protected:
	Hook* hookObj;
public:
	//
	void SetHook(Hook* inputHookObj)
	{
		this->hookObj = inputHookObj;
	}
	virtual void HookOnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
	{
		std::cout<<"Received market data: "<<pDepthMarketData->InstrumentID<<std::endl;
	}
	virtual void TimerCallback(void)
	{
		std::cout<<"Time is up"<<std::endl;
	}

};