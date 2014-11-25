#pragma once
#include <boost\date_time\posix_time\posix_time.hpp>
#include <string>
#include <strstream>
using namespace boost::posix_time;
using namespace std;
class TryTime
{
private:
	ptime mTime;
public:
	TryTime()
	{
		int lTimeSuffix = 500;
		stringstream lStream;	
		lStream.str("");
		lStream<<"."<<lTimeSuffix;
		string lMyTime = "2000-01-01 12:01:05"+lStream.str();
		mTime = time_from_string(lMyTime);
		cout<<mTime<<endl;
	}
};