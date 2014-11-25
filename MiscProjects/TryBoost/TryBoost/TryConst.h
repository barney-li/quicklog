#pragma once
#include <iostream>
using namespace std;
class TryConst
{
public:
	const int mConstInt;
	static int mStaticInt;//static成员不可以使用初始化列表初始化，因为static成员是属于整个类而不是每个对象的
	int mNormalInt;
	TryConst(int a):mConstInt(a){};
	void ConstFunc() const
	{
		cout<<"const object can invoke const method"<<endl;
		cout<<"const function read const variable: "<<mConstInt<<endl;
		cout<<"const function read static variable: "<<mStaticInt<<endl;
		cout<<"const function modify static variable: "<<++mStaticInt<<endl;
		cout<<"const function read normal variable: "<<mNormalInt<<endl;
		cout<<"const function modify normal variale: ILLEGAL"<<endl;//const函数是不能修改除了static成员之外的变量的
		cout<<"const functoin invoke normal function: ILLEGAL"<<endl;//const函数不能调用除了非const类型的成员函数
	}
	void NormalFunc()
	{
		cout<<"const object can not invoke normal method"<<endl;
		cout<<"normal function read const variable: "<<mConstInt<<endl;
		cout<<"normal function modify static variable: "<<++mStaticInt<<endl;
	}
	static void StaticFunc()
	{
		cout<<"const object can invoke static method"<<endl;
		cout<<"static function read static variable: "<<mStaticInt<<endl;
	}
	static void ObjectStaticFunc(TryConst const* obj)
	{
		//obj->mNormalInt = 100;
		cout<<"const object can not has its normal member modified by invoke static method: "<<obj->mNormalInt<<endl;

	}
};
int TryConst::mStaticInt=1;
//int TryConst::mConstInt=0;//非static成员不可以在类外初始化，因为说到底const成员是属于每一个对象而不是整个类的。