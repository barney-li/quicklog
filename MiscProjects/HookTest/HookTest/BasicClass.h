#pragma once
#include <iostream>
class BasicClass
{
public:
	BasicClass(void){};
	~BasicClass(void){};
	void MarketDataCallBack(void)
	{
		std::cout<<"call back"<<std::endl;
	}
	BasicClass* hookedObj;
	void SetHook(BasicClass* obj)
	{
		hookedObj = obj;
	}

};

