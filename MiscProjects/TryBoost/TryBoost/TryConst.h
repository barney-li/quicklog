#pragma once
#include <iostream>
using namespace std;
class TryConst
{
public:
	const int mConstInt;
	static int mStaticInt;//static��Ա������ʹ�ó�ʼ���б��ʼ������Ϊstatic��Ա�����������������ÿ�������
	int mNormalInt;
	TryConst(int a):mConstInt(a){};
	void ConstFunc() const
	{
		cout<<"const object can invoke const method"<<endl;
		cout<<"const function read const variable: "<<mConstInt<<endl;
		cout<<"const function read static variable: "<<mStaticInt<<endl;
		cout<<"const function modify static variable: "<<++mStaticInt<<endl;
		cout<<"const function read normal variable: "<<mNormalInt<<endl;
		cout<<"const function modify normal variale: ILLEGAL"<<endl;//const�����ǲ����޸ĳ���static��Ա֮��ı�����
		cout<<"const functoin invoke normal function: ILLEGAL"<<endl;//const�������ܵ��ó��˷�const���͵ĳ�Ա����
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
//int TryConst::mConstInt=0;//��static��Ա�������������ʼ������Ϊ˵����const��Ա������ÿһ�����������������ġ�