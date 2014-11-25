#include <iostream>
#include "Base.h"
using namespace std;
class Derived : public Base
{
public:
	// 下边这个，类中的静态成员变量，非常狡猾的小东西。这个
	// 东西的本质是和类外部的全局变量、静态变量一样，存储在
	// 数据区的而不是栈区，与他们的唯一区别就是scope不同，
	// scope仅限于这个类的内部而已。所以下边这行代码其实并
	// 不是对mStaticNumber的定义，仅仅是声明而已，就像C中的
	// extern int mStaticNumber一个概念，声明不产生数据的实
	// 体，仅仅是告诉编译器我这个符号在外部而已。因此，想要
	// 正常使用下边这玩意，还必须在外部“定义”他，定义的方
	// 式：int Derived::mStaticNumber = 0;
	static int mStaticNumber;
public:
	virtual void Print();
	Derived()
	{
		cout<<"derived constructor"<<endl;
		mStaticNumber += 1;
		cout<<"static number is: "<<mStaticNumber<<endl;
	}
	// 在使用拷贝的方式声明对象时，上边的构造函数不会被调用，
	// 下边的拷贝构造函数会被调用。注意，基类那边仍然是调用
	// 构造函数。
	Derived(const Derived& aCopyFrom)
	{
		cout<<"derived copy constructor"<<endl;
	}
	// 值得注意的是：
	// Derived mDerivedThree = mDerivedTwo;// 这句指令会导致拷贝构造函数被调用，而不是赋值运算符
	// mDerivedThree = mDerivedTwo;// 这句才会导致赋值运算符被调用
	void operator=(const Derived& aCopyFrom)
	{
		cout<<"derived assignment operator"<<endl;
	}
	~Derived()
	{
		cout<<"derived destructor"<<endl;
	}
};