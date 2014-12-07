#include <stdafx.h>
#include "CommonLog.h"
list<CommonLog*> CommonLog::mLoggerList;
boost::thread* CommonLog::mAutoSyncThread = NULL;
bool CommonLog::mDestroyAutoSyncThread = false;
