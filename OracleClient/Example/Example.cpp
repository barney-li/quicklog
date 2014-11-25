// Example.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Log.h>
#include <OracleClient.h>
using namespace Utilities;
using namespace DatabaseUtilities;
int _tmain(int argc, _TCHAR* argv[])
{
	Log logger("./log/", "oracle client example log.log", 1024, true, 1);	
	logger.LogThisAdvance("oracle client example started", 
							LOG_INFO, 
							LOG_STDIO_FILESYSTEM);
	OracleClient lClient();
	return 0;
}

