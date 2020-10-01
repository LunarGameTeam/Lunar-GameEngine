#pragma once
#include <boost/function.hpp>
#include <boost/thread.hpp>


class LThread
{
public:
	LThread(boost::function<void()> func);
	LThread();
	void Join();
private:
	boost::thread m_thread;

};

LThread::LThread(boost::function<void()> func):
	m_thread(func)
{
	

}

void LThread::Join()
{
	m_thread.join();
}

