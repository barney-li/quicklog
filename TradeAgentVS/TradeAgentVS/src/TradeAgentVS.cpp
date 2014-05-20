// TradeAgentVS.cpp : 
//
//#define BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG //开启后，计时精度为纳秒
//#define BOOST_DATE_TIME_SOURCE
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

	Pas::PrimeryAndSecondary strategy;
	strategy.StartStrategy();
	//CloseAndFar caf;
	//caf.StartStratergy();


	return 0;
}

