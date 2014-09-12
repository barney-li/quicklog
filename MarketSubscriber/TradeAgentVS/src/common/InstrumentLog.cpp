#include "InstrumentLog.h"
// 如果把这里的声明扔到头文件里编译的时候会报重复声明
list<InstrumentLog*> InstrumentLog::mLoggerList;
boost::thread* InstrumentLog::mAutoSyncThread = NULL;
bool InstrumentLog::mDestroyAutoSyncThread = false;
