#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <string.h>
using namespace std;
class ConfigReader
{
private:
	string ConfigDir;
	string ConfigName;
	string ConfigFullName;
public:
	ConfigReader(void)
	{
		// default config file is ./Config/Config.txt
		ConfigDir = "./data/Config";
		ConfigName = "Config.txt";
		ConfigFullName = ConfigDir + '/' + ConfigName;
	}
	ConfigReader(string inputConfigFullName)
	{
		// config file full name can be change
		if(inputConfigFullName.length()>0)
		{
			ConfigFullName = inputConfigFullName;
		}
		else
		{
			cout<<"Configuration name can not be empty"<<endl;
		}
	}
	// read all the instrument ID
	int ReadInstrumentID(char* outputList[])
	{
		return ReadList(outputList, "InstrumentID", ";");
	}
	// read all the market data front server address
	int ReadMarketFrontAddr(char* outputList[])
	{
		return ReadList(outputList, "MDFrontServer", ";");
	}
	// read all the trade front server address
	int ReadTradeFrontAddr(char* outputList[])
	{
		return ReadList(outputList, "TradeFrontServer", ";");
	}
	// read configuration list according to the start symble and end symble
	int ReadList(char* outputList[], string startSymble, string endSymble = ";")
	{
		startSymble.append("=");
		unsigned int insListIndex = 0;
		string dataLine;
		string lastLine;
		unsigned int payloadStart = 0;
		unsigned int payloadEnd = 0;
		string payload;
		fstream configFile;
		configFile.open(ConfigFullName, ios::in);
		if(configFile.is_open() != true)
		{
			cout<<"Open configuration file failed, please make sure the file directory and name is consistent with your input."<<endl;
			return 0;
		}
		else
		{
			do
			{
				lastLine = dataLine;
				getline(configFile, dataLine);
				// find the start location of start symble and end symble, return -1 if not found
				payloadStart = dataLine.find(startSymble);
				payloadEnd = dataLine.find(endSymble);
				// make sure both locations are valid and there are no overlap between them 
				if(((payloadStart+startSymble.length())<payloadEnd)&&(payloadStart != string::npos)&&(payloadEnd != string::npos))
				{
					// abstract the payload from whole line
					payload = dataLine.substr(payloadStart+startSymble.length(), payloadEnd-(payloadStart+startSymble.length()));
					payload += '\0';
					assert(outputList[insListIndex] != NULL);
					if(outputList[insListIndex] != NULL)
					{
						strcpy(outputList[insListIndex], payload.c_str());
						insListIndex++;
						cout<<dataLine<<endl;
					}
				}
			}while(lastLine != dataLine);
		}
		return insListIndex;
	}
	// read an integer from configuration
	int ReadInteger(int& outputInt, string startSymble, string endSymble = ";")
	{
		startSymble.append("=");
		string dataLine;
		string lastLine;
		unsigned int payloadStart = 0;
		unsigned int payloadEnd = 0;
		string payload;
		fstream configFile;
		configFile.open(ConfigFullName, ios::in);
		if(configFile.is_open() != true)
		{
			cout<<"Open configuration file failed, please make sure the file directory and name is consistent with your input."<<endl;
			return 0;
		}
		else
		{
			do
			{
				lastLine = dataLine;
				getline(configFile, dataLine);
				// find the start location of start symble and end symble, return -1 if not found
				payloadStart = dataLine.find(startSymble);
				payloadEnd = dataLine.find(endSymble);
				// make sure both locations are valid and there are no overlap between them 
				if(((payloadStart+startSymble.length())<payloadEnd)&&(payloadStart != string::npos)&&(payloadEnd != string::npos))
				{
					// abstract the payload from whole line
					payload = dataLine.substr(payloadStart+startSymble.length(), payloadEnd-(payloadStart+startSymble.length()));
					outputInt = std::atoi(payload.c_str());
					// once the symble has been found, return with 0
					return 0;
				}
			}while(lastLine != dataLine);
		}
		return -1;
	}
	// read an float number from configuration
	int ReadDouble(double& outputVal, string startSymble, string endSymble = ";")
	{
		startSymble.append("=");
		string dataLine;
		string lastLine;
		unsigned int payloadStart = 0;
		unsigned int payloadEnd = 0;
		string payload;
		fstream configFile;
		configFile.open(ConfigFullName, ios::in);
		if(configFile.is_open() != true)
		{
			cout<<"Open configuration file failed, please make sure the file directory and name is consistent with your input."<<endl;
			return 0;
		}
		else
		{
			do
			{
				lastLine = dataLine;
				getline(configFile, dataLine);
				// find the start location of start symble and end symble, return -1 if not found
				payloadStart = dataLine.find(startSymble);
				payloadEnd = dataLine.find(endSymble);
				// make sure both locations are valid and there are no overlap between them 
				if(((payloadStart+startSymble.length())<payloadEnd)&&(payloadStart != string::npos)&&(payloadEnd != string::npos))
				{
					// abstract the payload from whole line
					payload = dataLine.substr(payloadStart+startSymble.length(), payloadEnd-(payloadStart+startSymble.length()));
					outputVal = std::atof(payload.c_str());
					// once the symble has been found, return with 0
					return 0;
				}
			}while(lastLine != dataLine);
		}
		return -1;
	}
	// read an integer from configuration
	int ReadString(string& outputString, string startSymble, string endSymble = ";")
	{
		startSymble.append("=");
		string dataLine;
		string lastLine;
		unsigned int payloadStart = 0;
		unsigned int payloadEnd = 0;
		string payload;
		fstream configFile;
		configFile.open(ConfigFullName, ios::in);
		if(configFile.is_open() != true)
		{
			cout<<"Open configuration file failed, please make sure the file directory and name is consistent with your input."<<endl;
			return 0;
		}
		else
		{
			do
			{
				lastLine = dataLine;
				getline(configFile, dataLine);
				// find the start location of start symble and end symble, return -1 if not found
				payloadStart = dataLine.find(startSymble);
				payloadEnd = dataLine.find(endSymble);
				// make sure both locations are valid and there are no overlap between them 
				if(((payloadStart+startSymble.length())<payloadEnd)&&(payloadStart != string::npos)&&(payloadEnd != string::npos))
				{
					// abstract the payload from whole line
					payload = dataLine.substr(payloadStart+startSymble.length(), payloadEnd-(payloadStart+startSymble.length()));
					outputString = payload;
					// once the symble has been found, return with 0
					return 0;
				}
			}while(lastLine != dataLine);
		}
		return -1;
	}
};
