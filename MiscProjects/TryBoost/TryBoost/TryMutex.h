#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/date_time.hpp>
using namespace std;
using namespace boost::date_time; 
class TryMutex
{
private:
	
	boost::thread mThread1;
	boost::thread mThread2;
	boost::mutex mMutex;
	boost::posix_time::time_period* mPeriod;
public:
	TryMutex()
	{
		mPeriod = new boost::posix_time::time_period(boost::posix_time::time_from_string("2000-01-01 00:00:00"), boost::posix_time::time_from_string("2000-01-01 00:00:01"));
	}
	void StartMutexTest()
	{
		cout<<*mPeriod<<endl;
		mThread1 = boost::thread(boost::bind(&TryMutex::Thread1, this, 1));
		mThread2 = boost::thread(boost::bind(&TryMutex::Thread2, this, 5));

		mThread1.join();
		mThread2.join();
	}
private:
	void Thread1(int aOutput)
	{
		Thread(aOutput);
	}
	void Thread2(int aOutput)
	{
		Thread(aOutput);
	}
	void Thread(int aOutput)
	{
		boost::lock_guard<boost::mutex> lLockGuard(mMutex);

		for(int i=0; i<10; i++)
		{
			cout<<aOutput;
			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}
		cout<<endl;
	}
};
