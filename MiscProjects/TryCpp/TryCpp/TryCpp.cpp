// TryCpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "Derived.h"
#include <list>
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	list<string> lList = list<string>();
	lList.push_back("aaa");
	lList.push_back("bbb");
	lList.push_back("ccc");
	list<string>::iterator lListIterator = lList.begin();
	char* pString = new char[10];
	for(lListIterator = lList.begin(); lListIterator != lList.end(); lListIterator++)
	{
		cout<<*lListIterator<<endl;
		char abc[20];

		strncpy(pString, &*lListIterator->c_str(), sizeof(10));
	}
	if(!lList.empty())
	{
		lList.clear();
	}

	string abc;
	Derived mDerived;
	Base* pBase;
	pBase = &mDerived;
	pBase->Print();
	Derived mDerivedTwo(mDerived);
	Derived mDerivedThree = mDerivedTwo;// ���ָ��ᵼ�¿������캯�������ã������Ǹ�ֵ�����
	mDerivedThree = mDerivedTwo;// ���Żᵼ�¸�ֵ�����������
	Derived* pDerivedFour = new Derived();
	pBase = pDerivedFour;
	delete pBase;
	return 0;

}

