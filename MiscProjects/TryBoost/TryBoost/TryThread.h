#include <boost/thread.hpp>
using namespace boost;
using namespace std;
class TryThread
{
public:
	boost::thread* mThread1;
	void CreateThread()
	{
		mThread1 = new boost::thread(ThreadRoutine, this);
		cout<<"thread created"<<endl;
	}
	
	static void ThreadRoutine(TryThread* obj)
	{
		int i=0;
		cout<<"thread started"<<endl;
		char lHugeMatrix[1000000] = {0};
		while(1)
		{
			i++;
			boost::this_thread::sleep_for(boost::chrono::microseconds(100));
			cout<<".";
		}
		cout<<"thread ended"<<endl;
	}
	void CreateALotThread(int aNum)
	{
		int i=0;
		for(i=0; i<aNum; i++)
		{
			new boost::thread(ThreadRoutine, this);
			cout<<"thread #"<<i<<endl;
		}
	}
	
};