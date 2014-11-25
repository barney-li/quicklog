#include <iostream>
#include "Base.h"
using namespace std;
class Derived : public Base
{
public:
	// �±���������еľ�̬��Ա�������ǳ��ƻ���С���������
	// �����ı����Ǻ����ⲿ��ȫ�ֱ�������̬����һ�����洢��
	// �������Ķ�����ջ���������ǵ�Ψһ�������scope��ͬ��
	// scope�������������ڲ����ѡ������±����д�����ʵ��
	// ���Ƕ�mStaticNumber�Ķ��壬�������������ѣ�����C�е�
	// extern int mStaticNumberһ������������������ݵ�ʵ
	// �壬�����Ǹ��߱�����������������ⲿ���ѡ���ˣ���Ҫ
	// ����ʹ���±������⣬���������ⲿ�����塱��������ķ�
	// ʽ��int Derived::mStaticNumber = 0;
	static int mStaticNumber;
public:
	virtual void Print();
	Derived()
	{
		cout<<"derived constructor"<<endl;
		mStaticNumber += 1;
		cout<<"static number is: "<<mStaticNumber<<endl;
	}
	// ��ʹ�ÿ����ķ�ʽ��������ʱ���ϱߵĹ��캯�����ᱻ���ã�
	// �±ߵĿ������캯���ᱻ���á�ע�⣬�����Ǳ���Ȼ�ǵ���
	// ���캯����
	Derived(const Derived& aCopyFrom)
	{
		cout<<"derived copy constructor"<<endl;
	}
	// ֵ��ע����ǣ�
	// Derived mDerivedThree = mDerivedTwo;// ���ָ��ᵼ�¿������캯�������ã������Ǹ�ֵ�����
	// mDerivedThree = mDerivedTwo;// ���Żᵼ�¸�ֵ�����������
	void operator=(const Derived& aCopyFrom)
	{
		cout<<"derived assignment operator"<<endl;
	}
	~Derived()
	{
		cout<<"derived destructor"<<endl;
	}
};