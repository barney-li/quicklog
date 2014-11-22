// LogExample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Log.h>
#include <string>
#include <boost\date_time\posix_time\posix_time.hpp>
using namespace std;
using namespace Utilities;

int _tmain(int argc, _TCHAR* argv[])
{
	string lLogDir = ".\\Log\\";	
	int i=0;
	boost::posix_time::ptime startTime;
	boost::posix_time::ptime endTime;
	boost::posix_time::time_duration duration;

	Log* logger5 = new Log(lLogDir, "LogThisAdvance.log", 4096, true, 1);
	startTime = boost::posix_time::microsec_clock::local_time();
	for(i=0; i<100000; i++)
	{
		logger5->LogThisAdvance("log system test", LOG_INFO, LOG_FILESYSTEM, true, true, true);
	}
	delete logger5;
	endTime = boost::posix_time::microsec_clock::local_time();
	duration = endTime-startTime;
	cout<<"100000 times LogThisAdvance takes "<<duration.total_milliseconds()<<" ms"<<endl;

	Log* logger1 = new Log(lLogDir, "LogThisFastTest.log", 4096, true, 1);
	startTime = boost::posix_time::microsec_clock::local_time();
	for(i=0; i<100000; i++)
	{
		logger1->LogThisFast("log system test");
	}
	delete logger1;
	endTime = boost::posix_time::microsec_clock::local_time();
	duration = endTime-startTime;
	cout<<"100000 times LogThisFast takes "<<duration.total_milliseconds()<<" ms"<<endl;

	Log* logger2 = new Log(lLogDir, "LogThisFastNoTimeStampTest.log", 4096, true, 1);
	startTime = boost::posix_time::microsec_clock::local_time();
	for(i=0; i<100000; i++)
	{
		logger2->LogThisFastNoTimeStamp("log system test");
	}
	delete logger2;
	endTime = boost::posix_time::microsec_clock::local_time();
	duration = endTime-startTime;
	cout<<"100000 times LogThisFastNoTimeStamp takes "<<duration.total_milliseconds()<<" ms"<<endl;

	Log* logger3 = new Log(lLogDir, "LogThisTest.log", 4096, false, 1);
	startTime = boost::posix_time::microsec_clock::local_time();
	for(i=0; i<100000; i++)
	{
		logger3->LogThis("log system test");
	}
	delete logger3;
	endTime = boost::posix_time::microsec_clock::local_time();
	duration = endTime-startTime;
	cout<<"100000 times LogThis takes "<<duration.total_milliseconds()<<" ms"<<endl;

	Log* logger4 = new Log(lLogDir, "LogThisNoTimeStampTest.log", 4096, false, 1);
	startTime = boost::posix_time::microsec_clock::local_time();
	for(i=0; i<100000; i++)
	{
		logger4->LogThisNoTimeStamp("log system test");
	}
	delete logger4;
	endTime = boost::posix_time::microsec_clock::local_time();
	duration = endTime-startTime;
	cout<<"100000 times LogThisNoTimeStamp takes "<<duration.total_milliseconds()<<" ms"<<endl;

	return 0;
}

