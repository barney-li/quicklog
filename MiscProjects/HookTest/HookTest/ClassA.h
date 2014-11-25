#pragma once
#include "ClassB.h"
namespace ClassANameSpace
{
class ClassA
{
private:
	ClassBNameSpace::ClassB* bObj;
public:
	ClassA(void){};
	~ClassA(void){};
	void SetHook(ClassBNameSpace::ClassB* inputObj)
	{
		bObj = inputObj;
	}
};
}

