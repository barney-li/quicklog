#include "StdAfx.h"
#include "Log.h"
#include <direct.h>
#include <boost\date_time\posix_time\posix_time.hpp>
void Log::LogThis(const char* message, bool enter)
{
	boost::posix_time::ptime localTime = boost::posix_time::microsec_clock::local_time();
	logFileHandler.open(logDir+logName, ios::in|ios::out|ios::app);
	if(logFileHandler.is_open() == false)
	{
		mkdir(logDir.c_str());
		logFileHandler.open(logDir+logName, ios::in|ios::out|ios::app);
		if(logFileHandler.is_open() == false)
		{
			cout<<"Open log file failed"<<endl;
		}
		else
		{
			//cout<<"Open log file successed"<<endl;
		}
	}
	else
	{
		//cout<<"Open log file successed"<<endl;
	}
	if(enter)
	{
		logFileHandler<<boost::gregorian::to_iso_extended_string(localTime.date())<<" "<<localTime.time_of_day()<<"	"<<message<<"\n";
	}
	else
	{
		logFileHandler<<boost::gregorian::to_iso_extended_string(localTime.date())<<" "<<localTime.time_of_day()<<"	"<<message;
	}
	logFileHandler.close();
}
void Log::LogThisNoTimeStamp(const char* message, bool enter)
{
	logFileHandler.open(logDir+logName, ios::in|ios::out|ios::app);
	if(logFileHandler.is_open() == false)
	{
		mkdir(logDir.c_str());
		logFileHandler.open(logDir+logName, ios::in|ios::out|ios::app);
		if(logFileHandler.is_open() == false)
		{
			cout<<"Open log file failed"<<endl;
		}
		else
		{
			//cout<<"Open log file successed"<<endl;
		}
	}
	else
	{
		//cout<<"Open log file successed"<<endl;
	}
	if(enter)
	{
		// this is a tricky one, you must not append "\r\n" as it supposed to, because 
		// it seems that library will automatically added a "\r" prior "\n", so if '\r'
		// is added manually, then the end of every line will looks like "\r\r\n".
		logFileHandler<<message<<"\n";
	}
	else
	{
		logFileHandler<<message;
	}
	logFileHandler.close();
}
void Log::LogThisFast(string message, bool enter)
{
	boost::posix_time::ptime localTime = boost::posix_time::microsec_clock::local_time();
	// when the file has already opened, don't do it again
	if(logFileHandler.is_open() == false)
	{
		logFileHandler.open(logDir+logName, ios::in|ios::out|ios::app);
		if(logFileHandler.is_open() == false)
		{
			mkdir(logDir.c_str());
			logFileHandler.open(logDir+logName, ios::in|ios::out|ios::app);
			if(logFileHandler.is_open() == false)
			{
				cout<<"Open log file failed"<<endl;
				return;
			}
			else
			{
				//cout<<"Open log file successed"<<endl;
			}
		}
		else
		{
			//cout<<"Open log file successed"<<endl;
		}
	}
	if(bufferIndex == 1)
	{
		bufferNo1.append(boost::gregorian::to_iso_extended_string(localTime.date()));
		bufferNo1.append(" ");
		bufferNo1.append(to_simple_string(localTime.time_of_day()));
		bufferNo1.append("	");
		bufferNo1.append(message);
		if(enter)
		{
			// this is a tricky one, you must not append "\r\n" as it supposed to, because 
			// it seems that library will automatically added a "\r" prior "\n", so if '\r'
			// is added manually, then the end of every line will looks like "\r\r\n".
			bufferNo1.append("\n");
		}
		// do not sync the file unless it excees the threashold
		if(bufferNo1.size() >= SIZE_TO_SYNC)
		{
			bufferIndex = 2;
			logFileHandler<<bufferNo1;
			logFileHandler.flush();
			bufferNo1.clear();
		}
	}
	else if(bufferIndex == 2)
	{
		bufferNo2.append(boost::gregorian::to_iso_extended_string(localTime.date()));
		bufferNo2.append(" ");
		bufferNo2.append(to_simple_string(localTime.time_of_day()));
		bufferNo2.append("	");
		bufferNo2.append(message);
		if(enter)
		{
			// this is a tricky one, you must not append "\r\n" as it supposed to, because 
			// it seems that library will automatically added a "\r" prior "\n", so if '\r'
			// is added manually, then the end of every line will looks like "\r\r\n".
			bufferNo2.append("\n");
		}
		// do not sync the file unless it excees the threashold
		if(bufferNo2.size() >= SIZE_TO_SYNC)
		{
			bufferIndex = 1;
			logFileHandler<<bufferNo2;
			logFileHandler.flush();
			bufferNo2.clear();
		}
	}
	

}
void Log::LogThisFastNoTimeStamp(string message, bool enter)
{
	// when the file has already opened, don't do it again
	if(logFileHandler.is_open() == false)
	{
		logFileHandler.open(logDir+logName, ios::in|ios::out|ios::app);
		if(logFileHandler.is_open() == false)
		{
			mkdir(logDir.c_str());
			logFileHandler.open(logDir+logName, ios::in|ios::out|ios::app);
			if(logFileHandler.is_open() == false)
			{
				cout<<"Open log file failed"<<endl;
				return;
			}
			else
			{
				//cout<<"Open log file successed"<<endl;
			}
		}
		else
		{
			//cout<<"Open log file successed"<<endl;
		}
	}
	if(bufferIndex == 1)
	{
		bufferNo1.append(message);
		if(enter)
		{
			// this is a tricky one, you must not append "\r\n" as it supposed to, because 
			// it seems that library will automatically added a "\r" prior "\n", so if '\r'
			// is added manually, then the end of every line will looks like "\r\r\n".
			bufferNo1.append("\n");
		}
		// do not sync the file unless it excees the threashold
		if(bufferNo1.size() >= SIZE_TO_SYNC)
		{
			bufferIndex = 2;
			logFileHandler<<bufferNo1;
			logFileHandler.flush();
			bufferNo1.clear();
		}
	}
	else if(bufferIndex == 2)
	{
		bufferNo2.append(message);
		if(enter)
		{
			// this is a tricky one, you must not append "\r\n" as it supposed to, because 
			// it seems that library will automatically added a "\r" prior "\n", so if '\r'
			// is added manually, then the end of every line will looks like "\r\r\n".
			bufferNo2.append("\n");
		}
		// do not sync the file unless it excees the threashold
		if(bufferNo2.size() >= SIZE_TO_SYNC)
		{
			bufferIndex = 1;
			logFileHandler<<bufferNo2;
			logFileHandler.flush();
			bufferNo2.clear();
		}
	}
	
}

void Log::Sync(void)
{
	if(1 == bufferIndex)
	{
		// do not sync the file unless there are new datas in the buffer
		if(bufferNo1.size() > 0)
		{
			bufferIndex = 2;
			logFileHandler<<bufferNo1;
			logFileHandler.flush();
			bufferNo1.clear();
		}
	}
	else if(2 == bufferIndex)
	{
		// do not sync the file unless there are new datas in the buffer
		if(bufferNo2.size() > 0)
		{
			bufferIndex = 1;
			logFileHandler<<bufferNo2;
			logFileHandler.flush();
			bufferNo2.clear();
		}
	}
}
void Log::AutoSync(Log* logger)
{
	while(!logger->endAutoSyncThread)
	{
		//cout<<logger->logName<<endl;
		boost::this_thread::sleep_for(boost::chrono::seconds(1));
		logger->Sync();
		// do not sync the file unless it excees the threashold
	}
	
}
void Log::SetLogFile(string aDir, string aLogName)
{
	this->logDir = aDir;
	this->logName = aLogName;
}