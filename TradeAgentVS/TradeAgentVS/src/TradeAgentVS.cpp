// TradeAgentVS.cpp : 
// 使用posix_time库的时候貌似要定义下边这玩意，要不然构造ptime时会有异常
#include "stdafx.h"
#include <iostream>
#include <TradeProcess.h>
#include <MarketProcess.h>
#include <PrimeryAndSecondary.h>
#include <ConfigReader.h>
#include <boost\thread.hpp>
#include <boost\progress.hpp>
#include <Log.h>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "common/BackTestAgent.h"
//#include <CloseAndFar.h>
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	boost::progress_display progressDisp(100);
	for(int i=0;i<100;i++)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		progressDisp+=1;
	}

	Log logger;
	logger.LogThisFast("arbitrage stratergy started...");
	logger.Sync();
#ifndef BACK_TEST
	Pas::PrimeryAndSecondary strategy;
	strategy.StartStrategy();
#endif
	//CloseAndFar caf;
	//caf.StartStratergy();
#ifdef BACK_TEST
	BackTestAgent test;
	test.StartBackTest();
#endif

	return 0;
}

