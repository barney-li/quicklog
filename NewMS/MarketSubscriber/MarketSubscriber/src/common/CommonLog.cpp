#include <stdafx.h>
#include "CommonLog.h"
vector<CommonLog*> CommonLog::mLoggerList;
boost::thread* CommonLog::mAutoSyncThread = NULL;
bool CommonLog::mDestroyAutoSyncThread = false;
