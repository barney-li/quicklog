#pragma once
#include "ClassA.h"
namespace ClassBNameSpace
{
class ClassB
{
public:
	ClassB(void)
	{
		ClassANameSpace::ClassA aObj;
		aObj.SetHook(this);
	}
	~ClassB(void){};
};
}
