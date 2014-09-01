#pragma once
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
using namespace boost::posix_time;
using namespace boost;
using namespace std;
using namespace boost::asio;
class Timer
{
protected:
	// io_service, used by the timer
	io_service ios;
	// the timer
	boost::asio::deadline_timer* coreTimer;
	// handler to the timer task
	boost::thread* timerTaskHandler;
	// end timer task flag
	bool endTimerTask;
	// timer time
	unsigned int timerTime;
public:
	Timer(void)
	{
		endTimerTask = false;
		timerTime = 0;
		coreTimer = new boost::asio::deadline_timer(ios, boost::posix_time::millisec(timerTime));
		timerTaskHandler = new boost::thread(TimerTask, this);
	}
	Timer(unsigned int inputTimerTimeInMs) : timerTime(inputTimerTimeInMs)
	{
		endTimerTask = false;
		coreTimer = new boost::asio::deadline_timer(ios, boost::posix_time::millisec(timerTime));
		timerTaskHandler = new boost::thread(TimerTask, this);
	}
	~Timer(void)
	{
		endTimerTask = true;
		if(NULL != timerTaskHandler)
		{
			delete timerTaskHandler;
		}
	}
	// timer task
	static void TimerTask(Timer* timerObj)
	{
		while(false == timerObj->endTimerTask)
		{
			
			timerObj->ios.run();
			boost::this_thread::sleep_for(boost::chrono::nanoseconds(1000000));
		}
	}
	// set timer time
	void SetTimerTime(int inputTimerTime)
	{
		timerTime = inputTimerTime;
	}
	// timer callback
	virtual void TimerCallback(const system::error_code& errorCode)
	{
		if(errorCode)
		{
			cout<<"timer cancelled"<<endl;
		}
		else
		{
			cout<<"time is up"<<endl;
		}
	}
	// start timer
	void StartTimer(int inputTimerTime)
	{
		timerTime = inputTimerTime;
		coreTimer->expires_from_now(boost::posix_time::millisec(timerTime));
		coreTimer->async_wait(boost::bind(&Timer::TimerCallback, this, _1));
	}
	void StartTimer(void)
	{
		coreTimer->expires_from_now(boost::posix_time::millisec(timerTime));
		coreTimer->async_wait(boost::bind(&Timer::TimerCallback, this, _1));
	}
	// cancel timer
	void CancelTimer(void)
	{
		coreTimer->cancel();
	}
};