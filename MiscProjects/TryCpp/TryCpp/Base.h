#include <iostream>
using namespace std;
class Base
{
public:
	// ������ﲻ��virtual�Ļ��������޷�����ʵ�֡���д����
	// ��ʹ���������ָ�����Printʱ����ӡ����derived������
	// ʹ�û������ָ�����Printʱ����ӡ�ľ���base�ˡ��м��мǡ�
	virtual void Print()
	{
		cout<<"base"<<endl;
	}
	Base()
	{
		cout<<"base constructor"<<endl;
	}
	Base(const Base& aCopyFrom)
	{
		cout<<"base copy constructor"<<endl;
	}
	// �������������һ��Ҫ��virtual�ģ�����deleteһ��ָ��
	// �������Ļ���ָ��ʱ������������������ᱻ���á�
	virtual ~Base()
	{
		cout<<"base destructor"<<endl;
	}
};