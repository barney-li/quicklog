#include "stdafx.h"
#include "Log.h"
#include <direct.h>
#include <boost\date_time\posix_time\posix_time.hpp>
using namespace Utilities;
LOG_OPS_STATUS Log::LogThis(string message, bool enter)
{
	try
	{
		if(OpenLogFile() == LOG_NO_ERROR)
		{
			boost::posix_time::ptime localTime = boost::posix_time::microsec_clock::local_time();
			if(enter)
			{
				logFileHandler<<boost::gregorian::to_iso_extended_string(localTime.date())<<" "<<localTime.time_of_day()<<"	"<<message.c_str()<<"\n";
			}
			else
			{
				logFileHandler<<boost::gregorian::to_iso_extended_string(localTime.date())<<" "<<localTime.time_of_day()<<"	"<<message.c_str();
			}

			if(CloseLogFile() == LOG_NO_ERROR)
			{
				return LOG_NO_ERROR;
			}
			else
			{
				return CLOSE_FILE_FAILED;
			}
		}
		else
		{
			return OPEN_FILE_FAILED;
		}
	}
	catch(...)
	{
		cout<<"error in LogThis()"<<endl;
		return WRITE_FILE_FAILED;
	}
}
LOG_OPS_STATUS Log::LogThisNoTimeStamp(string message, bool enter)
{
	try
	{
		if(OpenLogFile() == LOG_NO_ERROR)
		{
			if(enter)
			{
				// this is a tricky one, you must not append "\r\n" as it supposed to, because 
				// it seems that library will automatically added a "\r" prior "\n", so if '\r'
				// is added manually, then the end of every line will looks like "\r\r\n".
				logFileHandler<<message.c_str()<<"\n";
			}
			else
			{
				logFileHandler<<message.c_str();
			}
			if(CloseLogFile() == LOG_NO_ERROR)
			{
				return LOG_NO_ERROR;
			}
			else
			{
				return CLOSE_FILE_FAILED;
			}
		}
		else
		{
			return OPEN_FILE_FAILED;
		}
	}
	catch(...)
	{
		cout<<"error in LogThisNoTimeStamp()"<<endl;
		return WRITE_FILE_FAILED;
	}
}
LOG_OPS_STATUS Log::LogThisFast(string message, bool enter)
{
	try
	{
		boost::posix_time::ptime localTime = boost::posix_time::microsec_clock::local_time();
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
			TrySync();	
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
			TrySync();	
		}
		return LOG_NO_ERROR;
	}
	catch(...)
	{
		cout<<"error in LogThisFast()"<<endl;
		return WRITE_BUFFER_FAILED;
	}
}
LOG_OPS_STATUS Log::LogThisFastNoTimeStamp(string message, bool enter)
{
	try
	{
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
			TrySync();
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
			TrySync();
		}
		return LOG_NO_ERROR;
	}
	catch(...)
	{
		cout<<"error in LogThisFastNoTimeStamp()"<<endl;
		return WRITE_BUFFER_FAILED;
	}
}

LOG_OPS_STATUS Log::Sync(void)
{
	try
	{
		if(1 == bufferIndex)
		{
			// do not sync the file unless there are new datas in the buffer
			if(bufferNo1.size() > 0)
			{
				if(OpenLogFile() == LOG_NO_ERROR)
				{
					bufferIndex = 2;
					logFileHandler<<bufferNo1;
					logFileHandler.flush();
					bufferNo1.clear();			
					CloseLogFile();
				}
				else
				{
					return OPEN_FILE_FAILED;
				}
			}
		}
		else if(2 == bufferIndex)
		{
			// do not sync the file unless there are new datas in the buffer
			if(bufferNo2.size() > 0)
			{
				if(OpenLogFile() == LOG_NO_ERROR)
				{
					bufferIndex = 1;
					logFileHandler<<bufferNo2;
					logFileHandler.flush();
					bufferNo2.clear();
					CloseLogFile();
				}
				else
				{
					return OPEN_FILE_FAILED;
				}
			}
		}
		return LOG_NO_ERROR;
	}
	catch(...)
	{
		cout<<"error in Sync()"<<endl;
		return WRITE_FILE_FAILED;
	}
}
void Log::AutoSync(Log* logger)
{
	while(!logger->endAutoSyncThread)
	{
		try
		{
			boost::this_thread::sleep_for(boost::chrono::seconds(logger->autoSyncPeriod));
			logger->Sync();// sync the file periodically
		}
		catch(...)
		{
			cout<<"error in AutoSync()"<<endl;
		}
	}
}
void Log::SetLogFile(string aDir, string aLogName)
{
	this->logDir = aDir;
	this->logName = aLogName;
}
LOG_OPS_STATUS Log::TrySync()
{
	if(bufferNo1.size()>syncSize && bufferIndex == 1)
	{
		return Sync();
	}
	if(bufferNo2.size()>syncSize && bufferIndex == 2)
	{
		return Sync();
	}
	return LOG_NO_ERROR;
}
LOG_OPS_STATUS Log::OpenLogFile()
{
	try
	{
		if(logFileHandler.is_open() == false)
		{
			logFileHandler.open(logDir+logName, ios::in|ios::out|ios::app);
			if(logFileHandler.is_open() == false)
			{
				_mkdir(logDir.c_str());
				logFileHandler.open(logDir+logName, ios::in|ios::out|ios::app);
				if(logFileHandler.is_open() == false)
				{
					cout<<"Open log file failed"<<endl;
					return OPEN_FILE_FAILED;
				}// file can not be opened
				else
				{
					//cout<<"Open log file successed"<<endl;
					return LOG_NO_ERROR;
				}// file opened
			}// if the file can't be opened
			else
			{
				//cout<<"Open log file successed"<<endl;
				return LOG_NO_ERROR;
			}// if the file has been opened
		}// when the file has not been opened
		else
		{
			return LOG_NO_ERROR;
		}// when the file has already opened
	}
	catch(...)
	{
		cout<<"error in OpenLogFile()"<<endl;
		return OPEN_FILE_FAILED; 
	}
}
LOG_OPS_STATUS Log::CloseLogFile()
{
	try
	{
		if(logFileHandler.is_open() == true)
		{
			logFileHandler.close();
			if(logFileHandler.is_open() == true)
			{
				return CLOSE_FILE_FAILED;
			}
			else
			{
				return LOG_NO_ERROR;
			}
		}	
		else
		{
			return LOG_NO_ERROR;
		}
	}
	catch(...)
	{
		cout<<"error in CloseLogFile()"<<endl;
		return CLOSE_FILE_FAILED; 
	}
}
LOG_OPS_STATUS Log::LogThisAdvance(string aMessage, LOG_LEVEL aLevel, LOG_OUTPUT aOutput, bool aAsyncMode, bool aWithTimeStamp, bool aEnter)
{
	string lLevelPrefix;
	switch (aLevel)
	{
	case LOG_DEBUG:
		lLevelPrefix = "[DEBUG]: ";
		break;
	case LOG_INFO:
		lLevelPrefix = "[INFO]: ";
		break;
	case LOG_WARNING:
		lLevelPrefix = "[WARNING]: ";
		break;
	case LOG_ERROR:
		lLevelPrefix = "[ERROR]: ";
		break;
	case LOG_FATAL:
		lLevelPrefix = "[FATAL]: ";
		break;
	default:
		lLevelPrefix = "";
	}
	aMessage = lLevelPrefix + aMessage;
	if(LOG_STDIO == aOutput || LOG_STDIO_FILESYSTEM == aOutput)
	{
		if(aWithTimeStamp)
		{
			boost::posix_time::ptime localTime = boost::posix_time::microsec_clock::local_time();
			cout<<boost::gregorian::to_iso_extended_string(localTime.date())<<" "<<localTime.time_of_day()<<"	"<<aMessage.c_str();
		}
		else
		{
			cout<<aMessage.c_str();
		}
		if(aEnter) cout<<endl;
	}
	if(aOutput == LOG_FILESYSTEM || aOutput == LOG_STDIO_FILESYSTEM)
	{
		if(aWithTimeStamp)
		{
			if(aAsyncMode)
			{
				return LogThisFast(aMessage, aEnter);
			}
			else
			{
				return LogThis(aMessage, aEnter);
			}
		}
		else
		{
			if(aAsyncMode)
			{
				return LogThisFastNoTimeStamp(aMessage, aEnter);
			}
			else
			{
				return LogThisNoTimeStamp(aMessage, aEnter);
			}
		}
	}
	
}