// TryBoost.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TryThread.h"
#include "TryMutex.h"
#include "TryTime.h"
#include "TryStructure.h"
#include "TryConst.h"
#include "TryLog.h"
int _tmain(int argc, _TCHAR* argv[])
{
	TryConst myConst1(1);
	TryConst myConst2(2);
	myConst1.ConstFunc();
	myConst1.NormalFunc();
	myConst2.ConstFunc();
	const TryConst myConst3(3);
	myConst3.StaticFunc();
	myConst3.ConstFunc();
	//myConst3.NormalFunc();
	myConst3.ObjectStaticFunc(&myConst3);
	TryStructure myStruct;
	myStruct.StartTest();
	//TryMutex myTry;
	//myTry.StartMutexTest();
	TryTime myTime;
	TryThread myTry;
	myTry.CreateALotThread(400);
	TryLog lTryLog;
	lTryLog.PrintTrivialLog();
	return 0;
}

