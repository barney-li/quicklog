//#ifndef PARSETRADEINFO_H
#define PARSETRADEINFO_H
#include <ThostFtdcTraderApi.h>
#include <ThostFtdcUserApiDataType.h>
#include <ThostFtdcUserApiStruct.h>
class ParseTradeInfo
{
public:
	static char* ParseActionFlag(char input)
	{
		switch(input)
		{
		case '0':
			return "Delete";
		case '3':
			return "Modify";
		default:
			break;
		}
	}
};
//#endif
