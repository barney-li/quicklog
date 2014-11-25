#pragma once
class TryStructure
{
	struct S
	{
		int* p;
		int* p2;
	};
public:
	void StartTest()
	{
		struct S myS;
		int* p = (int*)&myS.p;
		p[0]=1;
		p[1]=2;
	}
	int foo(int x)
	{
		x=5678;
	}
};