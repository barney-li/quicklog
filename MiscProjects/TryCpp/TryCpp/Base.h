#include <iostream>
using namespace std;
class Base
{
public:
	// 如果这里不是virtual的话，子类无法真正实现“重写”：
	// 当使用子类对象指针调用Print时，打印的是derived，但是
	// 使用基类对象指针调用Print时，打印的就是base了。切记切记。
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
	// 基类的析构函数一定要是virtual的，否则当delete一个指向
	// 子类对象的基类指针时，子类的析构函数不会被调用。
	virtual ~Base()
	{
		cout<<"base destructor"<<endl;
	}
};