#include "InstrumentLog.h"
// ���������������ӵ�ͷ�ļ�������ʱ��ᱨ�ظ�����
list<InstrumentLog*> InstrumentLog::mLoggerList;
boost::thread* InstrumentLog::mAutoSyncThread = NULL;
bool InstrumentLog::mDestroyAutoSyncThread = false;
